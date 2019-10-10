/*
 * Block or character device RAM disk driver.
 * AT COHERENT (286 and 386).
 */

#include	<sys/coherent.h>
#include	<sys/con.h>
#include	<sys/buf.h>
#include	<errno.h>
#include	<sys/devices.h>
#include	<sys/inode.h>
#include	<sys/stat.h>
#include	<sys/seg.h>
#include	<sys/uproc.h>

/*
 * Minor number encoding: dsssssss
 * d       drive number (0 or 1)
 * sssssss allocation size: 0 to free, 1-127 allocsize (n*ASIZE*BSIZE bytes)
 */
#define	rm_drive(dev)	(minor(dev) >> 7)
#define	rm_asize(dev)	(minor(dev) & 0x7F)
#define	ASIZE		128	/* allocation chunk size in blocks (64KB) */
#define NUM_RM		2	/* number of ram disks
				   - tied to dev encoding (see above) */

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
#ifndef COH386
	fsize_t	rm_size;	/* Size in allocation chunks */
	paddr_t	rm_paddr;	/* Physical base of ram disc segment */
	SEG	*rm_segp;	/* Segment pointer of ram device */
#else
	off_t	rm_size;	/* Size in allocation chunks */
	SR	rm_sr;
#endif
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
		if (rm[i].rm_size != 0) {
#ifndef COH386
			sfree(rm[i].rm_segp);
#else
			unload(&rm[i].rm_sr);
			sfree(rm[i].rm_sr.sr_segp);
#endif
		}
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
#ifndef COH386
	register fsize_t asize, osize;
	register SEG *segp;
#else
	register off_t asize, osize;
#endif

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

#ifdef COH386
	if (ASIZE*BSIZE*asize > ctob(RAMSIZE)) {
		u.u_error = ENOMEM;
		return;
	}
#endif

	/*
	 * Allocate as required.
	 * Ignore case asize==0 && osize!=0, handled by rmclose().
	 * If asize!=0 && asize==osize, just bump the open count.
	 */
	if (asize != 0 && osize == 0) {
#ifndef COH386
		segp = rmp->rm_segp = salloc((fsize_t)ASIZE*BSIZE*asize,
			SFSYST|SFNSWP|SFNCLR|SFHIGH);
		if (segp == NULL) {
#else
		rmp->rm_sr.sr_segp =
		    salloc((off_t)ASIZE*BSIZE*asize, SFSYST|SFNSWP|SFNCLR);
		if (rmp->rm_sr.sr_segp == NULL) {
#endif
			u.u_error = ENOMEM;
			return;
		}
		rmp->rm_size = asize;
#ifndef COH386
		rmp->rm_paddr = segp->s_paddr;
		rmp->rm_nopen = 0;
		pclear(rmp->rm_paddr, 1024L);	/* clear 1st 2 blocks */
#else
		rmp->rm_sr.sr_base = rm_drive(dev)==0 ?
			ctob(RAM0) : ctob(RAM1);
		rmp->rm_sr.sr_size = rmp->rm_sr.sr_segp->s_size;
		doload(&rmp->rm_sr);
#endif
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
#ifndef COH386
	register fsize_t asize, osize;
#else
	register off_t asize, osize;
#endif

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
#ifndef COH386
			u.u_error = EDBUSY;
#else
			u.u_error = EBUSY;
#endif
			return;
		}
#ifndef COH386
		sfree(rmp->rm_segp);
#else
		unload(&rmp->rm_sr);
		sfree(rmp->rm_sr.sr_segp);
#endif
		rmp->rm_size = 0;
	}
}

static
rmblock(bp) register BUF *bp;
{
#ifndef COH386
	paddr_t base;
	register fsize_t asize, osize;
#else
	vaddr_t base;
	register off_t asize, osize;
#endif
	dev_t dev;
	register RM *rmp;

	dev = bp->b_dev;
	rmp = &rm[rm_drive(dev)];
	asize = rm_asize(dev);
	osize = rmp->rm_size;
	if (osize == 0 || asize != osize) {
		bp->b_flag |= BFERR;
		u.u_error = ENXIO;
	/*
	 * Make sure last block requested is within range of device.
	 */	
	} else if ((bp->b_bno + bp->b_count/BSIZE - 1) >= asize*ASIZE) {
		bp->b_flag |= BFERR;
		u.u_error = EIO;
	} else {
#ifndef COH386
		base = rmp->rm_paddr + (paddr_t)bp->b_bno * BSIZE;
#else
		base = rmp->rm_sr.sr_base + (paddr_t)bp->b_bno * BSIZE;
#endif
		if (bp->b_req == BREAD)
#ifndef COH386
			plrcopy(base, bp->b_paddr, (fsize_t)bp->b_count);
#else
			dmaout(bp->b_count, bp->b_paddr, base);
#endif
		else
#ifndef COH386
			plrcopy(bp->b_paddr, base, (fsize_t)bp->b_count);
#else
			dmaout(bp->b_count, bp->b_paddr, base);
#endif
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
