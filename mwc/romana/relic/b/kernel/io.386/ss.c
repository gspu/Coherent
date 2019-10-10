/*
 * io.386/ss.c
 *
 * Device driver for Seagate ST01/ST02 scsi host adapters.
 *
 * Revised: Wed May 26 16:57:51 1993 CDT
 */

/*
 * To do:
 *	nonzero LUN's
 *	start new command during disconnect
 *	rewrite as single state machine, instead of 7 of them
 *	separate SCSI layer from host-dependent stuff
 */

/*
 * Debug levels.
 * DEBUG = 0	No debug output.
 * DEBUG = 1	Debug output on error only.
 * DEBUG = 2	Debug output on error and at other selected places.
 * DEBUG = 3	Print state machine trace.
 * DEBUG = 4	Print info xfer phases and msg_in values.
 */
#if (DEBUG >= 1)
static int s_id;
#define PR1(str)		printf("%s%d ", str, s_id)
#else
#define PR1(str)
#endif
#if (DEBUG >= 2)
#define PR2(str)		printf(str)
#else
#define PR2(str)
#endif
#if (DEBUG >= 3)
#define PR3(str)		printf("%s%d ", str, s_id)
#else
#define PR3(str)
#endif
#if (DEBUG >= 4)
#define PR4(str)		printf("%s%d ", str, s_id)
#else
#define PR4(str)
#endif

/*
 * Includes.
 */
#include	<sys/coherent.h>

#ifdef _I386
#include	<sys/fakeff.h>
#include	<sys/dmac.h>
#endif
#include	<sys/io.h>
#include	<sys/sched.h>
#include	<sys/uproc.h>
#include	<sys/proc.h>
#include	<sys/con.h>
#include	<sys/stat.h>
#include	<sys/devices.h>		/* SCSI_MAJOR */
#include	<errno.h>
#include 	<sys/fdisk.h>
#include	<sys/hdioctl.h>
#include	<sys/buf.h>
#include	<sys/scsiwork.h>
#include	<sys/typed.h>

/*
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */
#define SS_RAM		0x1800	/* Offset of parameter RAM */

				/* Future Domain */
#define FD_CSR		0x1C00	/* Offset of control/status register */
#define FD_DAT		0x1E00	/* Offset of data port */

				/* Seagate */
#define SS_CSR		0x1A00	/* Offset of control/status register */
#define SS_DAT		0x1C00	/* Offset of data port */

#define SS_RAM_LEN	128	/* ST0x has 128 bytes of RAM */
#define SS_DAT_LEN	0x400	/* Byte range mapped to data port */
#define SS_SEL_LEN	0x2000	/* Total size of memory-mapped area */

#define WC_ENABLE_SCSI	0x80	/* Write Control (WC) register bits */
#define WC_ENABLE_IRPT	0x40
#define WC_ENABLE_PRTY	0x20
#define WC_ARBITRATE	0x10
#define WC_ATTENTION	0x08
#define WC_BUSY  	0x04
#define WC_SELECT  	0x02
#define WC_SCSI_RESET  	0x01

#define RS_ARBIT_COMPL	0x80	/* Read STATUS (RS) register bits */
#define RS_PRTY_ERROR	0x40
#define RS_SELECT	0x20
#define RS_REQUEST	0x10
#define RS_CTRL_DATA	0x08
#define RS_I_O  	0x04
#define RS_MESSAGE  	0x02
#define RS_BUSY  	0x01

#define DEV_SCSI_ID(dev)	((dev >> 4) & 0x0007)
#define DEV_LUN(dev)		((dev >> 2) & 0x0003)
#define DEV_DRIVE(dev)		((dev >> 2) & 0x001F)
#define DEV_PARTN(dev)		(dev & 0x0003)
#define DEV_SPECIAL(dev)	(dev & 0x0080)

#define HIPRI_RETRIES	5000	/* # of times to retry while hogging CPU */
#define LOPRI_RETRIES	5	/* # of retries with sleep between tries */
#define WHOLE_DRIVE	NPARTN
#define RESET_TICKS	50	/* # of clock ticks for reset settling */
#define LOAD_DELAY	30000	/* Loop counter during ssload() only */

#define BUS_FREE	((ffbyte(ss_csr) & (RS_BUSY | RS_SELECT)) == 0)
#define TGT_RSEL	\
	(  (ffbyte(ss_csr) & (RS_SELECT |  RS_I_O   )) \
	&& (ffbyte(ss_dat) & (host_id   | (1<<s_id) )) )

#define DELAY_ARB	10	/* delays units are 10 msec (clock ticks) */
#define DELAY_BDR	30
#define DELAY_BSY	20
#define DELAY_RES	50
#define DELAY_RST	40

#define MAX_AVL_COUNT	100
#define MAX_BDR_COUNT	3
#define MAX_BSY_COUNT	3
#define MAX_TRY_COUNT	10
#define INL_MAX_REQ_POLL	800000L
#define WKG_MAX_REQ_POLL	20000L

typedef enum {			/* values for current driver state */
	SST_DEQUEUE =0,
	SST_BUS_DEV_RESET,
	SST_HIPRI_RESET,
	SST_LOPRI_RESET,
	SST_POLL_ARBITN,
	SST_POLL_BEGIN_IO,
	SST_POLL_RESELECT,
	SST_REQ_SENSE,
	SST_RESET_OFF
} SST_TYPE;

typedef enum {			/* values for input to recovery routine */
	RV_A_TIMEOUT,
	RV_P_TIMEOUT,
	RV_R_TIMEOUT,
	RV_BF_TIMEOUT,
	RV_CS_BUSY,
	RV_CS_CHECK
} RV_TYPE;

typedef struct ss {
	ulong	capacity;
	ulong	blocklen;
	ulong	bno;
	int	msg_in;
	int	dr_watch;
	unchar	cmdbuf[G1CMDLEN];
	int	cmdlen;
	int	cmd_bytes_out;
	int	cmdstat;
	BUF	*bp;		/* current I/O request node, or NULL */
	struct	fdisk_s parmp[NPARTN+1];
	SST_TYPE state;
	TIM	tim;		/* for target-specific timers */
	unchar	avl_count;
	unchar	bdr_count;
	unchar	bsy_count;
	unchar	try_count;
	uint	busy:1;		/* 1 if command uses local buffer */
	uint	expired:1;	/* 1 if target's timer has expired */
	uint	ptab_read:1;	/* 1 if partition table has been read */
	uint	waiting:1;	/* 1 if target timer is running */
}	ss_type;

typedef struct {
	uint	ncyl;
	unchar	nhead;
	unchar	nspt;
}	drv_parm_type;

/*
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */

/* functions from bufq.c */
extern int bufq_init();
extern void bufq_rlse();
extern void bufq_wr_tail();
extern BUF * bufq_rd_head();
extern BUF * bufq_rm_head();

/* functions from ssas.s */
extern void	ss_get();
extern int	ss_put();
extern int	nulldev();
extern int	nonedev();
#ifndef _I386
extern unsigned char ffbyte();
#endif

static void	ssopen();		/* CON functions */
static void	ssclose();
static void	ssblock();
static void	ssread();
static void	sswrite();
static int	ssioctl();
static void	sswatch();
static void	ssload();
static void	ssunload();

static int	bus_dev_reset();	/* additional support functions */
static int	chk_reconn();
static void	do_connect();
static void	dummy_reconn();
static int	far_info_xfer();
static int	host_ident();
static int	init_call();
static void	init_pointers();
static int	inquiry();
static int	local_info_xfer();
static int	mode_sense();
static void	next_req();
static void	nonpolled();
static int	read_cap();
static void	recover();
static int	req_sense();
static int	rsel_handshake();
static void	ssdelay();
static void	ss_finished();
static void	ss_mach();
static void	set_timeout();
static int	ssinit();
static void	ssintr();
static int	start_arb();
static void	stop_timeout();
static void	tbparms();
static unchar	xpmod();

/*
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */

extern short n_atdr; /* set by atcount() before any load routines run */
 
CON	sscon	= {
	DFBLK|DFCHR,			/* Flags */
	SCSI_MAJOR,			/* Major index */
	ssopen,				/* Open */
	ssclose,			/* Close */
	ssblock,			/* Block */
	ssread,				/* Read */
	sswrite,			/* Write */
	ssioctl,			/* Ioctl */
	nulldev,			/* Powerfail */
	sswatch,			/* Timeout */
	ssload,				/* Load */
	ssunload,			/* Unload */
	nulldev				/* Poll */
};

	/* Patch these Export Variables to configure the driver. */
/*
 * In the low byte of NSDRIVE, bit n is 1 if SCSI ID n is an installed target.
 * The high byte indicates which type of host adapter:
 *   00 - ST01/ST02
 *   80 - TMC-845/850/860/875/885
 *   40 - TMC-840/841/880/881
 */
uint	NSDRIVE = 0x0001;
uint	SS_INT = 5;		/* ST0[12] use either IRQ3 or IRQ5 */
uint	SS_BASE = 0xCA00;	/* Segment addr of ST0x communication area */

