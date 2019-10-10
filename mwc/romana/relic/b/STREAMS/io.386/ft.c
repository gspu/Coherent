extern int OUTB_DB;

int FT_BLAB = 0;
int FT_ACKBLAB = 0;

/*
 * File:	ft.c
 *
 * Purpose:	Floppy tape device control.
 *		Requires 765 controller module fdc.c
 *		FDC = floppy disk controller, e.g. NEC upd765
 *
 * Revised: Tue Jun 29 08:22:14 1993 CDT
 *
 * To do:
 *   Read fractional segment or block, then come back for the rest.
 *   Coordinate conversion: tape <--> pseudo diskette
 *   Skip entire segment if bad block map says > 28 bad blocks.
 *   ECC.
 *   QIC-40.
 *   205' cartridges.
 *   Write.
 *   Bad block map.
 *   Don't rewind on close (or on next open?).
 *   Timeout protection whenever sleeping on FDC interrupt.
 *   VTBL
 *   Ability to read/write DOS tapes.
 *   Compression.
 *   ioctl: get info and status
 *   ioctl: format
 *   ioctl: rewind, retension, wind to end of data
 *   Ioctl: mapPhysUser() for improved speed.
 */

/*
 * Here is the protocol for QIC report commands, I think:
 * 
 * Send QIC report command to FDC (this will be an FDC seek command).
 * FDC interrupts when step pulses are sent.
 * Send Sense Interrupt Status to FDC.  (Clears interrupt line.)
 * Read interrupt status from FDC.
 * - Get ACK from tape drive.  Want Track 0 true.  Variable latency.
 * Do
 *   Send Sense Drive Status to FDC.
 *   Read drive status from FDC, including Track 0 bit.
 * Until Track 0 true.
 * - Get data bits for the report command.
 * For number-of-data-bits-in-report times
 *   Send QIC Report Next Bit command to FDC.
 *   FDC interrupts when step pulses are sent.
 *   Send Sense Interrupt Status to FDC.
 *   Read interrupt status from FDC.
 *   Send Sense Drive Status to FDC.
 *   Read drive status from FDC, including Track 0 bit.
 *   Save Track 0 bit value into report data.
 * End for
 * - Get final Track 0 true from tape drive.
 * Send QIC Report Next Bit command to FDC.
 * FDC interrupts when step pulses are sent.
 * Send Sense Interrupt Status to FDC.
 * Read interrupt status from FDC.
 * Send Sense Drive Status to FDC.
 * Read drive status from FDC, including Track 0 bit.
 * Track 0 bit must be true.
 */

/*
 * ----------------------------------------------------------------------
 * Includes.
 */
#include	<sys/coherent.h>

#include	<errno.h>
#include	<sys/buf.h>
#include	<sys/con.h>
#include	<sys/devices.h>
#include	<sys/dmac.h>
#include	<sys/fdc765.h>
#include	<sys/fdioctl.h>
#include	<sys/file.h>
#include	<sys/sched.h>
#include	<sys/stat.h>
#include	<sys/xl.h>
#include	<sys/xlfdc.h>
#include	<sys/xlft.h>

/*
 * ----------------------------------------------------------------------
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */
#define FT_MAX_NBUF	150	/* NBUF patch value is limited to this.	*/

#define	FT_BFRSZ	32768		/* 32k, size of QIC-80 segment	*/
#define	FT_BLKSZ	1024		/* 1k, size of QIC-80 block	*/
#define	FT_BLK_PER_SEG	32		/* 32 blocks per segment	*/
#define FT_NUM_ECC_BLKS 3		/* 3 ECC blocks per segment	*/

#define FT_ACK_TRIES	20	/* max # of tries for ACK to QIC rpt cmd*/
#define FT_CAL_TRIES	2	/* max # of tries for seek load point	*/

#define FT_CAL_SECS	120	/* max # seconds for seek load point	*/
#define FT_PAUSE_SECS	15	/* max # seconds for pause		*/
#define FT_RDY_SECS	120	/* max # seconds for drive ready	*/
#define FT_SEEK_SECS	15	/* max # seconds for seek head to track	*/
#define FT_WIND_SECS	180	/* max # seconds for wind or rewind	*/

#define FT_INITIAL_TRACK	0
#define FT_INITIAL_PHY_SEG	3	/* Skip 2 format headers + VTBL	*/

#define FT_OPEN_ERR(msg)	{devmsg(dev,msg);goto badFtOpen;}
#define FT_READ_ERR(msg)	{devmsg(dev,msg);goto badFtRead;}

/*
 * Macros to get information from tape unit number, which is 0..3.
 * Unit 0 is soft select - no motor on.
 * Units 1..3 are hardware select.
 */
#define FT_DRIVE(unit)		(unit)
#define FT_MOTOR(unit)		((unit > 0) ? FDC_MOTOR_ON : FDC_MOTOR_OFF)
#define FT_SOFT_SELECT(unit)	(unit == 0)

/* Divide, rounding up if remainder is nonzero. */
#define FT_DIV_RU(dividend, divisor) \
	(((dividend) + (divisor) - 1) / (divisor))

/* Real (tape) and pseudo (diskette) parameters for each cartridge format. */
struct FtParm {
	/* Tape parameters. */
	uint	segPerTrk;
	uint	tracks;

	/* FDC pseudo parameters. */
	uint	blkPerTrk;
	uint	heads;
	uint	cyls;
};

/* Driver state variables. */
struct FT {
	/* booleans */
	unchar	ft_wakeMeUp;		/* 1 = sleeping til next FDC IRQ */
	unchar	ft_dumpIrq;		/* 1 = dump IRQ status		*/
	unchar	ft_ackNeeded;		/* 1 = awaiting ACK to rpt cmd	*/
	unchar	ft_ackMissed;		/* 1 = wanted ACK, didn't get	*/
	unchar	ft_open;		/* 1 = device is open		*/
	unchar	ft_refd;		/* 1 = found reference burst	*/
	unchar	ft_useEcc;		/* 1 = Reed-Solomon enabled	*/
	unchar	ft_useBadMap;		/* 1 = use bad block map	*/

	/* very short ints */
	unchar	ft_pcn;			/* present cylinder #		*/
	unchar	ft_bitsNeeded;		/* # of Report Next Bit's to do	*/
	unchar	ft_bitsRcvd;		/* # of report bits received	*/
	unchar	ft_drv;			/* drive number, 0..3		*/
	unchar	ft_fmt;			/* index into ftParm		*/

	unchar	ft_cyl;			/* cylinder	FDC - C		*/
	unchar	ft_head;		/* head		FDC - H		*/
	unchar	ft_blk;			/* block	FDC - R		*/
	unchar	ft_ssz;			/* record size	FDC - N		*/

	/* short ints */
	ushort	ft_report;		/* where reported bits go	*/
	ushort	ft_track;		/* track for next access	*/
	ushort	ft_phySeg;		/* segment # for next access	*/

	/* other structs */
	TIM	ft_tim;
};

/* Descriptor for a run of good blocks within a single segment. */
struct FtRun {
	uint	blkToRead;	/* Next block wanted from current segment */
	uint	blksRead;	/* Number of blocks read from this seg    */
	uint	runLength;	/* Number of good blocks in current run   */
};

/*
 * ----------------------------------------------------------------------
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */
extern int	nulldev();
extern char *	getDmaMem();

/* CON entry points. */
static int	ftblock();
static int	ftclose();
static int	ftioctl();
static int	ftload();
static int	ftopen();
static int	ftread();
static int	ftunload();
static int	ftwrite();

/* Debug routines. */
static	void	ftDbPrintCmd();
static	void	ftDbPrintErr();
static	void	ftDbPrintPos();
static	void	ftDbPrintStat();
static	void	ftDbPrtStat();

