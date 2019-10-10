/* (-lgl
 * 	COHERENT Device Driver Kit version 1.2.0
 * 	Copyright (c) 1982, 1991 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Block or character device RAM disk driver.
 */

#include	<sys/coherent.h>
#include	<sys/buf.h>
#include	<errno.h>
#ifndef _I386
#include	<sys/uproc.h>
#include	<sys/devices.h>
#endif
#include	<sys/seg.h>
#include	<sys/con.h>
#include	<sys/inode.h>
#include	<sys/stat.h>

#if _I386
/*
 * Patchable variables for 386.
 * Starting addresses in kernel data and size for each ram disk.
 */
int	RAM0=0x80000000;
int	RAM1=0x80800000;
int	RAMSIZE=0x800000;
#endif

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
#ifdef _I386
#define	RMMAJ		8	/* major # for driver */
#endif

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
#ifndef _I386
	RM_MAJOR,
#else
	RMMAJ,
#endif
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
#ifndef _I386
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
#ifndef _I386
		if (rm[i].rm_size != 0)
			sfree(rm[i].rm_segp);
#else
		if (rm[i].rm_size != 0) {
			unload(&rm[i].rm_sr);
			sfree(rm[i].rm_sr.sr_segp);
		}
#endif
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
#ifndef _I386
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
		SET_U_ERROR( ENXIO,
			"can not read ram disk yet or bogus size" );
		return;
	}

#ifdef _I386
	if (ASIZE*BSIZE*asize > RAMSIZE) {
		SET_U_ERROR( ENOMEM, "Ram disk too big" );
		return;
	}
#endif

	/*
	 * Allocate as required.
	 * Ignore case asize==0 && osize!=0, handled by rmclose().
	 * If asize!=0 && asize==osize, just bump the open count.
	 */
	if (asize != 0 && osize == 0) {
#ifndef _I386
		segp = rmp->rm_segp = salloc((fsize_t)ASIZE*BSIZE*asize,
			SFSYST|SFNSWP|SFNCLR|SFHIGH);
		if (segp == NULL) {
			SET_U_ERROR( ENOMEM,
				"can not allocate segment for ram disk" );
			return;
		}
#else
		rmp->rm_sr.sr_segp =
		    salloc((off_t)ASIZE*BSIZE*asize, SFSYST|SFNSWP|SFNCLR);
		if (rmp->rm_sr.sr_segp == NULL) {
			SET_U_ERROR( ENOMEM,
				"can not allocate segment for ram disk" );
			return;
		}
#endif
		rmp->rm_size = asize;
#ifndef _I386
		rmp->rm_paddr = segp->s_paddr;
		rmp->rm_nopen = 0;
		pclear(rmp->rm_paddr, 1024L);	/* clear 1st 2 blocks */
#else
		rmp->rm_sr.sr_base = rm_drive(dev)==0 ? RAM0 : RAM1;
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
#ifndef _I386
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
#ifndef _I386
			u.u_error = EDBUSY;
#else
			u.u_error = EBUSY;
#endif
			return;
		}
#ifndef _I386
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
#ifndef _I386
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
	if (osize == 0 || asize != osize)
#ifndef _I386
		bp->b_flag |= BFERR;
	/*
	 * Make sure last block requested is within range of device.
	 */	
	else if ((bp->b_bno + bp->b_count/BSIZE - 1) >= asize*ASIZE)
		bp->b_flag |= BFERR;
#else
		u.u_error = ENXIO;
	else if (bp->b_bno >= asize*ASIZE)
		u.u_error = EIO;
#endif
	else {
#ifndef _I386
		base = rmp->rm_paddr + (paddr_t)bp->b_bno * BSIZE;
#else
		base = rmp->rm_sr.sr_base + (paddr_t)bp->b_bno * BSIZE;
#endif
		if (bp->b_req == BREAD)
#ifndef _I386
			plrcopy(base, bp->b_paddr, (fsize_t)bp->b_count);
#else
			dmaout(bp->b_count, bp->b_paddr, base);
#endif
		else
#ifndef _I386
			plrcopy(bp->b_paddr, base, (fsize_t)bp->b_count);
#else
			dmain(bp->b_count, bp->b_paddr, base);
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
