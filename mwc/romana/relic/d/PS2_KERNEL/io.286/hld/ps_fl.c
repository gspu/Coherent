/*
 * This is a driver for the IBM AT (286 & up) floppy, using interrupts and DMA
 * on the NEC 756 floppy chip.
 * Handles single/double/quad density drives, 8/9/15/18 sectors per track.
 *
 * Minor device assignments: xxuuhkkk
 *	uu - unit = 0/1/2/3
 *	kkk - kind, struct fdata infra.
 *	h - alternating head rather than side by side
 *
 */

#include	<sys/coherent.h>
#include	<sys/buf.h>
#include	<sys/con.h>
#include	<sys/stat.h>
#include	<errno.h>
#include	<sys/timeout.h>
#include	<sys/fdioctl.h>
#include	<sys/sched.h>
#include	<sys/dmac.h>
#include 	<sys/devices.h>

#ifdef _I386
#include	<sys/reg.h>
#else
#include	<sys/i8086.h>
#endif

#define		BIT(n)		(1 << (n))

#include	<sys/abios.h>
static short intimeout;
static request_block_fl fl_rb;

void floppy_function();
void floppy_functionb();
static void fl_drive_off();

/*
 * Patchable parameters (default to IBM PC/XT values).
 */

int	fl_srt = 0xC;	/* Floppy seek step rate, in unit 2 millisec */
			/* NOT DIRECTLY ENCODED */
			/* COMPAQ wants 0xD */
int	fl_hlt = 1;	/* Floppy head load time, in unit 4 millisec */
int	fl_hut = 0xF;	/* Floppy head unload time, in unit 32 millisec */

int	flload();
int	flunload();
void	flreset();
int	flopen();
int	flblock();
int	flread();
int	flwrite();
int	flioctl();
static void flstart();
static void flintr();
static void fldone();
static void fltimer();
int	nulldev();
int	nonedev();

CON	flcon	= {
	DFBLK|DFCHR,			/* Flags */
	FL_MAJOR,			/* Major index */
	flopen,				/* Open */
	nulldev,			/* Close */
	flblock,			/* Block */
	flread,				/* Read */
	flwrite,			/* Write */
	flioctl,			/* Ioctl */
	nulldev,			/* Powerfail */
	fltimer,			/* Timeout */
	flload,				/* Load */
	flunload			/* Unload */
};

/*
 * Driver States.
 */
#ifdef OLD
#define	SIDLE	0			/* Idle */
#define	SSEEK	1			/* Need seek */
#define	SRDWR	2			/* Need read/write command */
#define	SENDIO	3			/* Need end I/O processing */
#define	SDELAY	4			/* Delay before next disk operation */
#define	SHDLY	5			/* Head settling delay before r/w */
#define SRESET	6			/* Doing a reset */
#else
#define	SIDLE	0			/* controller idle */
#define	SRETRY	1			/* seeking */
#define	SREAD	2			/* reading */
#define	SWRITE	3			/* writing */
#define SRESET  4			/* reseting */

extern char *smsg[];
#endif

#define funit(x)	(minor(x)>>4)	/* Unit/drive number */
#define fkind(x)	(0x7)		/* Kind of format */
/* #define fkind(x)	(minor(x)&0x7)	 Kind of format */
#define	fhbyh(x)	(minor(x)&0x8)	/* 0=Side by side, 1=Head by head */

static
struct	fdata {
	int	fd_size;	/* Blocks per diskette */
	int	fd_nhds;	/* Heads per drive */
	int	fd_trks;	/* Tracks per side */
	int	fd_offs;	/* Sector base */
	int	fd_nspt;	/* Sectors per track */
	char	fd_GPL[4];	/* Controller gap param (indexed by rate) */
	char	fd_N;		/* Controller size param */
	char	fd_FGPL;	/* Format gap length */
} fdata[] = {
/* 8 sectors per track, surface by surface seek. */
	{  320,1,40,0, 8, { 0x00,0x23,0x2A }, 2,0x50 }, /* Single sided */
	{  640,2,40,0, 8, { 0x00,0x23,0x2A }, 2,0x50 }, /* Double sided */
	{ 1280,2,80,0, 8, { 0x00,0x23,0x2A }, 2,0x50 }, /* Quad density */
/* 9 sectors per track, surface by surface seek. */
	{  360,1,40,0, 9, { 0x00,0x23,0x2A }, 2,0x50 }, /* Single sided */
	{  720,2,40,0, 9, { 0x00,0x23,0x2A }, 2,0x50 }, /* Double sided */
	{ 1440,2,80,0, 9, { 0x00,0x23,0x2A }, 2,0x50 }, /* Quad density */
/* 15 sectors per track, surface by surface seek. */
	{ 2400,2,80,0,15, { 0x1B,0x00,0x00 }, 2,0x54 }, /* High capacity */
/* 18 sectors per track, surface by surface seek. */
	{ 2880,2,80,0,18, { 0x1B,0x00,0x00 }, 2,0x54 }	/* 1.44 3.5" */
};


