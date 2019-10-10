/* $Header: /src386/STREAMS/coh.386/RCS/bio.c,v 2.3 93/08/09 13:35:09 bin Exp Locker: bin $ */
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
 * Revision 2.3  93/08/09  13:35:09  bin
 * Kernel 82 changes
 * 
 * Revision 2.2  93/07/26  15:22:22  nigel
 * Nigel's R80
 * 
 * Revision 2.2  93/07/26  14:28:20  nigel
 * Nigel's R80
 * 
 * Revision 1.9  93/04/14  10:06:14  root
 * r75
 * 
 * Revision 1.7  92/10/06  23:48:44  root
 * Ker #64
 * 
 * Revision 1.6  92/07/27  18:15:08  hal
 * Kernel #59
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

#include <common/gregset.h>
#include <sys/debug.h>
#include <sys/coherent.h>
#include <sys/buf.h>
#include <sys/con.h>
#include <sys/errno.h>
#include <sys/io.h>
#include <sys/proc.h>
#include <sys/sched.h>
#include <sys/seg.h>
#include <sys/stat.h>

/*
 * This is here for the old-style Coherent I/O support hacks.
 */
#include <sgtty.h>

static	BUF	**hasharray;		/* pointer to hash buckets */
static	BUF	*firstbuf;		/* pointer to first in LRU chain */
static	BUF	*lastbuf;		/* pointer to last in LRU chain */

/*
 * The following hashing algorithm is used by bclaim().
 */
#define	HASH(device, blockno)	((device * 257) + blockno)

/*
 * Allocate and initialize buffer headers.
 */
bufinit()
{
	register BUF *bp;
	paddr_t p;
	caddr_t v;
	int	i;

	p = MAPIO(blockp.sr_segp->s_vmem, 0);
	v = blockp.sr_base;

	if (NBUF < 32)
		panic("NBUF not set correctly");
	if (NHASH < 32)
		panic("NHASH not set correctly");

	bufl = kalloc (NBUF * sizeof(BUF));
	hasharray = kalloc (NHASH * sizeof(BUF *));
	if (bufl == BNULL || hasharray == BNULL)
		panic("bufinit: insufficient memory for %d buffers", NBUF);

	for (i = 0; i < NHASH; ++i)
		hasharray [i] = BNULL;

	/*
	 * initialize the buffer header array with the physical and
	 * virtual addresses of the buffers, NULL values for the
	 * hash chain pointers, and pointers to the successor and
	 * predecessor of the current node.
	 */

	firstbuf = & bufl [0];
	for (bp = lastbuf = & bufl [NBUF - 1]; bp >= bufl; -- bp) {
		bp->b_dev = NODEV;
		bp->b_paddr = p;
		bp->b_vaddr = v;
		bp->b_hashf = BNULL;
		bp->b_hashb = BNULL;
		bp->b_LRUf = bp + 1;		/* next entry in chain */
		bp->b_LRUb = bp - 1;		/* prev entry in chain */

		__GATE_INIT (bp->b_gate, "buffer");

		p += BSIZE;
		v += BSIZE;
	}


	/*
	 * the first and last headers are special cases.
	 */

	bufl [0].b_LRUb = BNULL;		/* no predecessor */
	bufl [NBUF - 1].b_LRUf = BNULL;		/* no successor */
}

/*
 * NIGEL: This function is the only code that references drvl [] directly
 * other than the bogus code that manages the load and unload entry points,
 * which we will also need to "enhance". What we add to this code is a range
 * check so that it no longer can index off the end of drvl [], and in the
 * case that we would go off the end of drvl [] we vector instead to the
 * STREAMS system and ask it to return a kludged-up "CON *". The mapping
 * code referred to above is for the i286 and does nothing whatsoever, so
 * all this function really does as it stands is a table lookup.
 */

CON *
drvmap(dev)
dev_t dev;
{
	register DRV *dp;
	register unsigned m;

	if ((m = major(dev)) >= drvn) {
		CON	      *	conp;

		/*
		 * NIGEL: If STREAMS is disabled or there is no device
		 * corresponding to this (external) major number, flag ENXIO.
		 */

		if ((conp = STREAMS_GETCON (dev)) != NULL)
			return conp;

		SET_U_ERROR (ENXIO, "drvmap()");
		return NULL;
	}

	dp = drvl + m;
	if (dp->d_conp == NULL)
		SET_U_ERROR (ENXIO, "drvmap()");

	return dp->d_conp;
}

