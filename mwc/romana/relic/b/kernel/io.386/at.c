/* (-lgl
 * 	COHERENT Driver Kit Version 1.1.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 *
 -lgl) */
/*
 * This is a driver for the
 * hard disk on the AT.
 *
 * Reads drive characteristics from ROM (thru interrupt vector 0x41 and 0x46).
 * Reads partition information from disk.
 */

/*
 * -----------------------------------------------------------------
 * Includes.
 */
#include	<sys/coherent.h>
#include 	<sys/fdisk.h>
#include	<sys/hdioctl.h>
#include	<sys/buf.h>
#include	<sys/con.h>
#include	<sys/devices.h>
#include	<sys/stat.h>
#include	<sys/typed.h>
#include	<errno.h>

/*
 * -----------------------------------------------------------------
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */
/*
 * Configurable parameters
 */
#define	HDIRQ	14			/* Level 14 */
#define	HDBASE	0x01F0			/* Port base */
#define NDRIVE	2			/* only two drives supported */
#define	SOFTLIM	6			/*  (7) num of retries before diag */
#define	HARDLIM	8			/* number of retries before fail */
#define	BADLIM	100			/* num to stop recov if flagged bad */

#define	BIT(n)		(1 << (n))

#define	CMOSA	0x70			/* write cmos address to this port */
#define	CMOSD	0x71			/* read cmos data through this port */

#ifdef _I386
#  define	ATCACHE 0	/* no cache for us in this driver code */
#  define	VERBOSE	1
#else
#  ifndef	ATCACHE
#    if VERBOSE > 0
#      define	ATCACHE	2	/* local cache size in blocks */
#    else
#      define	ATCACHE 0	/* no cache for small code */
#    endif
#  endif
#endif

/*
 * I/O Port Addresses
 */
#define	DATA_REG	(HDBASE+0)	/* data (r/w) */
#define	AUX_REG		(HDBASE+1)	/* error(r), write precomp cyl/4 (w) */
#define	NSEC_REG	(HDBASE+2)	/* sector count (r/w) */
#define	SEC_REG		(HDBASE+3)	/* sector number (r/w) */
#define	LCYL_REG	(HDBASE+4)	/* low cylinder (r/w) */
#define	HCYL_REG	(HDBASE+5)	/* high cylinder (r/w) */
#define	HDRV_REG	(HDBASE+6)	/* drive/head (r/w) (D<<4)+(1<<H) */
#define	CSR_REG		(HDBASE+7)	/* status (r), command (w) */
#define	HF_REG		(HDBASE+0x206)	/* Usually 0x3F6 */

/*
 * Error from AUX_REG (r)
 */
#define	DAM_ERR		BIT(0)		/* data address mark not found */
#define	TR0_ERR		BIT(1)		/* track 000 not found */
#define	ABT_ERR		BIT(2)		/* aborted command */
#define	ID_ERR		BIT(4)		/* id not found */
#define	ECC_ERR		BIT(6)		/* data ecc error */
#define	BAD_ERR		BIT(7)		/* bad block detect */

/*
 * Status from CSR_REG (r)
 */
#define	ERR_ST		BIT(0)		/* error occurred */
#define	INDEX_ST	BIT(1)		/* index pulse */
#define	SOFT_ST		BIT(2)		/* soft (corrected) ECC error */
#define	DRQ_ST		BIT(3)		/* data request */
#define	SKC_ST		BIT(4)		/* seek complete */
#define	WFLT_ST		BIT(5)		/* improper drive operation */
#define	RDY_ST		BIT(6)		/* drive is ready */
#define	BSY_ST		BIT(7)		/* controller is busy */

/*
 * Commands to CSR_REG (w)
 */
#define	RESTORE(rate)	(0x10+(rate))	/* X */
#define	SEEK(rate)	(0x70+(rate))	/* X */
#define	READ_CMD	(0x20)		/* X */
#define	WRITE_CMD	(0x30)		/* X */
#define	FORMAT_CMD	(0x50)		/* X */
#define	VERIFY_CMD	(0x40)		/* X */
#define	DIAGNOSE_CMD	(0x90)		/* X */
#define	SETPARM_CMD	(0x91)		/* X */

/*
 * Device States.
 */
#define	SIDLE	0			/* controller idle */
#define	SRETRY	1			/* seeking */
#define	SREAD	2			/* reading */
#define	SWRITE	3			/* writing */

/*
 * -----------------------------------------------------------------
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */
extern int	nulldev();
extern int	nonedev();

/*
 * Driver configuration.
 */