static
struct	fl	{
	BUF	*fl_actf;		/* Queue, forward */
	BUF	*fl_actl;		/* Queue, backward */
	paddr_t	fl_addr;		/* Address */
	int	fl_nsec;		/* # of sectors */
	int	fl_secn;		/* Current sector */
	struct	fdata fl_fd;		/* Disk kind data */
	int	fl_fcyl;		/* Floppy cylinder # */
	char	fl_incal[4];		/* Disk in cal flags */
	char	fl_ndsk;		/* # of 5 1/4" drives */
	char	fl_unit;		/* Unit # */
	char	fl_mask;		/* Handy unit mask */
	char	fl_hbyh;		/* 0/1 = Side by side/Head by head */
	char	fl_nerr;		/* Error count */
	int	fl_ncmdstat;		/* Number of cmd status bytes recvd */
	char	fl_cmdstat[8];		/* Command Status buffer */
	int	fl_nintstat;		/* Number of intr status bytes recvd */
	char	fl_intstat[4];		/* Interrupt Status buffer */
	int	fl_fsec;		/* Floppy sector # */
	int	fl_head;		/* Floppy head */
	char	fl_init;		/* FDC init done flag */
	char	fl_state;		/* Processing state */
	char	fl_mstatus;		/* Motor status */
	char	fl_time[4];		/* Motor timeout */
	char	fl_rate;		/* Data rate: 500,300,250,?? kbps */
	char	fl_type[4];		/* Type of drive: 2 = HiCap */
	int	fl_wflag;		/* Write operation  */
	int	fl_recov;		/* Recovery initiated */
}	fl;

static	BUF	flbuf;
static	TIM	fltim;
static	TIM	flrstlck;

/*
 * The load routine asks the
 * switches how many drives are present
 * in the machine, and sets up the field
 * in the floppy database. It also grabs
 * the level 6 interrupt vector.
 */
static
flload()
{
	register int	eflag;	
	register int	s = 0;

	printf("loading fl\n");
	init_abios();

	/*
	 * Read floppy equipment byte from CMOS ram
	 *	drive 0 is in high nibble, drive 1 is in low nibble.
	 */
	outb( 0x70, 0x10 );
	/* delay */
	eflag = inb( 0x71 );

	/*
	 * Flag hardware as an IBM AT if neither equipment byte nibble is
	 * greater than 4 (since 5 through 15 are reserved nibble values - see
	 * IBM AT Technical Reference manual, page 1-50).  Note that this
	 * relies on the fact that in the XT, this byte will "float" high.
	 * NOTE: 1.44 Mbyte 3.5 inch drives are type 4
	 */
	if ( (eflag & 0x88) == 0 ) {

		/*
		 * Reinitialize patchable parameters for IBM AT.
		 */
		fl_srt = 0xD;	/* Floppy seek step rate, in unit 2 ms */
				/* NOT DIRECTLY ENCODED */
		fl_hlt = 25;	/* Floppy head load time, in unit 4 ms */

		/*
		 * Define AT drive information.
		 */
		fl.fl_type[0]	= eflag >> 4;
		fl.fl_type[1]	= eflag & 15;
		fl.fl_rate	= 1; /* Must not be 2 */

		/*
		 * Determine number of AT floppy drives.
		 */
		if ( eflag & 0xF0 ) {
			fl.fl_ndsk++;
			if ( eflag & 0x0F )
				fl.fl_ndsk++;
		}
	} else {
		/*
		 * Define XT drive information.
		 */
		eflag		= int11();
		fl.fl_rate	= 2;
		if ( eflag & 1 )
			fl.fl_ndsk = ((eflag >> 6) & 0x03) + 1;
	}

	fl.fl_actf = NULL; /* Start up with this Null to avoid phony calls */

	if ( fl.fl_ndsk ) {

		s = sphi();
		setivec(6, &flintr);
		spl( s );

		fl_rb.length = 0x51;
		fl_rb.logical_id = 3;
		fl_rb.unit = 0;
		fl_rb.function = 3;
		fl_rb.reserved = 0L;
		fl_rb.ret_code = 0xffff;
		d1_func(&fl_rb, START_P);
		if (fl_rb.ret_code)
			printf("\nfloppy function 3 returned = %d\n", 
							fl_rb.ret_code);
		flreset();
	}
}