/* Support routines. */
static	int	ftCmd();
static	int	ftCmdArg();
static	int	ftCmdSend();
static	void	ftFdcPos();
static	int	ftGetBlkRun();
static	void	ftGetInfo();
static	void	ftIrqHandler();
static	int	ftReadBlkRun();
static	int	ftReadBlks();
static	int	ftReadID();
static	int	ftReadSegs();
static	int	ftReadyWait();
static	int	ftRecal();
static	void	ftResetFDC();
static	void	ftRptBegin();
static	void	ftRptUpdate();
static	int	ftSeg();
static	void	ftSelect();
static	int	ftSkipBack();
static	int	ftSkipFwd();
static	int	ftStartTape();
static	int	ftStopTape();
static	int	ftStsWthErr();
static	int	ftWait();

/*
 * ----------------------------------------------------------------------
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */

CON	ftcon = {
	DFCHR,				/* Flags */
	FL_MAJOR,			/* Major index */
	ftopen,				/* Open */
	ftclose,			/* Close */
	ftblock,			/* Block */
	ftread,				/* Read */
	ftwrite,			/* Write */
	ftioctl,			/* Ioctl */
	nulldev,			/* Powerfail */
	nulldev,			/* Timeout */
	ftload,				/* Load */
	ftunload,			/* Unload */
	nulldev				/* Poll */
};

/*
 * Patchable values.
 *
 * FT_NBUF = number of 32 Kbyte segment buffers allocated.  In the
 *   present memory model, this buffer space cannot be used for
 *   anything else, and is part of the "PhysMem" pool reserved at
 *   startup.  The buffer set is attached at ftBigBuf during ftload().
 * FT_CUSHION = number of segments we try to have between current
 *   physical position of tape when stopped and the next segment to
 *   be accessed.  The cushion allows time for the tape to come up to
 *   speed.
 */

int	FT_NBUF = 16;
int	FT_CUSHION = 5;		/* # of segs passed during start/stop	*/

/* Parameters for FDC Specify Command */
int FT_SRT_2 = 0xE;
int FT_SRT_3 = 0xD;
int FT_HUT = 0xF;
int FT_HLT = 0x1;

static struct FT	ft;
static struct FtParm	ftParm[] = {
/* segments/track, tracks, blocks/pseudotrack, heads, cylinders	*/
	{ 150, 28, 128, 7, 150 },		/* QIC-80, 307.5'	*/
	{ 100, 28, 128, 5, 150 },		/* QIC-80, 205'		*/
	{ 102, 20, 128, 2, 255 },		/* QIC-40, 307.5'	*/
	{  68, 20, 128, 2, 170 }		/* QIC-40, 205'		*/
};

/*
 * DMA store.
 *   ftBigBuf is the virtual address of an aligned, physically contiguous
 *     buffer area, FT_NBUF segments in length.
 *   ftBufBlks[s] says how many blocks (0..32) in segment s of ftBigBuf were
 *     successfully read.  These blocks are contiguous, aligned at the start
 *     of segment buffer s.
 *   ftBadBlkCt[s] says how many blocks in segment s of ftBigBuf could not be
 *     read due to FDC error. This number is in the range (0..2) =
 *     (0..FT_NUM_ECC_BLKS).  It gives a count of blocks that should have
 *     been read (i.e., not listed in the bad block table) and that may be
 *     recovered using ecc.
 *   ftBadBlk[s][e] is the block number in segment s where read error
 *     number e (0..2) occurred.
 *   ftSegCt is the number of segment buffers in ftBigBuf which have data.
 *   ftCurrentSeg is the segment number being read, < ftSegCt.
 *   ftOffset is byte offset within current segment for next access.
 */
static unchar * ftBigBuf;
static unchar	ftBufBlks[FT_MAX_NBUF];
static unchar	ftBadBlkCt[FT_MAX_NBUF];
static unchar	ftBadBlk[FT_MAX_NBUF][FT_NUM_ECC_BLKS];
static int	ftSegCt;
static int	ftCurrentSeg;
static int	ftOffset;

/* Strings where multibyte FDC commands are built. */

unchar rwCmd[] = {
	0x46,		/* FDC read, skip deleted data, MFM.	*/
	0,		/* Head 0, unit 0.			*/
	0,		/* C = FDC cylinder			*/
	0,		/* H = FDC head				*/
	1,		/* R = FDC block number (1-based)	*/
	3,		/* N = block length (3 = 1024 bytes)	*/
	226,		/* EOT = last block number on cylinder	*/
	1,		/* GPL = number of bytes in gap 3	*/
	255		/* DTL, use 0xFF when N != 0		*/
};

/* Read/write command bytes. */
enum {
	RW_CMD_RD = 0x46,
	RW_CMD_WR = 0x45
};

/* Offsets into rwCmd. */
enum {
	RW_CMD_VERB	= 0,
	RW_CMD_UNIT	= 1,
	RW_CMD_CYL	= 2,
	RW_CMD_HEAD	= 3,
	RW_CMD_SCTR	= 4,
	RW_CMD_SCSZ	= 5,	/* encoded block length	*/
	RW_CMD_SCHI	= 6,	/* high block number		*/
	RW_CMD_GPL	= 7,
	RW_CMD_DTL	= 8,
	RW_CMD_LEN	= 9
};

/*
 * ----------------------------------------------------------------------
 * Code.
 */

/*
 * CON struct routines.
 */

/************************************************************************
 *	ftblock
 *
 * Tape is not a block device, but we need a block entry point since
 * the same driver controls diskette access.
 ***********************************************************************/
static int
ftblock(bp)
BUF *bp;
{
	u.u_error = EIO;
	bp->b_flag |= BFERR;
	bdone(bp);
	return;
}

/************************************************************************
 * ftclose
 *
 ***********************************************************************/
static int
ftclose(dev, mode)
dev_t dev;
int mode;
{
	printf("ftclose ");

	setFtIntr(1);
	if (FT_SOFT_SELECT(ft.ft_drv))
		ftCmd(QIC_CMD_SS_OFF);

	ft.ft_open = 0;
	setFtIntr(0);
}

/*************************************************************************
 *	ftioctl
 *	Handle tape drive & controller commands like erase, rewind,
 *	retension, read filemark, and write filemark
 ************************************************************************/
ftioctl(dev, cmd, arg)
register int	dev, cmd;
int		arg;
{
}

/*************************************************************************
 *	ftload
 ************************************************************************/
static int
ftload()
{
	uint bigBufLen;

	if (FT_NBUF > FT_MAX_NBUF)
		FT_NBUF = FT_MAX_NBUF;

	bigBufLen = FT_NBUF * FT_BFRSZ;
	ftIntr = ftIrqHandler;
	if ((ftBigBuf = getDmaMem(bigBufLen, FT_BFRSZ)) == NULL)
		printf("ERROR - can't allocate %d bytes for ft buffers\n",
		  bigBufLen);
}

/************************************************************************
 * ftopen
 *
 ***********************************************************************/