static void	atload();
static void	atunload();
static void	atopen();
static void	atread();
static void	atwrite();
static int	atioctl();
static void	atwatch();
static void	atblock();

/*
 * Forward Referenced Functions.
 */
static void	atreset();
static int	atdequeue();
static void	atstart();
static void	atintr();
static void	atdefer();
static int	aterror();
static void	atrecov();
static void	atdone();

/*
 * -----------------------------------------------------------------
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */
extern typed_space	boot_gift;
extern short		n_atdr;

#ifndef _I386
extern saddr_t		sds;
#endif

CON	atcon	= {
	DFBLK|DFCHR,			/* Flags */
	AT_MAJOR,			/* Major index */
	atopen,				/* Open */
	nulldev,			/* Close */
	atblock,			/* Block */
	atread,				/* Read */
	atwrite,			/* Write */
	atioctl,			/* Ioctl */
	nulldev,			/* Powerfail */
	atwatch,			/* Timeout */
	atload,				/* Load */
	atunload			/* Unload */
};

/*
 * Patchable variables.
 *	ATSECS is number of seconds to wait for an expected interrupt.
 *	ATSREG needs to be 3F6 for most new IDE drives;  needs to be
 *		1F7 for Perstor controllers and some old IDE drives.
 *		Either value works with most drives.
 */
int	ATSECS = 6;
int	ATSREG = 0x3F6;

/*
 * Drive Parameters - copied from ROM.
 * If patched, use the given values instead of reading from the ROM.
 * NOTE: Exactly duplicates hdparm_s struct.
 */
struct dparm_s {
	unsigned short	d_ncyl;		/* number of cylinders */
	unsigned char	d_nhead;	/* number of heads */
#pragma align 1
	unsigned short	d_rwcc;		/* reduced write current cyl */
	unsigned short	d_wpcc;		/* write pre-compensation cyl */
#pragma align
	unsigned char	d_eccl;		/* max ecc data length */
	unsigned char	d_ctrl;		/* control byte */
	unsigned char	d_fill2[3];
	unsigned short	d_landc;	/* landing zone cylinder */
	unsigned char	d_nspt;		/* number of sectors per track */
	unsigned char	d_fill3;

}	atparm[ NDRIVE ] = {
	0				/* Initialized to allow patching */
};

/*
 * Partition Parameters - copied from disk.
 *
 *	There are NDRIVE * NPARTN positions for the user partitions,
 *	plus NDRIVE additional partitions to span each drive.
 *
 *	Aligning partitions on cylinder boundaries:
 *	Optimal partition size: 2 * 3 * 4 * 5 * 7 * 17 = 14280 blocks
 *	Acceptable partition size:  3 * 4 * 5 * 7 * 17 =  7140 blocks
 */
static struct fdisk_s pparm[NDRIVE*NPARTN + NDRIVE];

/*
 * Per disk controller data.
 * Only one controller; no more, no less.
 */
static struct	at	{
	BUF		*at_actf;	/* Link to first */
	BUF		*at_actl;	/* Link to last */
#ifdef _I386
	paddr_t		at_addr;	/* Source/Dest virtual address */
#else
	faddr_t		at_addr;
#endif
	daddr_t		at_bno;		/* Block # on disk */
	unsigned	at_nsec;	/* # of sectors on current transfer */
	unsigned	at_drv;
	unsigned	at_head;
	unsigned	at_cyl;
	unsigned	at_sec;
	unsigned	at_partn;
	unsigned char	at_dtype[ NDRIVE ];	/* drive type, 0 if unused */
	unsigned char	at_tries;
	unsigned char	at_state;
	unsigned char	at_caching;		/* caching in progress */
#if	ATCACHE > 0
	unsigned char	at_cdrv[ ATCACHE ];	/* cached drive */
	daddr_t		at_cbno[ ATCACHE ];	/* cached block number */
	unsigned char *	at_cbuf[ ATCACHE ];	/* cached block */
#endif
	unsigned	at_bad_drv;
	unsigned	at_bad_head;
	unsigned	at_bad_cyl;
}	at;

static BUF	dbuf;			/* For raw I/O */

static char timeout_msg[] = "at%d: TO\n";

/**
 *
 * void
 * atload()	- load routine.
 *
 *	Action:	The controller is reset and the interrupt vector is grabbed.
 *		The drive characteristics are set up at this time.
 */