/*
 * Release resources.
 */
flunload()
{
	printf("doing an flunload\n");
	/*
	 * Clear interrupt vector.
	 */
	if ( fl.fl_ndsk )
		clrivec(6);

	/*
	 * Cancel timed function.
	 */
	timeout( &fltim, 0, NULL, NULL );

	/*
	 * Cancel periodic [1 second] invocation.
	 */
	drvl[FL_MAJOR].d_time = 0;

	/*
	 * Turn motors off.
	 */
/*	outb(FDCDOR, DORNMR | DORIEN );*/
}


/**
 * void
 * flreset()	-- reset floppy disk controller.
 */
static void
flreset()
{
	register int s;

	fl.fl_state = SRESET;
	fl_rb.length = 0x51;
	fl_rb.logical_id = 3;
	fl_rb.unit = 0;
	fl_rb.function = 5;
	fl_rb.reserved = 0L;
	fl_rb.ret_code = 0xffff;
	fl_rb.vars.f5.reserved = 0;
	floppy_function(START_P);

/* printf("before reset\n"); */
	{int i; for (i=0;i<1000; i++);}
	s = sphi();
	while ((fl_rb.ret_code == 1) || (fl_rb.ret_code == 2))
		sleep(&flrstlck, CVWAIT, IVWAIT, SVWAIT);
	spl( s );
	{int i; for (i=0;i<1000; i++);}
/* printf("after reset\n");  */
 
	fl.fl_state = SIDLE;
	if (fl_rb.ret_code != 0)
		printf("FL: reset failed - %u\n", fl_rb.ret_code);
	return;
}


/*
 * The open routine screens out
 * opens of illegal minor devices and
 * performs the NEC specify command if
 * this is the very first floppy disk
 * open call.
 */

static
flopen( dev, mode )
dev_t	dev;
int	mode;

{
	/*
	 * Validate existence and data rate [Gap length != 0].
	 */
	if ( funit(dev) >= fl.fl_ndsk )
	{
		u.u_error = ENXIO;
		return;
	}
}

/*
 * The read routine just calls
 * off to the common raw I/O processing
 * code, using a static buffer header in
 * the driver.
 */

static
flread( dev, iop )
dev_t	dev;
IO	*iop;
{
	ioreq(&flbuf, iop, dev, BREAD);
}

/*
 * The write routine is just like the
 * read routine, except that the function code
 * is write instead of read.
 */

static
flwrite( dev, iop )

dev_t	dev;
IO	*iop;

{
	ioreq(&flbuf, iop, dev, BWRITE);
}

/*
 * The ioctl routine simply queues a format request
 * using flbuf.
 * The only valid command is to format a track.
 * The parameter block contains the header records supplied to the controller.
 */

static
flioctl( dev, com, par )

dev_t	dev;
int	com;
char	*par;

{
	register unsigned s;
	register struct fdata *fdp;
	unsigned hd, cyl;

/*	{int i; for (i=0;i<1000; i++);} */
/*	printf("fl in fioctl\n"); */
	if (com != FDFORMAT) {
		u.u_error = EINVAL;
		return;
	}

	printf("No format allowed yet.\n");
	/* These is an ABIOS command to format. For now, just error out */
	u.u_error = EINVAL;
	return;

	fdp = &fdata[ fkind(dev) ];
	cyl = getubd(par);
	hd  = getubd(par+1);

	if (hd > 1 || cyl >= fdp->fd_trks) {
		u.u_error = EINVAL;
		return;
	}

	/*
	 * The following may need some explanation.
	 * dmareq will:
	 *	claim the buffer,
	 *	bounds check the parameter buffer,
	 *	lock the parameter buffer in memory,
	 *	convert io_seek to b_bno,
	 *	dispatch the request,
	 *	wait for completion,
	 *	and unlock the parameter buffer.
	 * The b_bno is reconverted to hd, cyl in flfsm.
	 */

	s = fhbyh(dev) ? (cyl * fdp->fd_nhds + hd) : (hd * fdp->fd_trks + cyl);
	s *= fdp->fd_nspt;
	u.u_io.io_seek = ((long)s) * BSIZE;
#ifdef _I386
	u.u_io.io.vbase = par;
#else
	u.u_io.io_base = par;
#endif
	u.u_io.io_ioc = fdp->fd_nspt * 4;
	dmareq(&flbuf, &u.u_io, dev, FDFORMAT);
}