static int
ftopen(dev, mode)
dev_t dev;
int mode;
{
	unsigned int drvStat;
	int i;
	int bit;
	int result;
	int notReady;
	int drvStatus;
	int putCount;

	/* Couldn't allocate buffer area. */
	if (ftBigBuf == NULL) {
		devmsg(dev, "no buffers ");
		u.u_error = EIO;
		return;
	}

	/* Can't append to tape. */
	if (mode & IPAPPEND) {
		devmsg(dev, "can't append ");
		u.u_error = EINVAL;
		return;
	}

	/* Only one open at a time. */
	if (ft.ft_open) {
		devmsg(dev, "only one ftopen at a time ");
		u.u_error = EBUSY;
		return;
	}
	ft.ft_open = 1;

	ft.ft_drv = FT_UNIT(dev);

	if (!setFtIntr(1))
		FT_OPEN_ERR("fdc unavailable ");

	ftIntr = ftIrqHandler;

	/* Select tape drive. */
	ftSelect(ft.ft_drv);

	if (ftCmd(QIC_CMD_RST))
		FT_OPEN_ERR("soft reset failed ");

	/* Need to wait a second after QIC software reset. */
	ftWait(HZ);

	/* Re-Select tape drive (reset clears soft select). */
	ftSelect(ft.ft_drv);

	/* HERE is the test for tape drive present. */
	drvStatus = ftStsWthErr();
	if (drvStatus == -1)
		FT_OPEN_ERR("get drive status failed ");

	/* Wait for drive to be ready, or give up. */
	if(ftReadyWait(FT_RDY_SECS))
		FT_OPEN_ERR("Tape Drive Not Ready");

	/* Need Cartridge Present to be true. */
	if ((ft.ft_report & QIC_STAT_PRSNT) == 0)
		FT_OPEN_ERR("No Cartridge");

	/* Initialize state variables & such. */
	ft.ft_refd = 0;
	ft.ft_fmt = 0;		/* 307.5' QIC-80 */
	ft.ft_useEcc = 0;	/* No ECC. Yet. */
	ft.ft_useBadMap = 0;	/* No bad block map use. Yet. */

	/* Wrap-up. */
	fdcSpecify(FT_SRT_3, FT_HUT, FT_HLT);
	setFtIntr(0);
	return;

	/* Error exit. */
badFtOpen:

	u.u_error = EIO;
	ft.ft_open = 0;
	fdcSpecify(FT_SRT_3, FT_HUT, FT_HLT);
	fdcDrvSelect(0, FDC_MOTOR_OFF);
	if (FT_SOFT_SELECT(ft.ft_drv))
		ftCmd(QIC_CMD_SS_OFF);
	setFtIntr(0);
	return;
}

/************************************************************************
 * ftread
 *
 * if tape not referenced
 *   seek load point
 *   wait for ready
 *   if tape still not referenced
 *     abort read - tape not formatted
 *   set next access to track 0, segment 3
 * while more data is to be read
 *   read as much as possible into DMA buffer area
 *   abort in case of read error
 *   iowrite() - copy DMA buffers into user space
 ***********************************************************************/
static int
ftread(dev, iop)
dev_t dev;
IO * iop;
{
#if 0
	unchar	drvStatus;
	uint	calTries;
	int	segsWanted;
	unchar	* bufPtr;
	int	i;
	int	bytesInBuffer;
	int	dataBlksPerSeg;
	int	dataBytesPerSeg;

	/* Has reference burst been seen? */
	drvStatus = ftStsWthErr();
	if (drvStatus == -1)
		FT_READ_ERR("get drive status failed ");

	/*
	 * If not, look for reference burst now.
	 * If this fails, it's probably due to unformatted tape.
	 */
	if ((drvStatus & QIC_STAT_REFD) == 0) {
		ftCmd(QIC_CMD_CAL);
		if (ftReadyWait(FT_CAL_SECS))
			FT_READ_ERR("seek load point failed");

		/* Again, has reference burst been seen? */
		drvStatus = ftStsWthErr();
		if (drvStatus == -1)
			FT_READ_ERR("get drive status failed ");

		if ((drvStatus & QIC_STAT_REFD) == 0)
			FT_READ_ERR("seek load point failed");

		/* Initialize access coordinates for next read. */
		ft.ft_track = FT_INITIAL_TRACK;
		ft.ft_phySeg = FT_INITIAL_PHY_SEG;
	}

	/*
	 * Compute expected number of data blocks, and bytes, in a segment.
	 * This is an estimate, as it does not consider bad blocks that are
	 * mapped out.
	 */
	dataBlksPerSeg = FT_BLK_PER_SEG;
	if (ft.ft_useEcc)
		dataBlksPerSeg -= FT_NUM_ECC_BLKS;
	dataBytesPerSeg = dataBlksPerSeg * FT_BLKSZ;

	/*
	 * While there is input needed
	 *   If DMA buffer is used up
	 *     Load DMA buffer area from tape.
	 *     Update DMA buffer stats:
	 *       Number of valid segments = number of segments read
	 *       Current segment and current offset both = 0
	 *   Transfer data:
	 *     Source is DMA buffer, current segment and offset.
	 *     Destination is IO struct.
	 *     Update DMA buffer current segment and offset.
	 */
	while (iop->io_ioc) {

		/* Do we need to fill DMA buffer from tape? */
		if (ftCurrentSeg >= ftSegCt) {

			/* Estimate how many segments will need to be read. */
			segsWanted = FT_DIV_RU(iop->io_ioc, dataBytesPerSeg);

			/* Try to fill DMA buffer area. */
			ftSegCt = ftReadSegs(segsWanted);

			/* Update current DMA data pointer. */
			ftCurrentSeg = 0;
			ftOffset = 0;

			/* Abort read if something went wrong. */
			if (ftSegCt == -1)
				FT_READ_ERR("ftBufRead failed");

			/* Terminate read if at end of tape. */
			if (ftSegCt == 0)
				break;

			/* HERE is where ecc will be done. */
			/* This will involve decrementing ftBufBlks[?] by 3. */
		}

XXX
		/* Transfer data from DMA buffer to IO area. */
		while
		for (i = 0, bufPtr = ftCurrentSeg;
		  bufPtr += FT_BFRSZ, i < ftSegCt; i++) {

			bytesInBuffer = ftBufBlks[i] * FT_BLKSZ;
			if (bytesInBuffer != 0) {

				int transferCount = iop->io_ioc;

				if (transferCount > bytesInBuffer)
					transferCount = bytesInBuffer;

			/* May transfer less than a segment. */
			if (bytesInBuffer > iop->io_ioc)
				bytesInBuffer = iop->io_ioc;
			iowrite(iop, bufPtr, bytesInBuffer);
			}
		}
	}

	return;

badFtRead:
	SET_U_ERROR(EIO, "ftread");
	return;
#endif
}

/************************************************************************
 * ftunload
 *
 ***********************************************************************/
static int
ftunload()
{
}

/************************************************************************
 * ftwrite
 *
 ***********************************************************************/
static int
ftwrite(dev, iop)
dev_t dev;
IO * iop;
{
	if (!ft.ft_open) {			/* exit if not open		*/
		u.u_error = EIO;
		return;
	}
}

/*********************
**********************
**
** Support routines.
**
**********************
**********************/

/************************************************************************
 * ftCmd
 *
 * Given a QIC-117 command number, send the command.
 * If report bits are expected in response, initialize the bit counter.
 * Then sleep until the commmand is done and report bits are gathered.
 ***********************************************************************/
static int
ftCmd(cmd)
int cmd;
{
	int	bitsNeeded;

	if (cmd != QIC_CMD_STS)
		printf("[%d]", cmd);

	/* Will sleep until command done and report bits are in. */
	ft.ft_wakeMeUp = 1;

	/*
	 * The following commands expect report bits from the tape drive.
	 * After receiving the QIC command (and subsequent delay),
	 * the drive sends a leading ACK bit (always 1).  This bit is
	 * not counted in the numbers below.
	 * Subseqeunt bits are sent in response to Report Next Bit,
	 * least significant bit first, then a trailing 1.
	 * The value of bitsNeeded is either one more than the number of
	 * data bits in the report, or zero.

	 */
	switch(cmd) {
	case QIC_CMD_STS:
	case QIC_CMD_DRVCN:
	case QIC_CMD_ROMVN:
	case QIC_CMD_TPSTAT:
		bitsNeeded = 9;
		break;
	case QIC_CMD_ECD:
	case QIC_CMD_VNDID:
		bitsNeeded = 17;
		break;
	default:
		bitsNeeded = 0;
	}
	ftRptBegin(bitsNeeded);
	ft.ft_ackMissed = 0;

	ftCmdSend(cmd);

	if (x_sleep(&ft.ft_wakeMeUp, pritape, slpriSigCatch, "ftCmd")) {
		if (FT_BLAB)
			printf("sig to cmd %d ", cmd);
		/* Signal woke us prematurely. */
		return -1;
	} else if (ft.ft_ackMissed) {
		if (FT_BLAB)
			printf(" Ack missed ");
		ft.ft_ackMissed = 0;
		return -1;
	} else
		return 0;
}

