/* (-lgl
 * 	COHERENT Driver Kit Version 1.1.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 *
 * $Log:	ps.c,v $
 * Revision 1.10  91/11/11  12:29:03  hal
 * Use n_atdr.
 * 
 * Revision 1.9  91/10/30  10:47:46  hal
 * Get psparms from tboot.
 *
 * Revision 1.8  91/10/24  12:36:25  hal
 * Bump PSSECS from 4 to 6.
 * Poll HF_REG (3F6) rather than CSR_REG (1F6).
 * COH 3.2.03.
 *
 * Revision 1.7  91/09/11  14:45:38  hal
 * Trial patch for Seagate 157A problems.
 *
 * Revision 1.6  91/09/11  13:23:12  hal
 * Explicit sys in include paths.  AT_MAJOR.
 *
 * Revision 1.5  91/05/22  15:06:59  hal
 * Don't force 8's bit of control byte.
 *
 * Revision 1.4	91/03/14  14:22:32	hal
 *
 -lgl) */
/*
 * This is a driver for the
 * hard disk on the PS.
 *
 * Reads drive characteristics from ROM (thru interrupt vector 0x41 and 0x46).
 * Reads partition information from disk.
 */
#include	<sys/coherent.h>
#include 	<sys/fdisk.h>
#include	<sys/hdioctl.h>
#include	<sys/buf.h>
#include	<sys/con.h>
#include	<sys/devices.h>
#include	<sys/stat.h>
#include	<sys/uproc.h>
#include	<sys/typed.h>
#include 	<sys/timeout.h>			/* TIM */
#include	<sys/sched.h>
#include	<errno.h>

extern	saddr_t	sds;		/* System Data Selector */
extern	short	n_atdr;		/* Number of "ps" drives */

/*
 * Configurable parameters
 */
#define	HDIRQ	14			/* Level 14 */
#define NDRIVE	2			/* only two drives supported */
#define	SOFTLIM	6			/*  (7) num of retrys before diag */
#define	HARDLIM	4			/* number of retrys before fail */
#define	BADLIM	100			/* num to stop recov if flagged bad */

#define	BIT(n)		(1 << (n))

#define	CMOSA	0x70			/* write cmos address to this port */
#define	CMOSD	0x71			/* read cmos data through this port */

/*
 * Driver configuration.
 */
void	psload();
void	psunload();
void	psopen();
void	psread();
void	pswrite();
int	psioctl();
void	pstimer();
void	psblock();
int	nulldev();
int	nonedev();

CON	pscon	= {
	DFBLK|DFCHR,			/* Flags */
	AT_MAJOR,			/* Major index */
	psopen,				/* Open */
	nulldev,			/* Close */
	psblock,			/* Block */
	psread,				/* Read */
	pswrite,			/* Write */
	psioctl,			/* Ioctl */
	nulldev,			/* Powerfail */
	pstimer,			/* Timeout */
	psload,				/* Load */
	psunload			/* Unload */
};

/*
 * Forward Referenced Functions.
 */
void	psreset();
int	psdequeue();
void	psstart();
void	psintr();
int	pserror();
void	psrecov();
void	psdone();
void	disk_function();
void	disk_functionb();

/*
 * Device States.
 */
#define	SIDLE	0			/* controller idle */
#define	SRETRY	1			/* seeking */
#define	SREAD	2			/* reading */
#define	SWRITE	3			/* writing */
#define SRESET  4			/* reseting */

char *smsg[] = { "SIDLE", "SRETRY", "SREAD", "SWRITE", "SRESET" };

/*
 * Drive Parameters - copied from ROM.
 * If patched, use the given values instead of reading from the ROM.
 * NOTE: Exactly duplicates hdparm_s struct.
 */