static void
atload()
{
	register unsigned int u;
	register struct dparm_s * dp;
	struct { unsigned short off, seg; } p;

	if (n_atdr <= 0)
		return;

	/* Flag drives 0, 1 as present or not. */
	at.at_dtype[0] = 1;
	at.at_dtype[1] = n_atdr > 1 ? 1 : 0;

#if 0
/* hex dump boot gift */
{
int bgi;
unsigned char * bgp = (char *)&boot_gift;
printf("&boot_gift = %lx", &boot_gift);
for (bgi = 0; bgi < 80; bgi++) {
	printf(" %x", (*bgp++));
}
}
#endif

	/*
	 * Obtain Drive Characteristics.
	 */
	for (u = 0, dp = &atparm[0]; u < n_atdr; ++dp, ++u) {
		struct dparm_s int_dp;

		if (dp->d_ncyl == 0) {
			/*
			 * Not patched.
			 *
			 * If tertiary boot sent us parameters,
			 *   Use "fifo" routines to fetch them.
			 *   This only gives us ncyl, nhead, and nspt.
			 *   Make educated guesses for other parameters:
			 *   Set landc to ncyl, wpcc to -1.
			 *   Set ctrl to 0 or 8 depending on head count.
			 *
			 * Follow INT 0x41/46 to get drive static BIOS drive
			 * parameters, if any.
			 *
			 * If there were no parameters from tertiary boot,
			 * or if INT 0x4? nhead and nspt match tboot parms,
			 *   use "INT" parameters (will give better match on
			 *   wpcc, landc, and ctrl fields, which tboot can't
			 *   give us).
			 */

			FIFO *ffp;
			typed_space *tp;
			int found, parm_int;

if (F_NULL != (ffp = fifo_open(&boot_gift, 0))) {
			for (found = 0; !found && (tp = fifo_read(ffp)); ) {
				BIOS_DISK *bdp = (BIOS_DISK *)tp->ts_data;
				if ((T_BIOS_DISK == tp->ts_type) &&
				    (u == bdp->dp_drive) ) {
					found = 1;
					dp->d_ncyl = bdp->dp_cylinders;
					dp->d_nhead = bdp->dp_heads;
					dp->d_nspt = bdp->dp_sectors;
					dp->d_wpcc = 0xffff;
					dp->d_landc = dp->d_ncyl;
					if (dp->d_nhead > 8)
						dp->d_ctrl |= 8;
				}
			}
			fifo_close(ffp);
}

			if (u == 0)
				parm_int = 0x41;
			else /* (u == 1) */
				parm_int = 0x46;
#ifdef _I386
			pxcopy((paddr_t)(parm_int*4), &p, sizeof p, SEG_386_KD);
			pxcopy((paddr_t)(p.seg<<4L)+p.off,
				&int_dp, sizeof(int_dp), SEG_386_KD);
#else
			pkcopy((paddr_t)(parm_int*4), &p, sizeof p);
			pkcopy((paddr_t) (p.seg << 4L) + p.off,
				&int_dp, sizeof(int_dp));
#endif
			if (!found || 
			    (dp->d_nhead == int_dp.d_nhead
			     && dp->d_nspt == int_dp.d_nspt)) {
			     *dp = int_dp;
				printf("Using INT 0x%x",parm_int);
			} else
				printf("Using INT 0x13(08)");
		} else {
			printf("Using patched");
			/*
			 * Avoid incomplete patching.
			 */
			if (at.at_dtype[u] == 0)
				at.at_dtype[u] = 1;
			if (dp->d_nspt == 0)
				dp->d_nspt = 17;
#if FORCE_CTRL_8
			if (dp->d_nhead > 8)
				dp->d_ctrl |= 8;
#endif

		}
#if VERBOSE > 0
	printf(" drive %d parameters\n", u);

	/* intersegment printf only gets 6 words of arguments */	
	printf( "at%d: ncyl=%d nhead=%d wpcc=%d ",
	  u, dp->d_ncyl, dp->d_nhead, dp->d_wpcc);
	printf(" eccl=%d ctrl=%d landc=%d nspt=%d\n",
	  dp->d_eccl, dp->d_ctrl, dp->d_landc, dp->d_nspt);
#endif
	}

	/*
	 * Initialize Drive Size.
	 */
	for (u = 0, dp = &atparm[0]; u < n_atdr; ++dp, ++u) {

		if (at.at_dtype[u] == 0)
			continue;

		pparm[NDRIVE*NPARTN + u].p_size =
			(long) dp->d_ncyl * dp->d_nhead * dp->d_nspt;
	}

	/*
	 * Initialize Drive Controller.
	 */
	atreset();

	setivec(HDIRQ, atintr);

#if ATCACHE > 0
	at.at_cdrv[0] = -1;
	at.at_cbuf[0] = kalloc(BSIZE);
#endif

#if ATCACHE > 1
	at.at_cdrv[1] = -1;
	at.at_cbuf[1] = kalloc(BSIZE);
#endif

	at.at_bad_drv = -1;
}

