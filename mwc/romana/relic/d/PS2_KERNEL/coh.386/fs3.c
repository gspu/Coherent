/* $Header: /kernel/kersrc/coh.386/RCS/fs3.c,v 1.2 92/08/04 12:32:30 bin Exp Locker: bin $ */
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
 * Filesystem (I/O).
 *
 * $Log:	fs3.c,v $
 * Revision 1.2  92/08/04  12:32:30  bin
 * changed for kernel 59
 * 
 * Revision 1.2  92/01/06  11:59:34  hal
 * Compile with cc.mwc.
 * 
 * Revision 1.1	88/03/24  16:13:54	src
 * Initial revision
 * 
 * 87/11/25	Allan Cornish		/usr/src/sys/coh/fs3.c
 * vaddr_t bp->b_vaddr --> faddr_t bp->b_faddr.
 *
 * 86/02/01	Allan Cornish
 * Added code to fwrite() to avoid needless writing of pipe blocks.
 * Throughput on 6 Mhz AT rose from 30 Kbytes/sec to 79 Kbytes/sec.
 */
#include <sys/coherent.h>
#include <sys/buf.h>
#include <canon.h>
#include <sys/con.h>
#include <errno.h>
#include <sys/filsys.h>
#include <sys/mount.h>
#include <sys/io.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <sys/stat.h>

/*
 * Given an inode, open it.
 */
iopen(ip, mode)
register INODE *ip;
{
	register int type;

	type = ip->i_mode & IFMT;
	switch (type) {
	case IFCHR:
	case IFBLK:
		iunlock(ip);
		dopen(ip->i_a.i_rdev, mode, type==IFCHR ? DFCHR : DFBLK);
		ilock(ip);
		break;
	case IFDIR:
		if ((mode&IPW) != 0) {
			if (super() == 0)
				return;
			if (mode == IPW) {
				u.u_error = EISDIR;
				return;
			}
		}
		break;
	case IFPIPE:
		popen(ip, mode);
		break;
	}
}

/*
 * Given an inode, close it.
 */
iclose(ip)
register INODE *ip;
{
	ilock(ip);
	switch (ip->i_mode&IFMT) {
	case IFBLK:
		bflush(ip->i_a.i_rdev);
	case IFCHR:
		iunlock(ip);
		dclose(ip->i_a.i_rdev);
		ilock(ip);
		break;
	case IFPIPE:
		pclose(ip);
		break;
	}
	idetach(ip);
}

/*
 * Read from a file described by an inode and an io strucuture.
 */
iread(ip, iop)
register INODE *ip;
register IO *iop;
{
	if (iop->io_ioc == 0)
		return;
	switch (ip->i_mode&IFMT) {
	case IFCHR:
		dread(ip->i_a.i_rdev, iop);
		break;
	case IFBLK:
	case IFREG:
	case IFDIR:
		fread(ip, iop);
		break;
	case IFPIPE:
		pread(ip, iop);
		break;
	default:
		u.u_error = ENXIO;
		break;
	}
}

/*
 * Write to a file described by an inode and io structure.
 */
iwrite(ip, iop)
register INODE *ip;
register IO *iop;
{
	imod(ip);	/* write - mtime */
	icrt(ip);	/* write - ctime */
	if (iop->io_ioc == 0)
		return;
	switch (ip->i_mode&IFMT) {
	case IFCHR:
		dwrite(ip->i_a.i_rdev, iop);
		break;
	case IFBLK:
		fwrite(ip, iop);
		break;
	case IFREG:
	case IFDIR:
		if (getment(ip->i_dev, 1) == NULL)
			return;
		fwrite(ip, iop);
		break;
	case IFPIPE:
		pwrite(ip, iop);
		break;
	default:
		u.u_error = ENXIO;
		break;
	}
}

/*
 * Read from a regular or block special file.
 */
fread(ip, iop)
INODE *ip;
register IO *iop;
{
	register unsigned n;
	register unsigned i;
	register off_t res;
	register unsigned off;
	register dev_t dev;
	register daddr_t lbn;
	register daddr_t pbn;
	register daddr_t abn;
	register daddr_t zbn;
	register BUF *bp;
	register int blk;
	daddr_t list[NEXREAD];

	if ((ip->i_mode&IFMT) == IFBLK) {
		blk = 1;
		dev = ip->i_a.i_rdev;
	} else {
		blk = 0;
		dev = ip->i_dev;
	}
	abn = 0;
	zbn = 0;
	lbn = blockn(iop->io_seek);
	off = blocko(iop->io_seek);
	res = ip->i_size - iop->io_seek;
	if (blk!=0 || res>iop->io_ioc)
		res = iop->io_ioc;
	if (res <= 0)
		return;
	if (res+off <= BSIZE) {
		bp = blk ? bread(dev, lbn, 1) : vread(ip, lbn);
		if (bp == NULL)
			return;
		iowrite(iop, bp->b_vaddr+off, (unsigned)res);
		brelease(bp);
		return;
	}
	while (res > 0) {
		if (lbn >= zbn) {
			if ((n=blockn(res+BSIZE-1)) > NEXREAD)
				n = NEXREAD;
			if (n <= 0)
				n = 1;
			abn = lbn;
			for (i=0, zbn=lbn; i<n; i++, zbn++) {
				if (blk != 0)
					pbn = zbn;
				else {
					if ((pbn=vmap(ip, zbn)) < 0)
						return;
					if (pbn == 0) {
						list[i] = -1;
						continue;
					}
				}
				list[i] = pbn;
				bread(dev, pbn, 0);
			}
		}
		if ((pbn=list[lbn-abn]) < 0) {
			bp = bclaim(NODEV, (daddr_t)0);
			kclear(bp->b_vaddr, BSIZE);
		} else {
			if ((bp=bread(dev, pbn, 1)) == NULL)
				return;
		}
		n = BSIZE - off;
		n = res>n ? n : res;
		iowrite(iop, bp->b_vaddr+off, n);
		brelease(bp);
		if (u.u_error)
			return;
		lbn++;
		off = 0;
		res -= n;
	}
}