/* ncyl, nhead, nspt */
drv_parm_type drv_parm[MAX_SCSI_ID] = {
	{ 0, 0, 0},
	{ 0, 0, 0},
	{ 0, 0, 0},
	{ 0, 0, 0},
	{ 0, 0, 0},
	{ 0, 0, 0},
	{ 0, 0, 0}
};

static BUF	dbuf;		/* For raw I/O */
static paddr_t	ss_base;	/* physical address of ST0x comm area */
static faddr_t	ss_fp;		/* (far *) to ST0x comm area */

static faddr_t	ss_ram;		/* (far *) to parameter RAM */
static faddr_t	ss_csr;		/* (far *) to control/status */
static faddr_t	ss_dat;		/* (far *) to data port */

static TIM	delay_tim;	/* needed for calls to ssdelay() */
static int	do_sst_op;	/* 1 when state machine iteration continues */
static int	ss_expired;	/* 1 after local timeout */

static uint	max_req_poll;	/* this changes after initialization */

static unchar	host_id;	/* Host is SCSI ID #7 for Seagate, 6 for FD */
static unchar	swap_status_bits;

static ss_type	*ss_tbl;	/* points to block of "ss" structs */
static ss_type  *ss[MAX_SCSI_ID];

/*
 * host_claimed is -1 if host is available, else it's the SCSI id of the
 *	target that claims the host.
 *
 * host is claimed at start of any of the follwoing:
 *	SCSI bus reset
 *	arbitration for block i/o request
 *	reselect
 *
 * host is released at:
 *	end of SCSI bus reset
 *	completion (successful or not) of block i/o request (ss_finished)
 *	disconnect <-- temporarily disabled
 */
static int	host_claimed;

/*
 * ssload()	- load routine.
 *
 *	Action:	The controller is reset and the interrupt vector is grabbed.
 *		The drive characteristics are set up at this time.
 */
static void ssload()
{
	int erf = 0;  /* 1 if error occurs */
	int i;
	int max_id = -1;
	int num_drives = 0;
	int tbnum;


	/*
	 * Allocate a selector to map into ST0x memory-mapped comm area.
	 */
	ss_base = (paddr_t)((long)(unsigned)SS_BASE << 4);
#ifdef _I386
	ss_fp = map_pv(ss_base, (fsize_t)SS_SEL_LEN);
#else /* _I386 */
	ss_fp = ptov(ss_base, (fsize_t)SS_SEL_LEN);
#endif /* _I386 */
	ss_ram = ss_fp + SS_RAM;

	/*
	 * Primitive test of ST0x RAM.
	 */
	sfword(ss_ram, 0xA55A);
	sfword(ss_ram + 2, 0x3CC3);
	sfword(ss_ram + SS_RAM_LEN - 4, 0xA55A);
	sfword(ss_ram + SS_RAM_LEN - 2, 0x3CC3);
	if (ffword(ss_ram) != 0xA55A		/* fetch a "far" word */
	||  ffword(ss_ram + 2) != 0x3CC3
	||  ffword(ss_ram + SS_RAM_LEN - 4) != 0xA55A
	||  ffword(ss_ram + SS_RAM_LEN - 2) != 0x3CC3) {
		printf("Error - host failed memory test\n");
		erf = 1;
	}

	/*
	 * Set host-dependent constants.
	 */
	switch(NSDRIVE >> 8) {
	case 0x00:	/* ST01/ST02 */
		ss_csr = ss_fp + SS_CSR;
		ss_dat = ss_fp + SS_DAT;
		host_id = 0x80;		/* host is id #7 */
		break;
	case 0x80:	/* TMC-845/850/860/875/885 */
		ss_csr = ss_fp + FD_CSR;
		ss_dat = ss_fp + FD_DAT;
		host_id = 0x40;		/* host is id #6 */
		break;
	case 0x40:	/* TMC-840/841/880/881 */
		ss_csr = ss_fp + SS_CSR;
		ss_dat = ss_fp + SS_DAT;
		host_id = 0x40;		/* host is id #6 */
		swap_status_bits = 1;
		break;
	}
	NSDRIVE &= ~(uint)host_id;

	/*
	 * Allocate drive structs.
	 *
	 * Do a single call to kalloc() then put allocated pieces into
	 * array ss.
	 *
	 * First allocate and clear storage.  Then hook up the pointers.
	 */
	if (!erf) {
		for (i = 0; i < MAX_SCSI_ID; i++)
			if ((NSDRIVE >> i) & 1) {
				max_id = i;
				num_drives++;
			}
		if (num_drives == 0) {
			printf("Error - ss has no valid target id's\n");
			erf = 1;
		} else if ((ss_tbl = kalloc(num_drives*sizeof(ss_type)))
		== NULL) {
			printf("Error - ss can't allocate structs\n");
			erf = 1;
		} else
			kclear(ss_tbl, num_drives * sizeof(ss_type));
	}
	if (!erf) {
		ss_type *foo = ss_tbl;

		for (i = 0; i < MAX_SCSI_ID; i++)
			if ((NSDRIVE >> i) & 1)
				ss[i] = foo++;
	}

	/*
	 * Claim IRQ vector.
	 */
	setivec(SS_INT, ssintr);

	/*
	 * Initialize drives we know about (i.e. in NSDRIVE bitmap).
	 *
	 * Part of this is getting parameters from tboot, if any.
	 * The drive number in tboot's data block must be matched with
	 * the SCSI id in question.  Drive numbering in tboot is assumed
	 * to start with any "at" drives (n_atdr counts these)
	 * then proceed with SCSI drives in increasing id number order.
	 */
	tbnum = n_atdr; /* tboot drive number for first SCSI drive */
	host_claimed = -1;
	bufq_init(max_id + 1);
	max_req_poll = INL_MAX_REQ_POLL;
	if (!erf) {
		for (i = 0; i < MAX_SCSI_ID; i++)
			if ((NSDRIVE >> i) & 1) {
				tbparms(tbnum, i);  /* get tboot parms */
				ssinit(i);
				tbnum++;
			}
	}
	max_req_poll = WKG_MAX_REQ_POLL;
}

/*
 * ssunload()	- unload routine.
 */
static void ssunload()
{
	/*
	 * Deallocate driver heap space.
	 */
	if (ss_tbl)
		kfree(ss_tbl);
	bufq_rlse();

	/*
	 * Free the ST0x selector.
	 */
#ifdef _I386
	unmap_pv(ss_fp);
#else /* _I386 */
	vrelse(ss_fp);
#endif /* _I386 */

	/*
	 * Release IRQ vector.
	 */
	clrivec(SS_INT);
}

/*
 * ssopen()
 *
 *	Input:	dev = disk device to be opened.
 *		mode = access mode [IPR,IPW, IPR+IPW].
 *
 *	Action:	Validate the minor device.
 *		Update the paritition table if necessary.
 */
static void ssopen(dev, mode)
register dev_t	dev;
{
	int drive, partn;
	struct	fdisk_s	*fdp;
	ss_type * ssp;
	int s_id;
	unchar * msg;

	/*
	 * Set up local variables.
	 */
	drive = DEV_SCSI_ID(dev);
	partn = DEV_PARTN(dev);
	s_id = DEV_SCSI_ID(dev);
	ssp = ss[s_id];
	fdp = ssp->parmp;

#if (DEBUG >= 3)
devmsg(dev, "ssopen");
#endif

	/*
	 * LUN must be zero.
	 * SCSI id must have corresponding 1 in NSDRIVE bitmapped variable.
	 */
	if (DEV_LUN(dev) != 0 || ((1 << drive) & NSDRIVE) == 0) {
		msg = "bad LUN or SCSI id";
		u.u_error = ENXIO;
		goto bad_open;
	}

	/*
	 * If "special" bit is set, partition field must be zero.
	 */
	if (DEV_SPECIAL(dev) && partn != 0) {
		msg = "bad special partition";
		u.u_error = ENXIO;
		goto bad_open;
	}

	/*
	 * Subscripting gimmick for partition table.
	 */
	if (dev & SDEV)
		partn = WHOLE_DRIVE;

	/*
	 * If not accessing whole drive and the partition table has not
	 * been read yet, try to read it now.
	 * Do this by calling fdisk() with partition table device on the drive
	 * that is being accessed.
	 */
	if (partn != WHOLE_DRIVE && !(ssp->ptab_read)) {
		int fdisk_dev;

		fdisk_dev = (dev | SDEV) & 0xfff0;

#if (DEBUG >=3)
		devmsg(fdisk_dev, "calling fdisk");
		if (fdisk(fdisk_dev, fdp)) {
			int p;

			fdp[WHOLE_DRIVE].p_size = ssp->capacity;
			fdp[WHOLE_DRIVE].p_base = 0;
			printf("fdisk() succeeded\n");
			for (p=0; p<WHOLE_DRIVE; p++)
	printf("p=%d base=%ld size=%ld\n", p, fdp[p].p_base, fdp[p].p_size);
			ssp->ptab_read = 1;
		} else {
			printf("fdisk() failed\n");
			u.u_error = ENXIO;
			goto bad_open;
		}
#else
		if (fdisk(fdisk_dev, fdp)) {
			fdp[WHOLE_DRIVE].p_size = ssp->capacity;
			fdp[WHOLE_DRIVE].p_base = 0;
			ssp->ptab_read = 1;
		} else {
			msg = "bad partition table";
			u.u_error = ENXIO;
			goto bad_open;
		}
#endif

	}

	/*
	 * Ensure partition lies within drive boundaries and is non-zero size.
	 */
	if (partn != WHOLE_DRIVE
	&& (fdp[partn].p_base+fdp[partn].p_size) > fdp[WHOLE_DRIVE].p_size) {
		msg = "partition exceeds drive capacity";
#ifdef _I386
		u.u_error = EINVAL;
#else
		u.u_error = EBADFMT;
#endif /* _I386 */
		goto bad_open;
	}

	if (partn != WHOLE_DRIVE && fdp[partn].p_size == 0) {
		msg = "partition not found";
		u.u_error = ENODEV;
		goto bad_open;
	}

	/*
	 * OK to open the device.
	 * Start watchdog timer (if not already started) for the host adapter.
	 */
	++drvl[SCSI_MAJOR].d_time;
	++ssp->dr_watch;
	goto end_open;

bad_open:
	devmsg(dev, msg);
end_open:
	return;
}

