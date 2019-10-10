/* $Header: /src386/STREAMS/coh.386/RCS/fs3.c,v 2.3 93/08/09 13:35:38 bin Exp Locker: bin $ */
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
 * Revision 2.3  93/08/09  13:35:38  bin
 * Kernel 82 changes
 * 
 * Revision 2.2  93/07/26  14:28:33  nigel
 * Nigel's R80
 * 
 * Revision 1.5  93/04/14  10:06:33  root
 * r75
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

#include <common/_tricks.h>
#include <sys/debug.h>
#include <sys/coherent.h>
#include <sys/buf.h>
#include <canon.h>
#include <sys/con.h>
#include <sys/errno.h>
#include <sys/filsys.h>
#include <sys/mount.h>
#include <sys/io.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <sys/stat.h>
#include <sys/file.h>

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
		if (mode & IPW) {

			/* Return (EISDIR) if not superuser. */
			if (super() == 0) {
				/* Override EPERM set when super() failed. */
				u.u_error = EISDIR;
				return;
			}

			/*
			 * Opening a directory O_WRONLY is insane, even
			 * if you are superuser!
			 */
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
 *
 * NIGEL: Modified for new dclose ().
 */
iclose(ip, mode)
register INODE *ip;
{
	register int type;

	ilock(ip);
	switch (type = ip->i_mode&IFMT) {
	case IFBLK:
		bflush(ip->i_a.i_rdev);
		/* FALL THROUGH */
	case IFCHR:
		iunlock(ip);
		dclose(ip->i_a.i_rdev, mode,  type==IFCHR ? DFCHR : DFBLK);
		ilock(ip);
		break;

	case IFPIPE:
		pclose(ip, mode);
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

	switch (ip->i_mode & IFMT) {
	case IFCHR:
		dread (ip->i_a.i_rdev, iop);
		break;

	case IFBLK:
	case IFREG:
	case IFDIR:
		fread (ip, iop);
		break;

	case IFPIPE:
		pread (ip, iop);
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
	imod (ip);	/* write - mtime */
	icrt (ip);	/* write - ctime */
	if (iop->io_ioc == 0)
		return;

	switch (ip->i_mode & IFMT) {
	case IFCHR:
		dwrite (ip->i_a.i_rdev, iop);
		break;

	case IFBLK:
		fwrite (ip, iop);
		break;

	case IFREG:
	case IFDIR:
		if (getment (ip->i_dev, 1) == NULL)
			return;
		fwrite (ip, iop);
		break;

	case IFPIPE:
		pwrite (ip, iop);
		break;

	default:
		u.u_error = ENXIO;
		break;
	}
}

/*
 * Given a block number, `b', store the offsets for the indirect blocks
 * backwards in the array, `lp', and return a pointer just after the
 * position where the first offset is stored.
 */

static int *
lmap (b, lp, numblocks)
register daddr_t b;
register int *lp;
int	      *	numblocks;
{
	register int n;

	if ((n = ND - b) > 0) {
		/*
		 * Just the one direct block, and further blocks up to the end
		 * of the block list in the inode.
		 */
		* lp ++ = b;
		* numblocks = n;
		return lp;
	}
	b -= ND;

	/*
	 * First, the initial indirect block, followed by as many further
	 * layers of indirection as we need.
	 */

	n = nbnrem (b);
	* numblocks = NBN - n;
	* lp ++ = n;

	if ((b = nbndiv (b)) == 0) {
		* lp ++ = ND;
		return lp;
	}


#if	NI > 1
	b --;	/* Make offset in next indirect block zero-based */

	* lp ++ = nbnrem (b);
	if ((b = nbndiv (b)) == 0) {
		* lp ++ = ND + 1;
		return lp;
	}

#if	NI > 2
	b --;	/* Make offset in next indirect block zero-based */

	* lp ++ = nbnrem (b);
	if ((b = nbndiv (b)) == 0) {
		* lp ++ = ND + 2;
		return lp;
	}
#endif
#endif
	SET_U_ERROR (EFBIG, "lmap");
	return NULL;
}

int	t_groupmode = 0;

/*
 * Convert the given virtual block to a physical block for the given inode.
 * If the block does not map onto a physical block because the file is sparse
 * but it does exist, 0 is returned.  If an error is encountered, -1 is
 * returned.
 */

static int
vmap (ip, lb, count, blocklist, allocflag)
register INODE *ip;
daddr_t		lb;
int		count;
daddr_t	      *	blocklist;
int		allocflag;
{
	daddr_t pb;
	int list [1 + NI];
	int		nblocks;
	daddr_t	      *	outlist;
	BUF	      *	buf;
	int	      *	lp;
	int		resid = count;

more:
	if ((lp = lmap (lb, list, & nblocks)) == NULL)
		return -1;

	if (nblocks > resid)
		nblocks = resid;
	resid -= nblocks;
	lb += nblocks;

	outlist = ip->i_a.i_addr;
	buf = NULL;

	while (-- lp != list) {
		if ((pb = outlist [* lp]) == 0) {
			/*
			 * If an indirect block is not present, then this
			 * implies that at least the next "nblocks" leaf
			 * blocks are also not present.
			 */

			do
				* blocklist ++ = -1;
			while (-- nblocks > 0);
			goto done;
		}

		if (buf != NULL) {
			brelease (buf);
			candaddr (pb);
		}

		if ((buf = bread (ip->i_dev, pb, BUF_SYNC)) == NULL)
			return -1;

		outlist = (daddr_t *) buf->b_vaddr;
	}

	do {
		if ((pb = outlist [list [0] ++]) == 0)
			pb = -1;
		else if (buf != NULL)
			candaddr (pb);
		* blocklist ++ = pb;
	} while (-- nblocks > 0);

done:
	if (buf != NULL)
		brelease (buf);

	if (t_groupmode && resid > 0)
		goto more;

	return count - resid;
}

int	t_readahead = 0;
#define	READGROUP	16		/*
					 * Maximum # of blocks to read as a
					 * single normal group.
					 */
#define	READAHEAD	8		/*
					 * Maximum # of blocks to read ahead.
					 */

/*
 * Read from a regular or block special file.
 */
fread(ip, iop)
INODE *ip;
register IO *iop;
{
	register int	n;
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
	daddr_t		list [READGROUP + READAHEAD];
	int		do_readahead;

	if ((ip->i_mode & IFMT) == IFBLK) {
		blk = 1;
		dev = ip->i_a.i_rdev;
	} else {
		blk = 0;
		dev = ip->i_dev;
	}
	abn = 0;
	zbn = 0;
	lbn = blockn (iop->io_seek);
	off = blocko (iop->io_seek);

	/*
	 * NIGEL: The commented-out code talks about a mysterious "unsigned
	 * prob" which does not in reality exist. All this really wants to
	 * do is pick the minimum of the remaining size and the requested
	 * size.
	 */

#if	0
	res = ip->i_size - iop->io_seek;

	if (blk != 0 || (res > 0 && res > iop->io_ioc)) 
		res = iop->io_ioc;	/* unsigned prob with io_ioc */
	if (res <= 0)
		return;
#endif

	if (blk)
		res = iop->io_ioc;
	else if ((res = ip->i_size - iop->io_seek) > iop->io_ioc)
		res = iop->io_ioc;

	if (res == 0)
		return;

	/*
	 * NIGEL: Test whether we want readahead based on whether this access
	 * immediately follows some previous access... this does not apply to
	 * inodes made from pipes, because there readahead will make us seek
	 * beyond the space that is legal (pipes store funky data where a
	 * normal file has indirect block pointers).
	 */

	if ((lbn == ip->i_lastblock + 1) && (ip->i_mode & IFMT) != IFPIPE) {
		if ((do_readahead = t_readahead) < 0)
			do_readahead = 0;
	} else
		do_readahead = 0;

	do {
		if (lbn >= zbn) {
			if ((n = blockn (res + BSIZE - 1) + do_readahead) >
			    __ARRAY_LENGTH (list))
				n = __ARRAY_LENGTH (list);

			ASSERT (n > do_readahead);

			if (blk == 0 && (n = vmap (ip, lbn, n, list, 0)) < 0)
				return;

			abn = lbn;
			for (i = 0, zbn = lbn ; i < n ; i ++, zbn ++) {
				if (blk != 0)
					list [i] = pbn = zbn;
				else if ((pbn = list [i]) == 0)
					continue;

				if (t_readahead != -1)
					(void) bread (dev, pbn, BUF_ASYNC);
			}
		}

		if (res < (n = BSIZE - off))
			n = res;

		if ((pbn = list [lbn - abn]) < 0)
			ioclear (iop, n);
		else {
			if ((bp = bread (dev, pbn, BUF_SYNC)) == NULL)
				return;
			iowrite (iop, bp->b_vaddr + off, n);
			brelease (bp);
		}

		if (u.u_error)
			return;
		lbn ++;
		off = 0;
	} while ((res -= n) > 0);

	ip->i_lastblock = lbn - 1;
}

int	t_writemode = 0;

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
	blk = (ip->i_mode & IFMT) == IFBLK;
	while (iop->io_ioc > 0) {
		n = BSIZE - off;
		n = iop->io_ioc > n ? n : iop->io_ioc;
		com = off == 0 && n == BSIZE;
		if (blk == 0)
			bp = aread (ip, lbn, com);
		else {
			if (com)
				bp = bclaim (ip->i_a.i_rdev, lbn, BUF_SYNC);
			else
				bp = bread (ip->i_a.i_rdev, lbn, BUF_SYNC);
		}
		if (bp == NULL)
			return;
		ioread (iop, bp->b_vaddr + off, n);
		bp->b_flag |= BFMOD;
		if (com && (ip->i_mode & IFMT) != IFPIPE) {
			bwrite (bp, t_writemode);
			if (t_writemode)
				brelease (bp);
		} else
			brelease (bp);
		if (u.u_error)
			return;
		lbn ++;
		off = 0;
		if ((iop->io_seek += n) > ip->i_size)
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
	daddr_t		pb;
	register BUF *bp;

	if (vmap (ip, lb, 1, & pb, 0) < 0)
		return NULL;
	if (pb != -1)
		return bread (ip->i_dev, pb, BUF_SYNC);
	bp = bclaim (NODEV, (daddr_t) 0, BUF_SYNC);
	kclear (bp->b_vaddr, BSIZE);
	return bp;
}

/*
 * Given an inode pointer, read the requested virtual block and return a
 * buffer with the data.  In sparse files, the necessary blocks are allocated.
 * If the flag, `fflag' is set, the final buffer is just claimed rather than
 * read as we are going to change it's contents completely.
 */

BUF *
aread (ip, lb, fflag)
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
	int		nblocks;

	if ((lp = lmap (lb, list, & nblocks)) == NULL)
		return (NULL);
	aflag = 0;
	dev = ip->i_dev;
	pb = ip->i_a.i_addr [l = * -- lp];
	if (pb == 0) {
		aflag = 1;
		if ((pb = balloc (dev)) == 0)
			return NULL;
		ip->i_a.i_addr [l] = pb;
	}
	for (;;) {
		lflag = lp == list;
		/*
		 * If we are not allocating a new block and the caller is
		 * going to preserve any of the data that we are going to
		 * return, then read in the previous block contents.
		 */
		if (! (aflag || (fflag && lflag))) {
			if ((bp = bread (dev, pb, BUF_SYNC)) == NULL)
				return NULL;
		} else {
			bp = bclaim (dev, pb, BUF_SYNC);

			/*
			 * If this is the last block and the caller is just
			 * going to overwrite it, don't zero-fill.
			 */

			if (! (fflag && lflag))
				kclear (bp->b_vaddr, BSIZE);
			bp->b_flag |= BFMOD;
		}
		if (lflag)
			return bp;

		aflag = 0;
		dp = bp->b_vaddr;
		pb = dp [l = * -- lp];
		candaddr (pb);
		if (pb == 0) {
			aflag = 1;
			if ((pb = balloc (dev)) == 0) {
				brelease(bp);
				return (NULL);
			}
			dp [l] = pb;
			candaddr (dp [l]);
			bp->b_flag |= BFMOD;
		}
		brelease (bp);
	}
}