/**
 *
 * void
 * atunload()	- unload routine.
 */
static void
atunload()
{
	clrivec(HDIRQ);
}

/**
 *
 * void
 * atreset()	-- reset hard disk controller, define drive characteristics.
 */
static void
atreset()
{
	register int u;
	register struct dparm_s * dp;

	/*
	 * Reset controller for a minimum of 4.8 microseconds.
	 */
	outb(HF_REG, 4);
	for (u = 100; --u != 0;)
		;
	outb(HF_REG, atparm[0].d_ctrl & 0x0F);
	myatbsyw(0);
	if (inb(AUX_REG) != 0x01) {
		/*
		 * Some IDE drives always timeout on initial reset.
		 * So don't report first timeout.
		 */
		static one_bad;

		if (one_bad) {
			printf("at: hd controller reset timeout\n");
		} else
			one_bad = 1;
	}

	/*
	 * Initialize drive parameters.
	 */
	for (u = 0, dp = &atparm[0]; u < n_atdr; ++dp, ++u) {

		if (at.at_dtype[u] == 0)
			continue;

		myatbsyw(u);

		/*
		 * Set drive characteristics.
		 * 0x1F1 - AUX_REG
		 * 0x1F2 - NSEC_REG
		 * 0x1F3 - SEC_REG
		 * 0x1F4 - LCYL_REG
		 * 0x1F5 - HCYL_REG
		 * 0x1F6 - HDRV_REG
		 * 0x1F7 - CSR_REG
		 */
		outb(HF_REG,	dp->d_ctrl);
		outb(AUX_REG,  dp->d_wpcc / 4);
		outb(NSEC_REG, dp->d_nspt);
		outb(SEC_REG, 0x01);
		outb(LCYL_REG, (char)(dp->d_ncyl));
		outb(HCYL_REG, (char)(dp->d_ncyl >> 8));
		outb(HDRV_REG, 0xA0 + (u<<4) + dp->d_nhead - 1);
		outb(CSR_REG,  SETPARM_CMD);
		myatbsyw(u);

		/*
		 * Restore heads.
		 */
		outb(CSR_REG, RESTORE(0));
		myatbsyw(u);
	}
}

/**
 *
 * void
 * atopen(dev, mode)
 * dev_t dev;
 * int mode;
 *
 *	Input:	dev = disk device to be opened.
 *		mode = access mode [IPR,IPW, IPR+IPW].
 *
 *	Action:	Validate the minor device.
 *		Update the paritition table if necessary.
 */
static void
atopen(dev, mode)
register dev_t	dev;
{
	register int d;		/* drive */
	register int p;		/* partition */

	p = minor(dev) % (NDRIVE*NPARTN);

	if (minor(dev) & SDEV) {
		d = minor(dev) % NDRIVE;
		p += NDRIVE * NPARTN;
	}
	else
		d = minor(dev) / NPARTN;

	if ((d >= NDRIVE) || (at.at_dtype[d] == 0)) {
printf("atopen: drive not present ");
		u.u_error = ENXIO;
		return;
	}

	if (minor(dev) & SDEV) {
		return;
	}

	/*
	 * If partition not defined read partition characteristics.
	 */
	if (pparm[p].p_size == 0)
		fdisk(makedev(major(dev), SDEV + d), &pparm[ d * NPARTN ]);

	/*
	 * Ensure partition lies within drive boundaries and is non-zero size.
	 */
	if ((pparm[p].p_base+pparm[p].p_size) > pparm[d+NDRIVE*NPARTN].p_size) {
#ifdef _I386
printf("atopen: p_size too big ");
		u.u_error = EINVAL;
#else
		u.u_error = EBADFMT;
#endif
	} else if (pparm[p].p_size == 0) {
printf("atopen: p_size zero ");
		u.u_error = ENODEV;
	}
}

/**
 *
 * void
 * atread(dev, iop)	- write a block to the raw disk
 * dev_t dev;
 * IO * iop;
 *
 *	Input:	dev = disk device to be written to.
 *		iop = pointer to source I/O structure.
 *
 *	Action:	Invoke the common raw I/O processing code.
 */
static void
atread(dev, iop)
dev_t	dev;
IO	*iop;
{
	ioreq(&dbuf, iop, dev, BREAD, BFRAW|BFBLK|BFIOC);
}