/*
 * ssclose()
 */
static void ssclose(dev)
dev_t dev;
{
	ss_type * ssp;
	int s_id;

	s_id = DEV_SCSI_ID(dev);
	ssp = ss[s_id];

	/*
	 * Decrement the number of watchdog timer requests open for host
	 * adapter and for target.
	 */
	--drvl[SCSI_MAJOR].d_time;
	--ssp->dr_watch;

#if (DEBUG >= 3)
devmsg(dev, "ssclose");
#endif

}

/*
 * ssread()	- read a block from the raw disk
 *
 *	Input:	dev = disk device to be written to.
 *		iop = pointer to source I/O structure.
 *
 *	Action:	Invoke the common raw I/O processing code.
 */
static void ssread(dev, iop)
dev_t	dev;
IO	*iop;
{
	T_PIGGY( 0x20, printf("ssread(iop->io.vbase: %x)", iop->io.vbase); );

	ioreq( &dbuf, iop, dev, BREAD, BFRAW|BFBLK|BFIOC );
}

/*
 * sswrite()	- write a block to the raw disk
 *
 *	Input:	dev = disk device to be written to.
 *		iop = pointer to source I/O structure.
 *
 *	Action:	Invoke the common raw I/O processing code.
 */
static void sswrite(dev, iop)
dev_t	dev;
IO	*iop;
{
	T_PIGGY( 0x20, printf("sswrite(iop->io.vbase: %x)", iop->io.vbase); );

	ioreq( &dbuf, iop, dev, BWRITE, BFRAW|BFBLK|BFIOC );
}

/*
 * ssioctl()
 *
 *	Input:	dev = disk device to be operated on.
 *		cmd = input/output request to be performed.
 *		vec = (pointer to) optional argument.
 */
static int ssioctl(dev, cmd, vec)
register dev_t	dev;
int cmd;
char * vec;
{
	int ret = 0;
	hdparm_t hdparm;
	struct	fdisk_s	*fdp;
	int s_id;
	ss_type * ssp;

	s_id = DEV_SCSI_ID(dev);
	ssp = ss[s_id];
	fdp = ssp->parmp;

	switch(cmd) {
	case HDGETA:
		/*
		 * Get hard disk attributes.
		 */
PR3("HDGETA");
		fdp = ssp->parmp;
		*(short *)&hdparm.landc[0] =
		*(short *)&hdparm.ncyl[0] = drv_parm[s_id].ncyl;
		hdparm.nhead = drv_parm[s_id].nhead;
		hdparm.nspt = drv_parm[s_id].nspt;
#if (DEBUG >= 3)
printf("ncyl=%d nhead=%d nspt=%d\n",
  hdparm.ncyl[0]+((int)hdparm.ncyl[1]<<8), (int)hdparm.nhead, (int)hdparm.nspt);
#endif
		kucopy(&hdparm, vec, sizeof hdparm);
		ret = 0;
		break;
	case HDSETA:
		/*
		 * Set hard disk attributes.
		 */
PR3("HDSETA");
		fdp = ssp->parmp;
		ukcopy(vec, &hdparm, sizeof hdparm);
		drv_parm[s_id].ncyl = *(short *)&hdparm.ncyl[0];
		drv_parm[s_id].nhead = hdparm.nhead;
		drv_parm[s_id].nspt = hdparm.nspt;
#if (DEBUG >= 3)
printf("ncyl=%d nhead=%d nspt=%d\n",
  hdparm.ncyl[0]+((int)hdparm.ncyl[1]<<8), (int)hdparm.nhead, (int)hdparm.nspt);
#endif
		ret = 0;
		break;
	default:
		u.u_error = EINVAL;
		ret = -1;
	}

	return ret;
}

/*
 * ssblock()	- queue a block to the disk
 *
 *	Input:	bp = pointer to block to be queued.
 *
 *	Action:	Queue a block to the disk.
 *		Make sure that the transfer is within the disk partition.
 */
static void ssblock(bp)
register BUF	*bp;
{
	struct	fdisk_s	*fdp;
	int partition, drive, s_id;
	dev_t dev;
	ss_type * ssp;
	unchar * msg = NULL;

	T_PIGGY( 0x20,
		printf("ssblock(bp->b_vaddr: %x, bp->b_paddr: %x)",
			bp->b_vaddr, bp->b_paddr);
	);

	/*
	 * Set up local variables.
	 */
	dev = bp->b_dev;
	partition = DEV_PARTN(dev);
	drive = DEV_DRIVE(dev);
	s_id = DEV_SCSI_ID(dev);
	ssp = ss[s_id];
	if (dev & SDEV)
		partition = WHOLE_DRIVE;
	fdp = ssp->parmp;

	bp->b_resid = bp->b_count;
#if (DEBUG >= 2)
if (bp->b_count != BSIZE)
	printf("b_count=%d ", bp->b_count);
#endif

	/*
	 * Range check disk region.
	 */
	if (!(ssp->ptab_read)) {
		if ( partition == WHOLE_DRIVE ) {
#if 0
/* Why did we only allow people to access the first block of WHOLE_DRIVE?
   in cases where there was not a valid partition table? */
			if ((bp->b_bno != 0) || (bp->b_count != BSIZE)) {
				msg = "invalid request";
				bp->b_flag |= BFERR;
				goto bad_blk;
			}
#endif
		} else {
			msg = "no partition table";
			bp->b_flag |= BFERR;
			goto bad_blk;
		}
	}

	/*
	 * Check for read at end of partition.
	 * (Need to return with b_resid = BSIZE to signal end of volume.)
	 */
	else if ((bp->b_req == BREAD) && (bp->b_bno == fdp[partition].p_size)) {
		goto bad_blk;
	}

	/*
	 * Check for read past end of partition.
	 */
	else if ( (bp->b_bno + (bp->b_count/BSIZE))
	> fdp[partition].p_size ) {
		msg = "partition overrun";
		bp->b_flag |= BFERR;
		goto bad_blk;
	}

	/*
	 * Fail if request is for zero bytes or is not even # of blocks.
	 */
	if ((bp->b_count % BSIZE) || bp->b_count == 0) {
		msg = "invalid byte count";
		bp->b_flag |= BFERR;
		goto bad_blk;
	}

	/*
	 * Operation appears valid.
	 * Fill fields in the node and queue the request.
	 */
	bufq_wr_tail(s_id, bp);
	ss_mach(s_id);
	goto end_blk;

	/*
	 * Operation cannot be done.  Release the kernel buffer structure.
	 * Value of "bp->b_flag" tells caller if error occurred.
	 */
bad_blk:
	if (msg)
		devmsg(dev, msg);
	bdone(bp);

end_blk:
	return;
}

/*
 * ssintr()	- Interrupt routine.
 *
 * If we have been reselected by a recognized target device
 *	let kernel get out of interrupt mode (defer) and do SCSI
 *	reconnect stuff.
 */
static void ssintr()
{
	int s_id;

	s_id = chk_reconn();
	if (s_id != -1) {
		if (ss[s_id]->state == SST_POLL_RESELECT)
			defer(ss_mach, s_id);
		else
			defer(dummy_reconn, s_id);
PR3("!");
	}
}

/*
 * dummy_reconn()
 *
 * Somehow we are in a state where the driver software does not expect
 * a reconnect but a device is trying one anyway.  Go thru the motions
 * of reconnect because not servicing a hanging reselect seems to leave
 * the target hung - in such a way that it fails to respond to reset
 * messages and to reset on the SCSI bus.
 */
