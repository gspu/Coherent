#define MWC_FT 1

/*
 * File:	xlft.c
 *
 * Purpose:	Floppy tape device control.
 *		Inspired by the Archive "xl" driver.
 *		Requires 765 controller module fdc.c
 *		FDC = floppy disk controller, e.g. NEC upd765
 *
 * Revised: Tue Jun  1 19:16:37 1993 CDT
 */

/*
 * ----------------------------------------------------------------------
 * Includes.
 */
#include	<sys/coherent.h>

#include	<sys/errno.h>
#include	<sys/buf.h>
#include	<sys/con.h>
#include	<sys/devices.h>
#include	<sys/fdc765.h>
#include	<sys/fdioctl.h>
#include	<sys/file.h>
#include	<sys/sched.h>
#include	<sys/stat.h>
#include	<sys/xl.h>
#include	<sys/xlfdc.h>
#include	<sys/xlft.h>
#include	<sys/xl8237.h>

/*
 * ----------------------------------------------------------------------
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */
#define	HDRSZ	0x4000		/* 16k, size of qic-40 header		*/
#define	BFRSZ	0x8000		/* 32k, size of qic-40 buffer		*/

#define MAX_PCN		200

/* compatibility area */

#define bcopy(src, dest, nbytes)	memcpy(dest, src, nbytes)
#define copyin(userSrc, drvrDest, nbytes) \
	ukcopy(userSrc, drvrDest, nbytes)
#define copyout(drvrSrc, userDest, nbytes) \
	kucopy(drvrSrc, userDest, nbytes)