/************************************************************************
 * ftCmdArg
 *
 * Like ftCmd, except this routine sends command arguments.
 * No setup for report next bit.
 * Different debug print handling.
 ***********************************************************************/
static int
ftCmdArg(arg)
int arg;
{
	int	bitsNeeded;

	printf("(%d)", arg);

	/* Will sleep until stepper pulses sent. */
	ft.ft_wakeMeUp = 1;

	bitsNeeded = 0;
	ftRptBegin(bitsNeeded);
	ft.ft_ackMissed = 0;

	ftCmdSend(arg);

	if (x_sleep(&ft.ft_wakeMeUp, pritape, slpriSigCatch, "ftArg")) {

		/* Signal woke us prematurely. */
		printf("sig to arg %d ", arg);

		return -1;
	} else
		return 0;
}

/************************************************************************
 * ftCmdSend
 *
 * Given a QIC-117 command number, cause that number of step pulses
 * to be sent from the FDC by faking a seek command.
 ***********************************************************************/
static int
ftCmdSend(cmd)
int cmd;
{
	/* ncn = new cylinder #. */
	unsigned char ncn;
	
	/*
	 * Will fake a seek command.
	 * Figure out whether to simulate seek to lower or higher
	 * cylinder number.
	 */
	if (ft.ft_pcn >= cmd)
		ncn = ft.ft_pcn - cmd;
	else
		ncn = ft.ft_pcn + cmd;

	fdcSeek(ft.ft_drv, 0, ncn);
}

/************************************************************************
 * ftFdcPos()
 *
 * Given track relative segment within the tape track, and relative block
 * within the segment, store C/H/R (cylinder/head/record) diskette pseudo
 * coordinates into 3-element unchar array given.
 ***********************************************************************/
static void
ftFdcPos(track, segment, block, cmdTbl)
int track, segment;
unchar * cmdTbl;
{
}

/************************************************************************
 * ftGetBlkRun()
 *
 * Given the next block desired from the current segment, get starting
 * block number and number of blocks in next run of consecutive good
 * blocks.
 * This routine checks the bad block map for the cartridge, if this
 *   feature is enabled.
 * We are at a lower level than ecc, so all 32 blocks in a segment are
 *   considered.  Ecc, if enabled, will be applied to the results of
 *   run-level processing.
 * Return 0 if a run can be found, -1 if no further run of good blocks
 *   exists in the current segment.
 ***********************************************************************/
static int
ftGetBlkRun(ftrp)
struct FtRun * ftrp;
{
	int	retval;

	if (ft.ft_useBadMap) {
		printf("Bad block map not enabled yet.\n");
		/* Will modify blkToRead and runLength fields here. */
		retval = -1;
	} else {
		if (ftrp->blkToRead < FT_BLK_PER_SEG) {
			ftrp->runLength = FT_BLK_PER_SEG - ftrp->blkToRead;
			retval = -1;
		} else
			retval = 0;
	}
	return retval;
}

/************************************************************************
 * ftGetInfo
 *
 * Issue several QIC report commands - all but Report Drive Status
 * and Report Error Code.
 ***********************************************************************/
static void
ftGetInfo(dev)
dev_t dev;
{
	if (ftCmd(QIC_CMD_DRVCN))
		devmsg(dev, "Warning: get drive configuration failed");
	else
		devmsg(dev, "Drive Configuration = %x", ft.ft_report);

	if (ftCmd(QIC_CMD_ROMVN))
		devmsg(dev, "Warning: get ROM version failed");
	else
		devmsg(dev, "Rom Version = 0x%x", ft.ft_report);

	if (ftCmd(QIC_CMD_VNDID))
		devmsg(dev, "Warning: get vendor ID failed");
	else
		devmsg(dev, "Vendor ID = 0x%x", ft.ft_report);

	if (ftCmd(QIC_CMD_TPSTAT))
		devmsg(dev, "Warning: get tape status failed");
	else
		devmsg(dev, "Tape Status = 0x%x", ft.ft_report);
}

/************************************************************************
 * ftIrqHandler
 *
 * Interrupt handler.
 ***********************************************************************/
static void
ftIrqHandler()
{
	int i, bit;

	/*
	 * Need to get FDC status from result phase - fdcCmdStatus -
	 * or clear interrupt - fdcIntStatus - that may have been
	 * generated by diskette change or seek/recal complete.
	 */
	if (FDC_BUSY()) {
		fdcCmdStatus();
	} else {
		fdcIntStatus();
/* WARNING - should squawk if wrong number of status bytes. */
		ft.ft_pcn = fdc.fdc_intstat[1];
	}

	/* If ACK needed, try several times to get it. */
	if (ft.ft_ackNeeded) {
		for (i = 0; i < FT_ACK_TRIES; i++) {
			fdcDrvStatus(ft.ft_drv, FDC_HEAD_0);
			fdcCmdStatus();
			bit = (fdc.fdc_cmdstat[0] & ST3_T0) ? 1 : 0;
			if (FT_ACKBLAB)
				putchar(bit?'|':'o');
			if (bit) {	
				ft.ft_ackNeeded = 0;
				break;
			}
			/* Wait about 20 usec. then try again. */
			busyWait2(NULL, 20);
		}

		/*
		 * Tape Drive did not send ACK in response to QIC
		 * Report command.  Probably the command is not
		 * supported by this drive, or there is no drive
		 * present.  Set ackMissed flag and clean up.
		 */
		if (ft.ft_ackNeeded) {
			ft.ft_ackNeeded = 0;
			ft.ft_ackMissed = 1;
			ft.ft_bitsNeeded = 0;
			if (ft.ft_wakeMeUp) {
				ft.ft_wakeMeUp = 0;
				wakeup(&ft.ft_wakeMeUp);
			}
		}
	} else if (ft.ft_bitsNeeded) {

		/* Report bits are needed.  Get ST3. */
		fdcDrvStatus(ft.ft_drv, FDC_HEAD_0);
		fdcCmdStatus();

		/* Get next report bit by checking Track Zero bit in ST3 */
		if (fdc.fdc_ncmdstat == 1) {
			int bit;

			bit = (fdc.fdc_cmdstat[0] & ST3_T0) ? 1 : 0;
			ftRptUpdate(bit);
		} else {
			printf("rnb status bad ");
		}
	}

	/*
	 * If more report bits will be needed
	 *   Send request for next bit.
	 * Else
	 *   See if original requestor needs wakeup, etc.
	 */
	if (ft.ft_bitsNeeded) {
		ftCmdSend(QIC_CMD_RNB);
	} else {
		if (ft.ft_wakeMeUp) {
			ft.ft_wakeMeUp = 0;
			wakeup(&ft.ft_wakeMeUp);
		}

		/* Print debug output if needed. */
		if (ft.ft_dumpIrq) {
			ft.ft_dumpIrq = 0;
			defer(ftDbPrtStat);
		}
	}
}