static void dummy_reconn(s_id)
int s_id;
{
	int bus_timeout;
	unchar phase_type;
	int s;
	int msg_in;
	int cmdstat;
	int xfer_good = 1;
PR1("DUM");
	if (ss[s_id]->state == SST_POLL_RESELECT) {
		defer(ss_mach, s_id);
		goto dum_done;
	}
	if (!rsel_handshake())
		goto dum_done;

	s = sphi();
	while (req_wait(&bus_timeout) && xfer_good) {
		phase_type = ffbyte(ss_csr) & (RS_MESSAGE|RS_I_O|RS_CTRL_DATA);
		switch (xpmod(phase_type)) {
		case XP_MSG_IN:
			msg_in = ffbyte(ss_dat);
			switch(msg_in){
			case MSG_CMD_CMPLT:
			case MSG_DISCONNECT:
				sfbyte(ss_csr, WC_ENABLE_PRTY | WC_ENABLE_IRPT);
				break;
			}
			break;
		case XP_MSG_OUT:
			sfbyte(ss_dat, MSG_NOP);
			sfbyte(ss_csr, WC_ENABLE_PRTY | WC_ENABLE_SCSI);
			break;
		case XP_STAT_IN:
			cmdstat = ffbyte(ss_dat);
			break;
		case XP_CMD_OUT:
		case XP_DATA_OUT:
			xfer_good = 0;
			break;
		case XP_DATA_IN:
			ffbyte(ss_dat);
			break;
		default:
			break;
		} /* endswitch */
	} /* endwhile */
	spl(s);

dum_done:
	return;
}

/*
 * sswatch()
 *
 * Invoked once per second if any devices going through this driver are open.
 * Poll for any reselect, in case interrupt got lost.
 */
static void sswatch()
{
	int s_id;
	ss_type * ssp;

	for (s_id = 0; s_id < MAX_SCSI_ID; s_id++) {
		ssp = ss[s_id];
		if (ssp && ssp->dr_watch)
			defer(ss_mach, s_id);
	} /* endfor */
}

/*
 * bus_wait()
 *
 * Wait for specified bit values to appear in Status Register.
 * This uses a tight loop and does not expect to be interrupted.
 *
 * Argument "flags" is a double-byte value;  the high byte is ANDed with
 * status register contents, and the result is tested for equality with
 * the low byte.
 *
 * Return 1 if values wanted appeared, 0 if timeout occurred.
 */
static int bus_wait(flags)
unsigned short flags;
{
	int found, i;
	unsigned char status;

	found = 0;
	for ( i = 0; i < HIPRI_RETRIES; i++) {
		status = ffbyte(ss_csr);
		if ((status & (flags >> 8)) == (flags & 0xff)) {
			found = 1;
			break;
		}
	}

#if (DEBUG >= 1)
	if (!found)
		printf("TO:f=%x s=%x ", flags, status);
#endif

	return found;
}

/*
 * ssinit()
 *
 * Attempt to initialize the (unique) drive with a given SCSI id.
 * Assume only one drive per SCSI id, having LUN = 0.
 *
 * Return 1 if success, 0 if failure.
 */
static int ssinit(s_id)
int s_id;
{
	int retval = 1;
	unchar query_buf[MODESENSELEN];
	ss_type * ssp = ss[s_id];
	int dev = ((sscon.c_mind << 8) | 0x80 | (s_id << 4));

	printf("SCSI ID %d  LUN 0\n", s_id);
	if (retval)
		if (init_call(inquiry, s_id, query_buf)) {
			query_buf[INQUIRYLEN] = 0;
#if (debug >= 2)
			devmsg(dev, query_buf + 8);
#endif
			if (query_buf[0] == 0) {
				retval = 1;
			} else
				devmsg(dev, "Not Direct Access Device");
		} else
			devmsg(dev, "Inquiry Failed");

	if (retval)
		if (init_call(read_cap, s_id, query_buf)) {
			retval = 1;
			ssp->capacity = query_buf[3] | (query_buf[2] << 8)
			| (((long)(query_buf[1])) << 16)
			| (((long)(query_buf[0])) << 24);
			ssp->blocklen = query_buf[7] | (query_buf[6] << 8)
			| (((long)(query_buf[5])) << 16)
			| (((long)(query_buf[4])) << 24);

			printf("Capacity=%ld blocks  Block length=%ld\n",
				ssp->capacity, ssp->blocklen);
		} else
			devmsg(dev, "Read Capacity Failed");

	if (retval)
		if (init_call(mode_sense, s_id, query_buf)) {
			/*
			 * Display physical drive parameters.
			 */
#define FMT_PG	(4+8+8+12)
#define DDG_PG	(4+8+8+12+24)
			unchar heads;
			unsigned short spt;
			ulong cyls;

			spt=((int)query_buf[FMT_PG+10]<<8)
				+ query_buf[FMT_PG+11];
			cyls=((int)query_buf[DDG_PG+2]<<16)
				+ ((int)query_buf[DDG_PG+3]<<8)
				+ query_buf[DDG_PG+4];
			heads=query_buf[DDG_PG+5];

			printf("Physical:  cylinders=%ld ", cyls);
			printf("heads=%d ", heads);
			printf("spt=%d\n", spt);

			if (drv_parm[s_id].ncyl == 0) {
				drv_parm[s_id].ncyl = cyls;
				drv_parm[s_id].nhead = heads;
				drv_parm[s_id].nspt = spt;
			} else {
				printf("Logical:  cylinders=%d ",
					drv_parm[s_id].ncyl);
				printf("heads=%d ", drv_parm[s_id].nhead);
				printf("spt=%d\n", drv_parm[s_id].nspt);
			}
		} else
			devmsg(dev, "Mode Sense Failed");

	return retval;
}

/*
 * far_info_xfer()
 *
 * Do bus cycle information transfer phases.
 * This includes message in/out, command in/out, and data in/out.
 *
 * If cmdlen is nonzero, cmdbuf is an array of bytes of that length,
 * to be sent to the target.
 *
 * Return 1 if bus timeout did not occur, else 0.
 *
 * pseudocode:
 *
 * while (wait for REQ true or BUSY false on SCSI bus)
 *   if (BUSY false)
 *     break from while loop
 *   else
 *     switch (xfer phase = RS_CTRL_DATA|RS_I_O|RS_MESSAGE)
 *       case XP_MSG_IN/XP_MSG_OUT/...
 *         handle the indicated information transfer phase
 *     endswitch
 *   endif
 * endwhile
 */
static int far_info_xfer(s_id)
int s_id;
{
	int bus_timeout;
	unchar phase_type;
	unchar msg_in;
	int s;
	int bytes_to_send;
	ss_type * ssp = ss[s_id];
	BUF * bp = ssp->bp;
	int xfer_good = 1;
	int xfer_count = bp->b_count - bp->b_resid;
	int irpts_masked;
	int block_done=0;

	ssp->cmd_bytes_out = 0;
	ssp->msg_in = -1;

	irpts_masked = 0;
	while (req_wait(&bus_timeout) && xfer_good) {
		phase_type = ffbyte(ss_csr) & (RS_MESSAGE|RS_I_O|RS_CTRL_DATA);
		if (!irpts_masked) {
			s = sphi();
			irpts_masked = 1;
		}
		switch (xpmod(phase_type)) {
		case XP_MSG_IN:
			msg_in = ffbyte(ss_dat);
			switch(msg_in){
			case MSG_CMD_CMPLT:
PR4("Mcc");
				ssp->msg_in = msg_in;
				sfbyte(ss_csr, WC_ENABLE_PRTY | WC_ENABLE_IRPT);
				break;
			case MSG_DISCONNECT:
PR4("Mdc");
				ssp->msg_in = msg_in;
				sfbyte(ss_csr, WC_ENABLE_PRTY | WC_ENABLE_IRPT);
				break;
			case MSG_SAVE_DPTR:
PR4("Msd");
				break;
			case MSG_RSTOR_DPTR:
PR4("Mrd");
				break;
			case MSG_ABORT:
PR4("Mab");
				break;
			case MSG_DEV_RESET:
PR4("Mdr");
				break;
			case MSG_IDENTIFY:
PR4("Mmi");
				break;
			case MSG_IDENT_DC:
PR4("Mmd");
				break;
			}
			break;
		case XP_MSG_OUT:
PR4("MO");
			/*
			 * This case shouldn't happen.  We weren't
			 * asserting ATTENTION.  Abort the bus cycle.
			 */
			sfbyte(ss_dat, MSG_NOP);
			sfbyte(ss_csr, WC_ENABLE_PRTY | WC_ENABLE_SCSI);
			break;
		case XP_STAT_IN:
PR4("SI");
			ssp->cmdstat = ffbyte(ss_dat);
			break;
		case XP_CMD_OUT:
			/*
			 * Ship out command bytes.
			 * Reset SCSI bus if too many command bytes are wanted.
			 */
			bytes_to_send = ssp->cmdlen - ssp->cmd_bytes_out;
			if(bytes_to_send > 0) {
				sfbyte(ss_dat, ssp->cmdbuf[ssp->cmd_bytes_out++]);
				/*
				 * If just sent last byte, allow interrupts.
				 */
				if (bytes_to_send == 1) {
PR4("CO");
					if (bp->b_req == BREAD) {
						if (irpts_masked) {
							spl(s);
							irpts_masked = 0;
						}
					}
				}
			} else {	/* This case should not happen. */
				xfer_good = 0;
			}
			break;
		case XP_DATA_IN:
			/*
			 * If caller's buffer has room, keep incoming
			 * data byte.
			 */
			if (block_done) {
				xfer_good = 0;
PR1("Data in overrun");
			} else if (bp->b_req != BREAD) {
				xfer_good = 0;
			} else {
#if 0
				int getbval;

				block_done=1;
PR4("DI");
				if(getbval = ss_getb(ss_dat,
#ifdef _I386
				bp->b_vaddr + xfer_count)) {
#else
				bp->b_faddr + xfer_count)) {
#endif /* _I386 */
					xfer_good = 0;
#if (DEBUG >= 1)
printf("getb=%d ", getbval);
#endif
				}
#else
				block_done=1;
#ifdef _I386

	    if (BSIZE != xpcopy(ss_dat, bp->b_paddr + xfer_count, 
		BSIZE, SEG_386_KD|SEG_VIRT)) {
		devmsg(bp->b_dev, "XP_DATA_IN: ss_dat: %x, bp->bpaddr: %x, xfer_count: %x\n",
	      		ss_dat, bp->b_paddr, xfer_count);
		break;
	    }
#else
				ffcopy(ss_dat, bp->b_faddr + xfer_count, BSIZE);
#endif /* _I386 */
#endif /* 0 */
			}
			break;
		case XP_DATA_OUT:
			/*
			 * Copy output buffer bytes to data register.
			 */
			if (block_done) {
				xfer_good = 0;
PR1("Data out overrun");
			} else if (bp->b_req != BWRITE) {
				xfer_good = 0;
			} else {
#if 0
				int putbval;
				block_done=1;
PR4("DO");
				if (putbval = ss_putb(ss_dat,
#ifdef _I386
				bp->b_vaddr + xfer_count)) {
#else
				bp->b_faddr + xfer_count)) {
#endif /* _I386 */
					xfer_good = 0;
#if (DEBUG >= 1)
printf("putb=%d ", putbval);
#endif
				}
#else
				block_done=1;
#ifdef _I386
	    if (BSIZE != pxcopy(bp->b_paddr + xfer_count, ss_dat,
	    			BSIZE, SEG_386_KD|SEG_VIRT)) {
		devmsg(bp->b_dev, "XP_DATA_OUT: bp->b_paddr: %x, xfer_count: %x, ss_dat: %x\n",
	      		bp->b_paddr, xfer_count, ss_dat);
		break;
	    }

#else
				ffcopy(bp->b_faddr + xfer_count, ss_dat, BSIZE);
#endif /* _I386 */
#endif
				if (irpts_masked) {
					spl(s);
					irpts_masked = 0;
				}
			}
			break;
		default:
			break;
		} /* endswitch */
	}
	if (irpts_masked)
		spl(s);