/*
 * Synchronise the buffer cache.
 */
bsync()
{
	register BUF *bp;

	for (bp = & bufl [NBUF - 1] ; bp >= bufl ; -- bp) {
		if ((bp->b_flag & BFMOD) == 0)
			continue;
		lock (bp->b_gate);
		if (bp->b_flag & BFMOD)
			bwrite (bp, 1);
		unlock (bp->b_gate);
	}
}

/*
 * Synchronise all blocks for a particular device in the buffer cache
 * and invalidate all references.
 */
bflush(dev)
register dev_t dev;
{
	register BUF *bp;

	for (bp = & bufl [NBUF - 1] ; bp >= bufl ; -- bp) {
		if (bp->b_dev != dev)
			continue;
		lock (bp->b_gate);
		if (bp->b_dev == dev) {
			if (bp->b_flag & BFMOD)
				bwrite (bp, 1);
			bp->b_dev = NODEV;
		}
		unlock (bp->b_gate);
	}
}

int	t_async = 0;

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

	bp = bclaim (dev, bno, sync);
	if (sync == BUF_ASYNC && t_async) {
		lock (bp->b_gate);
		unlock (bp->b_gate);
	}
	if (bp->b_flag & BFNTP) {
		if (sync == BUF_SYNC)
			ASSERT ((bp->b_flag & BFASY) == 0);
		else {
			/*
			 * If the BFASY flag is set, then we don't need to
			 * actually initiate a new operation. Whatever is
			 * happening to the buffer now is fine by us...
			 */
			if ((bp->b_flag & BFASY) != 0)
				return (BUF *) 1;

			/*
			 * Since we are actually going to perform some I/O
			 * on the buffer, we need to lock it first (it used
			 * to be that bclaim () would always do this, but that
			 * prevented useful parallelism).
			 */

			ASSERT (__GATE_LOCKED (bp->b_gate) == 0);
			lock (bp->b_gate);
			bp->b_flag |= BFASY;
		}
		bp->b_req = BREAD;
		bp->b_count = BSIZE;
		dblock (dev, bp);
		if (sync == BUF_ASYNC)
			return (BUF *) 2;

		/*
		 * If buffer is not valid, wait for it.
		 */

		s = sphi ();
		while (bp->b_flag & BFNTP) {
			x_sleep ((char *) bp, pridisk, slpriNoSig, "bpwait");
			/* If buffer is not valid, wait for it.  */
		}
		spl(s);

		if (bp->b_flag & BFERR) {
			SET_U_ERROR (bp->b_err ? bp->b_err : EIO, "bread()");
			brelease (bp);
			return NULL;
		}
		if (bp->b_resid == BSIZE) {
			brelease (bp);
			return NULL;
		}
	}
	if (sync == BUF_ASYNC)
		return (BUF *) 3;

	u.u_block ++;
	return bp;
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
bclaim(dev, bno, sync)
dev_t dev;
register daddr_t bno;
int		sync;
{
	register BUF *bp;
	register int s;
	unsigned long hashval;

	hashval = HASH (dev, bno) % NHASH;	/* select a hash bucket */

again:
	for (bp = hasharray [hashval]; bp != BNULL; bp = bp->b_hashf) {
		if (bp->b_bno == bno && bp->b_dev == dev) {
			if (sync == BUF_ASYNC) {
#if	1
				LRUupdate (bp);
#endif
				return bp;
			}

			lock (bp->b_gate);

			if (bp->b_bno != bno || bp->b_dev != dev) {
				ASSERT (0);
				unlock (bp->b_gate);
				goto again;
			}

			/*
			 * Now that we have located the buffer in the cache,
			 * unlink it from its current location in the
			 * LRU chain and move it to the front.
			 */

			LRUupdate (bp);

			/*
			 * If the buffer had an I/O error, mark it as
			 * invalid.
			 */

			if (bp->b_flag & BFERR)
				bp->b_flag |= BFNTP;
			return bp;
		}
	}

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
		for (bp = lastbuf ; bp != BNULL ; bp = bp->b_LRUb) {
			/*
			 * NIGEL: This code assumes that buffers can be locked
			 * only by other process-level code.
			 */

			if (__GATE_LOCKED (bp->b_gate))
				continue;	/* not available */

			if (bp->b_flag & BFMOD) {
				lock (bp->b_gate);
				bwrite (bp, 0);	/* flush dirty buffer */
				continue;
			}

			if (sync == BUF_SYNC)
				lock (bp->b_gate);

			/*
			 * Update the hash chain for this old
			 * buffer.  Unlink it from it's old location
			 * fixing up any references. Also, update
			 * the LRU chain to move the buffer to the head.
			 */

			HASHdelete (bp);
			LRUupdate (bp);

			bp->b_flag = BFNTP;
			bp->b_dev = dev;
			bp->b_bno = bno;
			bp->b_hashval = hashval;

			HASHinsert (bp);
			return bp;
		}
		s = sphi();
		bufneed = 1;
		x_sleep((char *)&bufneed, pridisk, slpriNoSig, "bufneed");
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

	if (sync)
		bp->b_flag &= ~BFASY;
	else
		bp->b_flag |= BFASY;

	bp->b_flag |= BFNTP;
	bp->b_req = BWRITE;
	bp->b_count = BSIZE;

	dblock (bp->b_dev, bp);

	if (! sync)
		return;

	s = sphi ();
	while (bp->b_flag & BFNTP) {
		x_sleep ((char *) bp, pridisk, slpriNoSig, "bwrite");
		/* Waiting for a buffer write to finish.  */
	}
	spl (s);
}