/**
 *
 * void
 * atwrite(dev, iop)	- write a block to the raw disk
 * dev_t dev;
 * IO * iop;
 *
 *	Input:	dev = disk device to be written to.
 *		iop = pointer to source I/O structure.
 *
 *	Action:	Invoke the common raw I/O processing code.
 */
static void
atwrite(dev, iop)
dev_t	dev;
IO	*iop;
{
	ioreq(&dbuf, iop, dev, BWRITE, BFRAW|BFBLK|BFIOC);
}

/**
 *
 * int
 * atioctl(dev, cmd, arg)
 * dev_t dev;
 * int cmd;
 * char * vec;
 *
 *	Input:	dev = disk device to be operated on.
 *		cmd = input/output request to be performed.
 *		vec = (pointer to) optional argument.
 *
 *	Action:	Validate the minor device.
 *		Update the paritition table if necessary.
 */
static int
atioctl(dev, cmd, vec)
register dev_t	dev;
int cmd;
char * vec;
{
	int d;

	/*
	 * Identify drive number.
	 */
	if (minor(dev) & SDEV)
		d = minor(dev) % NDRIVE;
	else
		d = minor(dev) / NPARTN;

	/*
	 * Identify input/output request.
	 */
	switch (cmd) {

	case HDGETA:
		/*
		 * Get hard disk attributes.
		 */
		kucopy(&atparm[d], vec, sizeof(atparm[0]));
		return(0);

	case HDSETA:
		/* Set hard disk attributes. */
		ukcopy(vec, &atparm[d], sizeof(atparm[0]));
		at.at_dtype[d] = 1;		/* set drive type nonzero */
		pparm[NDRIVE * NPARTN + d].p_size =
			(long) atparm[d].d_ncyl * atparm[d].d_nhead * atparm[d].d_nspt;
		atreset();
		return 0;

	default:
		u.u_error = EINVAL;
		return(-1);
	}
}

/**
 *
 * void
 * atwatch()		- guard against lost interrupt
 *
 *	Action:	If drvl[AT_MAJOR] is greater than zero, decrement it.
 *		If it decrements to zero, simulate a hardware interrupt.
 */
static void
atwatch()
{
	register BUF * bp = at.at_actf;
	register int s;

	s = sphi();
	if (--drvl[AT_MAJOR].d_time > 0) {
		spl(s);
		return;
	}
	printf("at%d%c: bno=%U head=%u cyl=%u <Watchdog Timeout>\n",
		at.at_drv,
		(bp->b_dev & SDEV) ? 'x' : at.at_partn % NPARTN + 'a',
		bp->b_bno, at.at_head, at.at_cyl);

	/*
	 * Reset hard disk controller.
	 *
	 * Mark current cylinder as bad so atstart() will fail.
	 * Otherwise would lock up if this track NEVER gives enough IRQ's.
	 */
	at.at_bad_drv	= at.at_drv;
	at.at_bad_head	= at.at_head;
	at.at_bad_cyl	= at.at_cyl;
	atreset();
	atstart();
	spl(s);
}

/**
 *
 * void
 * atblock(bp)	- queue a block to the disk
 *
 *	Input:	bp = pointer to block to be queued.
 *
 *	Action:	Queue a block to the disk.
 *		Make sure that the transfer is within the disk partition.
 */
static void
atblock(bp)
register BUF	*bp;
{
	register struct fdisk_s *pp;
	int partn = minor(bp->b_dev) % (NDRIVE*NPARTN);

	bp->b_resid = bp->b_count;

	if (minor(bp->b_dev) & SDEV)
		partn += NDRIVE * NPARTN;

	pp = &pparm[ partn ];

	/*
	 * Check for read at end of partition.
	 */
	if ((bp->b_req == BREAD) && (bp->b_bno == pp->p_size)) {
		bdone(bp);
		return;
	}

	/*
	 * Range check disk region.
	 */
	if (((bp->b_bno + (bp->b_count/BSIZE)) > pp->p_size)
	|| (bp->b_count % BSIZE) || bp->b_count == 0) {
		bp->b_flag |= BFERR;
		bdone(bp);
		return;
	}

	bp->b_actf = NULL;
	if (at.at_actf == NULL)
		at.at_actf = bp;
	else
		at.at_actl->b_actf = bp;
	at.at_actl = bp;

	if (at.at_state == SIDLE)
		if (atdequeue())
			atstart();
}

/**
 *
 * int
 * atdequeue()		- obtain next disk read/write operation
 *
 *	Action:	Pull some work from the disk queue.
 *
 *	Return:	0 = no work.
 *		* = work to do.
 */