#if (DEBUG >= 1)
	switch(ssp->cmdstat) {
	case -1:
		if (msg_in != MSG_DISCONNECT)
			printf("CS-",ssp->cmdstat);
		break;
	case CS_GOOD:
		break;
	case CS_CHECK:
		printf("CSK",ssp->cmdstat);
		break;
	case CS_BUSY:
		printf("CSY",ssp->cmdstat);
		break;
	case CS_RESERVED:
	default:
		printf("CS%x",ssp->cmdstat);
	}
#endif

	return (bus_timeout) ? 0 : 1 ;
}

/*
 * req_wait()
 *
 * This routine is called at the start of each information transfer
 * phase and after the last such phase.
 *
 * It returns 1 if REQ is asserted on the SCSI bus, meaning another phase
 * may begin, and 0 otherwise.  A REQ signal will not be seen if the function
 * times out or if BUSY drops.  A value of 1 is written to the pointer argument
 * if timeout occurred, else 0 is written.
 */
static int req_wait(to_ptr)
int *to_ptr;
{
	int req_found;
	unsigned char status;
	ulong poll_ct;
	int s;

	s = splo();
	*to_ptr = 1;
	req_found = 0;
	for (poll_ct = 0L; poll_ct < max_req_poll; poll_ct++) {
		status = ffbyte(ss_csr);
		if (status & RS_REQUEST) {
			req_found = 1;
			*to_ptr = 0;
			break;
		} else if ((status & RS_BUSY) == 0) {
			*to_ptr = 0;
			break;
		}
	}

#if (DEBUG >= 1)
	if (*to_ptr) {
		printf("TX: s=%x ", status);
	}
#endif

	spl(s);
	return req_found;
}

/*
 * req_sense()
 *
 * Request Sense for a device.  The main reason for doing this is to
 * clear a standing Command Status of Device Check.
 *
 * Full results are discarded.  Return 1 if Device returns No Sense or
 * or Unit Attention.  Else return 0.
 *
 */
static int req_sense(s_id)
int s_id;
{
	unchar sense_buf[SENSELEN];
	unchar cmdbuf[G0CMDLEN];
	int ret = 0;

	cmdbuf[0] = ScmdREQUESTSENSE;
	cmdbuf[1] = 0;
	cmdbuf[2] = 0;
	cmdbuf[3] = 0;
	cmdbuf[4] = SENSELEN;
	cmdbuf[5] = 0;

#if (DEBUG >= 2)
{int i; for (i=0; i<SENSELEN; i++) sense_buf[i]=0;}
#endif

PR2("rqs:");
	if (!start_arb()) {
PR2("NO arb");
#if (DEBUG >= 2)
printf("status=%x ", ffbyte(ss_csr));
#endif
		goto rqs_done;
	}

	if (!host_ident(s_id, 0)) {
PR2("NO host ident");
#if (DEBUG >= 2)
printf("status=%x ", ffbyte(ss_csr));
#endif
		goto rqs_done;
	}

	if(!local_info_xfer(cmdbuf, G0CMDLEN, sense_buf, SENSELEN, NULL, 0)) {
PR2("NO local xfer");
		goto rqs_done;
	} else {
		/*
		 * Return 1 if drive responded with any of these sense keys:
		 *	0x00	No Sense
		 *	0x06	Unit Attention
		 *	0x0B	Aborted Command
		 * In any of the above cases, a retry will likely succeed
		 * without Buse Device Reset or SCSI Bus Reset.
		 */
		switch (sense_buf[2]) {
		case 0x00:
		case 0x06:
		case 0x0B:
			ret = 1;
			break;
		} /* endswitch */
	}

rqs_done:
#if (DEBUG >= 2)
{
	int i;

	for (i=0; i<SENSELEN;i++)
		printf("%x ", sense_buf[i]);
	printf("\n");
}
#endif
	return ret;
}

/*
 * inquiry()
 *
 * Inquiry command for a device.
 * Find out if device is direct access, removable, etc.
 *
 * Put result of inquiry into supplied buffer.
 * Return 1 if command succeeds, else 0.
 */
static int inquiry(s_id, buf)
int s_id;
unchar * buf;
{
	int ret = 0;
	unchar cmdbuf[G0CMDLEN];

	cmdbuf[0] = ScmdINQUIRY;
	cmdbuf[1] = 0;
	cmdbuf[2] = 0;
	cmdbuf[3] = 0;
	cmdbuf[4] = INQUIRYLEN;
	cmdbuf[5] = 0;

	if (start_arb() && host_ident(s_id, 0) &&
	local_info_xfer(cmdbuf, G0CMDLEN, buf, INQUIRYLEN, NULL, 0))
		ret = 1;

	return ret;
}

/*
 * mode_sense()
 *
 * Mode Sense command for a device.
 * Use this to get disk parameters:
 *	number of cylinders
 *	number of heads
 *	number of sectors per track.
 *
 * Put result of mode sense into supplied buffer.
 * Return 1 if command succeeds, else 0.
 */
static int mode_sense(s_id, buf)
int s_id;
unchar * buf;
{
	int ret = 0;
	unchar cmdbuf[G0CMDLEN];

	cmdbuf[0] = ScmdMODESENSE;
	cmdbuf[1] = 0;
	cmdbuf[2] = 0x3F;
	cmdbuf[3] = 0;
	cmdbuf[4] = MODESENSELEN;
	cmdbuf[5] = 0;

	if (start_arb() && host_ident(s_id, 0) &&
	local_info_xfer(cmdbuf, G0CMDLEN, buf, MODESENSELEN, NULL, 0))
		ret = 1;

	return ret;
}

/*
 * read_cap()
 *
 * Read Capacity command for a device.
 *
 * Return 1 if command succeeds, else 0.
 */
static int read_cap(s_id, buf)
int s_id;
unchar * buf;
{
	int ret = 0;
	unchar cmdbuf[G1CMDLEN];

	cmdbuf[0] = ScmdREADCAPACITY;
	cmdbuf[1] = 0;
	cmdbuf[2] = 0;
	cmdbuf[3] = 0;
	cmdbuf[4] = 0;
	cmdbuf[5] = 0;
	cmdbuf[6] = 0;
	cmdbuf[7] = 0;
	cmdbuf[8] = 0;
	cmdbuf[9] = 0;

	if (start_arb() && host_ident(s_id, 0) &&
	local_info_xfer(cmdbuf, G1CMDLEN, buf, READCAPLEN, NULL, 0))
		ret = 1;

	return ret;
}

/*
 * bus_dev_reset()
 *
 * Send Bus Device Reset message to the given SCSI id.
 * Return 1 if host adapter was not busy and no obvious timeouts occurred,
 * else 0.
 */
