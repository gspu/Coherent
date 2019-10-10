/* $Header: /kernel/kersrc/coh.386/RCS/bio.c,v 1.2 92/08/04 12:30:18 bin Exp Locker: bin $ */
/* (lgl-
 *	The information contained herein is a trade secret of Mark Williams
 *	Company, and  is confidential information.  It is provided  under a
 *	license agreement,  and may be  copied or disclosed  only under the
 *	terms of  that agreement.  Any  reproduction or disclosure  of this
 *	material without the express written authorization of Mark Williams
 *	Company or persuant to the license agreement is unlawful.
 *
 *	COHERENT Version 2.3.37
 *	Copyright (c) 1982, 1983, 1984.
 *	An unpublished work by Mark Williams Company, Chicago.
 *	All rights reserved.
 -lgl) */
/*
 * Coherent.
 * Buffered I/O.
 *
 * $Log:	bio.c,v $
 * Revision 1.2  92/08/04  12:30:18  bin
 * changed for kernel 59
 * 
 * Revision 1.2  92/01/06  11:58:35  hal
 * Compile with cc.mwc.
 * 
 * Revision 1.1	88/03/24  16:13:29	src
 * Initial revision
 * 
 * 87/11/05	Allan Cornish		/usr/src/sys/coh/bio.c
 * New seg struct now used to allow extended addressing.
 *
 * 87/01/05	Allan Cornish		/usr/src/sys/coh/bio.c
 * ioreq() now only wakes &stimer if the swap timer is active.
 *
 * 86/12/12	Allan Cornish		/usr/src/sys/coh/bio.c
 * Added 3rd arg to dpoll() to specify blocking poll if non-zero.
 *
 * 86/11/19	Allan Cornish		/usr/src/sys/coh/bio.c
 * Added dpoll() routine to perform device polls [System V.3 compatible].
 *
 * 86/07/24	Allan Cornish		/usr/src/sys/coh/bio.c
 * Added check in devinit() for null dp->d_conp->c_load function pointer.
 */
#include <sys/coherent.h>
#include <sys/buf.h>
#include <sys/con.h>
#include <errno.h>
#include <sys/io.h>
#include <sys/proc.h>
#include <sys/sched.h>
#include <sys/seg.h>
#include <sys/stat.h>

static	BUF	**hasharray;		/* pointer to hash buckets */
static	BUF	*firstbuf;		/* pointer to first in LRU chain */
static	BUF	*lastbuf;		/* pointer to last in LRU chain */

/*
 * The following hashing algorithm is used by bclaim().
 */
#define	HASH(device, blockno)	((device * 257) + blockno)

#if BDEBUG
#include <sys/bufdebug.h>

static	BUFDEBUG	bufdebug;	/* actual counters used in profiling */

/*
 * Perform buffer cache debugging ioctl's.
 * These will not stay in the production release!
 */
bufioctl(cmd, vec)
BUFDEBUG *vec;
{
	switch (cmd) {
	case BDINIT:			/* clear (init) all counters */
		kclear(&bufdebug, sizeof(bufdebug));
		bufdebug.nbuf = NBUF;
		bufdebug.version = BDVERSION;
		break;
	case BDGETVAL:			/* return current counters to user */
		kucopy(&bufdebug, vec, sizeof(bufdebug));
		break;
	default:
		SET_U_ERROR(EINVAL, "bufioctl()");
	}
}
#endif

/*
 * Allocate and initialize buffer headers.
 */
bufinit()
{
	register BUF *bp;
	paddr_t p;
	vaddr_t v;
	int	i;

	p = MAPIO(blockp.sr_segp->s_vmem, 0);
	v = blockp.sr_base;

	if (NBUF < 32)
		panic("NBUF not set correctly");
	if (NHASH < 32)
		panic("NHASH not set correctly");

	bufl = kalloc(NBUF * sizeof(BUF));
	hasharray = kalloc(NHASH * sizeof(BUF *));
	if (bufl == BNULL || hasharray == BNULL)
		panic("bufinit: insufficient memory for %d buffers", NBUF);

	for (i = 0; i < NHASH; ++i)
		hasharray[i] = BNULL;

	/*
	 * initialize the buffer header array with the physical and
	 * virtual addresses of the buffers, NULL values for the
	 * hash chain pointers, and pointers to the successor and
	 * predecessor of the current node.
	 */
	firstbuf = &bufl[0];
	for (bp = lastbuf = &bufl[NBUF-1]; bp >= bufl; --bp) {
		bp->b_dev = NODEV;
		bp->b_paddr = p;
		bp->b_vaddr = v;
		bp->b_hashf = BNULL;
		bp->b_hashb = BNULL;
		bp->b_LRUf = bp + 1;		/* next entry in chain */
		bp->b_LRUb = bp - 1;		/* prev entry in chain */
		p += BSIZE;
		v += BSIZE;
	}
	/*
	 * the first and last headers are special cases.
	 */
	bufl[0].b_LRUb = BNULL;			/* no predecessor */
	bufl[NBUF-1].b_LRUf = BNULL;		/* no successor */
}