static int
atdequeue()
{
	register BUF * bp;
	register struct fdisk_s * pp;
	unsigned int nspt;

	for (;;) {
		at.at_caching = 0;
		at.at_tries   = 0;

		if ((bp = at.at_actf) == NULL)
			return (0);

		at.at_partn = minor(bp->b_dev) % (NDRIVE*NPARTN);

		if (minor(bp->b_dev) & SDEV) {
			at.at_partn += (NDRIVE*NPARTN);
			at.at_drv  = minor(bp->b_dev) % NDRIVE;
		}
		else
			at.at_drv = minor(bp->b_dev) / NPARTN;
		nspt = atparm[at.at_drv].d_nspt;

		pp = &pparm[ at.at_partn ];
		at.at_bno   = pp->p_base + bp->b_bno;
		at.at_nsec  = bp->b_count / BSIZE;
#ifdef _I386
		at.at_addr = bp->b_paddr;
#else
		at.at_addr = bp->b_faddr;
#endif

#if ATCACHE > 0
		if (bp->b_req == BWRITE) {

			/*
			 * Invalidate cache if write might overlap.
			 */
			if (at.at_nsec > 1) {
				at.at_cdrv[0] = -1;
#if ATCACHE > 1
				at.at_cdrv[1] = -1;
#endif
			}
			else if (at.at_bno == at.at_cbno[0])
				at.at_cdrv[0] = -1;
#if ATCACHE > 1
			else if (at.at_bno == at.at_cbno[1])
				at.at_cdrv[1] = -1;
#endif
		}
		else if (at.at_nsec == 1) {

			/*
			 * Test for cache hit on block 0.
			 */
			if ((at.at_drv == at.at_cdrv[0])
			&&   (at.at_bno == at.at_cbno[0])) {
#ifdef _I386
				xpcopy(at.at_cbuf[0], bp->b_paddr,
					BSIZE, SEG_386_KD+SEG_VIRT);
#else
				kpcopy(at.at_cbuf[0], bp->b_paddr, BSIZE);
#endif
				at.at_actf  = bp->b_actf;
				bp->b_resid = 0;
				bdone(bp);
				continue;
			}

#if ATCACHE > 1
			/*
			 * Test for cache hit on block 1.
			 */
			if ((at.at_drv == at.at_cdrv[1])
			&&   (at.at_bno == at.at_cbno[1])) {
#ifdef _I386
				xpcopy(at.at_cbuf[1], bp->b_paddr,
					BSIZE, SEG_386_KD|SEG_VIRT);
#else
				kpcopy(at.at_cbuf[1], bp->b_paddr, BSIZE);
#endif
				at.at_actf  = bp->b_actf;
				bp->b_resid = 0;
				bdone(bp);
				continue;
			}
#endif

			/*
			 * Enable caching if no backlog for disk i/o.
			 */
			if (bp->b_actf == NULL) {
				/*
				 * Enable caching on single block reads
				 * when at least one block left on same track.
				 */
				at.at_caching = nspt - 1 - (at.at_bno % nspt);
#if ATCACHE > 1
				if (at.at_caching >= 2) {
					at.at_caching   = 2;
					at.at_cdrv[2-1] = -1;
				}
#endif

				if (at.at_caching) {
					at.at_nsec  += at.at_caching;
					at.at_cdrv[1-1] = -1;
				}
			}
		}
#endif

		return (1);
	}
}

/**
 *
 * void
 * atstart()	- start or restart next disk read/write operation.
 *
 *	Action:	Initiate disk read/write operation.
 */
static void
atstart()
{
	register struct dparm_s *dp;

	dp = &atparm[ at.at_drv ];

	at.at_cyl  = (at.at_bno / dp->d_nspt) / dp->d_nhead;
	at.at_head = (at.at_bno / dp->d_nspt) % dp->d_nhead;
	at.at_sec  = (at.at_bno % dp->d_nspt) + 1;

	/*
	 * Check for repeated access to most recently identified bad track.
	 */
	if ((at.at_drv  == at.at_bad_drv)
	  && (at.at_cyl  == at.at_bad_cyl)
	  && (at.at_head == at.at_bad_head)) {
	  	BUF * bp = at.at_actf;
		printf("at%d%c: bno=%U head=%u cyl=%u <Track Flagged Bad>\n",
			at.at_drv,
			(bp->b_dev & SDEV) ? 'x' : at.at_partn % NPARTN + 'a',
			bp->b_bno,
			at.at_head,
			at.at_cyl);
		bp->b_flag |= BFERR;
		atdone(bp);
		return;
	}

	myatbsyw(at.at_drv);

	outb(HF_REG,   dp->d_ctrl);
	outb(AUX_REG,  dp->d_wpcc / 4);
	outb(NSEC_REG, at.at_nsec);
	outb(SEC_REG,  at.at_sec);
	outb(LCYL_REG, at.at_cyl);
	outb(HCYL_REG, at.at_cyl >> 8);
	outb(HDRV_REG, (at.at_drv << 4) + at.at_head + 0xA0);

	if (at.at_actf->b_req == BWRITE) {

		outb(CSR_REG, WRITE_CMD);

		while (atdrq() == 0)
			printf(timeout_msg, at.at_drv);

		atsend(at.at_addr);
		at.at_state = SWRITE;
	}
	else {
		outb(CSR_REG, READ_CMD);
		at.at_state = SREAD;
	}
	drvl[AT_MAJOR].d_time = ATSECS;
}