#define getfdc(unit)			setFtIntr(1)
#define getfdcn(unit)			setFtIntr(1)
#define kvtophys(vaddr)			vtop(vaddr)
#define relfdc()			setFtIntr(0)
#define sleep(address, priority)	\
	x_sleep(address, pritape, slpriSigLjmp, #address)
#define spl5()				sphi()
#define splx(lvl)			spl(lvl)
#define	tenmicrosec()			busyWait2(NULL, 12)

static	void	cmn_err();

enum {
	CE_CONT	= 0,		/* used to continue a previous message */
	CE_NOTE,		/* used to display a NOTICE: message */
	CE_WARN,		/* used to display a WARNING: message */
	CE_PANIC,		/* used to display a PANIC: message */
	CE_INVALID		/* used to warn about invalid severity */
};

/************************************************************************/
/*	Additional fdc equates						*/
/************************************************************************/

#define	FDTMO	65535		/* xlfdc_in_byte, xlfdc_out_byte timeout*/
#define	IOTMO	(12 * HZ)	/* io timeout				*/
#define	TMSKP	18		/* # segments missed if IOTMO		*/

/*
 * ----------------------------------------------------------------------
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */
extern int	nulldev();

extern int	xl_tbi();	/* init ecc tables			*/
extern int	xl_enc();	/* encode ecc data			*/
extern int	xl_dec();	/* decode ecc data			*/

/* CON entry points. */
static int	ftblock();
static int	ftclose();
static int	ftioctl();
static int	ftload();
static int	ftopen();
static int	ftread();
static int	ftunload();
static int	ftwrite();

static	int	getmem();
static	int	putrb();
static	struct	rb *getrb();
static	void	stdma();
static	void	strbfm();		/* set up rb for format		*/
static  void	strbsg();
static  void	stseg();
static  void	wvtbl();

static	void	xlcal(), xlcal0(), (*pfcal)();	/* calibrate		*/
static	void	xlcomplete();
static	void	xldelay(), (*pfdly)();	/* "delay"			*/
static	void	xldma();
static	unchar	xlfdc_in_byte();
static	unchar	xlfdc_out_byte();
static	unchar	xlfdc_out_str();
static	void	xlfdc_reset();
static	void	xlflush();
static	void	xlfmq(), xlfmq0(), xlfmq1(), xlfmq2(); /* format queue	*/
static	void	xlfms(), xlfm0(), (*pffms)();	/* format segment	*/
static	int	xlformat();		/* format main routine		*/
static	void	xlgtdat();
static	void	xlhalt(), (*pfhlt)();	/* halt tape motion		*/
static	void	xlintr(), (*pfint)();	/* xl2 ptr to intr handler	*/
static	void	xlnull();
static	int	xlopn();
static	void	xloutput_step();	/* output steps, exit=pfint	*/
static	void	xlpark(), (*pfprk)();
static	void	xlpark0(), xlpark1(), xlpark2();
static	void	xlpos(), xlpos0(), xlpos1(), xlpos2(), xlpos3(), (*pfpos)();
static	void	xlptdat();
static	void	xlque(), xlqr0(), xlqw0();
static	void	xlrds(), xlrd0(), xlrd1(), (*pfrds)();	/* read segment	*/
static	void	xlreadid(), xlreadid0(), (*pfrdi)();	/* read id	*/
static	void	xlready(), xlready0(), (*pfrdy)(); /* "wait" for ready	*/
static	void	xlrel();
static	void	xlreset(), xlreset0(), (*pfrst)();	/* reset drive	*/
static	void	xlrn9(), xlrn17();
static	void	xlrnb(), xlrnb0(), (*pfrnb)();	/* report next bits	*/
static	void	xlrqr();
static	void	xlseek(), (*pfsek)();	/* seek to tptrk		*/
static	void	xlseek0(), xlseek1();
static	void	xlsel();
static	void	xlskipb(), (*pfskp)();	/* skip n segs back		*/
static	void	xlskipb0(), xlskipb1(), xlskipb2();
static	void	xlstatus(), (*pfsts)();	/* get status			*/
static	void	xlstatus0(), xlstatus1();
static	void	xltimout(), xltimfn();
static	int	xlwait();
static	void	xlwds(), (*pfwds)();	/* write del adm segment	*/
static	void	xlwts(), (*pfwts)();	/* write segment		*/
static	void	xlwt0(), xlwt1();

static	void	xlDbPrintErr(), xlDbPrintCmd(), xlDbPrintStat();

static	void	ftDbPrtStat();
static	int	ftCmd();
static	void	ftIrqHandler();
static	int	ftRecal();
static	void	ftSelect();

/*
 * ----------------------------------------------------------------------
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */

/* from assembler module */
extern	ushort	*xltbl;		/* ptr to rb.tbl			*/
extern	unchar	xlbst;		/* base sector for segment		*/
extern	unchar	xlsct;		/* sector # for next chunk		*/
extern	int	xlcnt;		/* # bytes to xfer next chunk		*/
extern	int	xlofs;		/* offset for next chunk		*/

CON	ftxlcon = {
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
 * Patchable variables.
 */
int	XL_VERBOSE = 1;
int	XL_NBUFS = 17;		/* ~ 3K total 'DATA' for driver		*/
				/* 1.25K for ecc, + XL_NBUFS * 94	*/

/* Parameters for FDC Specify Command */
int FT_SRT = 0xE;
int FT_HUT = 0xF;
int FT_HLT = 0x1;

/************************************************************************/
/*	UNIX data areas	 and externals					*/
/************************************************************************/
static	ulong	allocated_add;	/* mem allocate addr for kernel buffer	*/

/************************************************************************/
/*	data area							*/
/************************************************************************/
static	int	xldataw;		/* data queue wakeup flag	*/
static	int	xlcompw;		/* completion wakeup flag	*/
static	int	xlfreew;		/* free queue wakeup flag	*/
static	int	xlcompf;		/* completion flag		*/
static	int	xlnactw;		/* inactive wakeup flag		*/
static	int	xlskip_count;		/* # segments to skip		*/

static	fplng	ptr_header;		/* ptr to header bfr		*/
static	fplng	ptr_buffer;		/* ptr to bfr			*/

static	struct	rb  * rbmp;		/* request packet pool		*/

static	struct	rbq xlfree_q;	 	/* free queue			*/
static	struct	rbq req;		/* request queue		*/
static	struct	rbq data_queue;	 	/* read data queue		*/

static	int	xlmem_allocated = 0;	/* set if memory allocated	*/

static	struct	{ 			/* flags			*/
	unsigned short init :1;
	unsigned short open :1;
	unsigned short writ :1;
	unsigned short cal  :1;
	unsigned short actv :1;
	unsigned short tmov :1;
	unsigned short werr :1;
}f;

static	int	nseg_p_track;		/* # segments/track		*/
static	int	nseg_p_head;		/* # segments/head		*/
static	int	nseg_p_cyl;		/* # segments/cylinder		*/

static	int	fdcmd;			/* last fdc cmd			*/

static	unchar	status_buf[ 10 ]; 	/* status buffer		*/
					/* bytes 0-6 = 8272 status	*/
					/*	7   = nec phase err	*/
					/*	8   = 8272 st3		*/

static	ushort	xlrnbw;			/* xl2 report next bit word	*/
static	unchar	xlster;			/* set if last bit not a 1	*/
static	unchar	xl6sts;			/* xl2 status			*/
static	unchar	xlests;
static	ushort	xl7sts;
static	int	xlbcnt;			/* xlrnb counter, # of sts bits	*/

static	unchar	fdstb[ 4 ] = { 0x1c, 0x2d, 0x4e, 0x8f };
static	unchar	fdsel, unit, fdselr, ftfmt;

static	ushort	sgwrd;			/* strbsg params		*/
static	ulong	sgmap;

static	paddr_t	xadr;			/* segment io params		*/
static	struct	rb *xprb;
static	ushort	xtbl;
static	int	xsct;
static	int	xrty;
static	int	xstop;			/* stop io flag (close)		*/

static	int	tptrk;			/* current tape track		*/
static	int	tpseg;			/* next tape segment		*/
static	int	fmtrk;			/* format track			*/
static	int	vftrk;			/* verify track (debug display)	*/

static	int	xltimoutf = 0;		/* timeout flag			*/

/*	fdc commands							*/
static	unchar	sf2cms[ 3 ] = { 0x03, 0xef, 0x02 };

static	unchar	sf3cms[ 3 ] = { 0x03, 0xdf, 0x02 };
static	unchar	sekcms[ 3 ] = { 0x0f, 0x00, 0x00 };
static	unchar	rdicms[ 2 ] = { 0x4a, 0x00 };
static	unchar	rwdcms[ 9 ] = { 0x46, 0, 0, 0, 0, 3, 226, 1, 0xff };
static	unchar	fmtcms[ 6 ] = { 0x4d, 0, 3, 32, 233, 0x6d };
/*		sekcms[ 2 ] = current track, see xlintr()		*/

static	unchar	vtbl[  ] = "VTBL";
static	unchar	xnxnm[  ] = "unix";
static	unchar	xnxvs[ 20 ] = {
				0x55, 0xaa, 0x55, 0xaa, 0x02, 0x00, 0x00, 0x00,
				0x01, 0x00, 0x02, 0x00, 0x4f, 0x05, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00
			};

static	int	h0sgn, h1sgn;		/* header seg #			*/
static	int	volume_seg_num;		/* volume seg #			*/
static	int	data_seg_num;		/* data seg #			*/
static	int	curr_seg_num;		/* current seg #		*/
static	int	read_ahead_seg_num;	/* read ahead seg #		*/
static	struct	rb *cprb; 		/* offset to current rb		*/
static	fpchr	cptr;			/* ptr to current bfr		*/
static	int	cnbr;			/* # bytes remaining		*/
static	fpchr	wptr;			/* copy of cptr for write encod	*/
static	int	last_seg_num;		/* last segment			*/
static	int	lnbr;			/* last seg # bytes		*/

static	int	rnbr;			/* # requested bytes remaining	*/
static	int	rcnt;			/* # bytes to copy		*/

static	struct	rb *fprb;	 	/* used to enque requests	*/

static	TIM	xltmo, xldly;

static struct	FT {
	unsigned char ft_pcn;		/* present cylinder #		*/
	unsigned char ft_bitsNeeded;	/* # of Report Next Bit's to do	*/
	unsigned char ft_wakeMeUp;	/* 1 = sleeping til next FDC IRQ */
	unsigned char ft_dumpIrq;	/* 1 = dump IRQ status		*/
	unsigned char ft_senseDrvStat;	/* 1 = need to sense drv status	*/
} ft;

/*
 * ----------------------------------------------------------------------
 * Code.
 */

/*
 * CON struct routines.
 */

/************************************************************************/
/*	ftblock								*/
/*									*/
/* Tape is not a block device, but we need a block entry point since	*/
/* the same driver controls diskette access.				*/
/************************************************************************/
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
#if MWC_FT
	printf("ftclose ");
	f.open = 0;
#else
	if (f.open) {
		/* f.open = 0;		reset open flag	 U001 		*/
		xlflush();		/* flush all bfrs		*/
		if (f.werr)		/* flag error if occurred	*/
			u.u_error = EIO;

		getfdc(unit);	
		xlsel(unit);
		if (minor(dev) & M_REW) { /* rewind if rewind_on_close  */
			pfprk = xlcomplete;/* clear drive status and rewind*/
			xlpark();
			xlwait();
		} else {
			pfsts = xlcomplete;	/* clear drive status 	*/
			xlstatus();
			xlwait();
		}
		xlrel();
		relfdc();
		f.open = 0;		/* reset open flag 	U001	*/
		cmn_err(CE_CONT, "xlclose: xlster %x  f.werr %d\n", xlster, f.werr);
	}
#endif
}

/************************************************************************/
/*	ftioctl								*/
/*	Handle tape drive & controller commands like erase, rewind,	*/
/*	retension, read filemark, and write filemark			*/
/************************************************************************/
ftioctl(dev, cmd, arg)
register int	dev, cmd;
int		arg;
{
	int			xlarg;	/* ioctl argument		*/
	union	xl_status	xl;	/* status structure		*/

	cmn_err(CE_CONT, "ftioctl: cmd %x\n", cmd);

	if ((cmd & 0xff00) != XLIOC) {
		u.u_error = EINVAL;
		return;
	}

	getfdc(unit);			/* get control of fdc		*/
	xlsel(unit);			/* select tape drive		*/

	switch (cmd) {
	case XL_DEBUG:
		if  (copyin(arg, (caddr_t)&XL_VERBOSE, sizeof(int)) == -1)
			u.u_error = EFAULT;
		break;
	case XL_STATUS: 		/* read status			*/
		/*
		* Report current status, then clear
		* any errors by reading the status again.
		*/
		xl.stat[ 0 ] = xl6sts;
		xl.stat[ 1 ] = xl7sts;
		xl.stat[ 2 ] = xl7sts >> 8;

		if (copyout(xl.stat, arg, sizeof(xl.stat)) == -1)
			u.u_error = EFAULT;
		else {
			xlster = 0;
			xlests = 0;
			xl7sts = 0;
			pfsts = xlcomplete;
			xlstatus();
			xlwait();
			if (f.werr)	/* flag error if occurred	*/
				u.u_error = EIO;
			break;

		case XL_RESET:		/* reset drive			*/
			pfrst = xlcomplete;
			xlreset();
			xlwait();
			if (f.werr)	/* flag error if occurred	*/
				u.u_error = EIO;
			break;

		case XL_RETEN : 	/* retention tape		*/
			pfint = xlready;
			pfrdy = xlcomplete;
			xloutput_step(QIC_CMD_EOT);
			xlwait();

			pfint = xlready;
			pfrdy = xlcomplete;
			xloutput_step(QIC_CMD_BOT);
			xlwait();
			if (xlster)
				u.u_error = ENXIO;

			break;

		case XL_REWIND: 	/* rewind			*/
			pfint = xlready;
			pfrdy = xlcomplete;
			xloutput_step(QIC_CMD_BOT);
			xlwait();
			if (xlster)
				u.u_error = ENXIO;
			break;

		case XL_ERASE:		/* erase tape			*/
			break;

		case XL_AMOUNT: 	/* report amount of data xfered	*/
			/*if (copyout(&ct_amount, arg, sizeof(ct_amount)) == -1)
			u.u_error = EFAULT;*/
			break;

		case XL_FORMAT: 	/* format tape			*/
			if  (copyin(arg, (caddr_t) &xlarg, sizeof(int)) == -1)
				u.u_error = EFAULT;

			else if ((xlarg >= 0 && xlarg <= 14 && ftfmt == 0) ||
			         (xlarg >= 0 && xlarg <= 28 && ftfmt == 1)) {
				xlformat(xlarg);
				if (f.werr)	/* flag err if occurred	*/
					u.u_error = EIO;
			} else		/* invalid track number		*/
				u.u_error = EINVAL;
		}
		break;
	case XL_RFM:			/* read file mark		*/
		;			/* not implemented		*/

	default:
		u.u_error = EINVAL;
		break;
	}
	xlrel();			/* release fdc			*/
	relfdc();
	cmn_err(CE_CONT, "xlioctl: returning\n");
}

/************************************************************************/
/*	ftload								*/
/************************************************************************/
static int
ftload()
{
	register int	eflag;
	register int	s, t;

#if MWC_FT
	ftIntr = ftIrqHandler;
#else
	ftIntr = xlintr;
#endif

	f.init = 0;			/* reset flags			*/
	f.open = 0;
	f.cal = 0;
	f.actv = 0;
	f.tmov = 0;
	xlnactw = 0;
	xlfreew = 0;
	xldataw = 0;
	xlcompw = 0;
	xlcompf = 0;
	xlster = 0;
	status_buf[ 7 ] = 0;
	pfint = xlnull;			/* reset ptr to fun		*/
	xl_tbi();			/* init ecc tables		*/
	if(getmem())
		printf("XL: getmem() failed.\n");
	else
		printf("Archive xl floppy tape driver v1.0 COH loaded\n");
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

printf("ftopen %x ", dev);

	/* Can't append to tape. */
	if (mode & IPAPPEND) {
printf("can't append ");
		u.u_error = EINVAL;
		return;
	}

	/* Only one open at a time. */
	if (f.open) {
printf("only one ftopen at a time ");
		u.u_error = EBUSY;
		return;
	}
	f.open = 1;

	unit = FT_UNIT(dev);		/* set unit #			*/

	if (! getfdcn(unit)) {	/* get control of fdc		*/
printf("fdc unavailable ");
		u.u_error = EBUSY;	/* exit if fdc is being used	*/
		f.open = 0;
		return;
	}

#if MWC_FT
	/* Select tape drive. */
	ftSelect(unit);

	/* Do recal to set pcn in fdc - shouldn't affect tape drive. */
	if (ftRecal(unit)
	  || fdc.fdc_nintstat < 2
	  || fdc.fdc_intstat[1] != 0) {
printf("recal failed ");
		u.u_error = EIO;
		f.open = 0;
		setFtIntr(0);
		return;
	}

	if (ftCmd(QIC_CMD_STS)) {
printf("get drive status failed ");
		u.u_error = EIO;
		f.open = 0;
		setFtIntr(0);
		return;
	}

	xlrel();
	relfdc();
#else
	/* Select tape unit. */
	xlsel(unit);

	if (0 == f.init) {		/* init drive if 1st time	*/
		pfrst = xlcomplete;
		xlreset();
		if (xlwait()) {
printf("init interrupted ");
			u.u_error = EINTR;
			f.open = 0;
			relfdc();
			return;
		}
		f.init = 1;
	}

	if (!xlopn(minor(dev))) {	/* open drive			*/
		/* init current params					*/
		read_ahead_seg_num = curr_seg_num = data_seg_num;
		cmn_err(CE_CONT, "xlopen: volume_seg_num %d  data_seg_num %d  last_seg_num %d  lnbr %d\n",
		   volume_seg_num, data_seg_num, last_seg_num, lnbr);

		cnbr = 0;
		xstop = 0;
		f.writ = 0;
		f.werr = 0;
	} else {
printf("xlopn failed ");
		u.u_error = ENODEV;	/* set no device		*/
		f.open = 0;
	}
	xlrel();			/* release fdc			*/
	relfdc();
	cmn_err(CE_CONT, "xlopen: xlster %x, xl6sts %x, xlests %x, xl7sts %x\n",
	   xlster, xl6sts, xlests, xl7sts);
#endif
}

/************************************************************************
 * ftread
 *
 ***********************************************************************/
static int
ftread(dev, iop)
dev_t dev;
IO * iop;
{
	if (!f.open) {			/* exit if not open		*/
		u.u_error = EIO;
		return;
	}
	xlgtdat(iop);
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
	if (!f.open) {			/* exit if not open		*/
		u.u_error = EIO;
		return;
	}
	if (xl6sts & XLSWRP) {		/* exit if write protect	*/
		u.u_error = EIO;
		return;
	}
	while(fprb = getrb(&data_queue))/* rel any pending read bfrs*/
		putrb(&xlfree_q, fprb);/* allows swt from read to write*/
	f.writ = 1;			/* set write flag		*/
	xlptdat(iop);
	if (f.werr)			/* flag error if occurred	*/
		u.u_error = EIO;
}

/*
 * Support routines.
 */

/************************************************************************/
/*	cmn_err() - doesn't really belong here				*/
/************************************************************************/
static void
cmn_err(level, format)
int level;
char * format;
{
	if (XL_VERBOSE)
		printf("%r ", &format);
}

/************************************************************************/
/*	dsperr	display floppy error					*/
/************************************************************************/
static	int
dsperr()
{					/* disregard fdc ecc error */
	if (!(status_buf[ 0 ] == 0x41 &&
	       status_buf[ 1 ] == 0x20 &&
	       status_buf[ 2 ] == 0x20)) 
		cmn_err(CE_CONT, "fdcerr: ST0=%x ST1=%x ST2=%x\n",
			status_buf[ 0 ], status_buf[ 1 ], status_buf[ 2 ]);
}

/*
 * For debugging, print command status and interrupt status to console.
 */
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

/*
 * Given a QIC-117 command number, cause that number of step pulses
 * to be sent from the FDC by faking a seek command.
 */
static int
ftCmdSend(cmd)
int cmd;
{
	/* Like NEC - pcn=present cylinder #; ncn=new cylinder #. */
	unsigned char ncn;
	
	xlDbPrintCmd(cmd);

	/*
	 * Will fake a seek command.
	 * Figure out whether to simulate seek to lower or higher
	 * cylinder number.
	 */
	if (ft.ft_pcn + cmd <= MAX_PCN) {
		ncn = ft.ft_pcn + cmd;
	} else if (ft.ft_pcn - cmd >= 0) {
		ncn = ft.ft_pcn - cmd;
	} else {
		printf("ftCmd %d invalid, pcn %d ", cmd, ft.ft_pcn);
		return -1;
	}

	ft.ft_dumpIrq = 1;
	fdcSeek(unit, 0, ncn);
}

/*
 * Given a QIC-117 command number, send the command.
 * If report bits are expected in response, initialize the bit counter.
 * Then sleep until the commmand is done and report bits are gathered.
 */
static int
ftCmd(cmd)
int cmd;
{
#if MWC_FT

	/* Will sleep until command done and report bits are in. */
	ft.ft_wakeMeUp = 1;

	/*
	 * The following commands expect report bits from the tape drive.
	 * The drive sends a leading ACK bit (always 1), then data bits
	 * (least significant bit first), then a trailing 1.  So, the
	 * number of bits needed is 2 more than the number of data bits
	 * in the report, or zero.
	 */
	switch(cmd) {
	case QIC_CMD_STS:
	case QIC_CMD_DRVCN:
	case QIC_CMD_ROMVN:
	case QIC_CMD_TPSTAT:
		ft.ft_bitsNeeded = 10;
		break;
	case QIC_CMD_ECD:
	case QIC_CMD_VNDID:
		ft.ft_bitsNeeded = 18;
		break;
	default:
		ft.ft_bitsNeeded = 0;
	}

	ftCmdSend(cmd);

	if (x_sleep(&ft.ft_wakeMeUp, pritape, slpriSigCatch, "ftCmd"))
		return -1;
	else
		return 0;
#else
	sekcms[ 1 ] = unit;
	if (sekcms[ 2 ] > 80)
		sekcms[ 2 ] -= cnt;
	else
		sekcms[ 2 ] += cnt;
	xlfdc_out_str(sekcms, 3);	/* start seek cmd		*/
#endif
}

/*
 * Interrupt handler.
 */
static void
ftIrqHandler()
{
	/*
	 * Need to get FDC status from result phase, and clear interrupt
	 * that may have been generated by diskette change or seek/recal
	 * complete.
	 */
	if (FDC_BUSY()) {
		fdcCmdStatus();
	} else {
		/* After seek or recal, must Sense Interrupt to clear IRQ. */
		fdcIntStatus();

		/* Update present cylinder number. */
		if (fdc.fdc_nintstat >= 2)
			ft.ft_pcn = fdc.fdc_intstat[1];
	}

	/*
	 * If report bits are needed
	 *   Get current report bit.
	 *   Update number of bits needed.
	 */
	if (ft.ft_bitsNeeded) {
		ft.ft_bitsNeeded--;

		/*
		 * Need unit # for Sense Drive Status command.
		 * Get it from preceding Sense Interrupt command.
		 */
		if (fdc.fdc_nintstat > 1) {
			unit = fdc.fdc_intstat[0] & 3;
			ft.ft_senseDrvStat = 0;
			fdcDrvStatus(unit, 0);	/* Send FDC command.		*/
			fdcCmdStatus();		/* Stash command result.	*/
		} else {
			printf("missing int status \n");
		}

		if (fdc.fdc_ncmdstat == 1) {
			int bit;
			bit = (fdc.fdc_cmdstat[0] & ST3_T0) ? 1 : 0;
			printf("<%d>", bit);
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
	}

	/* Print debug output if needed. */
	if (ft.ft_dumpIrq) {
		ft.ft_dumpIrq = 0;
		defer(ftDbPrtStat);
	}
}

/*
 * Send Recalibrate command to FDC and wait for it to finish.
 *
 * Return 0 if normal operation, -1 if signaled before recal complete.
 */
static int
ftRecal(unit)
int unit;
{
	ft.ft_wakeMeUp = 1;
	fdcRecal(unit);
	if (x_sleep(&ft.ft_wakeMeUp, pritape, slpriSigCatch, "ftRecal"))
		return -1;
	else
		return 0;
}

/*
 * Select tape unit.
 */
static	void
ftSelect(unit)
int unit;
{
	fdcRate(FDC_RATE_500K);		/* set transfer rate		*/
	fdcDrvSelect(unit, 1);		/* 1=motor on			*/
	fdcResetSel();			/* Reset, preserving selection	*/
	fdcSpecify(FT_SRT, FT_HUT, FT_HLT);

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
}

/************************************************************************
 * getmem
 *
 * Grab XL_NBUFS # of 32k DMA buffers.
 * Only use 16k of one of these, for QIC headers.
 *
 * Return 0 on success, 1 on failure.
 ***********************************************************************/
static int
getmem()
{
	register	int	i, nb;
	struct		rb	*prb;
	unsigned int	tmpad, highAddr;

	if (xlmem_allocated)
		return 0;

	nb = XL_NBUFS;
	if (nb < 4)
		nb = 4;

	rbmp = (struct rb *)kalloc(XL_NBUFS * sizeof(struct rb));
	if (rbmp == NULL) {
		printf("XL: initial kalloc() failed\n");
		return 1;
	}

	allocated_add = getDmaMem(nb * BFRSZ, BFRSZ);
	if (allocated_add == NULL) {
		printf("XL: getDmaMem() failed\n");
		return 1;
	}

	highAddr = allocated_add + (nb * BFRSZ);

	cmn_err(CE_CONT, "Allocated %d buffers.  Virtual <%x-%x>.  "
	  "Physical <%x-%x>.\n",
	  nb, allocated_add, highAddr, vtop(allocated_add), vtop(highAddr));

	/* tmpad = 1st 32k bfr adr			*/
	tmpad = (allocated_add + BFRSZ);

	ptr_header = (fplng)allocated_add;

	xlfree_q.top = prb = rbmp;/* build free pool	*/
	for (i = 0; i < (nb - 1); ++i) {
		xlfree_q.bot = prb;
		++prb;
		xlfree_q.bot->nxt = prb;
		xlfree_q.bot->adr = tmpad;
		tmpad += BFRSZ;
	}
	xlfree_q.bot->nxt = 0;

	cmn_err(CE_CONT, "getmem: allocated %d buffers\n", nb);

	xlmem_allocated = 1;
	data_queue.top = 0;
	data_queue.bot = 0;
	return 0;
}

/************************************************************************/
/*	getrb	get request buffer from queue				*/
/************************************************************************/
static	struct	rb *
getrb(prbq)
struct	rbq	*prbq;
{
	struct	rb *prb;
	int	ilv;

	if (0 == (prb = prbq->top))	/* return if empty		*/
		return 0;
	ilv = spl5();			/* playing with queue		*/
	prbq->top = prb->nxt;		/* advance top ptr		*/
	if (0 == prbq->top)
		prbq->bot = 0;
	splx(ilv);			/* done with queue		*/
	return prb;
}

/************************************************************************/
/*	putrb	add req buf to end of queue				*/
/*	return 1 if added to empty queue				*/
/************************************************************************/
static	int
putrb(prbq, prb)
struct	rbq	*prbq;
struct	rb	*prb;
{
	int	ilv;

	prb->nxt = 0;			/* reset this buf's fwd ptr	*/
	ilv = spl5();			/* playing with queues		*/
	if (0 == prbq->top) {		/* if empty queue,		*/
		prbq->top = prb;	/* pnt top and bot to new buf	*/
		prbq->bot = prb;
		splx(ilv);
		return 1;
	} else {
		(prbq->bot)->nxt = prb;/* else pnt old end to new buf	*/
		prbq->bot = prb;	/* and bot to new buf		*/
		splx(ilv);
		return 0;
	}
}

/************************************************************************/
/*	stdma	 set dma						*/
/************************************************************************/
static void
stdma(mode, adr, count)
int	mode;
paddr_t	adr;
int	count;
{
	cmn_err(CE_CONT, "dir: %x Vadd: %x, Padd: %x, count: %x\n",
	  mode, adr, kvtophys(adr), count);

	/* dma_param(DMA_CH2, mode, kvtophys(adr), count - 1); */
	/* dma_enable(DMA_CH2); */
	xldma((char)(mode + DMA_CH2), (long)kvtophys(adr), count - 1);
}

/************************************************************************/
/*	strbfm	set up rb params for format				*/
/************************************************************************/
static void
strbfm(prb)
struct	rb	*prb;
{
	register fpchr	p0;
	register int d0;
	int	d1;

	d0 = prb->sgn;			/* get segment #		*/
	prb->hed = d0/nseg_p_head;	/* set up fdc params		*/
	d0 = d0 % nseg_p_head;
	prb->cyl = d0/nseg_p_cyl;
	prb->sct = ((d0 % nseg_p_cyl) << 5) + 1;

	p0 = (fpchr)cprb->adr;

	/*				fill format bfr			*/

	d0 = prb->sct;
	for (d1 = d0 + 32; d0 != d1; ++d0) {
		*p0++ = prb->cyl;
		*p0++ = prb->hed;
		*p0++ = d0;
		*p0++ = 3;
	}
}

/************************************************************************/
/*	strbsg	set up rb params for segment				*/
/*		should be called at task time				*/
/*									*/
/*	rb.tbl	is used to skip bad sectors in segments.		*/
/*		segments are split up into "chunks"    			*/
/*		index into rb.tbl with relative sector.			*/
/*		each word in rb.tbl:					*/
/*		bits 15-10:  sector count for chunk			*/
/*		bits  9- 5:  dma offset for chunk			*/
/*		bits  4- 0:  sector # for chunk 			*/
/************************************************************************/
static void
strbsg(prb)
struct	rb	*prb;
{
	register int	d0, d1, d2;
	ushort		*p0;

	prb->erc = 0;			/* reset error count		*/
	d0 = prb->sgn;			/* set tape params		*/
	prb->trk = d0 / nseg_p_track;
	prb->tps = d0 % nseg_p_track;
	prb->hed = d0 / nseg_p_head;	/* set up fdc params		*/
	d0 = d0 % nseg_p_head;
	prb->cyl = d1 = d0 / nseg_p_cyl;
	prb->sct = d0 = ((d0 % nseg_p_cyl) << 5) + 1;
	if (d0 == 1) {			/* set up position params	*/
		--d1;
		d0 = (nseg_p_cyl << 5) - 16;
	}
	else
		d0 -= 16;
	prb->idc = d1;
	prb->ids = d0;
	d2 = 32;			/* set up rb.tbl		*/
	sgwrd = 0;
	sgmap = prb->map;
	p0 = prb->tbl;
	prb->nbk = 0;
	while(d2) {
		d0 = d2;		/* d0 = # scts to skip		*/
		while(sgmap & 1) {
			--d2;
			sgmap >>= 1;
		}
		d0 -= d2;
		sgwrd += d0;		/* bump sct # in sgwrd		*/
		if (!d2)		/* exit if done			*/
			break;
		d1 = d2;		/* d1 = # scts this chunk	*/
		while(d2 && 0 == (sgmap & 1)) {
			--d2;
			sgmap >>= 1;
		}
		d1 -= d2;
		prb->nbk += d1;		/* adjust # blocks in segment	*/
		sgwrd += d1 << 10;	/* set count in sgwrd		*/
		while(d0) {		/* fill table for skipped scts	*/
			*p0++ = sgwrd;
			--d0;
		}
		while(d1) {		/* for each sct xferred		*/
			*p0++ = sgwrd;	/* store table entry		*/
			sgwrd += 0xfc21;/* dec cnt, bump ofset bump sct	*/
			--d1;
		}
	}
	for (++d0; d0; --d0)		/* zero out rest of rb.tbl	*/
		*p0++ = 0;
}

/************************************************************************/
/*	stseg	set up segment io					*/
/************************************************************************/
static void
stseg(prb)
struct	rb	*prb;
{
	rwdcms[ 1 ] = unit;		/* set rwdcmd constants		*/
	rwdcms[ 2 ] = prb->cyl;
	rwdcms[ 3 ] = prb->hed;
	xadr = prb->adr;		/* set base adr			*/
	xsct = prb->sct;		/* set base sct			*/
	xrty = 6;			/* set default # retries	*/
}

/************************************************************************/
/*	wvtbl	create volume table entry				*/
/************************************************************************/
static void
wvtbl()
{
	xlvtbl	*p;
	int	ilvl;
	int	i;

	ilvl = spl5();			/*   wait for a new buffer	*/
	while(!(cprb = getrb(&xlfree_q))) {
		if (!f.actv)
			xlque();
		xlfreew = 1;
		sleep(&xlfreew, PRIBIO);
	}
	splx(ilvl);
	p = (xlvtbl *)cprb->adr;
	wptr = (fpchr)p;		/* save ptr for encode		*/
	for (i = 0; vtbl[ i ]; ++i)
		p->ident[ i ] = vtbl[ i ];
	p->data_seg_num = (unsigned short)data_seg_num;
	p->last_seg_num = (unsigned short)curr_seg_num - 1;
	for (i = 0; p->op_system[ i ] = xnxnm[ i ]; ++i);

	for (ilvl = 0; ilvl < 43; ++ilvl)	/* fill zero not implemented */	
		p->p1[ ilvl ] = 0;
	p->c_seq_num = 1;			/* cartridge sequence # =1   */

	for (ilvl = 0; ilvl < 34; ++ilvl) 	/* fill zero		     */
		p->p3[ ilvl ] = 0;

	p->last_blk_size = (unsigned short)(lnbr - cnbr);
	cprb->sgn = volume_seg_num;
	cprb->map = ptr_header[ volume_seg_num + 0x200 ];
	cprb->fun = RBFWT;
	strbsg(cprb);
	xl_enc(wptr, cprb->nbk);
	putrb(&req, cprb);
	ilvl = spl5();
	if (!f.actv)
		xlque();
	splx(ilvl);
}

/************************************************************************/
/*	xlcal() 	calibrate drive 				*/
/*			exits via (*xlosb)()				*/
/************************************************************************/
static void
xlcal()
{
	if (xlster) {			/* exit if error		*/
		(*pfcal)();
		return;
	}
	f.cal = 0;			/* reset calibrate sent flag	*/
	pfrdy = xlcal0; 		/* wait for ready		*/
	xlready();
}

static void
xlcal0()
{
	if (xlster) {			/* exit if error		*/
		(*pfcal)();
		return;
	}
	if ((xl6sts & 0xf7) == 0x65) {/* if ok, seek track 0, exit	*/
printf("xlcal:Seek tk 0 ");
		tptrk = 0;
		pfsek = pfcal;
		xlseek();
		return;
	}
	if ((xl6sts & XLSREF) == 0) {/* if not referenced		*/
printf("xlcal:calibrate ");
		if (f.cal) {		/* and calibrate sent, exit	*/
			(*pfcal)();
			return;
		}
		f.cal = 1;		/* else send calibrate		*/
		pfint = xlready;
		xloutput_step(QIC_CMD_CAL);
		return;
	}
	pfint = xlready;		/* rewind tape			*/
	xloutput_step(QIC_CMD_BOT);
}

/************************************************************************/
/*	xlcomplete  interrupt sequence complete				*/
/************************************************************************/
static void
xlcomplete()
{
	pfint = xlnull;			/* clean up int handler		*/
	xlcompf = 1;			/* indicate completion		*/
	if (xlcompw) {			/* wake up if waiting		*/
		xlcompw = 0;
		wakeup(&xlcompw);
	}
}

/************************************************************************/
/*	xldelay		delay for cnt ticks				*/
/************************************************************************/
static void
xldelay(cnt)
int	cnt;
{
	if (cnt)
		timeout(&xldly, cnt, pfdly, 0);
	else
		timeout(&xldly, cnt, NULL, 0);
}

/************************************************************************/
/*	xldma								*/
/************************************************************************/
static void
xldma(rw, addr, count)
char	rw;
long	addr;
int	count;
{
	int	oldpri;

	oldpri = sphi();
	outb(DMA1CBPFF, 0);
	outb(DMA1WMR, rw);
	outb(DMA1BCA2, addr & 0xff);
	outb(DMA1BCA2, (addr >> 8) & 0xff);
	outb(DMACH2PG, (addr >> 16) & 0xff);
	tenmicrosec();
	outb(DMA1BCWC2, count & 0xff);
	outb(DMA1BCWC2, (count >> 8) & 0xff);
	spl(oldpri);
	outb(DMA1WSMR, 2);
}

/************************************************************************/
/*	xlfdc_in_byte	input byte from fdc				*/
/************************************************************************/
static	unchar
xlfdc_in_byte()				/* input byte from fdc		*/
{
	register int	d0;

	for (d0 = FDTMO; d0; --d0) {	/* wait for ready		*/

		/* Wait for Request from Master asserted. */
		if (inb(FDSTAT) & 0x80) {
			/* exit if in output mode			*/
			if ((inb(FDSTAT) & 0x40) == 0)
				return(status_buf[ 7 ] = -1);
			return(inb(FDDATA));
		}
	}
	return(status_buf[ 7 ] = -1);	/* exit if timeout		*/
}

/************************************************************************/
/*	xlfdc_out_byte	output byte to fdc				*/
/************************************************************************/
static	unchar
xlfdc_out_byte(chr)			/* output byte to fdc		*/
int	chr;
{
	register int	d0;

	for (d0 = FDTMO; d0; --d0) {	/* wait for ready		*/

		/* Wait for Request from Master asserted. */
		if (inb(FDSTAT) & 0x80) {
			/* exit if in status mode			*/
			if (inb(FDSTAT) & 0x40)
				return(status_buf[ 7 ] = -1);
			outb(FDDATA, chr);	/* output byte, exit ok */
			return 0;
		}
	}
	return(status_buf[ 7 ] = -1);	/* exit if timeout		*/
}

/************************************************************************/
/*	xlfdc_out_str	output cmd string to fdc			*/
/************************************************************************/
static	unchar
xlfdc_out_str(cms, cnt) 		/* output command string	*/
unchar	*cms;
int	cnt;
{
	register unchar	*p0, *p1;

	p0 = cms;			/* set up for output		*/
	fdcmd = *p0;			/* save command type		*/
	for (p1 = p0 + cnt; p0 != p1; ++p0) {
		if (xlfdc_out_byte((int)(*p0)))/* stop if error	*/
			break;
	}
	return(status_buf[ 7 ]);	 /* exit ok			*/
}

/************************************************************************/
/*	xlfdc_reset	reset fdc					*/
/************************************************************************/
static void
xlfdc_reset() 				/* fdc reset			*/
{
	sekcms[ 1 ] = 0xff;
	status_buf[ 7 ] = 0;		/* reset error flag		*/
	outb(FDCTRL, fdselr);		/* reset fdc			*/
	outb(FDCTRL, fdsel);
}

/************************************************************************/
/*	xlflush	flush any pending io's         			        */
/************************************************************************/
static void
xlflush(dev)
{
	int	ilvl;

	xstop = 1;			/* stop any pending reads	*/
	if (f.writ) {			/* flush any pending writes	*/
		if (cnbr && cprb->fun == RBFWT) {
			xl_enc(wptr, cprb->nbk);
			putrb(&req, cprb);
		}
		ilvl = spl5();
		if (!f.actv)
			xlque();
		splx(ilvl);
		wvtbl();
	}
	ilvl = spl5();			/* wait for idle state		*/
	while(f.actv) {
		xlnactw = 1;
		sleep(&xlnactw, PRIBIO);
	}
	splx(ilvl);
	cnbr = 0;
	xstop = 0;
	while(cprb = getrb(&data_queue))/* relse pending read bfrs	*/
		putrb(&xlfree_q, cprb);
	while(cprb = getrb(&req))	/* release any pending que bfrs	*/
		putrb(&xlfree_q, cprb);
}

/************************************************************************/
/*	xlfmq	format queue handler					*/
/************************************************************************/
static void
xlfmq()
{
	while(xprb = getrb(&req)) {	/* while more to do		*/
		if (!f.actv) {		/* if not active, start tape	*/
			f.actv = 1;
			f.tmov = 1;
			pfint = xlfmq0;
			xloutput_step(QIC_CMD_FWD);
			return;
		}
		else{
			pffms = xlfmq1;	/* else, continue format	*/
			xlfms();
			return;
		}
	}

	pfrdy = xlfmq2; 		/* wait for tape stopped	*/
	xlready();
	return;
}

static void
xlfmq0()
{
	pffms = xlfmq1;
	xlfms();
	return;
}

static void
xlfmq1()
{
	if (xprb->sts)		/* set f.werr if error		*/
		f.werr = 1;
	putrb(&xlfree_q, xprb);	/* relse bfr back to free pool	*/
	if (xlfreew) {			/* wakeup if needed		*/
		xlfreew = 0;
		wakeup(&xlfreew);
	}
	xlfmq();			/* start next i/o		*/
}

static void
xlfmq2()
{
	if (xlnactw) {			/* if sleepg on xlnactw, wakeup	*/
		xlnactw = 0;
		wakeup(&xlnactw);
	}
	f.actv = 0;			/* rset actv, moving flgs, exit	*/
	f.tmov = 0;
	return;
}

/************************************************************************/
/*	xlfms	format a segment					*/
/************************************************************************/
static void
xlfms()
{
	pfint = xlfm0;			/* set up for format		*/
	xprb->sts = 0;
	if (f.werr) {			/* if error, do idle int	*/
		xloutput_step(0);
		return;
	}
	fmtcms[ 1 ] = unit;
	stdma(DMA_Wrmode, xprb->adr, 128);

	xltimout(IOTMO);		/* do the format		*/
	xlfdc_out_str(fmtcms, 6);
	if (status_buf[ 7 ]) {		/* exit if nec error		*/
		xltimout(0);
		xlster |= XLSNEC;
		xprb->sts = 1;
		pfint = xlnull;
		(*pffms)();
		return;
	}
}

static void
xlfm0()
{
	if (status_buf[ 0 ] & 0xc0)
		xprb->sts = 1;
	(*pffms)();
}

/************************************************************************/
/*	xlformat		format tape, task time			*/
/************************************************************************/
static int
xlformat(ntrkf)
int	ntrkf;
{
	int	ilvl, i;
	fplng	p0;

	cmn_err(CE_CONT, "xlformat: formatting %d tracks\n", ntrkf);

	pfint = xlready;		/* rewind tape			*/
	pfrdy = xlcomplete;
	xloutput_step(QIC_CMD_BOT);
	xlwait();
	if (xlster)
		return 2;

	bcopy((fpchr)xnxvs, ptr_header, 20);	/* init header		*/
	bcopy(ptr_header + 4, ptr_header + 5, 16 * 1024 - 20);

	pfint = xlcomplete;		/* format mode			*/
	xloutput_step(QIC_CMD_FMD);
	xlwait();

	cmn_err(CE_CONT, "writing reference bursts...\n");
	pfint = xlready;
	pfrdy = xlcomplete;
	xloutput_step(QIC_CMD_WRF);	/* start wrt reference bursts	*/
	xlwait();

	cmn_err(CE_CONT, "reference bursts write done, starting format...\n");

	f.werr = 0;

	for (fmtrk = 0; fmtrk < ntrkf; fmtrk += 2) {

		tptrk = fmtrk;		/* set tape track		*/
		curr_seg_num = fmtrk * nseg_p_track;

		pfint = xlcomplete;	/* normal mode			*/
		xloutput_step(QIC_CMD_NMD);
		xlwait();

		pfint = xlcomplete;	/* format mode			*/
		xloutput_step(QIC_CMD_FMD);
		xlwait();
		/*				format a track pair	*/
		do{
			cmn_err(CE_CONT, "\tformatting track %d\n", tptrk);
			pfsek = xlcomplete;	/* seek to track	*/
			xlseek();
			xlwait();
			for (last_seg_num = curr_seg_num + nseg_p_track; curr_seg_num != last_seg_num; ++curr_seg_num) {
				/* wait for a new buffer		*/
				ilvl = spl5();	
				while(!(cprb = getrb(&xlfree_q))) {
					if (!f.actv)
						xlfmq();
					xlfreew = 1;
					sleep(&xlfreew, PRIBIO);
				}
				splx(ilvl);
				/* set up buffer			*/
				cprb->sgn = curr_seg_num;
				strbfm(cprb);
				putrb(&req, cprb);
			}	/* queue up format request		*/

			ilvl = spl5();	/* track done, wait for idle	*/
			while(f.actv) {
				xlnactw = 1;
				sleep(&xlnactw, PRIBIO);
			}
			splx(ilvl);
			++tptrk;
		}  while(tptrk & 1);
		if (f.werr) {		/* exit if error formatting	*/
			pfprk = xlcomplete;
			xlpark();
			xlwait();
			return 1;
		}

		/*				verify a track pair	*/
		pfint = xlcomplete;	/* normal mode			*/
		xloutput_step(QIC_CMD_NMD);
		xlwait();

		pfint = xlcomplete;	/* verify mode			*/
		xloutput_step(QIC_CMD_VMD);
		xlwait();

		/* set up for verify					*/
		curr_seg_num = fmtrk * nseg_p_track;
		xlrel();
		relfdc();
		vftrk = -1;

		read_ahead_seg_num = curr_seg_num;/* queue up requests	*/
		while(cprb = getrb(&xlfree_q)) {
			cprb->sgn = read_ahead_seg_num;
			cprb->map = 0L;
			cprb->fun = RBFRD;
			strbsg(cprb);
			cprb->erc = 3;	/* no errors allowed		*/
			putrb(&req, cprb);
			++read_ahead_seg_num;
		}
		for (last_seg_num = curr_seg_num + 2 * nseg_p_track; curr_seg_num != last_seg_num; ++curr_seg_num ) {
			ilvl = spl5();	/* wait for a new buffer	*/
			while(!(cprb = getrb(&data_queue))) {
				if (!f.actv)
					xlque();
				xldataw = 1;
				sleep(&xldataw, PRIBIO);
			}
			splx(ilvl);
			if (vftrk != cprb->trk) {
				vftrk = cprb->trk;
				cmn_err(CE_CONT, "\tverifying track %d\n", vftrk);
			}
			if (cprb->sts) /* mask out segment if error	*/
				*(ptr_header + 0x200 + cprb->sgn) = 0xffffffffL;
			/* generate another request or			*/
			if (read_ahead_seg_num != last_seg_num) {
				cprb->sgn = read_ahead_seg_num;
				cprb->map = 0L;
				cprb->fun = RBFRD;
				strbsg(cprb);
				cprb->erc = 3;
				putrb(&req, cprb);
				++read_ahead_seg_num;
			} else {
				putrb(&xlfree_q, cprb);
			}
		}	/*  release buffer				*/

		ilvl = spl5(); 	/* track pair done, wait for idle	*/
		while(f.actv) {
			xlnactw = 1;
			sleep(&xlnactw, PRIBIO);
		}
		splx(ilvl);

		getfdc(unit); 	/* get fdc again		*/
		xlsel(unit);
	}

	if (xlster) {			/* if fatal error, exit		*/
		pfprk = xlcomplete;
		xlpark();
		xlwait();
		return 1;
	}

	pfint = xlcomplete;		/* normal mode			*/
	xloutput_step(QIC_CMD_NMD);
	xlwait();

	/*	done with format / verify				*/
	/*	now find out where headers and volumne table go		*/

	p0 = ptr_header + 0x200;
	for (i = 0; *p0++; ++i);	/* set hdr segment  #'s		*/
	((fpwrd)ptr_header)[ 3 ] = h0sgn = i;	/*  and vol segment	*/
	do {
		++i;
	} while(*p0++);
	((fpwrd)ptr_header)[ 4 ] = h1sgn = i;
	do {
		++i;
	} while(*p0++);
	((fpwrd)ptr_header)[ 5 ] = volume_seg_num = i;
	/* set last seg number						*/
	((fpwrd)ptr_header)[ 6 ] = last_seg_num = ntrkf * nseg_p_track - 1;

	cmn_err(CE_CONT,
           "xlformat: h0sgn %d  h1sgn %d  volume_seg_num %d  last_seg_num %d\n",
	   h0sgn, h1sgn, volume_seg_num, last_seg_num);

	pfprk = xlcomplete;		/* park tape			*/
	xlpark();
	xlwait();

	if (volume_seg_num > 10)	/* if too many bad segment exit	*/
		return 1;

	xlrel();			/* relse fdc, so xlque can work	*/
	relfdc();

	/* start header writes						*/
	for (curr_seg_num = 0; curr_seg_num <= volume_seg_num; ++curr_seg_num) {
		ilvl = spl5(); 		/*   get a buffer		*/
		while(!(cprb = getrb(&xlfree_q))) {
			if (!f.actv)
				xlque();
			xlfreew = 1;
			sleep(&xlfreew, PRIBIO);
		}
		splx(ilvl);
		cprb->sgn = curr_seg_num;/* set up for hdr or del adm write */
		cprb->map = 0L;
		if (ptr_header[ curr_seg_num + 0x200 ]) {
			cmn_err(CE_CONT, "-");
			cprb->fun = RBWFD;
		}
		else{
			cmn_err(CE_CONT, "*");
			cprb->fun = RBFWT;
		}
		strbsg(cprb);
		cptr = (fpchr)cprb->adr;

		bcopy(ptr_header, cptr, 16 * 1024);/* copy in header	*/
		bcopy(cptr + 16 * 1024 - 4, cptr + 16 * 1024, 13 * 1024 - 4);
		xl_enc(cptr, cprb->nbk);
		putrb(&req, cprb);
	}		/* queue up the request */

	if (!f.actv)			/* activate driver		*/
		xlque();

	ilvl = spl5();			/* wait for idle state		*/
	while(f.actv) {
		xlnactw = 1;
		sleep(&xlnactw, PRIBIO);
	}
	splx(ilvl);

	getfdc(unit);			/* park tape			*/
	xlsel(unit);
	pfprk = xlcomplete;
	xlpark();
	xlwait();
	return 0;
}

/************************************************************************/
/*	xlgtdat	get data						*/
/************************************************************************/
static void
xlgtdat(iop)
IO * iop;
{
	caddr_t	p0;
	fpchr	p1;
	int	ilvl;

	/*	set up requests using all free buffers			*/
	xlrqr();

	/*	set up to xfer data to user buffer			*/

	rnbr = iop->io_ioc;		/* set request params		*/
	p0 = iop->io.vbase;

	p1 = cptr;			/* use local copy of cptr	*/

	while(rnbr) {			/* while more to xfer		*/
		if (curr_seg_num > last_seg_num) {
			break;
		}
		if (!cnbr) {		/* if a new segment is needed	*/
			ilvl = spl5();	/* wait for data		*/
			while(!(cprb = getrb(&data_queue))) {
				if (!f.actv)
					xlque();
				xldataw = 1;
				sleep(&xldataw, PRIBIO);
			}
			splx(ilvl);

			/*	just got a new buffer from data queue	*/

			p1 = (fpchr)cprb->adr;
			if (cprb->sts) {/* check for errors		*/
				u.u_error = EIO;
			}
			else{
				if (!xl_dec(p1, cprb->nbk, cprb->erc,
				   cprb->ers[ 0 ], cprb->ers[ 1 ], cprb->ers[ 2 ])) {
					u.u_error = EIO;
				}
			}
			/* cnbr = # data bytes				*/
			cnbr = (cprb->nbk - 3) << 10;
			if (cprb->sgn != curr_seg_num)
				curr_seg_num = cprb->sgn;
			if (cprb->sgn == last_seg_num) {
				cnbr = lnbr;
			}
		}
		/* copy some data					*/
		rcnt = (cnbr >= rnbr) ? rnbr : cnbr;
		copyout(p1, p0, rcnt);
		p0 += rcnt;		/* adjust pointers and counts	*/
		p1 += rcnt;
		rnbr -= rcnt;
		cnbr -= rcnt;
		iop->io_ioc -= rcnt;
		if (!cnbr) {		/* release bfr if done		*/
			putrb(&xlfree_q, cprb);
			++curr_seg_num;
			xlrqr();
		}
	}				/* set up new read request	*/

	cptr = p1;			/* update cptr			*/
}

/************************************************************************/
/*	xlhalt	halt tape						*/
/************************************************************************/
static void
xlhalt()
{
	/* adjust tpseg for ramp down/up				*/
	if (f.tmov)
		tpseg += 2;
	f.tmov = 0;			/* reset tape moving flag	*/
	if (xlster) {			/* exit if error		*/
		(*pfhlt)();
		return;
	}
	pfint = xlready;		/* after stop cmd wait for rdy	*/
	pfrdy = pfhlt;			/* after ready, exit		*/
	xloutput_step(QIC_CMD_STOP);
}

/************************************************************************/
/*	xlintr	xl interrupt handler					*/
/************************************************************************/
static void
xlintr()				/* fdc interrupt handler	*/
{
	register unchar	*p0, *p1;

	xltimout(0);			/* reset interrupt timeout	*/

	/*
	 * Wait for Request from Master asserted in Main Status -
	 * indicates data register available for read/write.
	 */
	while (inb(FDSTAT) & 0x80 == 0);

	/* FDC busy indicates read/write command in progress. */
	if (inb(FDSTAT) & 0x10) {	/* br if non-data int		*/
		p0 = status_buf;	/* set up			*/
		for (p1 = p0 + 7; p0 != p1; ++p0) {
			*p0 = xlfdc_in_byte();	/* store next byte	*/
			if (status_buf[ 7 ]) {
				xlster |= XLSNEC;
				break;
			}
		}
	} else {
		/* handle non-I/O int		*/
		for(;;) {
			if (xlfdc_out_byte(8))/* start sense int	*/
				break;
			status_buf[ 0 ] = xlfdc_in_byte();/* get 1st sts byte*/
			if (status_buf[ 7 ]) {
				xlster |= XLSNEC;
				break;
			}
			if (status_buf[ 0 ] == 0x80) {/* br if done	*/
				status_buf[ 0 ] = unit;
				if (xltimoutf) {
					xltimoutf = 0;
					status_buf[ 0 ] = 0xc0;
				}
				break;
			}
			status_buf[ 1 ] = xlfdc_in_byte();/* get 2nd sts byte*/
			/* update track if my unit			*/
			if (unit == (3 & status_buf[ 0 ]))
				sekcms[ 2 ] = status_buf[ 1 ];
			/* check for more bytes				*/
		}
	}
	(*pfint)();			/* exit via caller handler	*/
	return;
}

/************************************************************************/
/*	xlnull	null interrupt handler					*/
/************************************************************************/
static void
xlnull()
{
}

/************************************************************************/
/*	xlopn	open tape (task time call)				*/
/************************************************************************/
static int
xlopn(dev)
dev_t	dev;
{
	register int	i;
	xlvtbl		*pp;

printf("xlopn ");

	/* try 100 times to see drive ready - will fix this later */
	for (i = 0; i < 100; i++) {
		xlster = 0;		/* reset error status		*/
		xlests = 0;
		xl7sts = 0;
		pfsts = xlcomplete;	/* get drive status		*/
		xlstatus();
		xlwait();
printf("try:%d ", i+1);
		if (xl6sts & XLSRDY)
			break;
	}

	xlster &= ~XLSSFT;		/* clear soft error bits	*/
	xlests = 0;
	xl7sts = 0;

	if (!(xl6sts & XLSCIN)) {	/* exit if no cartridge		*/
printf("no cart ");
		xlster |= XLSSFT;
		return 2;
	}

	if (xlster) {			/* exit if fatal error		*/
printf("fatal error ");
		return 2;
	}

	if (dev & M_CTL) {		/* exit if control open		*/
		volume_seg_num = data_seg_num = 0;
		return 0;
	}

	if (dev & M_RET) {		/* retension if retension_on_open */
		pfint = xlready;
		pfrdy = xlcomplete;
		xloutput_step(QIC_CMD_EOT);
		xlwait();
		if (xlster) {
printf("another fatal error ");
			return 2;
		}
	}
	pfint = xlready;		/* rewind tape			*/
	pfrdy = xlcomplete;
	xloutput_step(QIC_CMD_BOT);
	xlwait();
printf("should now be at BOT ");
	if (xlster) {
printf("yet another fatal error ");
		return 2;
	}

	pfcal = xlcomplete;		/* calibrate drive		*/
	xlcal();
	xlwait();
printf("should now be referenced ");
	if (xlster) {			/* exit if fatal error		*/
printf("still yet another fatal error ");
		return 2;
	}
	i = xl6sts & 0xf7;
	if (i == 0x45) {		/* return if no ref bursts	*/
printf("no ref bursts ");
		return 1;
	}
	if (i != 0x65) {		/* return general error		*/
printf("general error ");
		xlster |= XLSSFT;
		return 2;
	}
	xprb = getrb(&xlfree_q);	/* get a buffer for open	*/
	xprb->sgn = 0;			/* set up buffer		*/
xlopn0:
	xprb->map = 0L;
	strbsg(xprb);

	ptr_buffer = (fplng)xprb->adr;

	pfpos = xlrds;			/* read a segment		*/
	pfrds = xlcomplete;
	xlpos();
	xlwait();

	if (!xprb->sts) { 	/* ck for errors */
printf("xlopn trying ecc ");
		if (!xl_dec(ptr_buffer, xprb->nbk, xprb->erc,
		   xprb->ers[ 0 ], xprb->ers[ 1 ], xprb->ers[ 2 ])) {
printf("failed ");
			xprb->sts = 1;
		} else {
printf("succeeded ");
		}
	}

	if (xprb->sts) {		/* if error, try next segment	*/
		++xprb->sgn;
		if (xprb->sgn < 10 && !xlster) {
			goto xlopn0;
		}
	}
	if (xprb->sts) {		/* if fatal, return bfr & exit	*/
		putrb(&xlfree_q, xprb);
		pfprk = xlcomplete;
		xlpark();
		xlwait();
printf("fatal ");
		return 2;
	}

	bcopy(ptr_buffer, ptr_header, HDRSZ);	/* copy header data	*/

	volume_seg_num = ((fpwrd)ptr_header)[ 5 ];/* set seg numbers	*/
	data_seg_num = volume_seg_num + 1;

	xprb->sgn = volume_seg_num;	/* read in volume segment	*/
	xprb->map = ptr_header[ volume_seg_num + 0x200 ];
	strbsg(xprb);
	pfpos = xlrds;
	pfrds = xlcomplete;
	xlpos();
	xlwait();

	if (!xprb->sts) { 		/* ck for errors		*/
printf("xlopn trying 2nd ecc ");
		if (!xl_dec(ptr_buffer, xprb->nbk, xprb->erc,
		   xprb->ers[ 0 ], xprb->ers[ 1 ], xprb->ers[ 2 ])) {
printf("failed ");
			xprb->sts = 1;
		} else {
printf("succeeded ");
		}
	}

	/* set last seg num	*/
	pp = (xlvtbl *)ptr_buffer;
	last_seg_num = (int)pp->last_seg_num;
	/* set last # bytes	*/
	lnbr = (int)pp->last_blk_size;
	putrb(&xlfree_q, xprb);	/* return buffer		*/
	pfprk = xlcomplete;		/* park tape			*/
	xlpark();
	xlwait();
	if (xprb->sts) {		/* return error if error	*/
printf("sts err ");
		return 2;
	}
	return 0;
}

/************************************************************************/
/*	xloutput_step	output cnt steps				*/
/************************************************************************/
static void
xloutput_step(cnt)			/* output cnt steps		*/
int	cnt;
{

	xlDbPrintCmd(cnt);

	sekcms[ 1 ] = unit;
	if (sekcms[ 2 ] > 80)
		sekcms[ 2 ] -= cnt;
	else
		sekcms[ 2 ] += cnt;
	xlfdc_out_str(sekcms, 3);	/* start seek cmd		*/
	if (status_buf[ 7 ]) {		/* exit if nec error		*/
		xlster |= XLSNEC;
		(*pfint)();
	}
}

/************************************************************************/
/*	xlpark	park tape						*/
/************************************************************************/
static void
xlpark()
{
	if (xlster) {			/* exit if error		*/
		(*pfprk)();
		return;
	}
	pfsts = xlpark0; 		/* clear any drive status	*/
	xlstatus();
}

static void
xlpark0()
{
	if (xlster) {			/* exit if error		*/
		(*pfprk)();
		return;
	}
	pfint = xlpark1; 		/* set normal mode		*/
	xloutput_step(QIC_CMD_NMD);
}

static void
xlpark1()
{
	tpseg = tptrk = 0;		/* reset current track, segment	*/
	pfsek = xlpark2; 		/* seek track 0			*/
	xlseek();
}

static void
xlpark2()
{
	if (xlster) {			/* exit if error		*/
		(*pfprk)();
		return;
	}
	pfint = xlready;		/* after rewind, wait for rdy	*/
	pfrdy = pfprk;			/* after ready, exit		*/
	xloutput_step(QIC_CMD_BOT);	/* start rewind			*/
}

/************************************************************************/
/*	xlpos	position to segment					*/
/*		in: xprb = pointer to r					*/
/************************************************************************/
static void
xlpos()
{
	register int d0;

	cmn_err(CE_CONT, "xlpos ");

	if (xlster) {			/* exit if error		*/
		pfint = xlpos0;		/* use null int to cleanup stak	*/
		xloutput_step(0);
		return;
	}
	d0 = xprb->trk;
	if (tptrk != d0) {		/* if not same track		*/
		if ((tptrk ^ d0) & 1)	/* adjust tpseg		*/
			tpseg = nseg_p_track + 5 - tpseg;
		/* adjust tpseg if 1st seg on trk			*/
		if (!xprb->tps)
			tpseg = nseg_p_track;
printf("seek %d->%d ", tptrk, d0);
		tptrk = d0;		/* seek track			*/
		pfsek = xlpos0;
		xlseek();
		return;
	}

	d0 = xprb->tps; 		/* d0 = new tpseg		*/
	/* adjust tpseg if 1st seg on trk				*/
	if (!d0)
		tpseg = nseg_p_track;
	if (d0 == tpseg && f.tmov == 1) { /* exit if on target		*/
printf("xpos on target ");
		(*pfpos)();
		return;
	}
	if (d0 > tpseg) { 		/* if before target read id	*/
printf("before: %d < %d ", tpseg, d0);
		pfrdi = xlpos2;
		if (f.tmov) {
			xlreadid();
			return;
		}
		f.tmov = 1;
		pfint = xlreadid;
		xloutput_step(QIC_CMD_FWD);
		return;
	}
	else{
		pfskp = xlpos1;		/* else skip backwards		*/
		xlskipb(tpseg - d0);
		return;
	}
}

static void
xlpos0()				/* new track, tape stopped	*/
{
	register int d0;

	if (xlster) {			/* exit if error		*/
		(*pfpos)();
		return;
	}
	d0 = xprb->tps; 		/* d0 = new tpseg		*/
	/* adjust tpseg if 1st seg on trk				*/
	if (!d0)
		tpseg = nseg_p_track;
	if (d0 >= tpseg) {		/* if before target		*/
		f.tmov = 1;
		pfint = xlreadid;	/* start tape fwd and read id	*/
		pfrdi = xlpos2;
		xloutput_step(QIC_CMD_FWD);
		return;
	}
	else{
		pfskp = xlpos1;		/* else skip backwards		*/
		xlskipb(tpseg - d0);
		return;
	}
}

static void
xlpos1()				/* skip backwards done		*/
{
	cmn_err(CE_CONT, "xlpos1()\n");

	if (xlster) {			/* exit if error		*/
		(*pfpos)();
		return;
	}
	f.tmov = 1;
	if (!xprb->tps) { 		/* check for 1st seg on trk	*/
		pfint = pfpos;
		xloutput_step(QIC_CMD_FWD);
		return;
	}
	pfint = xlreadid;		/* start tape fwd and read id's	*/
	pfrdi = xlpos2;
	xloutput_step(QIC_CMD_FWD);
}

static void
xlpos2()				/* read id finished		*/
{
	cmn_err(CE_CONT, "xlpos2()");

	if (xlster) {			/* exit if error		*/
		(*pfpos)();
		return;
	}
	if (status_buf[ 0 ] & 0xc0) {	/* error handler		*/
		/* if crc error, read id again				*/
		if (status_buf[ 1 ] & 0x20) {
printf("crc err ");
			xlreadid();
			return;
		}
		pfhlt = xlpos3;
		xlhalt();
		return;
	}
	if (xprb->idc > status_buf[ 3 ] ||/* if bef tgt, read id again	*/
					xprb->ids > status_buf[ 5 ]) {
printf("before target ");
		xlreadid();
		return;
	}
	if (xprb->cyl <= status_buf[ 3 ] &&	/* if past it back up	*/
 					xprb->sct <= status_buf[ 5 ]) {
		pfskp = xlpos1;
		xlskipb(9);
		return;
	}
					/* we are at target spot	*/
	tpseg = xprb->tps;		/* set tpseg and exit		*/
	(*pfpos)();
}

static void
xlpos3()				/* status complete tape stopped	*/
{
	cmn_err(CE_CONT, "xlpos3()\n");

	if (xlster) {			/* exit if error		*/
		(*pfpos)();
		return;
	}
	if (xl6sts & (XLSEOT | XLSBOT)) {/* if end track skip backwd	*/
		pfskp = xlpos1;
		xlskipb((int)(nseg_p_track + 5 - xprb->tps));
		return;
	}
	xlster |= XLSNID;		/* else can't read id's		*/
	(*pfpos)();
}

/************************************************************************/
/*	xlptdat	put data						*/
/************************************************************************/
static void
xlptdat(iop)
IO * iop;
{
	caddr_t	p0;
	fpchr	p1;
	int	ilvl;

	rnbr = iop->io_ioc;		/* set request params		*/
	p0 = iop->io.vbase;
	while(rnbr) {
		if (!cnbr) {		/* if a new buffer is needed	*/
			ilvl = spl5();	/* wait for a new buffer	*/
			while(!(cprb = getrb(&xlfree_q))) {
				if (!f.actv)
					xlque();
				xlfreew = 1;
				sleep(&xlfreew, PRIBIO);
			}
			splx(ilvl);
			do{
				/* set up segment			*/
				cprb->sgn = curr_seg_num;
				cprb->map = ptr_header[ curr_seg_num + 0x200 ];
				cprb->fun = RBFWT;
				strbsg(cprb);
				++curr_seg_num;
			}while(cprb->nbk < 4);
			cnbr = (cprb->nbk - 3) << 10;	/* set params	*/
			lnbr = cnbr;
			cptr = (fpchr)cprb->adr;
			wptr = cptr;
		}		/* save ptr for later encode		*/
		p1 = cptr;			/* copy data		*/
		rcnt = (cnbr >= rnbr) ? rnbr : cnbr;
		copyin(p0, p1, rcnt);
		p0 += rcnt;
		p1 += rcnt;
		cptr = p1;
		rnbr -= rcnt; 		/* adjust count			*/
		cnbr -= rcnt;
		if (!cnbr) {		/* if filled, encode and q bfr	*/
			xl_enc(wptr, cprb->nbk);
			putrb(&req, cprb);
		}
	}
	iop->io_ioc = 0;
}

/************************************************************************/
/*	xlque		process queue request				*/
/************************************************************************/
static void
xlque()
{
	cmn_err(CE_CONT, "xlque()\n");
xlque0:
	xprb = getrb(&req);		/* get next request		*/
	if (!xprb) {			/* if end queue and		*/
		if (f.tmov) {		/* if tape moving, stop tape	*/
			cmn_err(CE_CONT, "xlque: stopping\n");
			pfhlt = xlque;
			xlhalt();
			return;
		}
		cmn_err(CE_CONT, "xlque: inactive\n");

		if (f.actv) {		/* if active, release fdc	*/
			xlrel();
			relfdc();
		}
		if (xlnactw) {		/* if sleeping on xlnactw, wakeup */
			xlnactw = 0;
			wakeup(&xlnactw);
		}
		f.actv = 0;		/* reset active flag, exit	*/
		return;
	}
	if (!f.actv) {		/* if called from task (not active)	*/
		getfdc(unit); 	/*    get fdc			*/
		xlsel(unit);
	}
	f.actv = 1;
	switch(xprb->fun) {
	case RBFRD:			/* read segment			*/
		if (xstop) {		/* if stop, ignore request	*/
			putrb(&xlfree_q, xprb);
			goto xlque0;
		}
		pfpos = xlrds;
		pfrds = xlqr0;
		xlpos();
		return;
	case RBFWT:			/* write segment		*/
		pfpos = xlwts;
		pfwts = xlqw0;
		xlpos();
		return;
	case RBWFD:			/* write del adr mrk segment	*/
		pfpos = xlwds;
		pfwds = xlqw0;
		xlpos();
		return;
	default:
		;
	}
}

static void
xlqr0()
{
	putrb(&data_queue, xprb);	/* add to data queue		*/
	if (xldataw) {			/* wakeup if needed		*/
		xldataw = 0;
		wakeup(&xldataw);
	}
	xlque();			/* start next i/o		*/
}

static void
xlqw0()
{
	if (xprb->sts)		/* set f.werr if error		*/
		f.werr = 1;
	putrb(&xlfree_q, xprb);	/* relse bfr back to free pool	*/
	if (xlfreew) {			/* wakeup if needed		*/
		xlfreew = 0;
		wakeup(&xlfreew);
	}
	xlque();			/* start next i/o		*/
}

/************************************************************************/
/*	xlrds	read segment						*/
/*		in: xprb = pointer to rb				*/
/************************************************************************/
static void
xlrds()
{
	if (xlster || xstop) {		/* exit if error or stop	*/
		pfint = xlrd0;		/* use null int to cleanup stck	*/
		xloutput_step(0);
		return;
	}
	rwdcms[ 0 ] = 0x46;		/* set for read			*/
	stseg(xprb);
	xrty = 2;			/* 2 tries per sector		*/
	xlrd0();			/* start io			*/
}

static void
xlrd0()					/* initiate io			*/
{
	if (xlster || xstop) {		/* exit if error or stop	*/
printf("xlrd0 - err xlster=%x xstop=%x ", xlster, xstop);
		pfint = xlnull;
		xprb->sts = 1;
		(*pfrds)();
		return;
	}
	pfint = xlrd1;			/* set int handler		*/
	xtbl = (xprb->tbl)[ xsct - xprb->sct ];/* get rb.tbl entry	*/
	if (xtbl) {		      /* if more data, start other read	*/
		rwdcms[ 4 ] = (xtbl & 0x1f) + xprb->sct;
		stdma(DMA_Rdmode, xadr | ((xtbl & 0x3e0) << 5), (int)(xtbl & 0xfc00));
		xltimout(IOTMO);
		xlfdc_out_str(rwdcms, 9);
		if (status_buf[ 7 ]) { 	/* exit if nec error	*/
			xltimout(0);
			xlster |= XLSNEC;
			pfint = xlnull;
			(*pfrds)();
			return;
		}
		return;
	}
	else{				/* else, seg is done		*/
		xprb->sts = 0;
		tpseg = xprb->tps + 1;
		if (tpseg == nseg_p_track) {	/* if last seg on trk,	*/
			pfrdy = pfrds;		/* exit via xlready	*/
			xlready();
			return;
		}
		pfint = xlnull;		/* else just exit		*/
		(*pfrds)();
		return;
	}
}

static void
xlrd1()
{
	if (xlster || xstop) {		/* exit if error or stop	*/
printf("xlrd1 - err xlster=%x xstop=%x ", xlster, xstop);
		pfint = xlnull;
		xprb->sts = 1;
		(*pfrds)();
		return;
	}
	if (status_buf[ 0 ] & 0xc0) {	/* if error			*/
		dsperr();
		if (status_buf[ 2 ] & 0x40) {/* if del adr mark, exit	*/
			++tpseg;
			goto xlrd2;
		}
		if (status_buf[ 0 ] == 0xc0)/* adjust tpseg		*/
			tpseg += TMSKP;
		else{
			tpseg += 2;
		}
		if (tpseg > nseg_p_track)
			tpseg = nseg_p_track;

		if (status_buf[ 0 ] != 0xc0) { /* if not timeout	*/
			/* if no data read				*/
			if (xsct == status_buf[ 5 ]) {
				--xrty;		/* skip sct if 2nd time	*/
				if (xrty == 0) {
					if (xprb->erc < 3) {
						xprb->ers[ xprb->erc ] = xsct-xprb->sct;
						++xprb->erc;
						++xsct;
						xrty = 2;
					}
					else{	/* fail if too many err	*/
						goto xlrd2;
					}
				}
			}
			else{			/* some data read in	*/
				xrty = 1; 	/* 1mor try on this sct	*/
				xsct = status_buf[ 5 ];
			}
		}				/* update xsct		*/
		else{ 				/* if timeout		*/
			--xrty;			/* fail if too many err	*/
			if (!xrty) {
xlrd2:
printf("xlrd2 - err xlster=%x xstop=%x ", xlster, xstop);
				xprb->sts = 1;
				pfint = xlnull;
				(*pfrds)();
				return;
			}
		}
		pfpos = xlrd0;		/* restart io after positioning	*/
		xlpos();
		return;
	}

	/*	no errors, start io for next part of segment (if any)	*/

	xsct = status_buf[ 5 ];		/* update xsct, start next io	*/
	xlrd0();
}

/************************************************************************/
/*	xlreadid	read id 						*/
/************************************************************************/
static void
xlreadid()
{
	cmn_err(CE_CONT, "xlreadid()");

	pfint = xlreadid0; 		/* set pfint			*/
	xltimout(IOTMO);
	xlfdc_out_str(rdicms, 2);	/* start read id cmd		*/
	if (xlster) {			/* exit if error		*/
printf("readid err ");
		xltimout(0);
		pfint = xlnull;
		(*pfrdi)();
		return;
	}
}

static void
xlreadid0()
{
	pfint = xlnull;			/* reset pfint			*/
	(*pfrdi)();			/* and exit			*/
}

/************************************************************************/
/*	xlready		wait for ready					*/
/************************************************************************/
static void
xlready()
{
	if (xlster) {			/* exit if error		*/
		(*pfrdy)();
		return;
	}
	pfsts = xlready0; 		/* get drive status		*/
	xlstatus();
}

static void
xlready0()
{
	if (xlster || (xl6sts & XLSRDY)) {	/* exit if done		*/
		(*pfrdy)();
		return;
	}
	xlstatus();			/* else get status again	*/
}

/************************************************************************/
/*	xlrel	release unit						*/
/************************************************************************/
static void
xlrel()
{
	while(0x80 != (0xc0 & inb(FDSTAT)));  /* wait for ready	*/
	outb(FDCTRL, 0x0c);		/* deselect all units		*/
	xlfdc_out_str(sf3cms, 3);	/* set step rate to 3ms		*/
}

/************************************************************************/
/*	xlreset		reset drive					*/
/************************************************************************/
static void
xlreset()
{
	pfint = xlreset0; 		/* output 1 step		*/
	xloutput_step(QIC_CMD_RST);
}

static void
xlreset0()
{
	pfint = xlnull;			/* reset pfint			*/
	pfdly = xlready;		/* after delay "wait" for ready	*/
	pfrdy = pfrst;			/* after ready, exit via pfrst	*/
	xldelay(HZ);			/* start delay			*/
}

/************************************************************************/
/*	xlrnb	report next bit(s)					*/
/************************************************************************/
static void
xlrnb(cnt)
int	cnt;
{
	xlrnbw = 0;			/* reset status int		*/
	pfint = xlrnb0; 		/* set int handler		*/
	xlbcnt = cnt;			/* set up			*/
	xloutput_step(QIC_CMD_RNB);	/* start 2 step seek		*/
}

static void
xlrn9() 				/* get 9 bits			*/
{
	xlrnbw = 0;
	pfint = xlrnb0;
	xlbcnt = 9;
	xloutput_step(QIC_CMD_RNB);
}

static void
xlrn17()				/* get 17 bits			*/
{
	xlrnbw = 0;
	pfint = xlrnb0;
	xlbcnt = 17;
	xloutput_step(QIC_CMD_RNB);
}

static void
xlrnb0()
{
	register int xlrnbb;		/* xl2 status bit		*/
	int bit;

	/* Sense drive status. */
	xlfdc_out_byte(4);
	xlfdc_out_byte((int)unit);

	/* Read ST3, look at T0 bit. */
	bit = (0x10 & xlfdc_in_byte()) ? 1 : 0;

	xlrnbb = bit ? 0x8000 : 0x0000;
	if (status_buf[ 7 ])		/* exit if nec handshake error	*/
		xlster |= XLSNEC;
	if (xlster) {
		pfint = xlnull;
		(*pfrnb)();
		return;
	}
	--xlbcnt;
	if (xlbcnt) {			/* if not done, shift in bit	*/
		xlrnbw >>= 1; 		/*  and continue		*/
		xlrnbw |= xlrnbb;
		xloutput_step(QIC_CMD_RNB);
		return;
	} else {
		if (!xlrnbb)		/* last bit must = 1		*/
			xlster |= XLSLSB;
		pfint = xlnull;
		(*pfrnb)();		/* call end step handler	*/
		return;
	}
}

/************************************************************************/
/*	xlrqr	set up read requests					*/
/************************************************************************/
static void
xlrqr()
{
	while(fprb = getrb(&xlfree_q)) {/* enqueue any free bfrs	*/
xlrqr0:
		if (read_ahead_seg_num > last_seg_num) {
			putrb(&xlfree_q, fprb);
			break;
		}
		fprb->sgn = read_ahead_seg_num;
		fprb->map = ptr_header[ read_ahead_seg_num + 0x200 ];
		fprb->fun = RBFRD;
		strbsg(fprb);
		if (fprb->nbk < 4) {	/* skip if < 4 sectors		*/
			++read_ahead_seg_num;
			goto xlrqr0;
		}
		putrb(&req, fprb);	/* add bfr to req queue		*/
		++read_ahead_seg_num;
	}
}

/************************************************************************/
/*	xlseek	seek head to tptrk					*/
/************************************************************************/
static void
xlseek()
{
	if (xlster) {			/* exit if error		*/
		(*pfsek)();
		return;
	}
	pfhlt = xlseek0; 		/* halt tape			*/
	xlhalt();
}

static void
xlseek0()
{
	if (xlster) {			/* exit if error		*/
		(*pfsek)();
		return;
	}
	pfint = xlseek1; 		/* do 13 steps			*/
	xloutput_step(QIC_CMD_SEEK);
}

static void
xlseek1()
{
	if (xlster) {			/* exit if error		*/
		(*pfsek)();
		return;
	}
	pfint = xlready;		/* after seek "wait" for rdy	*/
	pfrdy = pfsek;			/* after ready, exit		*/
	xloutput_step(2 + tptrk);
}

/************************************************************************
 *	xlsel	select unit, wait for done
 ***********************************************************************/
static	void
xlsel()
{
	fdsel = fdstb[unit];
	fdselr = 0xfb & fdsel;
	outb(FDCSR1, 0); 		/* set 500khz speed		*/
	outb(FDCTRL, fdsel);		/* select unit			*/
	pfint = xlcomplete;		/* reset fdc			*/
	xlfdc_reset();
	xlwait();
	xlfdc_out_str(sf2cms, 3);	/* set step rate to 2ms		*/

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
}

/************************************************************************/
/*	xlskipb		skip n segments back				*/
/************************************************************************/
static	void
xlskipb(cnt)
int	cnt;
{
	if (xlster) {			/* exit if error		*/
		(*pfskp)();
		return;
	}
	f.tmov = 0;			/* tape will be halted		*/
	xlskip_count = cnt;		/* set skip count		*/
	pfint = xlready;		/* stop tape first		*/
	pfrdy = xlskipb0;
	xloutput_step(QIC_CMD_STOP);
}

static	void
xlskipb0()
{
	pfint = xlskipb1; 		/* start skip back cmd		*/
	xloutput_step(QIC_CMD_SKPB);
}

static	void
xlskipb1()
{
	if (xlster) {			/* exit if error		*/
		(*pfskp)();
		return;
	}
	pfint = xlskipb2; 		/* issue 2nd part of cmd	*/
	xloutput_step(2 + (0xf & xlskip_count));
}

static	void
xlskipb2()
{
	if (xlster) {			/* exit if error		*/
		(*pfskp)();
		return;
	}
	/* after 3rd part cmd, wait for ready				*/
	pfint = xlready;
	pfrdy = pfskp;			/* after ready, exit		*/
	/* start 3rd part cmd	*/
	xloutput_step(2 + (0xf & (xlskip_count >> 4)));
}

/************************************************************************/
/*	xlstatus() 	get drive status				*/
/************************************************************************/
static void
xlstatus()
{
	if (xlster) {			/* exit if error		*/
		pfint = xlnull;
		(*pfsts)();
		return;
	}
	/* after 6 steps, get 9 report bits				*/
	pfint = xlrn9;
	pfrnb = xlstatus0; 		/* after 9 bits, goto xlstatus0	*/
	xloutput_step(QIC_CMD_STS);	/* start steps			*/
}

static void
xlstatus0()
{
	if (xlster) {			/* exit if error		*/
		pfint = xlnull;
		(*pfsts)();
		return;
	}

	xl6sts = xlrnbw >> 8;		/* set xl6sts			*/

	if (xl6sts == 0xff) {		/* exit if not tape drive	*/
		xlster |= XLSNTD;
		pfint = xlnull;
		(*pfsts)();
		return;
	} else {
		xlDbPrintStat(xl6sts);
	}

	if (xl6sts & (XLSEXC | XLSCHG)) {/* if exception condition,	*/
		xlests = xl6sts;	/* start type 7 status		*/
		pfint = xlrn17;
		pfrnb = xlstatus1;
		xloutput_step(QIC_CMD_ECD);
		return;
	}

	if (xlests || !(xl6sts & XLSCIN)) { /* set soft err if needed	*/
printf("soft err ");
		xlster |= XLSSFT;
	}

	pfint = xlnull;			/* exit				*/
	(*pfsts)();
}

static void
xlstatus1()
{
	if (xlster) {			/* exit if error		*/
		pfint = xlnull;
		(*pfsts)();
		return;
	}
	xl7sts = xlrnbw;		/* save type 7 status		*/
	xlDbPrintErr(xl7sts);
	pfint = xlrn9;			/* restart normal status sequce	*/
	pfrnb = xlstatus0;
	xloutput_step(QIC_CMD_STS);
}

/************************************************************************
 *	xltimout		set timeout for int
 *
 * "cnt" is number of ticks.  if zero, cancel any pending timeout
 ***********************************************************************/
static void
xltimout(cnt)
int	cnt;
{
	if (cnt)
		timeout(&xltmo, cnt, xltimfn, 0);
	else
		timeout(&xltmo, cnt, NULL, 0);
}

static void
xltimfn()
{
	cmn_err(CE_CONT, "Timeout!!\n");
	xltimoutf = 1;
	xlfdc_reset();
}

/************************************************************************/
/*	xlwait	wait for xlcomplete (xlcompw wakeup)			*/
/************************************************************************/
static int
xlwait()
{
	register int ilvl;

	ilvl = spl5();
	while(!xlcompf) {		/* wait for xlcompf to be set	*/
		xlcompw = 1;
		if (x_sleep(&xlcompw, pritape, slpriSigCatch, "xlwait")) {
printf("xlwait signaled ");
			return 1;
		}
	}
	xlcompf = 0;			/* reset flag			*/
	splx(ilvl);
	return 0;
}

/************************************************************************/
/*	xlwds	write del adm segment					*/
/*		in: xprb = pointer to rb				*/
/************************************************************************/
static void
xlwds()
{
	pfwts = pfwds;			/* using xlwts			*/
	if (xlster) {			/* exit if error		*/
		pfint = xlwt0;		/* use null int to cleanup stck	*/
		xloutput_step(0);
		return;
	}
	rwdcms[ 0 ] = 0x49;		/* set for write del adm	*/
	stseg(xprb);
	xrty = 2;
	pfint = xlwt1;
	xlwt0();
}

/************************************************************************/
/*	xlwts	write segment						*/
/*		in: xprb = pointer to rb				*/
/************************************************************************/
static void
xlwts()
{
	if (xlster) {			/* exit if error		*/
		pfint = xlwt0;		/* use null int to cleanup stck	*/
		xloutput_step(0);
		return;
	}
	rwdcms[ 0 ] = 0x45;		/* set for write		*/
	stseg(xprb);
	xlwt0();
}

static void
xlwt0()					/* start io			*/
{
	if (xlster) {			/* exit if error		*/
		xprb->sts = 1;
		pfint = xlnull;
		(*pfwts)();
		return;
	}
	xtbl = (xprb->tbl)[ xsct - xprb->sct ];/* get rb.tbl entry	*/
	if (xtbl) {		      /* if more data start other write	*/
		pfint = xlwt1;
		rwdcms[ 4 ] = (xtbl & 0x1f) + xprb->sct;
		stdma(DMA_Wrmode, xadr | ((xtbl & 0x3e0) << 5), (int)(xtbl & 0xfc00));
		xltimout(IOTMO);
		xlfdc_out_str(rwdcms, 9);
		if (status_buf[ 7 ]) { 	/* exit if nec error	*/
			xltimout(0);
			xlster |= XLSNEC;
			xprb->sts = 1;
			pfint = xlnull;
			(*pfwts)();
			return;
		}
		return;
	}
	else{				/* else, seg is ok		*/
		xprb->sts = 0;
		tpseg = xprb->tps + 1;
		if (tpseg == nseg_p_track) {	/* if last seg on trk,	*/
			pfrdy = pfwts;	/* exit via xlready		*/
			xlready();
			return;
		}
		pfint = xlnull;		/* else just exit		*/
		(*pfwts)();
		return;
	}
}

static void
xlwt1()
{
	if (xlster) {			/* exit if error		*/
		xprb->sts = 1;
		pfint = xlnull;
		(*pfwts)();
		return;
	}
	if (status_buf[ 0 ] & 0xc0) {	/* check for io error		*/
		dsperr();
		if (status_buf[ 0 ] == 0xc0)/* adjust tpseg		*/
			tpseg += TMSKP;
		else
			tpseg += 2;
		if (status_buf[ 0 ] != 0xc0)/* update xsct		*/
			xsct = status_buf[ 5 ];
		--xrty;			/* retry if more to try		*/
		if (xrty) {
			pfpos = xlwt0;
			xlpos();
			return;
		}
		else{
			/* if del adm write, continue			*/
			if (rwdcms[ 0 ] == 0x49) {
				++xsct;
				xrty = 2;
				pfpos = xlwt0;
				xlpos();
				return;
			}
			xprb->sts = 1;	/* give up on segment, exit	*/
			pfint = xlnull;
			(*pfwts)();
			return;
		}
	}
	xsct = status_buf[ 5 ];		/* if no error, start next io	*/
	xlwt0();
}

/******************* DEBUG AREA ******************/
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

/* print 2-byte error status as <error-code,command> */
static void
xlDbPrintErr(errword)
unsigned int errword;
{
	unsigned int lo, hi;

	lo = errword & 0xff;
	hi = (errword >> 8) & 0xff;

	if (lo >= 1 && lo < sizeof(qicErr)/sizeof(qicErr[0]))
		printf("<%s,", qicErr[lo]);
	else
		printf("<%d,", lo);

	if (hi >= 1 && hi < sizeof(qicCmd)/sizeof(qicCmd[0]))
		printf("%s> ", qicCmd[hi]);
	else
		printf("%d> ", hi);
}

/* print command as [command] */
static void
xlDbPrintCmd(cmd)
unsigned int cmd;
{
	if (cmd >= 1 && cmd < sizeof(qicCmd)/sizeof(qicCmd[0])) {
		if (cmd == QIC_CMD_RNB)
			putchar ('.');
		else
			printf("[%s] ", qicCmd[cmd]);
	} else
		printf("[%d] ", cmd);
}

/* print tape status as { status string,... } */
static void
xlDbPrintStat(stat)
unsigned int stat;
{
	int i;

	printf("{ ");
	for (i = 0; i < 8; i++) {
		if (xl6sts & (1 << i))
			printf("%s, ", qicStat[i]);
	}
	putchar('}');
}
