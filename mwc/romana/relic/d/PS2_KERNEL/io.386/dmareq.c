/* $Header: /kernel/kersrc/io.386/RCS/dmareq.c,v 1.2 92/08/04 12:52:39 bin Exp Locker: bin $ */
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
 * Like ioreq, but guarantee that no DMA straddle occurs.
 * And assume we are called by fl.c, xt.c, dv.c or someone
 * else who obeys the parameter rules that they do.
 *
 * $Log:	dmareq.c,v $
 * Revision 1.2  92/08/04  12:52:39  bin
 * update for ker59
 * 
 * Revision 1.2  92/01/06  12:26:52  hal
 * Compile with cc.mwc.
 * 
 * Revision 2.1	88/09/03  13:03:47	src
 * *** empty log message ***
 * 
 * Revision 1.1	88/03/24  17:04:28	src
 * Initial revision
 * 
 * 87/11/25	Allan Cornish		/usr/src/sys/i8086/drv/dmareq.c
 * vaddr_t bp->b_vaddr --> faddr_t bp->b_faddr.
 *
 * 87/01/05	Allan Cornish		/usr/src/sys/i8086/drv/dmareq.c
 * dmareq() now wakes &stimer only if the swap timer is active.
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
#include <sys/dmac.h>

dmareq(bp, iop, dev, req)
register BUF *bp;
register IO *iop;
dev_t dev;
{
	register int to_read;
	register SEG *sp;
	register CON *cp;
	dold_t dold;
	long next_block;	/* Next block to be read.  */
	paddr_t last;		/* Address of last byte we will read.  */
	paddr_t next_seg;	/* Beginning of next dma segment.  */
	BUF *tbp;

	if ((cp=drvmap(dev, &dold)) == NULL)
		return;
	lock(bp->b_gate);
	drest(dold);
	if (blocko(iop->io_seek) != 0) {
		u.u_error = EIO;
		goto out;
	}
	if ((sp=iomapvp(iop, bp)) == NULL) {
		u.u_error = EIO;
		goto out;
	}
	bp->b_dev = dev;
	bp->b_flag = 0;
	sp->s_lrefc++;
	/*
	 * The dma address is 20 bits; 16 bit offset counter from a 4 bit
	 * base segment.  Since io_ioc is limited to 32Kb positive, we
	 * have at most two raw transfers separated by a block which
	 * straddles the segment boundary.
	 * Life would be simpler if we assumed io_ioc % BSIZE, but
	 * flioctl comes through here with it's short format buffer.
	 */
	while (iop->io_ioc > 0 && (bp->b_flag&BFERR) == 0) {
		/*
		 * If the first and last bytes to be read are on different
		 * dma segments, we have a stradle, and must read a fraction
		 * of the entire animal.
		 *
 		 * I don't understand why a fractional block at the
		 * top of a segment is not read.
		 */
		last = bp->b_paddr+iop->io_ioc-1;
		if (dmaseg(last) != dmaseg(bp->b_paddr)) {
			/*
			 * We have a straddle--read at most to the end
			 * of the next segment.
			 */
			next_seg = dmaseg(bp->b_paddr + DMASEG_SIZE);
			to_read = (next_seg - bp->b_paddr) & ~((long)BSIZE-1);
		} else {
			/* No straddle.  */
			to_read = iop->io_ioc;
		}
		next_block = blockn(iop->io_seek);
		if (to_read == 0) {
			/* Straddle block */
			tbp = bp;		/* Save the raw buffer */
			to_read = BSIZE;
			if (to_read > iop->io_ioc)
				to_read = iop->io_ioc;
			bp = bclaim(dev, next_block);
			bp->b_count = to_read;
			bp->b_req = req;
			if (req != BREAD)
				ioread(iop, bp->b_vaddr, to_read);
			dmabuf(bp, dev);
			if ((bp->b_flag&BFERR) == 0) {
				if (req == BREAD)
					iowrite(iop, bp->b_vaddr, to_read);
			} else {
				tbp->b_flag = bp->b_flag;
				tbp->b_err = bp->b_err;
				if (req != BREAD)
					iop->io_ioc += bp->b_resid;
			}
			bp->b_flag |= BFERR;
			brelease(bp);
			bp = tbp;		/* Reclaim raw buffer */
		} else {
			/* Raw transfer */
			bp->b_count = to_read;
			bp->b_req = req;
			bp->b_bno = next_block;
			dmabuf(bp, dev);
			if ((bp->b_flag&BFERR) != 0)
				to_read -= bp->b_resid;
			iop->io_ioc -= to_read;	   /* cookedio do these */
			iop->io.pbase += to_read;  /* for everyone */
		}
		bp->b_vaddr += to_read;
		bp->b_paddr  += to_read;
		iop->io_seek += to_read;
		/* And continue for the next chunk */
	}
	sp->s_lrefc--;
	if ( stimer.t_last != 0 )
		wakeup((char *)&stimer);
	if ((bp->b_flag&BFERR) != 0 && (u.u_error = bp->b_err) == 0)
		u.u_error = EIO;
out:
	unlock(bp->b_gate);
}

static
dmabuf(bp, dev)
register BUF *bp;
dev_t dev;
{
	register int s;
	bp->b_flag = BFRAW|BFBLK|BFIOC|BFNTP;
	s = sphi();
	dblock(dev, bp);
	while ((bp->b_flag&BFNTP) != 0)
		sleep((char *)bp, CVBLKIO, IVBLKIO, SVBLKIO);
	spl(s);
}