/**
 *
 * void
 * atintr()	- Interrupt routine.
 *
 *	Clear interrupt then defer actual processing.
 */
static void
atintr()
{
	inb(CSR_REG);		/* clears controller interrupt */
	defer(atdefer, 0);
}

/**
 *
 * void
 * atdefer()	- Deferred service of hard disk interrupt.
 *
 *	Action:	Service disk interrupt.
 *		Transfer required data.
 *		Update state.
 */
static void
atdefer()
{
	register BUF * bp = at.at_actf;

	switch (at.at_state) {

	case SRETRY:
		atstart();
		break;

	case SREAD:
		/*
		 * Check for I/O error before waiting for data.
		 */
		if (aterror()) {
			atrecov();
			break;
		}

		/*
		 * Wait for data, or forever.
		 */
		if (atdrq() == 0)
			printf(timeout_msg, at.at_drv);

#if ATCACHE > 0
		/*
		 * Cache data block.
		 */
		if (at.at_caching == at.at_nsec) {
#ifdef _I386
			atrecv(at.at_cbuf[ at.at_nsec - 1 ]);
#else
			atrecv(at.at_cbuf[ at.at_nsec - 1 ], sds);
#endif
		} else
#endif
		/*
		 * Read data block.
		 */
			atrecv(at.at_addr);

		/*
		 * Check for I/O error after reading data.
		 */
		if (aterror()) {
			atrecov();
			break;
		}

#if ATCACHE > 0
		/*
		 * Validate cached blocks.
		 */
		if (at.at_caching == at.at_nsec) {
			at.at_cbno[ at.at_nsec - 1 ] = at.at_bno;
			at.at_cdrv[ at.at_nsec - 1 ] = at.at_drv;
			at.at_caching--;
		}
		else
#endif
		{
#ifdef _I386
			at.at_addr += BSIZE;
#else
			FP_OFF(at.at_addr) += BSIZE;
#endif
			bp->b_resid -= BSIZE;
		}

		at.at_tries = 0;
		at.at_bno++;

		/*
		 * Check for end of transfer.
		 */
		if (--at.at_nsec == 0)
			atdone(bp);
		break;

	case SWRITE:
		/*
		 * Check for I/O error.
		 */
		if (aterror()) {
			atrecov();
			break;
		}

#ifdef _I386
		at.at_addr += BSIZE;
#else
		FP_OFF(at.at_addr) += BSIZE;
#endif
		bp->b_resid -= BSIZE;
		at.at_tries  = 0;
		at.at_bno++;

		/*
		 * Check for end of transfer.
		 */
		if (--at.at_nsec == 0) {
			atdone(bp);
			break;
		}

		/*
		 * Wait for ability to send data, or forever.
		 */
		while (atdrq() == 0)
			printf(timeout_msg, at.at_drv);

		/*
		 * Send data block.
		 */
		atsend(at.at_addr);
	}
}

/**
 *
 * int
 * aterror()
 *
 *	Action:	Check for drive error.
 *		If found, increment error count and report it.
 *
 *	Return: 0 = No error found.
 *		1 = Error occurred.
 */