/*
 * Synchronise the buffer cache.
 */
bsync()
{
	register BUF *bp;

#if BDEBUG
	++bufdebug.bsync;
#endif
	for (bp = &bufl[NBUF-1]; bp >= bufl; --bp) {
		if ((bp->b_flag&BFMOD) == 0)
			continue;
		lock(bp->b_gate);
		if (bp->b_flag&BFMOD)
			bwrite(bp, 1);
		unlock(bp->b_gate);
	}
}

/*
 * Synchronise all block for a particular device in the buffer cache
 * and invalidate all references.
 */
bflush(dev)
register dev_t dev;
{
	register BUF *bp;

#if BDEBUG
	++bufdebug.bflush;
#endif
	for (bp = &bufl[NBUF-1]; bp >= bufl; --bp) {
		if (bp->b_dev != dev)
			continue;
		lock(bp->b_gate);
		if (bp->b_dev == dev) {
			if (bp->b_flag&BFMOD)
				bwrite(bp, 1);
			bp->b_dev = NODEV;
		}
		unlock(bp->b_gate);
	}
}

/*
 * Return a buffer containing the given block from the given device.
 * If `sync' is not set, the read is asynchronous and no buffer is returned.
 */
BUF *
bread(dev, bno, sync)
dev_t dev;
daddr_t bno;
register int sync;
{
	register BUF *bp;
	register int s;

#if BDEBUG
	++bufdebug.bread;
#endif
	bp = bclaim(dev, bno);
	if (bp->b_flag&BFNTP) {
		if (sync)
			bp->b_flag &= ~BFASY;
		else {
			bp->b_flag |= BFASY;
			bumap(bp);
		}
		bp->b_req = BREAD;
		bp->b_count = BSIZE;
		s = sphi();
		dblock(dev, bp);
		if (!sync) {
			spl(s);
			return (NULL);
		}
		/*
		 * If buffer is not valid, wait for it.
		 */
		while (bp->b_flag&BFNTP) {
			v_sleep((char *)bp, CVBLKIO, IVBLKIO, SVBLKIO, "bpwait");
			/* If buffer is not valid, wait for it.  */
		}
		spl(s);
		if (bp->b_flag&BFERR) {
			SET_U_ERROR(bp->b_err ? bp->b_err : EIO, "bread()");
			brelease(bp);
			return (NULL);
		}
		if (bp->b_resid == BSIZE) {
			brelease(bp);
			return (NULL);
		}
	}
	if (!sync) {
		brelease(bp);
		return (NULL);
	}
	u.u_block++;
	return (bp);
}

/*
 * Perform an LRU chain update by unlinking the specified buffer
 * from it present location in the LRU chain and inserting it
 * at the head of the chain, as pointed to by "firstbuf".  Handle
 * updating "lastbuf" if current buffer is the last buffer on the chain.
 */
static
LRUupdate(bp)
register BUF *bp;
{
	if (bp != firstbuf) {
		if (bp == lastbuf)
			lastbuf = bp->b_LRUb;
		if (bp->b_LRUb != BNULL)
			bp->b_LRUb->b_LRUf = bp->b_LRUf;
		if (bp->b_LRUf != BNULL)
			bp->b_LRUf->b_LRUb = bp->b_LRUb;
		bp->b_LRUb = BNULL;
		bp->b_LRUf = firstbuf;
		firstbuf->b_LRUb = bp;
		firstbuf = bp;
	}
}

/*
 * If the requested buffer header is in the hash chain, delete it.
 */