/*
 * Write to a regular or block special file.
 */
fwrite(ip, iop)
INODE *ip;
register IO *iop;
{
	register unsigned n;
	register unsigned off;
	register daddr_t lbn;
	register BUF *bp;
	register int blk;
	register int com;

	lbn = blockn(iop->io_seek);
	off = blocko(iop->io_seek);
	blk = (ip->i_mode&IFMT) == IFBLK;
	while (iop->io_ioc > 0) {
		n = BSIZE - off;
		n = iop->io_ioc>n ? n : iop->io_ioc;
		com = off==0 && n==BSIZE;
		if (blk == 0)
			bp = aread(ip, lbn, com);
		else {
			if (com)
				bp = bclaim(ip->i_a.i_rdev, lbn);
			else
				bp = bread(ip->i_a.i_rdev, lbn, 1);
		}
		if (bp == NULL)
			return;
		ioread(iop, bp->b_vaddr+off, n);
		bp->b_flag |= BFMOD;
		if (com && ((ip->i_mode&IFMT) != IFPIPE) )
			bwrite(bp, 0);
		else
			brelease(bp);
		if (u.u_error)
			return;
		lbn++;
		off = 0;
		if ((iop->io_seek+=n) > ip->i_size)
			if (blk == 0)
				ip->i_size = iop->io_seek;
	}
}

/*
 * Given an inode pointer, read the requested virtual block and return
 * a buffer with the data.
 */
BUF *
vread(ip, lb)
register INODE *ip;
daddr_t lb;
{
	register daddr_t pb;
	register BUF *bp;

	if ((pb=vmap(ip, lb)) < 0)
		return (NULL);
	if (pb != 0)
		return (bread(ip->i_dev, pb, 1));
	bp = bclaim(NODEV, (daddr_t)0);
	kclear(bp->b_vaddr, BSIZE);
	return (bp);
}

/*
 * Convert the given virtual block to a physical block for the given inode.
 * If the block does not map onto a physical block because the file is sparse
 * but it does exist, 0 is returned.  If an error is encountered, -1 is
 * returned.
 */
daddr_t
vmap(ip, lb)
register INODE *ip;
daddr_t lb;
{
	register BUF *bp;
	register int *lp;
	daddr_t * dp;
	daddr_t pb;
	int list[1+NI];

	if ((lp=lmap(lb, list)) == NULL)
		return (-1);
	pb = ip->i_a.i_addr[*--lp];
	for (;;) {
		if (pb==0 || lp==list)
			return (pb);
		if ((bp=bread(ip->i_dev, pb, 1)) == NULL)
			return (0);
		dp = bp->b_vaddr;
		pb = dp[*--lp];
		brelease(bp);
		candaddr(pb);
	}
}

/*
 * Given an inode pointer, read the requested virtual block and return a
 * buffer with the data.  In sparse files, the necessary blocks are allocated.
 * If the flag, `fflag' is set, the final buffer is just claimed rather than
 * read as we are going to change it's contents completely.
 */
BUF *
aread(ip, lb, fflag)
register INODE *ip;
daddr_t lb;
{
	register BUF *bp;
	register int *lp;
	register dev_t dev;
	register int l;
	register int aflag;
	register int lflag;
	daddr_t * dp;
	daddr_t pb;
	int list[1+NI];

	if ((lp=lmap(lb, list)) == NULL)
		return (NULL);
	aflag = 0;
	dev = ip->i_dev;
	pb = ip->i_a.i_addr[l=*--lp];
	if (pb == 0) {
		aflag = 1;
		if ((pb=balloc(dev)) == 0)
			return (NULL);
		ip->i_a.i_addr[l] = pb;
	}
	for (;;) {
		lflag = lp==list;
		if (aflag==0  &&  (fflag==0 || lflag==0)) {
			if ((bp=bread(dev, pb, 1)) == NULL)
				return (NULL);
		} else {
			bp = bclaim(dev, pb);
			kclear(bp->b_vaddr, BSIZE);
			bp->b_flag |= BFMOD;
		}
		if (lflag)
			return (bp);

		aflag = 0;
		dp = bp->b_vaddr;
		pb = dp[l=*--lp];
		candaddr(pb);
		if (pb == 0) {
			aflag = 1;
			if ((pb=balloc(dev)) == 0) {
				brelease(bp);
				return (NULL);
			}
			dp[l] = pb;
			candaddr( dp[l] );
			bp->b_flag |= BFMOD;
		}
		brelease(bp);
	}
}

/*
 * Given a block number, `b', store the offsets for the indirect blocks
 * backwards in the array, `lp', and return a pointer just after the
 * position where the first offset is stored.
 */
int *
lmap(b, lp)
register daddr_t b;
register int *lp;
{
	register int n;

	if (b < ND) {
		*lp++ = b;
		return (lp);
	}
	b -= ND;
	n = NI;
	do {
		if (n-- == 0) {
			u.u_error = EFBIG;
			return (NULL);
		}
		*lp = nbnrem(b);
		++lp;
		b = nbndiv(b);
	} while (b--);
	*lp++ = ND+NI-1-n;
	return (lp);
}