/***********************************************************************
 * ftRead1Seg
 *
 * Given buffer number (0..FT_NBUF-1), try to read the next segment
 * into that segment buffer in ftBigBuf DMA area.
 * Set ftBufBlks[bufn] to the number of blocks not marked bad.
 * Valid blocks are deposited contiguously starting at the beginning of
 * segment buffer[bufn].
 *
 * Tape should be in properly positioned and in motion when this
 *   function is called.
 *
 * Initialize number of blocks read to 0, in local counter and in buffer
 *   table.
 * Initialize number of next tape block (relative to start of segment)
 *   to read to 0.
 * Loop
 *   Check bad block map (if using bad block map) to find the next run
 *     of readable blocks, if any, in the current segment.
 *   If there are no further runs of good blocks in the current segment
 *     Exit loop.
 *   Try to read the next run of blocks.
 *     If successful, this will:
 *       Increment number of blocks read by length of current run.
 *       Update the number of the next block to read.
 *     If too many read failures (> 3 if ecc, > 0 if no ecc)
 *       Return -1 (segment read failed).
 * Endloop
 *
 * Store number of blocks read into ftBufBlks table.
 * Update current segment and track numbers.  (Advance to next segment.)
 * Return 0 (success).
 ***********************************************************************/
static int
ftRead1Seg(bufn)
uint bufn;
{
	struct FtRun ftRun;
	struct FtParm	*fp;

	/* Get parameter set pointer. */
	fp = ftParm + ft.ft_fmt;

	ftBufBlks[bufn] = 0;
	ftRun.blksRead = 0;

	ftRun.blkToRead = 0;

	for (;;) {
		if (ftGetBlkRun(&ftRun))
			break;
		if (ftReadBlkRun(&ftRun, bufn))
			return -1;
	}

	ftBufBlks[bufn] = ftRun.blksRead;

	ft.ft_phySeg++;
	if (ft.ft_phySeg == fp->segPerTrk
	  && ft.ft_track <= (fp->tracks - 1)) {
		ft.ft_track++;
		ft.ft_phySeg = 0;
	}

	return 0;
}

/***********************************************************************
 * ftReadID
 *
 * Send FDC Read ID command and store results in ft context.
 * Return 0 on success, -1 on failure (signal).
 *
 * Of principal interest is fdc.fdc_cmdstat[5] - present block number.
 ***********************************************************************/
static int
ftReadID()
{
	ft.ft_wakeMeUp = 1;
	fdcReadID(ft.ft_drv, 0);
	if (x_sleep(&ft.ft_wakeMeUp, pritape, slpriSigCatch, "ftReadID"))
		return -1;
	else {
		if (fdc.fdc_ncmdstat != 7) {
			printf("ftReadID bad command status: %d",
			  fdc.fdc_ncmdstat);
			return -1;
		} else {
			ft.ft_cyl = fdc.fdc_cmdstat[3];
			ft.ft_head = fdc.fdc_cmdstat[4];
			ft.ft_blk = fdc.fdc_cmdstat[5];
			ft.ft_ssz = fdc.fdc_cmdstat[6];
		}
		return 0;
	}
}

/***********************************************************************
 * ftReadSegs()
 *
 * Given a segment count, try to read the given number of segments from
 * the curent track.
 * Return the number of segments read (0 if at end of tape), -1 if failure.
 *
 * Reading a segment may yield from 0 to 32 blocks, depending on how
 * many bad blocks are marked in the segment and whether ecc is used.
 ***********************************************************************/
static int
ftReadSegs(segsWanted)
uint segsWanted;
{
	int	segsOnTrack;
	int	segsRead;
	struct FtParm	*fp;

	/* Get parameter set pointer. */
	fp = ftParm + ft.ft_fmt;

	/*
	 * Calculate how many segments are left on the current track.
	 * If no segments left on current track
	 *   Return no segments read.
	 */
	segsOnTrack = fp->segPerTrk - ft.ft_phySeg;
	if (segsOnTrack == 0)
		return 0;

	/*
	 * Will do a multisegment read.
	 * Requested number of segments will be the least of:
	 * - number of segments wanted
	 * - number of segments there is room for in DMA area
	 * - number of segments remaining on current tape track
	 */
	if (segsWanted > FT_NBUF)
		segsWanted = FT_NBUF;
	if (segsWanted > segsOnTrack)
		segsWanted = segsOnTrack;

	/*
	 * Try to position and start tape.
	 * Read as many segments as possible.
	 * If there is an irrecoverable read error
	 *   Abort multisegment read.
	 * Stop tape.
	 */
	if (ftStartTape())
		return -1;
	for (segsRead = 0; segsRead < segsWanted; segsRead++) {
		if (ftRead1Seg(segsRead)) {
			segsRead = -1;
			break;
		}
	}
	ftStopTape();
	
	/* Return the number of segments successfully read. */
	return segsRead;
}

/***********************************************************************
 * ftReadyWait
 *
 * Keep checking drive status every second until drive is ready or
 * the specified number of seconds has elapsed.
 * Return 0 if drive ready, -1 if timeout or signal or protocol failure.
 ***********************************************************************/
static int
ftReadyWait(seconds)
uint seconds;
{
	uint i;
	int retval = -1;
	int drvStatus;

	for (i = 0; i < seconds; i++) {
		drvStatus = ftStsWthErr();

		/* Protocol failure? */
		if (drvStatus == -1)
			break;

		/* Drive ready?  Change return value to show success. */
		if (drvStatus & QIC_STAT_RDY) {
			retval = 0;
			break;
		}

		/* Signal arrived while waiting a second? */
		if (ftWait(HZ))
			break;
	}

	return retval;
}

/************************************************************************
 * ftRecal
 *
 * Send Recalibrate command to FDC and wait for it to finish.
 * Return 0 if normal operation, -1 if signaled before recal complete.
 ***********************************************************************/
static int
ftRecal()
{
	ft.ft_wakeMeUp = 1;
	fdcRecal(ft.ft_drv);
	if (x_sleep(&ft.ft_wakeMeUp, pritape, slpriSigCatch, "ftRecal"))
		return -1;
	else
		return 0;
}

/************************************************************************
 * ftResetFDC
 *
 * Reset the FDC and wait for the resulting interrupt.
 * Reset is done keeping the current unit selected.
 ***********************************************************************/
static void
ftResetFDC()
{
	/*
	 * Since FDC reset generates an interrupt, we need to tell the
	 * interrupt handler there will be *no* report bits incoming.
	 */
	ftRptBegin(0);

	ft.ft_pcn = 0;
	ft.ft_wakeMeUp = 1;
	fdcResetSel(FT_DRIVE(ft.ft_drv), FT_MOTOR(ft.ft_drv));
	x_sleep(&ft.ft_wakeMeUp, pritape, slpriSigCatch, "ftRstFDC");
}

/************************************************************************
 * ftRptBegin
 *
 * Initialize ft state information in preparation for QIC report command.
 *
 * Argument "bitCount" is the total number of bits expected, including
 * initial ACK and final TRUE.  It is 1 more than the number of
 * Report Next Bit Commands that will be issued.
 ***********************************************************************/
static void
ftRptBegin(bitCount)
int bitCount;
{
	ft.ft_bitsNeeded = bitCount;
	ft.ft_bitsRcvd = 0;
	ft.ft_report = 0;
	ft.ft_ackNeeded = (bitCount) ? 1 : 0;
}

/************************************************************************
 * ftSeg()
 *
 * Given pseudo cylinder, head, and block numbers from FDC,
 * calculate the segment number relative to start of current track.
 ***********************************************************************/
static	int
ftSeg(c, h, r)
int c, h, r;
{
	int	lsn;	/* Logical block (not segment) number. */
	int	seg;
	struct FtParm	*fp;

	/* Get parameter set pointer. */
	fp = ftParm + ft.ft_fmt;

	/* Compute logical block number. */
	lsn = (r - 1) + c * fp->blkPerTrk + h * fp->blkPerTrk * fp->cyls;

	/* Divide with truncation to get containing segment number. */
	seg = lsn / FT_BLK_PER_SEG;

	/* Compute segment number relative to start of track. */
	seg %= fp->segPerTrk;

	return seg;
}