static
HASHdelete(bp)
register BUF *bp;
{
	if (bp->b_hashb == BNULL) {		/* we're first in the chain */
		hasharray[bp->b_hashval] = bp->b_hashf;
		if (bp->b_hashf != BNULL)
			bp->b_hashf->b_hashb = BNULL;
	} else {
		bp->b_hashb->b_hashf = bp->b_hashf;
		if (bp->b_hashf != BNULL)
			bp->b_hashf->b_hashb = bp->b_hashb;
	}
	bp->b_hashf = BNULL;
	bp->b_hashb = BNULL;
}

/*
 * Insert the current buffer at the head of the appropriate hash chain.
 */
static
HASHinsert(bp)
register BUF *bp;
{
	if (bp->b_hashf != BNULL || bp->b_hashb != BNULL)
		panic("HASHinsert");
	bp->b_hashf = hasharray[bp->b_hashval];
	if (bp->b_hashf != BNULL)
		bp->b_hashf->b_hashb = bp;
	hasharray[bp->b_hashval] = bp;
}

/*
 * If the requested buffer is in the buffer cache, return a pointer to
 * it.  If not, pick an empty buffer, set it up and return it.
 */
BUF *
bclaim(dev, bno)
dev_t dev;
register daddr_t bno;
{
	register BUF *bp;
	register int s;
	unsigned long hashval;
	static GATE bufgate;			/* better than sphi()/spl() */

#if BDEBUG
	++bufdebug.bclaim;
#endif
	hashval = HASH(dev, bno) % NHASH;	/* select a hash bucket */

again:
	lock(bufgate);				/* avoid pointer updates */

	for (bp = hasharray[hashval]; bp != BNULL; bp = bp->b_hashf) {
#if BDEBUG
		++bufdebug.compares;
#endif
		if (bp->b_bno == bno  &&  bp->b_dev == dev) {
			lock(bp->b_gate);
			if (bp->b_bno != bno  ||  bp->b_dev != dev) {
#if BDEBUG
				++bufdebug.fails;
#endif
				unlock(bp->b_gate);
				unlock(bufgate);
				goto again;
			}
#if BDEBUG
			++bufdebug.hits;
#endif
			/*
			 * Now that we have located the buffer in the cache,
			 * unlink it from its current location in the
			 * LRU chain and move it to the front.
			 */
			LRUupdate(bp);

			/*
			 * If the buffer had an I/O error, mark it as
			 * invalid.  Unlock the buffer gate and return
			 * the buffer to the requestor.
			 */
			if (bp->b_flag&BFERR)
				bp->b_flag |= BFNTP;
			unlock(bufgate);
			bsmap(bp);
			return (bp);
		}
	}
	unlock(bufgate);
#if BDEBUG
	++bufdebug.misses;
#endif

	/*
	 * The requested buffer is not resident in our cache.  Locate the
	 * oldest (least recently used) available buffer.  If it's dirty,
	 * queue up an asynchronous write for it and continue searching
	 * for the next old candidate. Once a candidate is found, move it
	 * to the front of the LRU chain, update the hash pointers, mark
	 * the buffer as invalid, unlock our buffer gate and return the
	 * buffer to the requestor.
	 */
	for (;;) {				/* loop until successful */
		lock(bufgate);
		for (bp = lastbuf; bp != BNULL; bp = bp->b_LRUb) {
			if (locked(bp->b_gate))
				continue;	/* not available */
			s = sphi();
			if (locked(bp->b_gate)) {
				spl(s);
				continue;	/* they snuck in ;-) */
			}
			lock(bp->b_gate);
			spl(s);
			if (bp->b_flag&BFMOD)
				bwrite(bp, 0);	/* flush dirty buffer */
			else {
				/*
				 * Update the hash chain for this old
				 * buffer.  Unlink it from it's old location
				 * fixing up any references. Also, update
				 * the LRU chain to move the buffer to the head.
				 */
				HASHdelete(bp);
				LRUupdate(bp);

				bp->b_flag = BFNTP;
				bp->b_dev = dev;
				bp->b_bno = bno;
				bp->b_hashval = hashval;

				HASHinsert(bp);
				unlock(bufgate);
				bsmap(bp);
				return (bp);
			}
		}
		unlock(bufgate);
#if BDEBUG
		++bufdebug.needbuf;
#endif
		s = sphi();
		bufneed = 1;
		v_sleep((char *)&bufneed, CVBLKIO, IVBLKIO, SVBLKIO, "bufneed");
		/* There are no buffers available.  */
		spl(s);
	} /* forever */
}