struct dparm_s {
	unsigned short	d_ncyl;		/* number of cylinders */
	unsigned char	d_nhead;	/* number of heads */
	unsigned short	d_rwcc;		/* reduced write current cyl */
	unsigned short	d_wpcc;		/* write pre-compensation cyl */
	unsigned char	d_eccl;		/* max ecc data length */
	unsigned char	d_ctrl;		/* control byte */
	unsigned char	d_fill2[3];
	unsigned short	d_landc;	/* landing zone cylinder */
	unsigned char	d_nspt;		/* number of sectors per track */
	unsigned char	d_fill3;

}	psparm[ NDRIVE ] = {
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
static
struct fdisk_s pparm[NDRIVE*NPARTN + NDRIVE];

/*
 * Per disk controller data.
 * Only one controller; no more, no less.
 */
static
struct	ps	{
	BUF		*ps_actf;	/* Link to first */
	BUF		*ps_actl;	/* Link to last */
	faddr_t		ps_faddr;	/* Source/Dest virtual address */
	daddr_t		ps_bno;		/* Block # on disk */
	unsigned	ps_nsec;	/* # of sectors on current transfer */
	unsigned	ps_drv;
	unsigned	ps_head;
	unsigned	ps_cyl;
	unsigned	ps_sec;
	unsigned	ps_partn;
	unsigned char	ps_dtype[ NDRIVE ];	/* drive type, 0 if unused */
	unsigned char	ps_tries;
	unsigned char	ps_state;
	unsigned char	ps_caching;		/* caching in progress */
	unsigned	ps_bad_drv;
	unsigned	ps_bad_head;
	unsigned	ps_bad_cyl;
}	ps;

static BUF	dbuf;			/* For raw I/O */


#include <sys/abios.h>
#include <sys/mmu.h>
#include <sys/types.h>
#include <stdio.h>

static request_block_hd rb;

char killbuf[1024]; /* don't forget to fix bio.c */
char l_com_data[0x80];
paddr_t com_data_p, dev1p, fcn1p, dev2p, fcn2p;
a_sys_parm sp;
a_init_table it0, it1, it2, it7, it15;


char fcn0[0x10], fcn1[0x50], fcn2[0x40], fcn7[0x50], fcn15[0x58];

extern int com_data,i0, i1, i2, i7, i15, end_table,
			fcn_trn0, fcn_trn1, fcn_trn2, fcn_trn7, fcn_trn15; 

#define cscolon(d)		((faddr_t)(0x600000L + (unsigned) (d)))
#define ftop(d)			((paddr_t)(FP_SEL(d)<<4L) + FP_OFF(d))
#define init_tab_cp(s,r)	fkcopy(cscolon(ffword(cscolon(s))), r, 24)

/*
 * Patchable variables.
 *	PSBSYW is a loop count for busy-waiting after issuing commands.
 *	PSSECS is number of seconds to wait for an expected interrupt.
 */
int	PSBSYW = 50;			/* patchable */
int	PSSECS = 6;			/* patchable */

/**
 *
 * void
 * psload()	- load routine.
 *
 *	Action:	The controller is reset and the interrupt vector is grabbed.
 *		The drive characteristics are set up ps this time.
 */
static void
psload()
{
	unsigned int u;
	register int	s;
	struct dparm_s * dp;

	{int i; for (i=0;i<1000; i++);}
	{int i; for (i=0;i<1000; i++);}
	{int i; for (i=0;i<1000; i++);}
	{int i; for (i=0;i<1000; i++);}
	{int i; for (i=0;i<1000; i++);}
	{int i; for (i=0;i<1000; i++);}
	{int i; for (i=0;i<1000; i++);}
	{int i; for (i=0;i<1000; i++);}
	{int i; for (i=0;i<1000; i++);}
	{int i; for (i=0;i<1000; i++);}
	{int i; for (i=0;i<1000; i++);}
	{int i; for (i=0;i<1000; i++);}
	{int i; for (i=0;i<1000; i++);}
	{int i; for (i=0;i<1000; i++);}
	{int i; for (i=0;i<1000; i++);}
	{int i; for (i=0;i<1000; i++);}
	{int i; for (i=0;i<1000; i++);}

	printf("loading ps\n");
	init_abios();

	n_atdr = 1;
/*	printf("n_atdr = %d\n", n_atdr); */

	if (n_atdr == 0)
		return;

	ps.ps_actf = NULL; /* Start up with this Null to avoid phony calls */

	/*
	 * Obtain (hardwire) Drive Types.
	 */
	ps.ps_dtype[0] = 1;		/* This just means that it exists */
	ps.ps_dtype[1] = 0;	 /* This just means that it doesn't exist */

	/*
	 * Obtain Drive Characteristics.
	 */

	for (u = 0, dp = &psparm[0]; u < n_atdr; ++dp, ++u) 
	{
		rb.length = 0x80;
		rb.logical_id = 6;
		rb.unit = 0;
		rb.function = 3;
		rb.reserved = 0L;
		rb.ret_code = 0xffff;
		rb.vars.f3.reserved = 0L;
		d2_func(&rb, START_P);
		if (rb.ret_code)
			printf("\nhard disk function 3 returned = %d\n", 
								rb.ret_code);
		dp->d_ncyl = rb.vars.f3.cylinders;
		dp->d_nhead = rb.vars.f3.heads;
		dp->d_nspt = rb.vars.f3.sectors_track;
		dp->d_wpcc = 0xffff;
		dp->d_landc = rb.vars.f3.cylinders;
		if (dp->d_nhead > 8)
			dp->d_ctrl |= 8;
 
#if 0

	printf(" drive %d parameters\n", u);
	printf(
	"ps%d: ncyl=%d nhead=%d wpcc=%d eccl=%d ctrl=%d landc=%d nspt=%d\n",
		u,
		dp->d_ncyl,
		dp->d_nhead,
		dp->d_wpcc,
		dp->d_eccl,
		dp->d_ctrl,
		dp->d_landc,
		dp->d_nspt);
	printf("\nhard disk function 3 returned = %d\n", rb.ret_code);
	printf("\nretries = %d", rb.vars.f3.retries);
	printf("\nblock_addresses = %lu", rb.vars.f3.block_addresses);
	printf("\nmax_blocks = %d\n", rb.vars.f3.max_blocks);

#endif
	}

	/*
	 * Initialize Drive Size.
	 */
	for (u = 0, dp = &psparm[0]; u < n_atdr; ++dp, ++u) {

		if (ps.ps_dtype[u] == 0)
			continue;

		pparm[NDRIVE*NPARTN + u].p_size =
			(long) dp->d_ncyl * dp->d_nhead * dp->d_nspt;
	}

/*	
	s = sphi(); 
	spl( s );
*/
	setivec(HDIRQ, psintr); 
	
	ps.ps_bad_drv = -1;

	/*
	 * Initialize Drive Controller.
	 */
	psreset();
}

paddr_t prot;

/* This is the protected mode part of the abios initialization */
init_abios()
{
	unsigned int i, count, *tmp1;
	paddr_t codeseg;
	paddr_t *tmp;
	static short done = 0;

	if (done)		/* Make sure we don't call this twice */
		return;

	done++;

	printf("init_abios_a() = %x\n", iresult());

	/* First create the virtual pointer to the comm area */
	com_data_p = ptov(vtop(l_com_data, sds), 0x80L);

	/* Now copy the common data table into local space */
	fkcopy(cscolon((unsigned)(&com_data) & 0xfff0), l_com_data, 0x80);

	/* Now copy the initialization tables into local space */
	init_tab_cp(&i0,  &it0);
	init_tab_cp(&i1,  &it1);
	init_tab_cp(&i7,  &it7);
	init_tab_cp(&i15, &it15);
	init_tab_cp(&i2,  &it2);
/*
	fkcopy(cscolon(ffword(cscolon(&i0))), &it0, 24);
	fkcopy(cscolon(ffword(cscolon(&i1))), &it1, 24);
	fkcopy(cscolon(ffword(cscolon(&i1))), &it7, 24);
	fkcopy(cscolon(ffword(cscolon(&i15))), &it15, 24);
	fkcopy(cscolon(ffword(cscolon(&i2))), &it2, 24);
*/
	/* Now convert each real mode device block pointer into a protected 
	 * mode device block pointer .
	 */

	tmp = tmp1 = l_com_data;      /* Make thing easier on the compiler */

	codeseg = ptov(0x0600L,&end_table);/* Set up an r/w alias to the cs */

					/* Fix the segments in the CDA */
					/* the offsets are still ok */
	tmp1[9] = tmp1[13] = tmp1[17] = tmp1[21] = tmp1[25] = FP_SEL(codeseg);

	dev1p = tmp[6];
	dev2p = tmp[12];	/* Set this up so calls are easier later */
	
	/* Now fix each function transfer table pointer in the cda so that 
         * it points to the proper spot in the ds
	 */
	
					/* Fix the segments in the CDA */
	tmp1[11] = tmp1[15] = tmp1[19] = tmp1[23] = tmp1[27] = sds;

	tmp1[10] = (unsigned)fcn0;	/* Now fix the offsets */	
	tmp1[14] = (unsigned)fcn1;
	tmp1[18] = (unsigned)fcn7;
	tmp1[22] = (unsigned)fcn15;
	tmp1[26] = (unsigned)fcn2;

	fcn1p = tmp[7];
	fcn2p = tmp[13];	/* Set this up so calls are easier later */

	/* Now convert the pointers in the ftts to protected mode pointers */
				      /* This is the location of the Abios */
	prot = ptovx((paddr_t)(0xE0000L)); 
				    /* Take out this hardwired number later */

				  /* First do the system level functions */
	cnvt_ptrs(&fcn_trn0, fcn0, it0.ftt_length);
					/* Now do the diskette functions */
	cnvt_ptrs(&fcn_trn1, fcn1, it1.ftt_length);
					   /* Now do the timer functions */
	cnvt_ptrs(&fcn_trn7, fcn7, it7.ftt_length);
				   	     /* Now do the DMA functions */
	cnvt_ptrs(&fcn_trn15, fcn15, it15.ftt_length);
			 /* Finish up with the hard disk functions table */
	cnvt_ptrs(&fcn_trn2, fcn2, it2.ftt_length);

	/* Finally,  Convert the data pointers */

	i = *(int *)l_com_data;	/* i is now a pointer to the first pointer */
	count = *(int *)(l_com_data+i+6);/* count is the # of data pointers*/
	while (count--)		/* Any more data pointers? */
	{			/* Redo the pointer using the saved size */
		tmp = l_com_data + i + 2;
		*tmp = ptov(ftop(*tmp),(long)*(unsigned *)(tmp-2));
		i -= 6;			/* Now point to the next pointer */
	}

	printf("abios inited\n");
}


cnvt_ptrs(cs_src, fcn_dest, length)
int cs_src, length;
char *fcn_dest;
{
	paddr_t *tmp;
	int count, i;

	/* First copy the tables into the data space */
	fkcopy(cscolon(cs_src), fcn_dest, length);

	/* First do the system level functions */

	tmp = fcn_dest;

 	FP_SEL(tmp[0]) = FP_SEL(prot);  /* First the start routine */
 	FP_SEL(tmp[1]) = FP_SEL(prot); 	/* Next the interrupt routine */
 	FP_SEL(tmp[2]) = FP_SEL(prot);	/* Next the time-out ruutine */

	/* Then do the individual functions */

	count = *((int *)(fcn_dest+0x0c));	/* how many functions */
	for (i=0; i < count; i++)		/* convert each function */
		if (FP_OFF(tmp[i+4]))
			FP_SEL(tmp[i+4]) = FP_SEL(prot); 
}


/**
 *
 * void
 * psunload()	- unload routine.
 */
static void
psunload()
{
	clrivec(HDIRQ);
}

static	TIM	psrstlck;

/**
 *
 * void
 * psreset()	-- reset hard disk controller, define drive characteristics.
 */
static void
psreset()
{
	register int s;

	ps.ps_state = SRESET;
	rb.length = 0x80;
	rb.logical_id = 6;
	rb.unit = 0;
	rb.function = 5;
	rb.reserved = 0L;
	rb.ret_code = 0xffff;
	rb.vars.f5.reserved = 0;

	d2_func(&rb, START_P);

	while (rb.ret_code == 2)	/* Wait for time */
	{
		unsigned long i;

		for(i=0L; i < rb.vars.f8.wait_time; i+=8)
			;
		d2_func(&rb, INTERRUPT_P); 
	}

	ps.ps_state = SIDLE;
	if (rb.ret_code != 0)
		printf("PS: reset failed - %x\n", rb.ret_code);
	return;
#if 0
	disk_function(START_P); 

	s = sphi();
	while ((rb.ret_code == 1) || (rb.ret_code == 2))
		sleep(&psrstlck, CVWAIT, IVWAIT, SVWAIT);
	spl( s );

	ps.ps_state = SIDLE;
	if (rb.ret_code != 0)
		printf("PS: reset failed - %u\n", rb.ret_code);
	return;
#endif
}

/**
 *
 * void
 * psopen(dev, mode)
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
psopen(dev, mode)
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

	if ((d >= NDRIVE) || (ps.ps_dtype[d] == 0)) {
		u.u_error = ENXIO;
		return;
	}

	if (minor(dev) & SDEV)
		return;

	/*
	 * If partition not defined read partition characteristics.
	 */
	if (pparm[p].p_size == 0)
		fdisk(makedev(major(dev), SDEV + d), &pparm[ d * NPARTN ]);
	/*
	 * Ensure partition lies within drive boundaries and is non-zero size.
	 */
	if ((pparm[p].p_base+pparm[p].p_size) > pparm[d+NDRIVE*NPARTN].p_size)
		u.u_error = EBADFMT;
	else if (pparm[p].p_size == 0)
		u.u_error = ENODEV;
}