/************************************************************************
 * ftRptUpdate
 *
 * Acquire another bit for QIC report command.
 * Last bit is discarded.  Other bits are accumulated,
 * least significant bit first, into ft_report.
 ***********************************************************************/
static void
ftRptUpdate(bit)
int bit;
{
	ft.ft_bitsNeeded--;
	ft.ft_bitsRcvd++;

	if (ft.ft_bitsNeeded == 0) {
		if (bit != 1)
			printf("Missing final TRUE ");
	} else {
		ft.ft_report |= (bit << (ft.ft_bitsRcvd - 1));
	}
}

/************************************************************************
 * ftSelect
 *
 * Select tape unit.
 ***********************************************************************/
static void
ftSelect()
{
	fdcRate(FDC_RATE_500K);
	fdcDrvSelect(FT_DRIVE(ft.ft_drv), FT_MOTOR(ft.ft_drv));

	/* Reset FDC and Initialize pseudo cylinder number for QIC commands. */
	ftResetFDC(ft.ft_drv);

	fdcSpecify(FT_SRT_2, FT_HUT, FT_HLT);

	if (FT_SOFT_SELECT(ft.ft_drv)) {
		ftCmd(77);
		ftWait(2);
		ftCmd(QIC_CMD_SS1);
		ftWait(2);
		ftCmd(QIC_CMD_SS2);
		ftWait(2);
	}

#if 0
	/* 80 MB nseg_p_track = 100, nseg_p_head = 600, nseg_p_cyl = 4	*/
	/* 40 MB nseg_p_track = 68, nseg_p_head = 680, nseg_p_cyl = 4	*/
	if (ftfmt) {
		nseg_p_track = 100;	/* set for 80 MB drive		*/
		nseg_p_head = 600;
		nseg_p_cyl = 4;
	} else {
		nseg_p_track = 68;	/* set for 40 MB drive		*/
		nseg_p_head = 680;
		nseg_p_cyl = 4;
	}
#endif
}

/************************************************************************
 * ftSkipBack()
 *
 * Skip segments in logical backward direction.
 * Return 0 on success, -1 on failure.
 ***********************************************************************/
static	int
ftSkipBack(segCount)
uint segCount;
{
	/* If possible, use short (non-extended) form of skip command. */
	if ((segCount & ~0xFF) == 0) {
		if (ftCmd(QIC_CMD_SKPB))
			return -1;
		ftCmdArg((segCount & 0xF) +2);
		ftCmdArg((segCount >> 4) +2);
	} else {
		if (ftCmd(QIC_CMD_SKPBX))
			return -1;
		ftCmdArg((segCount & 0xF) +2);
		ftCmdArg(((segCount >> 4) & 0xF) +2);
		ftCmdArg(((segCount >> 8) & 0xF) +2);
	}
}

/************************************************************************
 * ftSkipFwd()
 *
 * Skip segments in logical forward direction.
 * Return 0 on success, -1 on failure.
 ***********************************************************************/
static	int
ftSkipFwd(segCount)
int segCount;
{
	/* If possible, use short (non-extended) form of skip command. */
	if ((segCount & ~0xFF) == 0) {
		if (ftCmd(QIC_CMD_SKPF))
			return -1;
		ftCmdArg((segCount & 0xF) +2);
		ftCmdArg((segCount >> 4) +2);
	} else {
		if (ftCmd(QIC_CMD_SKPFX))
			return -1;
		ftCmdArg((segCount & 0xF) +2);
		ftCmdArg(((segCount >> 4) & 0xF) +2);
		ftCmdArg(((segCount >> 8) & 0xF) +2);
	}
}

/************************************************************************
 * ftStartTape
 *
 * Try to get tape into position to read next segment of interest, then
 * put tape in motion.
 *
 * Return 0 on success.
 * Return -1 on failure and make sure tape is stopped.
 *
 * Seek head to current track.
 * Wait for drive ready.
 * If trying to read first segment of any track
 *   If even track
 *     Rewind tape.
 *   Else (odd track)
 *     Full forward tape.
 *   Wait for drive ready.
 *   Logical forward.
 * Else (reading after first segment)
 *   Pause.  (Backs up a couple segments.)
 *   Wait for drive ready.
 *   While (not in position)
 *     Logical forward.
 *     Read segment ID.
 *     If current segment >= desired segment
 *       Compute skip count.
 *       Skip back some number of segments.
 *       Wait for drive ready.
 *     Else if current segment << desired segment
 *       Compute skip count.
 *       Skip forward some number of segments.
 *       Wait for drive ready.
 *     Else tape is in position.
 *   Endwhile
 ***********************************************************************/
static	int
ftStartTape()
{
	int	tries;
	int	notInPlace;
	unchar	currentSeg;
	int	skipCount;

	ftCmd(QIC_CMD_SEEK);
	ftCmdArg(2);
	if(ftReadyWait(FT_SEEK_SECS))
		return -1;

	if (ft.ft_phySeg == 0) {
		if ((ft.ft_track & 1) == 0)
			ftCmd(QIC_CMD_BOT);
		else
			ftCmd(QIC_CMD_EOT);
		if(ftReadyWait(FT_WIND_SECS))
			return -1;
		ftCmd(QIC_CMD_FWD);
	} else {
		ftCmd(QIC_CMD_PAUS);
		if(ftReadyWait(FT_PAUSE_SECS))
			return -1;

		notInPlace = 1;
		for (tries = 0; tries < 4 && notInPlace; tries++) {
			ftCmd(QIC_CMD_FWD);
			if (ftReadID())	
				continue;	/* Will try again. */
			currentSeg = ftSeg(fdc.fdc_cmdstat[3],
			  fdc.fdc_cmdstat[4], fdc.fdc_cmdstat[5]);
			if (currentSeg >= ft.ft_phySeg) {
				skipCount = currentSeg - ft.ft_phySeg
				  + FT_CUSHION;
				ftSkipBack(skipCount);
			} else if (currentSeg < ft.ft_phySeg - FT_CUSHION) {
				skipCount = ft.ft_phySeg - currentSeg
				  - FT_CUSHION;
				ftSkipFwd(skipCount);
			} else
				notInPlace = 0;
		} /* end for */
	}

	if (notInPlace) {
		ftStopTape();
		return -1;
	}

	return 0;
}

/************************************************************************
 * ftStopTape
 *
 * Send QIC stop tape command, then sleep til drive ready.
 * Return 0 if success, 1 if timed out before sensing drive ready.
 ***********************************************************************/
static	int
ftStopTape()
{
	/* Stop, wait for ready. */
	ftCmd(QIC_CMD_STOP);
	return ftReadyWait(15);
}

/************************************************************************
 * ftStsWthErr
 *
 * Do Report Drive Status.
 * If error detected, to Report Error Code.
 * Uses sleep, so may not be used in timeout/interrupt/load/unload, etc.
 * Return status if ok, -1 on protocol failure.
 ***********************************************************************/
static int
ftStsWthErr()
{
	int retval;

	if (ftCmd(QIC_CMD_STS))
		return -1;

	retval = ft.ft_report;

	if (FT_BLAB)
		ftDbPrintStat(ft.ft_report);

	if (ft.ft_report & QIC_STAT_ERR) {
		if (ftCmd(QIC_CMD_ECD))
			return -1;
		ftDbPrintErr(ft.ft_report);
	}
	return retval;
}