static int bus_dev_reset(s_id)
{
	int bdr_ok = 1;
	int dev = ((sscon.c_mind << 8) | 0x80 | (s_id << 4));

PR1("BDR");
	if (bdr_ok) {
		/*
		 * Do ST0x arbitration.
		 *
		 * De-assert SCSI enable bit.
		 * Write my SCSI id to port.
		 * Start arbitration.
		 */
		sfbyte(ss_csr, WC_ENABLE_PRTY);
		sfbyte(ss_dat, host_id);
		sfbyte(ss_csr, WC_ENABLE_PRTY | WC_ARBITRATE);

		/*
		 * SCSI spec says there is "no maximum" to the wait for
		 * arbitration complete.
		 */
		if (!bus_wait(RS_ARBIT_COMPL << 8 | RS_ARBIT_COMPL)) {
			bdr_ok = 0;
		}
	}

	/*
	 * Arbitration complete.  Now select, with ATN to allow messages.
	 */
	if (bdr_ok) {
		sfbyte(ss_dat, host_id | (1 << s_id));	/* Write both SCSI id's */
		sfbyte(ss_csr, WC_ENABLE_PRTY | WC_ENABLE_SCSI | WC_ATTENTION | WC_SELECT);

		if (!bus_wait(RS_BUSY << 8 | RS_BUSY))
			bdr_ok = 0;
	}

	if (bdr_ok) {
		sfbyte(ss_csr, WC_ENABLE_PRTY | WC_ENABLE_SCSI | WC_ATTENTION);

		if (!bus_wait(((RS_REQUEST|RS_CTRL_DATA|RS_I_O|RS_MESSAGE) << 8)
		| (RS_REQUEST|RS_CTRL_DATA|RS_MESSAGE)))
			bdr_ok = 0;
	}

	if (bdr_ok) {
		sfbyte(ss_csr, WC_ENABLE_PRTY | WC_ENABLE_SCSI);
		sfbyte(ss_dat, MSG_DEV_RESET);
		if (!bus_wait((0xFF << 8) | 0))
			bdr_ok = 0;
	}

	return bdr_ok;
}

/*
 * chk_reconn()
 *
 * Check SELECT to see if any SCSI device has tried to reconnect to the host
 * adapter.  Called if there is an interrupt, and by the timer in case
 * we somehow lose an interrupt.
 *
 * Return -1 if no reselect detected, or the SCSI ID of the reselecting
 * target if there is one.
 */
static int chk_reconn()
{
	unchar csr, dat;
	int s_id = -1;

	csr = ffbyte(ss_csr);
	if (csr & (RS_SELECT | RS_I_O)) {
		dat = ffbyte(ss_dat);
		if ((dat & host_id) && (dat & NSDRIVE)) {
			dat &= ~host_id;
			s_id = 0;
			while (dat >>=1)
				s_id++;
		}
	}

	return s_id;
}

/*
 * ss_mach()
 *
 *	Gives a distinct state machine for each target device.
 */
void	ss_mach(s_id)
int s_id;
{
	ss_type * ssp = ss[s_id];
	BUF * bp;

	do_sst_op = 1; /* plan to run this routine again in most cases */
	while (do_sst_op) {
		bp = ssp->bp;  /* nonpolled() below can change ssp->bp */
		switch (ssp->state) {
		/*
		 * Polling states execute whether ssp->waiting or not.
		 */
		case SST_POLL_ARBITN:
PR3("XPAR");
			if (ffbyte(ss_csr) & RS_ARBIT_COMPL) {
				ssp->waiting = 0;
				if (host_ident(s_id, 1))
					do_connect(s_id);
				else
					recover(s_id, RV_P_TIMEOUT);
			} else {
				if (ssp->expired) {
					ssp->expired = 0;
					recover(s_id, RV_A_TIMEOUT);
				} else
					do_sst_op = 0;
			}
			break;
		case SST_POLL_RESELECT:
PR3("XPRS");
			if (TGT_RSEL) {
				ssp->waiting = 0;
				if (host_claimed == -1)
					host_claimed = s_id;
				else if (host_claimed != s_id) {
#if (DEBUG >= 1)
	printf("%d->%d ", host_claimed, s_id);
#endif
				}
				if (rsel_handshake()) {
					do_connect(s_id);
				} else {
					recover(s_id, RV_P_TIMEOUT);
				}
			} else  { /* Reselect poll is negative */
				if (ssp->expired) {
					ssp->expired = 0;
					recover(s_id, RV_R_TIMEOUT);
				} else
					do_sst_op = 0;
			}
			break;
		case SST_POLL_BEGIN_IO:
PR3("XPBI");
			if (bp == NULL)
				ssp->state = SST_DEQUEUE;
			else {
				/*
				 * At this point a SCSI command is about to
				 * be initiated.  It may be a retry.
				 */
				if (host_claimed == -1 && BUS_FREE && BUS_FREE) {
					ssp->waiting = 0;
					init_pointers(s_id);
					if (start_arb()) {
						host_claimed = s_id;
						if (host_ident(s_id, 1)) {
							do_connect(s_id);
						} else {
							recover(s_id, RV_P_TIMEOUT);
						}
					} else {
	/*
	 * If arbitration does not succeed right away, it is usually
	 * because another drive is trying to reselect the host.
	 */
						set_timeout(s_id, DELAY_ARB);
					}
				} else { /* host busy or bus not free */
					int o_id;

					if ((o_id = chk_reconn()) != -1)
						defer(dummy_reconn, s_id);
					++ssp->avl_count;
					if (ssp->avl_count >= MAX_AVL_COUNT)
						recover(s_id, RV_BF_TIMEOUT);
					else
						set_timeout(s_id, DELAY_BSY);
				}
			}
			break;
		default:
			if (ssp->waiting)
				do_sst_op = 0;
			else {
				/*
				 * Nonpolling states execute only if no
				 * target timer is running.
				 */
				nonpolled(s_id);
			}
		} /* endswitch */
	} /* endwhile */
}

/*
 * nonpolled()
 *
 * Part of ss_mach() - handling of nonpolling states is taken out simply
 * for readability.
 */
static void nonpolled(s_id)
int s_id;
{
	ss_type * ssp = ss[s_id];
	BUF * bp = ssp->bp;
	struct	fdisk_s	*fdp;
	int partition;
	dev_t dev;

	switch (ssp->state) {
	case SST_BUS_DEV_RESET:
PR3("XBDR");
		if (bus_dev_reset(s_id)) {
			do_sst_op = 0;
			set_timeout(s_id, DELAY_BDR);
			ssp->state = SST_REQ_SENSE;
		} else
			recover(s_id, RV_P_TIMEOUT);
		break;
	case SST_DEQUEUE:
		if(bufq_rd_head(s_id) != NULL && !ssp->busy) {
PR3("XDQU");
			ssp->busy = 1;
			bp = bufq_rm_head(s_id);
			ssp->bp = bp;
			dev = bp->b_dev;
			partition = DEV_PARTN(dev);
			if (dev & SDEV)
				partition = WHOLE_DRIVE;
			fdp = ssp->parmp;
			if (partition != WHOLE_DRIVE)
				ssp->bno = fdp[partition].p_base + bp->b_bno;
			else
				ssp->bno = bp->b_bno;
			if (bp->b_req == BREAD)
				ssp->cmdbuf[0] = ScmdREADEXTENDED;
			else
				ssp->cmdbuf[0] = ScmdWRITEXTENDED;
			ssp->cmdbuf[1] = 0;
			ssp->cmdbuf[2] = ssp->bno >> 24;
			ssp->cmdbuf[3] = ssp->bno >> 16;
			ssp->cmdbuf[4] = ssp->bno >>  8;
			ssp->cmdbuf[5] = ssp->bno;
			ssp->cmdbuf[6] = 0;
			ssp->cmdbuf[7] = 0;
			ssp->cmdbuf[8] = 1;
			ssp->cmdbuf[9] = 0;
			ssp->cmdlen = G1CMDLEN;
			init_pointers(s_id);
			ssp->bdr_count = 0;
			ssp->bsy_count = 0;
			ssp->try_count = 0;
			ssp->state = SST_POLL_BEGIN_IO;
		} else /* queue is empty or ssp->busy */
			do_sst_op = 0;
		break;
	case SST_HIPRI_RESET:
	case SST_LOPRI_RESET:
PR1("XRST");
		/*
		 * SST_LOPRI_RESET is same as SST_HIPRI_RESET for now.
		 * Later, can implement a delay to allow other targets to
		 * finish pending operations.
		 */
		if (host_claimed == s_id || host_claimed == -1) {
			host_claimed = s_id;
			sfbyte(ss_csr, WC_ENABLE_PRTY | WC_ENABLE_SCSI | WC_SCSI_RESET); /* reset ON */
			ssp->state = SST_RESET_OFF;
			set_timeout(s_id, DELAY_RST);
PR1("+");
		} else
			set_timeout(s_id, DELAY_RST);
		break;
	case SST_REQ_SENSE:
PR1("XRQS");
		/*
		 * Come here at end of SCSI Bus reset (and at other times).
		 * If we have host claimed, release it.
		 */
		if (host_claimed == s_id)
			host_claimed = -1;
		if (req_sense(s_id))
			ssp->state = SST_POLL_BEGIN_IO;
		else
			recover(s_id, RV_P_TIMEOUT);
		break;
	case SST_RESET_OFF:
PR3("XRFF");
		sfbyte(ss_csr, WC_ENABLE_PRTY); /* reset OFF */
		ssp->state = SST_REQ_SENSE;
		set_timeout(s_id, DELAY_RST);
	} /* endswitch */
}