/**
 *
 * void
 * psread(dev, iop)	- read a block from the raw disk
 * dev_t dev;
 * IO * iop;
 *
 *	Input:	dev = disk device to be written to.
 *		iop = pointer to source I/O structure.
 *
 *	Action:	Invoke the common raw I/O processing code.
 */
static void
psread(dev, iop)
dev_t	dev;
IO	*iop;
{
	ioreq(&dbuf, iop, dev, BREAD, BFRAW|BFBLK|BFIOC);
}

/**
 *
 * void
 * pswrite(dev, iop)	- write a block to the raw disk
 * dev_t dev;
 * IO * iop;
 *
 *	Input:	dev = disk device to be written to.
 *		iop = pointer to source I/O structure.
 *
 *	Action:	Invoke the common raw I/O processing code.
 */
static void
pswrite(dev, iop)
dev_t	dev;
IO	*iop;
{
	ioreq(&dbuf, iop, dev, BWRITE, BFRAW|BFBLK|BFIOC);
}

/**
 *
 * int
 * psioctl(dev, cmd, arg)
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
psioctl(dev, cmd, vec)
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
		kucopy(&psparm[d], vec, sizeof(psparm[0]));
		return(0);

	case HDSETA:
		return 0;	/* For now, do not allow this */
		/* Set hard disk pstributes. */
		ukcopy(vec, &psparm[d], sizeof(psparm[0]));
		ps.ps_dtype[d] = 1;		/* set drive type nonzero */
		pparm[NDRIVE * NPARTN + d].p_size = (long) psparm[d].d_ncyl 
				* psparm[d].d_nhead * psparm[d].d_nspt;
		psreset();
		return 0;

	default:
		u.u_error = EINVAL;
		return(-1);
	}
}