/*
 * Write the given buffer out.  If `sync' is set, the write is synchronous,
 * otherwise asynchronous.  This routine must be called with the buffer
 * gate locked.
 */
bwrite(bp, sync)
register BUF *bp;
{
	register int s;

#if BDEBUG
	++bufdebug.bwrite;
#endif
	if (sync)
		bp->b_flag &= ~BFASY;
	else {
		bp->b_flag |= BFASY;
		bumap(bp);
	}
	bp->b_flag |= BFNTP;
	bp->b_req = BWRITE;
	bp->b_count = BSIZE;
	s = sphi();
	dblock(bp->b_dev, bp);
	if (!sync) {
		spl(s);
		return;
	}
	while (bp->b_flag&BFNTP) {
		v_sleep((char *)bp, CVBLKIO, IVBLKIO, SVBLKIO, "bwrite");
		/* Waiting for a buffer write to finish.  */
	}
	spl(s);
}

/*
 * This is called by the driver when I/O has completed on a buffer.
 */
bdone(bp)
register BUF *bp;
{
#if BDEBUG
	++bufdebug.bdone;
#endif
	if (bp->b_req == BWRITE)
		bp->b_flag &= ~BFMOD;
	if (bp->b_req == BREAD) {
		if (bp->b_flag&BFERR)
			bp->b_dev = NODEV;
	}
	if (bp->b_flag&BFASY) {
		bp->b_flag &= ~BFASY;
		brelease(bp);
	}
	bp->b_flag &= ~BFNTP;
	dwakeup((char *)bp);
}

/*
 * Release the given buffer.
 */
brelease(bp)
register BUF *bp;
{
#if BDEBUG
	++bufdebug.brelease;
#endif
	if (bp->b_flag&BFERR) {
		bp->b_flag &= ~BFERR;
		bp->b_dev = NODEV;
	}
	bp->b_flag &= ~BFNTP;
	bumap(bp);
	unlock(bp->b_gate);
	if (bufneed) {
		bufneed = 0;
		wakeup((char *)&bufneed);
	}
}

/*
 * Map the given buffer.
 */
bsmap(bp)
register BUF *bp;
{
	bsave(bp->b_map);
	bp->b_flag |= BFMAP;
	bmapv(bconv(bp->b_paddr));
}

/*
 * Unmap the given buffer.
 */
bumap(bp)
register BUF *bp;
{
	if ((bp->b_flag&BFMAP) == 0)
		return;
	bp->b_flag &= ~BFMAP;
	brest(bp->b_map);
}

/*
 * Read data from the I/O segment into kernel space.
 */
ioread(iop, v, n)
register IO *iop;
register char *v;
register unsigned n;
{
#if BDEBUG
	++bufdebug.ioread;
#endif
	switch (iop->io_seg) {
	case IOSYS:
#if BDEBUG
		++bufdebug.iosys;
#endif
		iop->io.vbase += kkcopy(iop->io.vbase, v, n);
		break;
	case IOUSR:
#if BDEBUG
		++bufdebug.iousr;
#endif
		iop->io.vbase += ukcopy(iop->io.vbase, v, n);
		break;
	case IOPHY:
#if BDEBUG
		++bufdebug.iophy;
#endif
		dmain(n, iop->io.pbase, v);
		iop->io.pbase += n;
		break;
	}
	iop->io_ioc -= n;
}

/*
 * Write data from kernel space to the I/O segment.
 */
iowrite(iop, v, n)
register IO *iop;
register char *v;
register unsigned n;
{
#if BDEBUG
	++bufdebug.iowrite;
#endif
	switch (iop->io_seg) {
	case IOSYS:
#if BDEBUG
		++bufdebug.iosys;
#endif
		iop->io.vbase += kkcopy(v, iop->io.vbase, n);
		break;
	case IOUSR:
#if BDEBUG
		++bufdebug.iousr;
#endif
		iop->io.vbase += kucopy(v, iop->io.vbase, n);
		break;
	case IOPHY:
#if BDEBUG
		++bufdebug.iophy;
#endif
		dmaout(n, iop->io.pbase, v);
		iop->io.pbase += n;
		break;
	}
	iop->io_ioc -= n;
}

/*
 * Get a character from the I/O segment.
 */
