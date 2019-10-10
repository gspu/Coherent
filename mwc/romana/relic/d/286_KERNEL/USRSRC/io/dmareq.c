/* $Header: /usr/src/sys/i8086/drv/RCS/dmareq.c,v 2.1 88/09/03 13:03:47 src Exp $ */
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
 * $Log:	/usr/src/sys/i8086/drv/RCS/dmareq.c,v $
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
#include <sys/uproc.h>
#include <sys/dmac.h>

dmareq(bp, iop, dev, req)
register BUF *bp;
register IO *iop;
dev_t dev;
{
	register int n;
	register SEG *sp;
	register CON *cp;
	dold_t dold;
	long l;
	BUF *tbp;

	if ((cp=drvmap(dev, &dold)) == NULL)
		return;
	lock(bp->b_gate);
	n = cp->c_flag;
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
	bp->b_faddr = ptov( bp->b_paddr, (fsize_t) bp->b_count );
	/*
	 * The dma address is 20 bits; 16 bit offset counter from a 4 bit
	 * base segment.  Since io_ioc is limited to 32Kb positive, we
	 * have at most two raw transfers separated by a block which
	 * straddles the segment boundary.
	 * Life would be simpler if we assumed io_ioc % BSIZE, but
	 * flioctl comes through here with it's short format buffer.
	 */
	while (iop->io_ioc > 0 && (bp->b_flag&BFERR) == 0) {
		l = dmaseg(bp->b_paddr+iop->io_ioc-1) - bp->b_paddr;
		if (l < 0)
			n = iop->io_ioc;
		else
			n = l & ~((long)BSIZE-1);
		l = blockn(iop->io_seek);
		if (n == 0) {
			/* Straddle block */
			tbp = bp;		/* Save the raw buffer */
			n = BSIZE;
			if (n > iop->io_ioc)
				n = iop->io_ioc;
			bp = bclaim(dev, l);
			bp->b_count = n;
			bp->b_req = req;
			if (req != BREAD)
				ioread(iop, FP_OFF(bp->b_faddr), n);
			dmabuf(bp, dev);
			if ((bp->b_flag&BFERR) == 0) {
				if (req == BREAD)
					iowrite(iop, FP_OFF(bp->b_faddr), n);
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
			bp->b_count = n;
			bp->b_req = req;
			bp->b_bno = l;
			dmabuf(bp, dev);
			if ((bp->b_flag&BFERR) != 0)
				n -= bp->b_resid;
			iop->io_ioc -= n;	/* cookedio do these */
			iop->io_base += n;	/* for everyone */
		}
		FP_OFF(bp->b_faddr) += n;
		bp->b_paddr  += n;
		iop->io_seek += n;
		/* And continue for the next chunk */
	}
	vrelse( bp->b_faddr );
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