/**
 *
 * void
 * pstimer()		- wait for timeout
 *
 *	Action:	If drvl[AT_MAJOR] is greater than zero, decrement it.
 *		If it decrements to zero, call the abios again
 */
static void
pstimer()
{
	register int s;
	
	s = sphi();
	if (--drvl[AT_MAJOR].d_time > 0) {
		spl(s);
		return;
	}
	disk_function(INTERRUPT_P); 
	spl(s);
}

/**
 *
 * void
 * psblock(bp)	- queue a block to the disk
 *
 *	Input:	bp = pointer to block to be queued.
 *
 *	Action:	Queue a block to the disk.
 *		Make sure that the transfer is within the disk partition.
 */
static void
psblock(bp)
register BUF	*bp;
{
	register struct fdisk_s *pp;
	int partn = minor(bp->b_dev) % (NDRIVE*NPARTN);

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
	if (ps.ps_actf == NULL)
		ps.ps_actf = bp;
	else
		ps.ps_actl->b_actf = bp;
	ps.ps_actl = bp;
	if (ps.ps_state == SIDLE)
		if (psdequeue())
			psstart();
}

/**
 *
 * int
 * psdequeue()		- obtain next disk read/write operation
 *
 *	Action:	Pull some work from the disk queue.
 *
 *	Return:	0 = no work.
 *		* = work to do.
 */