/*
 * This is called by the driver when I/O has completed on a buffer.
 */
bdone(bp)
register BUF *bp;
{
	if (bp->b_req == BWRITE)
		bp->b_flag &= ~ BFMOD;
	if (bp->b_req == BREAD) {
		if (bp->b_flag & BFERR)
			bp->b_dev = NODEV;
	}
	if (bp->b_flag & BFASY) {
		bp->b_flag &= ~ BFASY;
		brelease (bp);
	}
	bp->b_flag &= ~ BFNTP;
	dwakeup ((char *) bp);
}

/*
 * Release the given buffer.
 */
brelease(bp)
register BUF *bp;
{
	if (bp->b_flag & BFERR) {
		bp->b_flag &= ~ BFERR;
		bp->b_dev = NODEV;
	}
	bp->b_flag &= ~ BFNTP;

	unlock (bp->b_gate);
	if (bufneed) {
		bufneed = 0;
		wakeup ((char *) & bufneed);
	}
}

/*
 * Read data from the I/O segment into kernel space.
 *
 * "v" is the destination virtual address.
 * "n" is the number of bytes to read.
 */
ioread(iop, v, n)
register IO *iop;
register char *v;
register unsigned n;
{
	switch (iop->io_seg) {
	case IOSYS:
		iop->io.vbase += kkcopy(iop->io.vbase, v, n);
		break;

	case IOUSR:
		iop->io.vbase += ukcopy(iop->io.vbase, v, n);
		break;

	case IOPHY:
		dmain(n, iop->io.pbase, v);
		iop->io.pbase += n;
		break;
	}
	iop->io_ioc -= n;
}

/*
 * Clear I/O space.
 */

#if	__USE_PROTO__
void ioclear (IO * iop, size_t size)
#else
void
ioclear (iop, size)
IO	      *	iop;
size_t		size;
#endif
{
	switch (iop->io_seg) {
	case IOSYS:
		(void) memset (iop->io.vbase, 0, size);
		iop->io.vbase += size;
		break;

	case IOUSR:
		(void) umemclear (iop->io.vbase, size);
		iop->io.vbase += size;
		break;

	case IOPHY:
		dmaclear (size, iop->io.pbase);
		iop->io.pbase += size;
		break;
	}
	iop->io_ioc -= size;
}


/*
 * Write data from kernel space to the I/O segment.
 */