/************************************************************************
 * ftWait
 *
 * Wait the specified number of ticks.
 * Return 0 if full wait occurs, -1 if signaled.
 * Uses sleep, so may not be used in timeout/interrupt/load/unload, etc.
 ***********************************************************************/
static int
ftWait(ticks)
uint ticks;
{
	timeout(&ft.ft_tim, ticks, wakeup, &ft.ft_wakeMeUp);
	return x_sleep(&ft.ft_wakeMeUp, pritape, slpriSigCatch, "ftWait");
}

/*********************
**********************
**
** Debug Area.
**
**********************
**********************/

static	char	*qicErr[] = {
	"NULL err",
	"command received while drive not ready",
	"cartridge not present or removed",
	"motor speed error (not within 1%)",
	"motor speed fault (jammed, or gross speed error)",
	"cartridge write protected",
	"undefined or reserved command code",
	"illegal track address specified for seek",
	"illegal command in report subcontext",
	"illegal entry into a diagnostic mode",
	"broken tape detected (based on hole sensor)",
	"warning - read gain setting error",
	"command received while error status pending (obsolete)",
	"command received while new cartridge pending",
	"command illegal or undefined in primary mode",
	"command illegal or undefined in format mode",
	"command illegal or undefined in verify mode",
	"logical forward not at logical BOT in format mode",
	"logical EOT before all segments generated",
	"command illegal when cartridge not referenced",
	"self-diagnostic failed (cannot be cleared)",
	"warning EEPROM not initialized, defaults set",
	"EEPROM corrupt or hardware failure",
	"motion timeout error",
	"data segment too long - logical forward or pause",
	"transmit overrun (obsolete)",
	"power on reset occurred",
	"software reset occurred",
	"diagnostic mode 1 error",
	"diagnostic mode 2 error",
	"command received during noninterruptible process",
	"rate selection error",
	"illegal command while in high speed mode",
	"illegal seek segment value"
};

static	char	*qicStat[] = {
	"drive ready or idle",
	"error detected",
	"cartridge present",
	"cartridge write protected",
	"new cartridge",
	"cartridge referenced",
	"at physical BOT",
	"at physical EOT"
};

static	char	*qicCmd[] = {
	"NULL cmd",
	"soft reset",
	"report next bit",
	"pause",
	"micro step pause",
	"alternate command timeout",
	"report drive status",
	"report error code",
	"report drive configuration",
	"report ROM version",
	"logical forward",
	"physical reverse",
	"physical forward",
	"seek head to track",
	"seek load point",
	"enter format mode",
	"write reference burst",
	"enter verify mode",
	"stop tape",
	"reserved (19)",
	"reserved (20)",
	"micro step head up",
	"micro step head down",
	"reserved (23)",
	"reserved (24)",
	"skip n segments reverse",
	"skip n segments forward",
	"select rate",
	"enter diag mode 1",
	"enter diag mode 2",
	"enter primary mode",
	"reserved (31)",
	"report vendor ID",
	"report tape status",
	"skip n segments extended reverse",
	"skip n segments extended forward"
};

/* print command as [command] */
static void
ftDbPrintCmd(cmd)
unsigned int cmd;
{
	if (cmd >= 1 && cmd < sizeof(qicCmd)/sizeof(qicCmd[0])) {
		if (cmd != QIC_CMD_RNB)
			printf("[%s] ", qicCmd[cmd]);
	} else
		printf("[%x] ", cmd);
}

/* print 2-byte error status as <error-code,command> */
static void
ftDbPrintErr(errword)
unsigned int errword;
{
	unsigned int lo, hi;

	lo = errword & 0xff;
	hi = (errword >> 8) & 0xff;

	if (lo >= 1 && lo < sizeof(qicErr)/sizeof(qicErr[0]))
		printf("<%s,", qicErr[lo]);
	else
		printf("<%x,", lo);

	if (hi >= 1 && hi < sizeof(qicCmd)/sizeof(qicCmd[0]))
		printf("%s> ", qicCmd[hi]);
	else
		printf("%x> ", hi);
}

/* print current FDC position as C= H= R= N=. */
static void
ftDbPrintPos()
{
	printf("C=%d H=%d R=%d N=%d ",
	  ft.ft_cyl, ft.ft_head, ft.ft_blk, ft.ft_ssz);
}

/* print tape status as { status string,... } */
static void
ftDbPrintStat(stat)
unsigned int stat;
{
	int i;

	printf("{ ");
	for (i = 0; i < 8; i++) {
		if (stat & (1 << i))
			printf("%s, ", qicStat[i]);
	}
	putchar('}');
}

/************************************************************************
 * ftDbPrtStat
 *
 * For debugging, print command status and interrupt status to console.
 ***********************************************************************/
static void
ftDbPrtStat()
{
	int i;

	printf("[[");
	if (fdc.fdc_ncmdstat) {
		printf("cmd ");
		for (i = 0; i < fdc.fdc_ncmdstat; i++)
			printf("%x ", fdc.fdc_cmdstat[i]);
	}
	if (fdc.fdc_nintstat) {
		printf("int ");
		for (i = 0; i < fdc.fdc_nintstat; i++)
			printf("%x ", fdc.fdc_intstat[i]);
	}
	printf("]] ");
}

#if defined(FT_DEAD_CODE)
/*********************
**********************
**
** Test Read of Segment 0.
**
**********************
**********************/
	FT_BLAB = 0;

#if 0
	/* Rewind, wait until ready. */
	ftCmd(QIC_CMD_BOT);
	if(ftReadyWait(15)) {
		printf("Not ready after rewind ");
		goto rd0done;
	}

	/* Stop, wait for ready. */
	ftCmd(QIC_CMD_STOP);
	if(ftReadyWait(15)) {
		printf("Not ready after stop tape 1 ");
		goto rd0done;
	}
#endif

	/* Seek head to track 0, wait for ready. */
	ftCmd(QIC_CMD_SEEK);
	ftCmdArg(2);
	if(ftReadyWait(15)) {
		printf("Not ready after seek to track 0 ");
		goto rd0done;
	}

#if 0
	/* Stop, wait for ready. */
	ftCmd(QIC_CMD_STOP);
	if(ftReadyWait(15)) {
		printf("Not ready after stop tape 2 ");
		goto rd0done;
	}

	/* Skip back 68 segments. SKPB. Wait for ready. */
	ftCmd(QIC_CMD_SKPB);
	ftCmdArg(6);
	ftCmdArg(6);
	if(ftReadyWait(15)) {
		printf("Not ready after skip back 68 ");
		goto rd0done;
	}
#endif

	/* Logical forward. */
	ftCmd(QIC_CMD_FWD);

for (i = 0; i < 3; i++) {
	/* Setup DMA */
	if (!dmaon(DMA_CH2, vtop(ftBigBuf)+(i*FT_BFRSZ), FT_BFRSZ, DMA_TO_MEM)) {
		printf("Ft dma straddle ");
		goto rd0done;
	}
	dmago(DMA_CH2);

	/* Send Read command to FDC */
	ft.ft_wakeMeUp = 1;
	ft.ft_dumpIrq = 1;
	rwCmd[4] = (i*32)+1;
	if ((putCount = fdcPutStr(rwCmd, RD_CMD_LEN)) != RD_CMD_LEN) {
		printf("only wrote %d bytes of rwCmd ", putCount);
		goto rd0done;
	}

	/* Wait for interrupt when read is done. */
	if (x_sleep(&ft.ft_wakeMeUp, pritape, slpriSigCatch, "ftRd0")) {
		printf("signaled before DMA done ");
		goto rd0done;
	}

	/* Turn off the DMA channel. */
	if ((putCount = dmaoff(DMA_CH2)) != 0) {
		printf("DMA residual %d bytes ", putCount);
		goto rd0done;
	}
}

	/* Stop the tape. */
	ftCmd(QIC_CMD_STOP);
	if(ftReadyWait(15)) {
		printf("not ready after stop tape ");
		goto rd0done;
	}

	/* Quick check on data received. */
	for (i = 0; i < 4; i++)
		printf("%d:%x ", i, ftBigBuf[i]);