static int
psdequeue()
{
	register BUF * bp = ps.ps_actf;
	register struct fdisk_s * pp;
	unsigned int nspt;

	ps.ps_caching = 0;
	ps.ps_tries   = 0;

	if (bp == NULL)
		return (0);

	ps.ps_partn = minor(bp->b_dev) % (NDRIVE*NPARTN);

	if (minor(bp->b_dev) & SDEV) {
		ps.ps_partn += (NDRIVE*NPARTN);
		ps.ps_drv  = minor(bp->b_dev) % NDRIVE;
	}
	else
		ps.ps_drv = minor(bp->b_dev) / NPARTN;
	nspt = psparm[ps.ps_drv].d_nspt;

	pp = &pparm[ ps.ps_partn ];
	ps.ps_bno   = pp->p_base + bp->b_bno;
	ps.ps_nsec  = bp->b_count / BSIZE;
	ps.ps_faddr = bp->b_faddr;
	return (1);
}


/**
 *
 * void
 * psstart()	- start or restart next disk read/write operation.
 *
 *	Action:	Initiate disk read/write operation.
 */
static void
psstart()
{
	register struct dparm_s *dp;

	dp = &psparm[ ps.ps_drv ];
#if 0
	ps.ps_cyl  = (ps.ps_bno / dp->d_nspt) / dp->d_nhead;
	ps.ps_head = (ps.ps_bno / dp->d_nspt) % dp->d_nhead;
	ps.ps_sec  = (ps.ps_bno % dp->d_nspt) + 1;

	/*
	 * Check for repeated access to most recently identified bad track.
	 */
	if ((ps.ps_drv  == ps.ps_bad_drv)
	  && (ps.ps_cyl  == ps.ps_bad_cyl)
	  && (ps.ps_head == ps.ps_bad_head)) {
	  	BUF * bp = ps.ps_actf;
		printf("ps%d%c: bno=%U head=%u cyl=%u <Track Flagged Bad>\n",
			ps.ps_drv,
			(bp->b_dev & SDEV) ? 'x' : ps.ps_partn % NPARTN + 'a',
			bp->b_bno,
			ps.ps_head,
			ps.ps_cyl);
		bp->b_flag |= BFERR;
		psdone();
		return;
	}
#endif
	rb.length = 0x80;
	rb.logical_id = 6;
	rb.unit = 0;
	rb.reserved = 0L;
	rb.ret_code = 0xffff;

	/* Note that the items below are set up the same way for a
	 * read or a write. */
	rb.vars.f8.reserved = 0;
	rb.vars.f8.reserved1 = 0;
	rb.vars.f8.dptr = vtop(ps.ps_faddr);
	rb.vars.f8.reserved2 = 0;
	rb.vars.f8.rb_address = ps.ps_bno;
	rb.vars.f8.reserved3 = 0;
	rb.vars.f8.blocks_read = ps.ps_nsec;
	rb.vars.f8.caching = 0;
	if (ps.ps_actf->b_req == BWRITE) {
		rb.function = 9;
		ps.ps_state = SWRITE;
	}
	else {
		rb.function = 8;
		ps.ps_state = SREAD;

	}
	disk_function(START_P);
}