/*
 * start_arb()
 *
 * return 1 if host adapter returned Arbitration Complete within allotted
 * number of tries, else 0
 */
static int start_arb()
{
	int ret = 0;
	int poll_ct;

	sfbyte(ss_csr, WC_ENABLE_PRTY);
	sfbyte(ss_dat, host_id);
	sfbyte(ss_csr, WC_ENABLE_PRTY | WC_ARBITRATE);

	/*
	 * SCSI spec says there is "no maximum" to the wait for arbitration
	 * complete.
	 */
	for (poll_ct = 0; poll_ct < HIPRI_RETRIES; poll_ct++) {
		if (ffbyte(ss_csr) & RS_ARBIT_COMPL) {
			ret = 1;
			break;
		} else if (chk_reconn() != -1) {
			sfbyte(ss_csr, WC_ENABLE_PRTY);
			break;
		}
	}
#if (DEBUG >= 1)
if (!ret)
	PR1("oSA");
#endif
	return ret;
}

/*
 * host_ident()
 *
 * This routine is the bridge in a SCSI bus cycle between Abitration
 * Complete and the Information Transfer phases.
 *
 * return 1 if everything went ok, 0 in case of timeout
 */
static int host_ident(s_id, disconnect)
int s_id;
int disconnect;
{
	int ret = 0;

	/*
	 * Arbitration complete.  Now select, with ATN to allow messages.
	 */
	sfbyte(ss_dat, host_id | (1 << s_id));	/* Write both SCSI id's */
	sfbyte(ss_csr, WC_ENABLE_PRTY | WC_ENABLE_SCSI | WC_ATTENTION | WC_SELECT);

	if (bus_wait(RS_BUSY << 8 | RS_BUSY)) {
		/*
		 * Assert ATTN so target expects incoming message byte.
		 */
		sfbyte(ss_csr, WC_ENABLE_PRTY | WC_ENABLE_SCSI | WC_ATTENTION);

		if (bus_wait(((RS_REQUEST|RS_CTRL_DATA|RS_I_O|RS_MESSAGE) << 8)
		| (RS_REQUEST|RS_CTRL_DATA|RS_MESSAGE))) {
			if (disconnect) {
				sfbyte(ss_dat, MSG_IDENT_DC);
			} else {
				sfbyte(ss_dat, MSG_IDENTIFY);
			}
			sfbyte(ss_csr, WC_ENABLE_PRTY | WC_ENABLE_SCSI | WC_ENABLE_IRPT);
			ret = 1;
		} else {
PR1("oHI2");
		}
	} else {
PR1("oHI1");
	}
	return ret;
}

/*
 * rsel_handshake()
 *
 * After Reselect is detected, a couple steps are needed before entering
 * Information Transfer phases.  This routine does those steps.
 *
 * return 1 if ok, 0 in case of timeout.
 */
static int rsel_handshake()
{
	int ret = 0;

	sfbyte(ss_csr, WC_ENABLE_PRTY | WC_ENABLE_SCSI | WC_BUSY);
	if (bus_wait(RS_SELECT << 8 | 0)) {
		sfbyte(ss_csr, WC_ENABLE_PRTY | WC_ENABLE_SCSI);
		ret = 1;
	}
	return ret;
}

/*
 * set_timeout()
 *
 * Start a timer so as not to wait forever in case something goes wrong while
 * waiting for an event.  Available delays are:
 *
 * 	DELAY_ARB -	wait for arbitration complete
 * 	DELAY_BDR -	allow settling time after Bus Device Reset
 * 	DELAY_BSY -	wait for not HOST_BUSY and bus free
 * 	DELAY_RES -	wait for reselect by target
 * 	DELAY_RST -	allow settling times when doing SCSI Bus Reset
 *
 * Second argument is number of clock ticks to wait until timer expiration.
 */
static void set_timeout(s_id, delay)
int s_id, delay;
{
	ss_type * ssp = ss[s_id];

	ssp->expired = 0;
	ssp->waiting = 1;
	do_sst_op =  0;
	timeout(&(ssp->tim), delay, stop_timeout, s_id);
}

/*
 * stop_timeout()
 *
 * Called on expiration of the timer for a given target.
 * Don't expire a timer if it's no longer active.
 */
static void stop_timeout(s_id)
int s_id;
{
	ss_type * ssp = ss[s_id];

	if (ssp->waiting) {
		ssp->expired = 1;
		ssp->waiting = 0;
	}
	ss_mach(s_id);
}

/*
 * init_pointers()
 *
 * Initialize command and data pointers when starting (or restarting)
 * a block i/o command.
 */
static void init_pointers(s_id)
int s_id;
{
	ss_type * ssp = ss[s_id];
	BUF * bp = ssp->bp;

	ssp->cmdstat = -1;
	ssp->cmd_bytes_out = 0;
	ssp->avl_count = 0;
}

/*
 * recover()
 *
 * This routine is called directly or indirectly from ss_mach().  It
 * determines what to do when the interface fails to behave as desired.
 *
 * Arguments are the SCSI id of the target HDC and an error type.
 * Error types are:
 *
 * RV_A_TIMEOUT (arbitration timeout)
 * Host adapter takes too long to respond with arbitration complete.
 *
 * RV_P_TIMEOUT (protocol timeout)
 * Timeout waiting for desired SCSI bus status while connected to a target.
 *
 * RV_R_TIMEOUT (reconnect timeout)
 * Timeout after target disconnects, waiting for reconnect.
 *
 * RV_BF_TIMEOUT (bus free timeout)
 * Waited too long for host not busy and BUS_FREE.
 *
 * RV_CS_BUSY (target device busy)
 * Command status returned was Busy.
 *
 * RV_CS_CHECK (target device check)
 * Command status returned was CHECK.
 *
 * Whenever an error occurs, one of the above inputs, together with the SCSI id
 * of the target, is sent to the recovery process.  The recovery process in turn
 * programs the next state for the machine.
 */
static void recover(s_id, errtype)
int s_id;
RV_TYPE errtype;
{
	ss_type * ssp = ss[s_id];
	BUF * bp = ssp->bp;

#if (DEBUG >= 1)
int foo;
if ((foo=chk_reconn()) != -1)
	printf("HONK%d ", foo);
#endif

	++ssp->try_count;
	if (ssp->try_count < MAX_TRY_COUNT) {

		switch (errtype) {

		case RV_CS_BUSY:
			++ssp->bsy_count;
			if (ssp->bsy_count < MAX_BSY_COUNT) {
				ssp->state = SST_POLL_BEGIN_IO;
				set_timeout(s_id, DELAY_BSY);
			} else
				ssp->state = SST_BUS_DEV_RESET;
			break;

		case RV_CS_CHECK:
			ssp->state = SST_REQ_SENSE;
			break;

		case RV_P_TIMEOUT:
			/* fall thru */
		case RV_R_TIMEOUT:
			++ssp->bdr_count;
			if (ssp->bdr_count < MAX_BDR_COUNT)
				ssp->state = SST_BUS_DEV_RESET;
			else
				ssp->state = SST_LOPRI_RESET;
			break;

		case RV_BF_TIMEOUT:
			/* fall thru */
		case RV_A_TIMEOUT:
			ssp->state = SST_HIPRI_RESET;
		}
	} else { /* try_count >= MAX_TRY_COUNT */
		if (bp) {
			bp->b_flag |= BFERR;
			printf("(%d,%d): ", major(bp->b_dev), minor(bp->b_dev));
			printf("%s error bno=%ld\n",
				(bp->b_req == BREAD) ? "read" : "write",
				bp->b_bno);
		}
		ss_finished(s_id);
	}
}

/*
 * ss_finished
 *
 * Release current i/o buffer to the O/S.
 */
static void ss_finished(s_id)
int s_id;
{
	ss_type * ssp = ss[s_id];
	BUF * bp = ssp->bp;
	int go_again = 1;

	if (host_claimed == s_id)
		host_claimed = -1;
	ssp->busy = 0;
	if (bp) {
		if (!(bp->b_flag & BFERR))
			bp->b_resid -= BSIZE;
		if ((bp->b_flag & BFERR) || bp->b_resid == 0) {
			ssp->bp = NULL;
			bdone(bp);
			go_again = 0;
		}
	}
	if (go_again) {
		ssp->state = SST_POLL_BEGIN_IO;
		ssp->bdr_count = 0;
		ssp->bsy_count = 0;
		ssp->try_count = 0;

		ssp->bno++;
		ssp->cmdbuf[2] = ssp->bno >> 24;
		ssp->cmdbuf[3] = ssp->bno >> 16;
		ssp->cmdbuf[4] = ssp->bno >>  8;
		ssp->cmdbuf[5] = ssp->bno;
	} else {
		/*
		 * After processing a kernel i/o request, stop the
		 * state machine for the current id.  Then start
		 * this or some other machine which has a request
		 * pending.
		 */
		do_sst_op =  0;
		ssp->state = SST_DEQUEUE;
		next_req(s_id);
	}
}

/*
 * next_req()
 *
 * Given the SCSI id where an i/o request just completed, start handling
 * another i/o request - which may be for the same or other SCSI id.
 * For now, use round-robin scheduling.
 */
