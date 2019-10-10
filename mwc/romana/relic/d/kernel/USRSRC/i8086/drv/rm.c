/*
 * Block or character device RAM disk driver.
 */

#include	<sys/coherent.h>
#include	<sys/buf.h>
#include	<errno.h>
#include	<sys/uproc.h>
#include	<sys/seg.h>
#include	<sys/con.h>
#include	<sys/devices.h>
#include	<sys/inode.h>
#include	<sys/stat.h>

/*
 * Minor number encoding: dsssssss
 * d       drive number (0 or 1)
 * sssssss allocation size: 0 to free, 1-127 allocsize (n*ASIZE*BSIZE bytes)
 */
#define	rm_drive(dev)	(minor(dev) >> 7)
#define	rm_asize(dev)	(minor(dev) & 0x7F)
#define	ASIZE		128	/* allocation chunk size in blocks (64KB) */
#define NUM_RM		2	/* number of ram disks */
				/* - tied to dev encoding (see above) */

int	nulldev();
int	nonedev();
int	rmload();
int	rmuload();
int	rmopen();
int	rmclose();
int	rmread();
int	rmwrite();
int	rmblock();

CON	rmcon	= {
	DFBLK|DFCHR,
	RM_MAJOR,
	rmopen,			/* Open */
	rmclose,		/* Close */
	rmblock,		/* Block */
	rmread,			/* Read */
	rmwrite,		/* Write */
	nonedev,
	nulldev,
	nulldev,
	rmload,			/* Load */
	rmuload			/* Unload */
};

typedef struct rm {
	fsize_t	rm_size;	/* Size in allocation chunks */
	paddr_t	rm_paddr;	/* Physical base of ram disc segment */
	SEG	*rm_segp;	/* Segment pointer of ram device */
	BUF	rm_buf;		/* Static buffer for raw requests */
	int	rm_nopen;	/* Open count to avoid blowups */
} RM;
static	RM	rm[NUM_RM];

/*
 * Load.
 */
static
rmload()
{
}

/*
 * Unload.
 * Release the allocated buffers.
 */
static
rmuload()
{
	int i;

	for (i = 0; i < NUM_RM; i++){
		if (rm[i].rm_size != 0)
			sfree(rm[i].rm_segp);
	}
}

/*
 * Open.
 * Allocate on the first call.
 * Increment the open count.
 */
static
rmopen(dev, mode) dev_t dev; int mode;
{
	register RM *rmp;
	register fsize_t asize, osize;
	register SEG *segp;

	rmp = &rm[rm_drive(dev)];
	asize = rm_asize(dev);
	osize = rmp->rm_size;

	/* Fail on read before creation or bogus size. */
	if ((mode == IPR && osize == 0)
	 || (asize != 0 && osize != 0 && asize != osize)
	 || (asize == 0 && osize == 0)) {
		u.u_error = ENXIO;
		return;
	}

	/*
	 * Allocate as required.
	 * Ignore case asize==0 && osize!=0, handled by rmclose().
	 * If asize!=0 && asize==osize, just bump the open count.
	 */
	if (asize != 0 && osize == 0) {
		segp = rmp->rm_segp = salloc((fsize_t)ASIZE*BSIZE*asize,
			SFSYST|SFNSWP|SFNCLR|SFHIGH);
		if (segp == NULL) {
			u.u_error = ENOMEM;
			return;
		}
		rmp->rm_size = asize;
		rmp->rm_paddr = segp->s_paddr;
		rmp->rm_nopen = 0;
		pclear(rmp->rm_paddr, 1024L);	/* clear 1st 2 blocks */
	}
	rmp->rm_nopen++;
}

/*
 * Close.
 * Decrement the open count.
 * Release the allocated buffer if minor number is 0.
 */
static
rmclose(dev) dev_t dev;
{
	register RM *rmp;
	register fsize_t asize, osize;

	rmp = &rm[rm_drive(dev)];
	asize = rm_asize(dev);
	osize = rmp->rm_size;

	if (osize == 0
	 || (asize != 0 && asize != osize)
	 || rmp->rm_nopen == 0) {
		u.u_error = ENXIO;
		return;
	}
	rmp->rm_nopen--;
	if (asize == 0) {
		if (rmp->rm_nopen != 0) {
			u.u_error = EDBUSY;
			return;
		}
		sfree(rmp->rm_segp);
		rmp->rm_size = 0;
	}
}

static
rmblock(bp) register BUF *bp;
{
	paddr_t base;
	dev_t dev;
	register RM *rmp;
	register fsize_t asize, osize;

	dev = bp->b_dev;
	rmp = &rm[rm_drive(dev)];
	asize = rm_asize(dev);
	osize = rmp->rm_size;
	if (osize == 0 || asize != osize)
		bp->b_flag |= BFERR;
	/*
	 * Make sure last block requested is within range of device.
	 */	
	else if ((bp->b_bno + bp->b_count/BSIZE - 1) >= asize*ASIZE)
		bp->b_flag |= BFERR;
	else {
		base = rmp->rm_paddr + (paddr_t)bp->b_bno * BSIZE;
		if (bp->b_req == BREAD)
			plrcopy(base, bp->b_paddr, (fsize_t)bp->b_count);
		else
			plrcopy(bp->b_paddr, base, (fsize_t)bp->b_count);
	}
	bdone(bp);
}

/*
 * The read routine calls the common raw I/O processing code,
 * using a static buffer header in the driver.
 */
static
rmread(dev, iop) register dev_t dev; IO *iop;
{
	register BUF *bufp;

	bufp = &rm[rm_drive(dev)].rm_buf;
	ioreq(bufp, iop, dev, BREAD, BFIOC|BFRAW);
}

/*
 * The write routine is just like the read routine,
 * except that the function code is write instead of read.
 */
static
rmwrite(dev, iop) register dev_t dev; IO *iop;
{
	register BUF *bufp;

	bufp = &rm[rm_drive(dev)].rm_buf;
	ioreq(bufp, iop, dev, BWRITE, BFIOC|BFRAW);
}

/* end of rm.c */