static int
aterror()
{
	register BUF * bp = at.at_actf;
	register int csr;
	register int aux;

	if ((csr = inb(ATSREG)) & (ERR_ST|WFLT_ST)) {

		aux = inb(AUX_REG);

		/*
		 * Don't retry or report failures on cache reads.
		 */
#if ATCACHE > 0
		if ((at.at_state == SREAD) && (at.at_caching == at.at_nsec)) {
			at.at_tries = BADLIM;
			return 1;
		}
#endif

		if (aux & BAD_ERR) {
			at.at_tries	= BADLIM;
			at.at_bad_drv	= at.at_drv;
			at.at_bad_head	= at.at_head;
			at.at_bad_cyl	= at.at_cyl;
		}
		else if (++at.at_tries < SOFTLIM)
			return 1;

		printf("at%d%c: bno=%U head=%u cyl=%u",
			at.at_drv,
			(bp->b_dev & SDEV) ? 'x' : at.at_partn % NPARTN + 'a',
			(bp->b_count/BSIZE) + bp->b_bno
				+ at.at_caching - at.at_nsec,
			at.at_head, at.at_cyl);

#if VERBOSE > 0
		if ((csr & RDY_ST) == 0)
			printf(" <Drive Not Ready>");
		if (csr & WFLT_ST)
			printf(" <Write Fault>");

		if (aux & DAM_ERR)
			printf(" <No Data Addr Mark>");
		if (aux & TR0_ERR)
			printf(" <Track 0 Not Found>");
		if (aux & ID_ERR)
			printf(" <ID Not Found>");
		if (aux & ECC_ERR)
			printf(" <Bad Data Checksum>");
		if (aux & ABT_ERR)
			printf(" <Command Aborted>");
#else
		if ((csr & (RDY_ST|WFLT_ST)) != RDY_ST)
			printf(" csr=%x", csr);
		if (aux & (DAM_ERR|TR0_ERR|ID_ERR|ECC_ERR|ABT_ERR))
			printf(" aux=%x", aux);
#endif
		if (aux & BAD_ERR)
			printf(" <Block Flagged Bad>");

		if (at.at_tries < HARDLIM)
			printf(" retrying...");
		printf("\n");
		return 1;
	}
	return 0;
}

/**
 *
 * void
 * atrecov()
 *
 *	Action:	Attempt recovery.
 */
static void
atrecov()
{
	register BUF *bp = at.at_actf;
	register int cmd = SEEK(0);
	register int cyl = at.at_cyl;

	switch (at.at_tries) {

	case 1:
	case 2:
		/*
		 * Move in 1 cylinder, then retry operation
		 */
		if (--cyl < 0)
			cyl += 2;
		break;

	case 3:
	case 4:
		/*
		 * Move out 1 cylinder, then retry operation
		 */
		if (++cyl >= atparm[ at.at_drv ].d_ncyl)
			cyl -= 2;
		break;

	case 5:
	case 6:
		/*
		 * Seek to cylinder 0, then retry operation
		 */
		cyl = 0;
		break;

	default:
		/*
		 * Restore drive, then retry operation
		 */
		cmd = RESTORE(0);
		cyl = 0;
		break;
	}

	/*
	 * Retry operation [after repositioning head]
	 */
	if (at.at_tries < HARDLIM) {
		drvl[AT_MAJOR].d_time = (cmd == RESTORE(0))
			? (ATSECS * 2) : ATSECS;
		outb(LCYL_REG, cyl);
		outb(HCYL_REG, cyl >> 8);
		outb(HDRV_REG, (at.at_drv << 4) + 0xA0);
		outb(CSR_REG, cmd);
		at.at_state = SRETRY;
	}

	/*
	 * Give up on block.
	 */
	else {
		/*
		 * Not a cache-read error.
		 */
#if ATCACHE > 0
		if ((at.at_state != SREAD) || (at.at_caching != at.at_nsec))
#endif
			bp->b_flag |= BFERR;

		atdone(bp);
	}
}

/**
 *
 * void
 * atdone(bp)
 * BUF * bp;
 *
 *	Action:	Release current i/o buffer to the O/S.
 */
static void
atdone(bp)
register BUF * bp;
{
	drvl[AT_MAJOR].d_time = 0;
	at.at_state = SIDLE;
	at.at_actf  = bp->b_actf;
	bdone(bp);

	if (atdequeue())
		atstart();
}

int
notBusy()
{
	return (inb(ATSREG) & BSY_ST) == 0;
}

int
dataRequested()
{
	return inb(ATSREG) & DRQ_ST;
}

/*
 * Wait while controller is busy.
 *
 * Return 0 if timeout, nonzero if not busy.
 */
int
myatbsyw(unit) int unit;
{
	if (busyWait(notBusy, ATSECS * HZ))
		return 1;
	printf(timeout_msg, unit);
	return 0;
}

/*
 * Wait for controller to initiate request.
 *
 * Return 0 if timeout, 1 if data requested.
 */
int
atdrq()
{
	return busyWait(dataRequested, ATSECS * HZ);
}