static void next_req(s_id)
int s_id;
{
	int next_id = s_id;

	while (1) {
		next_id++;
		if (next_id >= MAX_SCSI_ID)
			next_id = 0;
		if (ss[next_id]
		&& (ss[next_id]->state != SST_DEQUEUE || bufq_rd_head(next_id))) {
			defer(ss_mach, next_id);
			break;
		}
		if (next_id == s_id)
			break;
	}
}

/*
 * do_connect()
 *
 * This function is called when the host is successfully connected to
 * the target.  It invokes information transfer protocol and then sets
 * up some sort of recovery unless the command completed successfully
 * or there was a normal disconnect.
 */
static void do_connect(s_id)
int s_id;
{
	int result;
	ss_type * ssp = ss[s_id];

	result = far_info_xfer(s_id);
	if (!result)
		recover(s_id, RV_P_TIMEOUT);
	else if (ssp->msg_in == MSG_DISCONNECT) {
		ssp->state = SST_POLL_RESELECT;
		set_timeout(s_id, DELAY_RES);
#if 0
		if (host_claimed == s_id)
			host_claimed = -1;
#endif
	} else if (ssp->msg_in == MSG_CMD_CMPLT && ssp->cmdstat == CS_GOOD)
		ss_finished(s_id);
	else if (ssp->cmdstat == CS_BUSY)
		recover(s_id, RV_CS_BUSY);
	else if (ssp->cmdstat == CS_CHECK)
		recover(s_id, RV_CS_CHECK);
	else  /* something else went wrong */
		recover(s_id, RV_P_TIMEOUT);
}

/*
 * local_info_xfer()
 *
 * Do bus cycle information transfer phases.
 * Transfer is for a command which will produce local results in the driver.
 * Other ...info_xfer routine handles kernel block i/o commands.
 *
 * Return 1 if transfer succeeded, else 0.
 *
 */
static int local_info_xfer(cmdbuf, cmdlen, inbuf, inlen, outbuf, outlen)
unchar * cmdbuf, * inbuf, * outbuf;
uint cmdlen, inlen, outlen;
{
	int bus_timeout;
	unchar phase_type;
	int s;
	int cmd_bytes_out = 0;
	int data_bytes_in = 0;
	int data_bytes_out = 0;
	int ret = 0;
	int xfer_good = 1;
	int cmdstat = -1;
	int msg_in = -1;
#if (DEBUG >= 1)
int x, xct=0;
unchar xch[100];
#endif

	s = sphi();
	while (req_wait(&bus_timeout) && xfer_good) {
		phase_type = ffbyte(ss_csr) & (RS_MESSAGE|RS_I_O|RS_CTRL_DATA);
#if (DEBUG >= 1)
if (xct < 100)
	xch[xct++]=phase_type;
#endif
		switch (xpmod(phase_type)) {
		case XP_MSG_IN:
			msg_in = ffbyte(ss_dat);
			switch(msg_in){
			case MSG_CMD_CMPLT:
			case MSG_DISCONNECT:
				sfbyte(ss_csr, WC_ENABLE_PRTY | WC_ENABLE_IRPT);
				break;
			}
			break;
		case XP_MSG_OUT:
			/*
			 * This case shouldn't happen.  We weren't
			 * asserting ATTENTION.
			 */
			sfbyte(ss_dat, MSG_NOP);
			sfbyte(ss_csr, WC_ENABLE_PRTY | WC_ENABLE_SCSI);
			break;
		case XP_STAT_IN:
			cmdstat = ffbyte(ss_dat);
			break;
		case XP_CMD_OUT:
			/*
			 * Ship out command bytes.
			 */
			if (cmd_bytes_out < cmdlen) {
				sfbyte(ss_dat, cmdbuf[cmd_bytes_out++]);
#if 1
				/*
				 * If just sent last byte, allow interrupts.
				 */
				if (cmd_bytes_out == cmdlen) {
					spl(s);
					s = sphi();
				}
#endif
			} else {	/* This case should not happen. */
				xfer_good = 0;
			}
			break;
		case XP_DATA_IN:
			/*
			 * If caller's buffer has room, keep incoming
			 * data byte.  Else toss it.
			 */
			if (data_bytes_in < inlen) {
#if 0
				do {
					inbuf[data_bytes_in++] = ffbyte(ss_dat);
				} while (data_bytes_in < inlen);
#else
				inbuf[data_bytes_in++] = ffbyte(ss_dat);
#endif
			} else
				xfer_good = 0;
			break;
		case XP_DATA_OUT:
			/*
			 * Copy output buffer bytes to data register.
			 */
			if (data_bytes_out < outlen) {
				sfbyte(outbuf[data_bytes_out++], ss_dat);
			} else { /* This case should not happen. */
				xfer_good = 0;
			}
			break;
		default:
			break;
		} /* endswitch */
	}
	spl(s);

	if (bus_timeout) {
PR1("oLX1");
	} else if (!xfer_good) {
PR1("oLX2");
	} else if (cmdstat != CS_GOOD) {
PR1("oLX3");
#if (DEBUG >= 1)
printf("cmdstat=%x ", cmdstat);
#endif
	} else
		ret = 1;
#if (DEBUG >= 1)
if (!ret) {
	printf("csr=%x ", ffbyte(ss_csr));
	printf("xct=%d  ", xct);
	for (x=0; x < xct; x++)
		printf("%x ", xch[x]);
}
#endif

	return ret;
}

/*
 * scsireset()
 *
 * Reset the SCSI bus.
 * Allow settling time when turning reset on/off.
 * Settling times were determined empirically.
 * Each tick is 10 msec.
 */
static void scsireset()
{
	int s;

#if (DEBUG >= 1)
printf("scsireset ");
#endif
	s = splo();
	sfbyte(ss_csr, WC_ENABLE_PRTY | WC_ENABLE_SCSI | WC_SCSI_RESET);
	ssdelay(RESET_TICKS);
	sfbyte(ss_csr, WC_ENABLE_PRTY);
	ssdelay(RESET_TICKS);
	spl(s);
}

/*
 * ssdelay()
 *
 * Delay for some number of arbitrary ticks.
 *
 * Using sleep() causes a panic if this driver is linked to the kernel,
 * even though this routine is called only via ssload().
 */
static void ssdelay(ticks)
int ticks;
{
#if 0
	timeout(&delay_tim, ticks, wakeup, (int)&delay_tim);
	sleep((char *)&delay_tim, CVPAUSE, IVPAUSE, SVPAUSE);
#else
	int i, j;

	for (i = 0; i < ticks; i++)
		for (j = 0; j < LOAD_DELAY; j++);
#endif
}

/*
 * init_call()
 *
 * Call SCSI command function during initialization, with error recovery.
 * If the simple command fails, try a Bus Device Reset, then SCSI Bus reset.
 */
static int init_call(fn, s_id, buf)
int (*fn)();
int s_id;
unchar * buf;
{
	int ret = 1;
	int i;
	int o_id;
int s;
s=sphi();
	for (i = 0; i < 2; i++) {
		o_id = chk_reconn();
		if (o_id != -1)
			dummy_reconn(s_id);
		if ((*fn)(s_id, buf))
			goto init_call_done;

		req_sense(s_id);
		if ((*fn)(s_id, buf))
			goto init_call_done;

		if (bus_dev_reset(s_id)) {
			ssdelay(RESET_TICKS);
			req_sense(s_id);
			if ((*fn)(s_id, buf))
				goto init_call_done;
		}

		scsireset();
		req_sense(s_id);
		if ((*fn)(s_id, buf))
			goto init_call_done;
	}

	ret = 0;

init_call_done:
spl(s);
	return ret;
}

/*
 * xpmod()
 *
 * Command/Data and Message bits are swapped on-board (outside the chip)
 * on older Future Domain host boards.
 */
static unchar xpmod(oldphase)
unchar oldphase;
{
	unchar ret = oldphase;

	if (swap_status_bits) {
		ret &= ~(RS_CTRL_DATA | RS_MESSAGE);
		if (oldphase & RS_MESSAGE)
			ret |= RS_CTRL_DATA;
		if (oldphase & RS_CTRL_DATA)
			ret |= RS_MESSAGE;
	}
	return ret;
}

/*
 * tbparms()
 *
 * If the drive table has already been patched for this SCSI id, do nothing.
 * Otherwise, given the real-mode drive number (tbnum) and the SCSI id (s_id),
 * look for drive parameters from tertiary boot, and copy into driver
 * data block if we find them.
 */
static void tbparms(tbnum, s_id)
int tbnum, s_id;
{
	FIFO *ffp;
	typed_space *tp;
	extern typed_space boot_gift;

	if (drv_parm[s_id].ncyl == 0
	&& F_NULL != (ffp = fifo_open(&boot_gift, 0))) {

		if (tp = fifo_read(ffp)) {
			BIOS_DISK *bdp = (BIOS_DISK *)tp->ts_data;
			if ((T_BIOS_DISK == tp->ts_type) &&
			    (tbnum == bdp->dp_drive) ) {
				drv_parm[s_id].ncyl = bdp->dp_cylinders;
				drv_parm[s_id].nhead = bdp->dp_heads;
				drv_parm[s_id].nspt = bdp->dp_sectors;
			}
		}
		fifo_close(ffp);
	}
}