/**
 *
 * void
 * psintr()	- Interrupt routine.
 *
 */
static void
psintr()
{
	d2_func(&rb, INTERRUPT_P);
	defer(disk_functionb, 0); 
}

/**
 *
 * int
 * pserror()
 *
 *	Action:	Check for drive error.
 *		If found, increment error count and report it.
 *
 *	Return: 0 = No error found.
 *		1 = Error occurred.
 */
static int
pserror()
{
	register BUF * bp = ps.ps_actf;

	if (rb.ret_code <= 2)
		return 0;	/* For now, do nothing */
	else 
	{
		printf("ps%d%c: bno=%U head=%u cyl=%u error=%x",
			ps.ps_drv,
			(bp->b_dev & SDEV) ? 'x' : ps.ps_partn % NPARTN + 'a',
			(bp->b_count/BSIZE) + bp->b_bno
				+ ps.ps_caching - ps.ps_nsec,
			ps.ps_head, ps.ps_cyl, rb.ret_code);
		return rb.ret_code;
	}
		
#if 0
	if ((csr = inb(HF_REG)) & (ERR_ST|WFLT_ST)) {

		aux = inb(AUX_REG);

		if (aux & BAD_ERR) {
			ps.ps_tries	= BADLIM;
			ps.ps_bad_drv	= ps.ps_drv;
			ps.ps_bad_head	= ps.ps_head;
			ps.ps_bad_cyl	= ps.ps_cyl;
		}
		else if (++ps.ps_tries < SOFTLIM)
			return 1;

		printf("ps%d%c: bno=%U head=%u cyl=%u",
			ps.ps_drv,
			(bp->b_dev & SDEV) ? 'x' : ps.ps_partn % NPARTN + 'a',
			(bp->b_count/BSIZE) + bp->b_bno
				+ ps.ps_caching - ps.ps_nsec,
			ps.ps_head, ps.ps_cyl);

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

		if (ps.ps_tries < HARDLIM)
			printf(" retrying...");
		printf("\n");
		return 1;
	}
	return 0;
#endif
}