void
iowrite(iop, v, n)
register IO *iop;
register char *v;
register unsigned n;
{
	switch (iop->io_seg) {
	case IOSYS:
		memcpy (iop->io.vbase, v, n);
		iop->io.vbase += n;
		break;

	case IOUSR:
		iop->io.vbase += kucopy (v, iop->io.vbase, n);
		break;

	case IOPHY:
		dmaout (n, iop->io.pbase, v);
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

	if (iop->io_ioc == 0)
		return -1;
	-- iop->io_ioc;
	if (iop->io_seg == IOSYS)
		c = * (unsigned char *) iop->io.vbase ++;
	else {
		c = getubd (iop->io.vbase ++);
		if (u.u_error)
			return -1;
	}
	return c;
}

/*
 * Put a character using the I/O segment.
 */
ioputc(c, iop)
register IO *iop;
{
	if (iop->io_ioc == 0)
		return -1;
	-- iop->io_ioc;
	if (iop->io_seg == IOSYS)
		* (char *) iop->io.vbase ++ = c;
	else {
		putubd (iop->io.vbase ++, c);
		if (u.u_error)
			return -1;
	}
	return c;
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

	if ((cp = drvmap (dev)) == NULL)
		return;

	lock (bp->b_gate);
	n = cp->c_flag;	/* n should do something with that flag */

	if (iop) {
		if (f & BFBLK) {
			if (blocko (iop->io_seek)) {
				SET_U_ERROR (EIO, "ioreq()");
				goto out;
			}
		}
		if (f & BFIOC) {
			if (! iomapvp (iop, bp)) {
				SET_U_ERROR (EIO, "ioreq()");
				goto out;
			}
		}
	}
	bp->b_flag = f | BFNTP;
	bp->b_req = req;
	bp->b_dev = dev;
	if (iop) {
		bp->b_bno = blockn (iop->io_seek);
		bp->b_count = iop->io_ioc;
	}

	dblock (dev, bp);

	s = sphi ();
	while (bp->b_flag & BFNTP)
		x_sleep ((char *) bp, pridisk, slpriNoSig, "ioreq");
	spl (s);

	if (stimer.t_last)
		wakeup((char *)&stimer);
	if (bp->b_flag & BFERR) {
		SET_U_ERROR (bp->b_err ? bp->b_err : EIO, "ioreq()");
		goto out;
	}
	if (iop) {
		n = iop->io_ioc - bp->b_resid;
		iop->io_seek += n;
		iop->io_ioc -= n;
	}
out:
	unlock (bp->b_gate);
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
	register caddr_t iobase, base;
	unsigned ioc;
	int i;

	if (iop->io_seg != IOUSR)
		panic("Raw I/O from non user");

	iobase = iop->io.vbase;
	ioc = iop->io_ioc;

	for (srp = u.u_segl; srp < &u.u_segl[NUSEG]; srp++) {
		if ((sp = srp->sr_segp) == NULL)
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

		if (iobase < base)
			continue;
		if (iobase + ioc > base + sp->s_size)
			continue;
 		bp->b_paddr = MAPIO(sp->s_vmem, iobase - base);
		return sp;
	}

	/* Is the io area in question contained in a shared memory segment? */
	if ((srp = accShm (iobase, ioc)) != NULL) {
		sp = srp->sr_segp;
		base = srp->sr_base;
 		bp->b_paddr = MAPIO (sp->s_vmem, iobase - base);
		return sp;
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

	for (dp = drvl, mind = 0 ; mind < drvn ; mind ++, dp ++) {
		if (dp->d_conp && dp->d_conp->c_load) {
			(* dp->d_conp->c_load) ();
			dev_loaded |= (1 << mind);
		}
	}

	/*
	 * Inform STREAMS that it is time to set up shop.
	 */

	STREAMS_INIT ();
}

/*
 * Open a device.
 *
 * NIGEL: In order to make it at all possible to support the System V DDI/DDK
 * calling conventions for driver entry points, it is necessary for this code
 * to pass the *type* of open being made to the underlying device (which is
 * passed in the 'f' parameter below).
 */
dopen(dev, m, f)
register dev_t dev;
{
	register CON *cp;

	if ((cp = drvmap (dev)) == NULL)
		return;

	if ((cp->c_flag & f) == 0) {
		SET_U_ERROR (ENXIO, "dopen()");
		return;
	}

	(* cp->c_open) (dev, m, f);			/* NIGEL */
}

/*
 * Close a device.
 *
 * NIGEL: In order to be able to support the System V DDI/DDK calling
 * conventions for driver entry points, this function has to be altered to
 * accept a file-mode and character/block mode parameter. Note that the
 * Coherent 4.0 driver kit documentation says that the driver close entry
 * point is passed the same parameters as the open entry. After this mod,
 * this will be true for the first time.
 */
dclose(dev, mode, typ)
register dev_t dev;
{
	register CON *cp;

	if ((cp = drvmap (dev)) == NULL)
		return;

	(* cp->c_close) (dev, mode, typ);			/* NIGEL */
}

/*
 * Call the block entry point of a device.
 */
dblock(dev, bp)
dev_t dev;
BUF *bp;
{
	register CON *cp;

	if ((cp = drvmap (dev)) == NULL)
		return;

	(* cp->c_block) (bp);
}

/*
 * Read from a device.
 */
dread(dev, iop)
register dev_t dev;
register IO *iop;
{
	register CON *cp;

	if ((cp = drvmap (dev)) == NULL)
		return;

	(* cp->c_read) (dev, iop);
}

/*
 * Write to a device.
 */
dwrite(dev, iop)
register dev_t dev;
register IO *iop;
{
	register CON *cp;

	if ((cp = drvmap (dev)) == NULL)
		return;

	(* cp->c_write) (dev, iop);
}

/*
 * Call the ioctl function for a device.
 *
 * NIGEL: In order to support the System V DDI/DDK calling conventions for
 * device driver entry points, this function needs to pass a "mode" parameter
 * indicating the open mode of the file. There are only two calls to this
 * function, for uioctl () and in the /dev/tty driver, "io.386/ct.c" which is
 * passing its arguments back here (ie, a layered open). The "ct.c" call has
 * not been changed.
 *
 * NIGEL: To support the elimination of u_regl, the current user register set
 * is passed in here (NULL if we are being called from a driver).
 */

dioctl (dev, com, vec, mode, regsetp)
register dev_t dev;
union ioctl *vec;
gregset_t     *	regsetp;
{
	register CON *cp;

	if ((cp = drvmap (dev)) == NULL)
		return;

	if (regsetp != NULL) {
		/*
		 * Here we do a bunch of special hacks so that the tty code
		 * can remain ignorant of the myriad variants on the tty
		 * ioctl's.
		 */

		if (__xmode_286 (regsetp))
			tioc (dev, com, vec, cp->c_ioctl, mode);
		else if ((com == TIOCGETP &&
			  ! useracc (vec, sizeof (struct sgttyb), 1)) ||
			 ((com == TIOCSETP || com == TIOCSETN) &&
			  ! useracc (vec, sizeof (struct sgttyb), 0)))
			SET_U_ERROR (EFAULT, "dioctl ()");
		else
			(* cp->c_ioctl) (dev, com, vec, mode);
	} else
		(* cp->c_ioctl) (dev, com, vec, mode);
}


/*
 * Call the powerfail entry point of a device.
 */
dpower(dev)
register dev_t dev;
{
	register CON *cp;

	if ((cp = drvmap (dev)) == NULL)
		return;

	(* cp->c_power) (dev);
}

/*
 * Call the timeout entry point of a device.
 */

dtime (dev)
register dev_t dev;
{
	register CON *cp;

	if ((cp = drvmap (dev)) == NULL)
		return;

	(* cp->c_timer) (dev);
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

	if ((cp = drvmap (dev)) == NULL)
		return POLLNVAL;

	if (cp->c_flag & DFPOL)
		ev = (* cp->c_poll) (dev, ev, msec);
	else
		ev = POLLNVAL;

	return ev;
}

/*
 * Non existant device.
 */
nonedev()
{
	SET_U_ERROR (ENXIO, "nonedev()");
}

/*
 * Null device.
 */
nulldev()
{
}