iogetc(iop)
register IO *iop;
{
	register int c;

#if BDEBUG
	++bufdebug.iogetc;
#endif
	if (iop->io_ioc == 0)
		return (-1);
	--iop->io_ioc;
	if (iop->io_seg == IOSYS)
		c = *(char*) iop->io.vbase++ & 0377;
	else {
		c = getubd(iop->io.vbase++);
		if (u.u_error)
			return (-1);
	}
	return (c);
}

/*
 * Put a character using the I/O segment.
 */
ioputc(c, iop)
register IO *iop;
{
#if BDEBUG
	++bufdebug.ioputc;
#endif
	if (iop->io_ioc == 0)
		return (-1);
	--iop->io_ioc;
	if (iop->io_seg == IOSYS)
		* (char *)iop->io.vbase++ = c;
	else {
		putubd(iop->io.vbase++, c);
		if (u.u_error)
			return (-1);
	}
	return (c);
}

/*
 * Given a buffer pointer, an I/O structure, a device, request type, and
 * a flags word, check the I/O structure and perform the I/O request.
 */
ioreq(bp, iop, dev, req, f)
register BUF *bp;
register IO *iop;
dev_t dev;
{
	register int n;
	register int s;
	register CON *cp;
	dold_t dold;

#if BDEBUG
	++bufdebug.ioreq;
#endif
	if ((cp=drvmap(dev, &dold)) == NULL)
		return;
	lock(bp->b_gate);
	n = cp->c_flag;	/* n should do something with that flag */
	drest(dold);
	if (iop) {
		if (f&BFBLK) {
			if (blocko(iop->io_seek)) {
				SET_U_ERROR(EIO, "ioreq()");
				goto out;
			}
		}
		if (f&BFIOC) {
			if (!iomapvp(iop, bp)) {
				SET_U_ERROR(EIO, "ioreq()");
				goto out;
			}
		}
	}
	bp->b_flag = f|BFNTP;
	bp->b_req = req;
	bp->b_dev = dev;
	if (iop) {
		bp->b_bno = blockn(iop->io_seek);
		bp->b_count = iop->io_ioc;
	}
	s = sphi();
	dblock(dev, bp);
	while (bp->b_flag&BFNTP) {
		v_sleep((char *)bp, CVBLKIO, IVBLKIO, SVBLKIO, "ioreq");
		/* Ask norm what this sleep means.  */
	}
	spl(s);
	if (stimer.t_last)
		wakeup((char *)&stimer);
	if (bp->b_flag&BFERR) {
		SET_U_ERROR(bp->b_err ? bp->b_err : EIO, "ioreq()");
		goto out;
	}
	if (iop) {
		n = iop->io_ioc - bp->b_resid;
		iop->io_seek += n;
		iop->io_ioc -= n;
	}
out:
	unlock(bp->b_gate);
}

/*
 * Given an I/O structure and a buffer header, see if the addresses
 * in the I/O structure are valid and set up the buffer header.
 *
 * Search the u area segment table for a data segment containing
 * iop->io.vbase.  If one is found, put the corresponding system
 * global address into bp->b_paddr and return the corresponding
 * SEG pointer, else return NULL.
 */
SEG *
iomapvp(iop, bp)
register IO *iop;
register BUF *bp;
{
	register SR *srp;
	register SEG *sp;
	register vaddr_t b, base;

	if (iop->io_seg != IOUSR)
		panic("Raw I/O from non user");
	for (srp=u.u_segl; srp<&u.u_segl[NUSEG]; srp++) {
		if ((sp=srp->sr_segp) == NULL)
			continue;
		if ((srp->sr_flag&SRFDATA) == 0)
			continue;
		/*
		 * The following calculation is because the system represents
		 * the 'base' of a stack as its upper limit (because it is the
		 * upper limit that is fixed).
		 */
		base = srp->sr_base;
		if (srp==&u.u_segl[SISTACK])
			base -= srp->sr_size;

		if ((b=iop->io.vbase) < base)
			continue;
		if ((long)b+iop->io_ioc > base + sp->s_size)
			continue;
 		bp->b_paddr = MAPIO(sp->s_vmem, b-base);
		return (sp);
	}
	return 0;
}

/*
 * Initialise devices.
 * Mark all initialized devices as loaded.
 */
devinit()
{
	register DRV *dp;
	register int mind;

	for ( dp = drvl, mind = 0; mind < drvn; mind++, dp++ ) {
		if (dp->d_conp && dp->d_conp->c_load) {
			(*dp->d_conp->c_load)();
			dev_loaded |= (1<<mind);
		}
	}
}