/**
 *
 * void
 * psrecov()
 *
 *	Action:	Attempt recovery.
 */
static void
psrecov()
{
	register BUF *bp = ps.ps_actf;
	if (ps.ps_tries < HARDLIM) {
		ps.ps_tries++;
		psdequeue();
		psstart();
	}

	/*
	 * Give up on block.
	 */
	else {
		bp->b_flag |= BFERR;
		psdone();
	}
}

/**
 *
 * void
 * psdone()
 *
 *	Action:	Release current i/o buffer to the O/S.
 */
static void
psdone()
{
	register BUF * bp = ps.ps_actf;

	drvl[AT_MAJOR].d_time = 0;
	ps.ps_state = SIDLE;
	bdone(bp);

	ps.ps_actf = bp->b_actf;

	if (psdequeue())
		psstart();
}


void disk_function(type)
int type;
{
	d2_func(&rb, type);
	disk_functionb();
}

static TIM pstim;

void disk_functionb()
{
static int test=0;
	while (rb.ret_code == 2)	/* Wait for time */
	{
test++;
if (test > 1) printf("%");
/*		timeout(&pstim, 1, wakeup, (int)&pstim);
		sleep((char *)&pstim, CVTTOUT, IVTTOUT, SVTTOUT);
*/
{
long i;
	for(i=0L; i < rb.vars.f8.wait_time; i+=64)
		;
}
		d2_func(&rb, INTERRUPT_P);
	}
test=0;
	if (rb.ret_code == 0)		/* Finished */
	{
		if (ps.ps_state != SRESET) 
		{
			ps.ps_actf->b_resid = 0;
			psdone();
		}
		else
			wakeup(&psrstlck);
	}
	else if (rb.ret_code == 1)	/* Wat for int */
 	{
		/*ps.ps_state = SINT; */
	}
	else
	{
		printf("PS: error %x in %s.\n", rb.ret_code, 
					smsg[ps.ps_state]);
		psrecov();
	}

}