/*
 * Start up block I/O on a
 * buffer. Check that the block number
 * is not out of range, given the style of
 * the disk. Put the buffer header into the
 * device queue. Start up the disk if the
 * device is idle.
 */

static
flblock( bp )

register BUF	*bp;
{
	register unsigned bno;

	intimeout = 0;

	bno = bp->b_bno + (bp->b_count >> 9) - 1;
	if ((unsigned)bp->b_bno > fdata[ fkind(bp->b_dev) ].fd_size) {
		bp->b_flag |= BFERR;
		bdone(bp);
		return;
	}

	if (bp->b_req != FDFORMAT && bno>=fdata[ fkind(bp->b_dev) ].fd_size) {
		bp->b_resid = bp->b_count;
		if (bp->b_flag & BFRAW)
			bp->b_flag |= BFERR;
		bdone(bp);		/* return w/ b_resid != 0 */
		return;
	}

	if ((bp->b_count&0x1FF) != 0) {
		if (bp->b_req != FDFORMAT) {
			bp->b_flag |= BFERR;
			bdone(bp);
			return;
		}
	}

/*	bp->b_actf = NULL; */

	if (fl.fl_actf == NULL)
		fl.fl_actf = bp;
	else
		fl.fl_actl->b_actf = bp;

	fl.fl_actl = bp;


	if (fl.fl_state == SRESET)
		printf("fl.fl_state (%d) != SIDLE (%d)\n", fl.fl_state,SIDLE);

	if (fl.fl_state == SIDLE)
	{
		drvl[FL_MAJOR].d_time = 0;
		if (fldequeue())
			flstart();
	}

}

/**
 *
 * int
 * fldequeue()		- obtain next disk read/write operation
 *
 *	Action:	Pull some work from the disk queue.
 *
 *	Return:	0 = no work.
 *		* = work to do.
 */
static int
fldequeue()
{
	register BUF * bp = fl.fl_actf;
	register struct fdata *dp;

	if (bp == NULL)
		return (0);

	dp = &fdata[fkind(bp->b_dev)];
	fl.fl_secn = (bp->b_bno % dp->fd_nspt) + 1;
        fl.fl_head =  bp->b_bno / dp->fd_nspt;
	fl.fl_fcyl = fl.fl_head / dp->fd_nhds;
	fl.fl_head = fl.fl_head % dp->fd_nhds;

	fl.fl_nsec  = bp->b_count / BSIZE;

	if (bp->b_faddr == 0L) 
	{
		printf("FL: called with a null address for b_faddr, bno=%d\n"
			,bp->b_bno);
		fl.fl_actf = bp->b_actf;
		return 0;
	}
	else
		fl.fl_addr = vtop(bp->b_faddr);

	return (1);
}


/**
 *
 * void
 * flstart()	- start or restart next disk read/write operation.
 *
 *	Action:	Initiate disk read/write operation.
 **/
static void
flstart()
{
	fl_rb.length = 0x51;
	fl_rb.logical_id = 3;
	fl_rb.unit = 0;
	fl_rb.reserved = 0L;
	fl_rb.ret_code = 0xffff;

	/* Note that the items below are set up the same way for a
	 * read or a write. */
	fl_rb.vars.f8.reserved = 0;
	fl_rb.vars.f8.reserved1 = 0L;
	fl_rb.vars.f8.dptr = fl.fl_addr;
	fl_rb.vars.f8.reserved2 = 0;
	fl_rb.vars.f8.cylinder = fl.fl_fcyl;
	fl_rb.vars.f8.head = fl.fl_head;
	fl_rb.vars.f8.sector = fl.fl_secn;
	fl_rb.vars.f8.sectors_read = fl.fl_nsec;

	if (fl.fl_actf->b_req == BWRITE) {
		fl_rb.function = 9;
		fl.fl_state = SWRITE;
	}
	else {
		fl_rb.function = 8;
		fl.fl_state = SREAD;
	}
	floppy_function(START_P);
}

/**
 *
 * void
 * flintr()	- Interrupt routine.
 *
 */
static void
flintr()
{
	d1_func(&fl_rb, INTERRUPT_P); 
/* 	printf("return val (int) = %x\n", fl_rb.ret_code); */
	defer(floppy_functionb, INTERRUPT_P);
}

/**
 *
 * int
 * flerror()
 *
 *	Action:	Check for drive error.
 *		If found, increment error count and report it.
 *
 *	Return: 0 = No error found.
 *		1 = Error occurred.
 */