/*
 * Open a device.
 */
dopen(dev, m, f)
register dev_t dev;
{
	register CON *cp;
	dold_t dold;

	if ((cp=drvmap(dev, &dold)) == NULL)
		return;
	if ((cp->c_flag&f) == 0) {
		SET_U_ERROR(ENXIO, "dopen()");
		return;
	}
	(*cp->c_open)(dev, m);
	drest(dold);
}

/*
 * Close a device.
 */
dclose(dev)
register dev_t dev;
{
	register CON *cp;
	dold_t dold;

	if ((cp=drvmap(dev, &dold)) == NULL)
		return;
	(*cp->c_close)(dev);
	drest(dold);
}

/*
 * Call the block entry point of a device.
 */
dblock(dev, bp)
dev_t dev;
BUF *bp;
{
	register CON *cp;
	dold_t dold;

	if ((cp=drvmap(dev, &dold)) == NULL)
		return;
	(*cp->c_block)(bp);
	drest(dold);
}

/*
 * Read from a device.
 */
dread(dev, iop)
register dev_t dev;
register IO *iop;
{
	register CON *cp;
	dold_t dold;

	if ((cp=drvmap(dev, &dold)) == NULL)
		return;
	(*cp->c_read)(dev, iop);
	drest(dold);
}

/*
 * Write to a device.
 */
dwrite(dev, iop)
register dev_t dev;
register IO *iop;
{
	register CON *cp;
	dold_t dold;

	if ((cp=drvmap(dev, &dold)) == NULL)
		return;
	(*cp->c_write)(dev, iop);
	drest(dold);
}

/*
 * Call the ioctl function for a device.
 */
dioctl(dev, com, vec)
register dev_t dev;
union ioctl *vec;
{
	register CON *cp;
	dold_t dold;

	if ((cp=drvmap(dev, &dold)) == NULL)
		return;
	(*cp->c_ioctl)(dev, com, vec);
	drest(dold);
}

/*
 * Call the powerfail entry point of a device.
 */
dpower(dev)
register dev_t dev;
{
	register CON *cp;
	dold_t dold;

	if ((cp=drvmap(dev, &dold)) == NULL)
		return;
	(*cp->c_power)(dev);
	drest(dold);
}

/*
 * Call the timeout entry point of a device.
 */
dtime(dev)
register dev_t dev;
{
	register CON *cp;
	dold_t dold;

	if ((cp=drvmap(dev, &dold)) == NULL)
		return;
	(*cp->c_timer)(dev);
	drest(dold);
}

/*
 * Poll a device.
 */
dpoll(dev, ev, msec)
register dev_t dev;
int ev;
int msec;
{
	register CON *cp;
	dold_t dold;

	if ((cp=drvmap(dev, &dold)) == NULL)
		return POLLNVAL;

	if ( cp->c_flag & DFPOL )
		ev = (*cp->c_poll)(dev, ev, msec);
	else
		ev = POLLNVAL;

	drest(dold);
	return ev;
}

/*
 * Given a device, return the flags word.
 */
dflag(dev)
dev_t dev;
{
	register CON *cp;
	register int f;
	dold_t dold;

	if ((cp=drvmap(dev, &dold)) == NULL)
		return (DFERR);
	f = cp->c_flag;
	drest(dold);
	return (f);
}

/*
 * Given a device, and a pointer to a driver map save area, save the
 * current map in the driver map save area and map in the new device,
 * returning a pointer to the configuration entry for that device.
 */
CON *
drvmap(dev, doldp)
dev_t dev;
dold_t *doldp;
{
	register DRV *dp;
	register unsigned m;

	if ((m=major(dev)) >= drvn) {
		SET_U_ERROR(ENXIO, "drvmap()");
		return (NULL);
	}
	dp = &drvl[m];
	if (locked(dp->d_gate)) {
		SET_U_ERROR(ENXIO, "drvmap()");
		return (NULL);
	}
	if (dp->d_conp == NULL) {
		SET_U_ERROR(ENXIO, "drvmap()");
		return (NULL);
	}
	dsave(*doldp);
	if (dp->d_map)
		dmapv(dp->d_map);
	return (dp->d_conp);
}

/*
 * Non existant device.
 */
nonedev()
{
	SET_U_ERROR(ENXIO, "nonedev()");
}

/*
 * Null device.
 */
nulldev()
{
}