rd0done:
	FT_BLAB = 0;
/*********************
**********************
**
** End of Read Test.
**
**********************
**********************/
/* old stuff from read routine */
	uint src, dest;
	uint startSeg, endSeg;

	src = iop->io_seek;
	dest = iop->io.pbase;

#if 0
	/*
	 * Seek 0-32767 is header segment.
	 */
	startSeg = src >> 15;
	endSeg = (src + iop->io_ioc - 1) >> 15;

	if (startSeg != endSeg || startSeg > 0) {
		u.u_error = EFAULT;
		return;
	}
#endif

	src += (uint)ftBigBuf;

	iowrite(iop, src, iop->io_ioc);

	return;
#endif
#if 0
/************************************************************************
 * ftBufRead
 *
 * Given a read count in bytes, read as much as possible into the DMA
 * buffer area.
 * Return the number of bytes retrieved, or -1 if total failure.
 ***********************************************************************/
static int
ftBufRead(bytesWanted)
int bytesWanted;
{
	int	bytesToRead;
	uint	bigBufLen = FT_NBUF * FT_BFRSZ;
	int	blocksWanted, blocksRead;
	unchar	* dmaBufPtr = ftBigBuf;

	/* Try for the smaller of amount wanted and buffer size. */
	bytesToRead = (bytesWanted > bigBufLen) ? bigBufLen : bytesWanted;

	/* WARNING: The following will change when we read the bad block map! */

	/* Calculate the number of 1K byte tape blocks to read. */
	blocksWanted = (bytesToRead + FT_BLKSZ - 1) / FT_BLKSZ;

	/* Keep asking for blocks until we have them all. */
	while (blocksWanted > 0) {
		blocksRead = ftReadBlks(dmaBufPtr, blocksWanted);
		if (blocksRead < 0)	/* Failure! */
			break;
		dmaBufPtr += FT_BLKSZ;
		blocksWanted -= blocksRead;
		bytesToRead -= blocksRead * FT_BLKSZ;
		if (bytesToRead < 0)
			bytesToRead = 0;
	}

	/* Return the number of bytes read into the DMA buffer. */
	return bytesWanted - bytesToRead;
}

/***********************************************************************
 * ftReadBlkRun()
 *
 * Given the location of the next run of good blocks to read from tape
 * and the index where to put it, try to read a run of blocks into
 * DMA buffer area.
 *
 * Requirement: tape must be positioned properly and in motion.
 *
 * Return 0 on success (the number of failed block reads does not exceed
 * the maximum allowable), or -1 on failure.
 * On success, update blkToRead and blksRead fields of the FtRun struct
 * passed in.  Also update error count and location tables for the current
 * segment buffer.
 *
 * Initialize retry count.
 * Initialize retry starting block.
 * While length of current run is > 0 blocks
 *   Make a command to request the run of blocks from the FDC.
 *   Send the command.
 *   When FDC done, get report phase info from FDC.
 *   Update blkToRead, blksRead, and runLength based on FDC report.
 *   Conditionally update retry count and retry starting block.
 *   If FDC reports error
 *     If retry limit not exceeded
 *       Increment retry count.
 *       Reposition with micro step.
 *     Else
 *       If allowable error limit for this segment exceeded
 *         Return read failure.
 *       Else
 *         Save relative block number of failed block for the segment.
 *         Update bad read count for the segment.
 *         Advance blkToRead, blksRead, and runLength to point past
 *           current bad block (may decrement runLength to zero).
 * Endwhile
 * 
	uint	blkToRead;	/* Next block wanted from current segment */
	uint	blksRead;	/* Number of blocks read from this seg    */
	uint	runLength;	/* Number of good blocks in current run   */
 ***********************************************************************/
static int
ftReadBlkRun(ftrp, bufn)
struct FtRun * ftrp;
int bufn;
{

#if 0
	int		putCount;
	struct FtParm	*fp;

	/* Get parameter set pointer. */
	fp = ftParm + ft.ft_fmt;

	/* Sanity check - don't try to read past end of track. */
	if (ft.ft_phySeg >= fp->segPerTrk)
		return -1;

	/* Set up DMA */
	if (!dmaon(DMA_CH2, vtop(ftBigBuf + (bufn * FT_BFRSZ)), FT_BFRSZ,
	  DMA_TO_MEM)) {
		printf("Ft dma straddle ");
		return -1;
	}
	dmago(DMA_CH2);

	/*
	 * Make a read command.
	 * Enter diskette pseudo coordinates into FDC read command.
	 * C = rwCmd[2], H = rwCmd[3], R = rwCmd[4]
	 */
	rwCmd[RW_CMD_VERB] = RW_CMD_RD;
	rwCmd[RW_CMD_UNIT] = ft.ft_drv;
	ftFdcPos(ft.ft_track, ft.ft_phySeg, rwCmd + RW_CMD_CYL);
	rwCmd[RW_CMD_SCHI] = fp->blkPerTrk;

	/* Send Read command to FDC */
	ft.ft_wakeMeUp = 1;
	ft.ft_dumpIrq = 1;
	if ((putCount = fdcPutStr(rwCmd, RW_CMD_LEN)) != RW_CMD_LEN) {
		printf("only wrote %d bytes of rwCmd ", putCount);
		return -1;
	}

	/* Wait for interrupt when read is done. */
	if (x_sleep(&ft.ft_wakeMeUp, pritape, slpriSigCatch, "ftRd1S")) {
		printf("signaled before DMA done ");
		return -1;
	}

	/* Turn off the DMA channel. */
	if ((putCount = dmaoff(DMA_CH2)) != 0) {
		printf("DMA residual %d bytes ", putCount);
		return -1;
	}

	/* Update current segment pointer. */
	ft.ft_phySeg++;
	if (ft.ft_phySeg == fp->segPerTrk && ft.ft_track <= (fp->tracks - 1)) {
		ft.ft_track++;
		ft.ft_phySeg = 0;
	}

	return 0;
#endif
}

/***********************************************************************
 * ftReadBlks()
 *
 * Given a pointer into the DMA buffer area and a block count, try to
 * read the given number of blocks.  May read fewer due to error or
 * reaching end of track.
 * Return the number of blocks read, or -1 in case of total failure.
 ***********************************************************************/
static int
ftReadBlks(dmaBufPtr, blocksWanted);
unchar * dmaBufPtr;
uint blocksWanted;
{
	int segsWanted, segsOnTrack;

	/*
	 * See how many segments are left on the current track.
	 * If no segments left on current track
	 *   If on last track of cartridge
	 *     Return failure.
	 *   Increment current track number.
	 *   Reset current segment number.
	 */
	segsOnTrack = ft.ft_segsPerTrk - ft.ft_phySeg;
	if (segsOnTrack == 0) {
		if (ft.ft_track + 1 >= ft.ft_trksPerCart)
			return -1;
		ft.ft_track++;
		ft.ft_phySeg = 0;
	}

	/* WARNING:  This will change with Reed-Solomon ecc. */

	/*
	 * Calculate how many segments to go after.
	 * First, round up the number of requested blocks.
	 * Take the smaller of these.
	 */
	segsWanted = (blocksWanted + FT_BLK_PER_SEG - 1) / FT_BLK_PER_SEG;
	if (segsWanted > segsOnTrk)
		segsWanted = segsOnTrk;

	/* Now ask for a bunch of segments, all from the same track. */
	return ftReadSegs(dmaBufPtr, segsWanted);
}

#endif