static int
flerror()
{
	register BUF * bp = fl.fl_actf;

	if (fl_rb.ret_code <= 2)
		return 0;	/* For now, do nothing */
	else 
	{
#if 0
		printf("fl%d%c: bno=%U head=%u cyl=%u error=%x",
			fl.fl_drv,
			(bp->b_dev & SDEV) ? 'x' : fl.fl_partn % NPARTN + 'a',
			(bp->b_count/BSIZE) + bp->b_bno
				+ fl.fl_caching - fl.fl_nsec,
			fl.fl_head, fl.fl_fcyl, fl_rb.ret_code);
#endif
		return fl_rb.ret_code;
	}
}

/**
 *
 * void
 * flrecov()
 *
 *	Action:	Attempt recovery.
 */
static void
flrecov()
{
	register BUF *bp = fl.fl_actf;

	switch (fl_rb.ret_code) {

	case 0x8006:			/* Media changed - retry */
		switch(fl.fl_state)
		{
		case SREAD:
		case SWRITE:
			flreset();
			fldequeue();
			flstart();
			break;
		default:
			break;
		}			
		break;
	default:			/* Anything else - Give up on block */
		printf("FL: error %x in %s.\n", fl_rb.ret_code, 
							smsg[fl.fl_state]);

		bp->b_flag |= BFERR;
		fldone();
	}
}

/**
 *
 * void
 * fldone()
 *
 *	Action:	Release current i/o buffer to the O/S.
 */
static void
fldone()
{
	register BUF * bp = fl.fl_actf;

	drvl[FL_MAJOR].d_time = 0;
	fl.fl_state = SIDLE;
	bdone(bp);

	if (bp != NULL)
		fl.fl_actf = bp->b_actf;
	else
		fl.fl_actf = NULL;

	if (fldequeue())
		flstart();
	else if (intimeout == 0) {
		intimeout = 1;		/* Set up to turn off the drive */
		drvl[FL_MAJOR].d_time = 5;
	}	
}



void floppy_function(type)
int type;
{
	d1_func(&fl_rb, type);
/*	printf("FL: return val = %x\n", fl_rb.ret_code); */
	{int i; for (i=0;i<1000; i++);}
	floppy_functionb();
}

void floppy_functionb() 
{
static int test=0;
	while (fl_rb.ret_code == 2)	/* Wait for time */
	{
test++;
if (test > 1) printf("@");
{
long i;
	for(i=0L; i < fl_rb.vars.f8.wait_time; i+=16)
		;
}
/*
		timeout(&fltim, 1, wakeup, (int)&fltim);
		sleep((char *)&fltim, CVTTOUT, IVTTOUT, SVTTOUT);
*/

		d1_func(&fl_rb, INTERRUPT_P);
	}
test=0;
	if (fl_rb.ret_code == 0)		/* Finished */
	{
		if (fl.fl_state != SRESET) 
		{
			fl.fl_actf->b_resid = 0;
			fldone();
		}
		else		
			wakeup(&flrstlck);
	}
	else if (fl_rb.ret_code == 1)	/* Wait for int */
 	{
		/*fl.fl_state = SINT; */
		/*printf("Floppy driver is waiting for an interrupt\n"); */
	}
	else
	{
		printf("bp->b_bno=%ld, fl.fl_secn=%d, fl.fl_fcyl=%d, fl.fl_h"
		       "ead=%d, fl.fl_nsec=%d\n", fl.fl_actf->b_bno, 
			fl.fl_secn, fl.fl_fcyl, fl.fl_head, fl.fl_nsec);
		flrecov();
	}
}

/**
 *
 * void
 * fltimer()		- wait for timeout
 *
 *	Action:	If drvl[FL_MAJOR] is greater than zero, decrement it.
 *		If it decrements to zero, call the abios again
 *		 or turn off the disktimer it timeout = 1;
 */
static void
fltimer()
{
	register int s;
	
	s = sphi();
	if (--drvl[FL_MAJOR].d_time > 0) {
		spl(s);
		return;
	}
	if (intimeout) {
		intimeout = 2;
		defer(fl_drive_off,0);
	}
	else
		floppy_function(INTERRUPT_P); 
	spl(s);
}


static void
fl_drive_off()
{
	fl_rb.logical_id = 3;
	fl_rb.unit = 0;
	fl_rb.function = 0xf;
	fl_rb.reserved = 0L;
	fl_rb.ret_code = 0xffff;
	fl_rb.vars.ff.reserved = 0;
	d1_func(&fl_rb, START_P);
	if (fl_rb.ret_code)
		printf("\nfloppy function 0xf returned = %d\n", 
							fl_rb.ret_code);
	intimeout = 0;		/* Drive is now turned off */

	return;
}

