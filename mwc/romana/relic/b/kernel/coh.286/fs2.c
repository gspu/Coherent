/* $Header: /kernel/kersrc/coh.286/RCS/fs2.c,v 1.1 92/07/17 15:18:06 bin Exp Locker: bin $ */
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
 * Filesystem (disk inodes).
 *
 * $Log:	fs2.c,v $
 * Revision 1.1  92/07/17  15:18:06  bin
 * Initial revision
 * 
 * Revision 1.1	88/03/24  16:13:51	src
 * Initial revision
 * 
 * 87/11/25	Allan Cornish		/usr/src/sys/coh/fs2.c
 * vaddr_t bp->b_vaddr --> faddr_t bp->b_faddr.
 *
 * 87/04/29	Allan Cornish		/usr/src/sys/coh/fs2.c
 * Fsminit panic messages now specify the root major and minor device.
 *
 * 86/11/19	Allan Cornish		/usr/src/sys/coh/fs2.c
 * setacct() initializes the (new) (IO).io_flag field to 0.
 *
 * 85/08/08	Allan Cornish
 * ialloc() erroneously did a brelease(NULL) if bclaim() returned NULL.
 * also, sbp->s_fmod was set BEFORE the in-core inode table was updated.
 * This created a critical race with msync() (called by sync system call).
 *
 * 85/04/17	Allan Cornish
 * eliminated test for rootdev in msync()
 */
#include <sys/coherent.h>
#include <acct.h>
#include <sys/buf.h>
#include <canon.h>
#include <sys/con.h>
#include <errno.h>
#include <sys/filsys.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <sys/io.h>
#include <sys/mount.h>
#include <sys/proc.h>
#include <sys/stat.h>

/*
 * Initialise filesystem.
 */
fsminit()
{
	register MOUNT *mp;

	/*
	 * Mount the root file system.
	 */
	if ( (mp = fsmount(rootdev, ronflag)) == NULL )
		panic(	"fsminit: no rootdev(%d,%d)",
			major(rootdev), minor(rootdev) );

	/*
	 * Set system time from the super block.
	 */
	timer.t_time = mp->m_super.s_time;

	/*
	 * Access the root directory.
	 */
	if ( (u.u_rdir = iattach(rootdev, ROOTIN)) == NULL )
		panic(	"fsminit: no / on rootdev(%d,%d)",
			major(rootdev), minor(rootdev) );

	/*
	 * Record current directory.
	 */
	u.u_cdir = u.u_rdir;
	u.u_cdir->i_refc++;
	iunlock(u.u_rdir);
}

/*
 * Mount the given device.
 */
MOUNT *
fsmount(dev, f)
register dev_t dev;
{
	register MOUNT *mp;
	register BUF *bp;

	if ((mp=kalloc(sizeof(MOUNT))) == NULL)
		return (NULL);
	dopen(dev, (f?IPR:IPR|IPW), DFBLK);
	if (u.u_error != 0) {
		kfree(mp);
		return (NULL);
	}
	if ((bp=bread(dev, (daddr_t)SUPERI, 1)) == NULL) {
		dclose(dev);
		kfree(mp);
		return (NULL);
	}
	kkcopy(FP_OFF(bp->b_faddr), &mp->m_super, sizeof(struct filsys));
	brelease(bp);
	cansuper(&mp->m_super);
	mp->m_ip = NULL;
	mp->m_dev = dev;
	mp->m_flag = f;
	mp->m_super.s_fmod = 0;
	mp->m_next = mountp;
	mountp = mp;
	return (mp);
}

/*
 * Canonize a super block.
 */
cansuper(fsp)
register struct filsys *fsp;
{
	register int i;

	canint(fsp->s_isize);
	candaddr(fsp->s_fsize);
	canshort(fsp->s_nfree);
	for (i=0; i<NICFREE; i++)
		candaddr(fsp->s_free[i]);
	canshort(fsp->s_ninode);
	for (i=0; i<NICINOD; i++)
		canino(fsp->s_inode[i]);
	cantime(fsp->s_time);
	candaddr(fsp->s_tfree);
	canino(fsp->s_tinode);
	canshort(fsp->s_m);
	canshort(fsp->s_n);
	canlong(fsp->s_unique);
}

/*
 * Given a pointer to a mount entry, write out all inodes on that device.
 */
msync(mp)
register MOUNT *mp;
{
	register struct filsys *sbp;
	register BUF *bp;

	if ((mp->m_flag&MFRON) != 0)
		return;
	isync(mp->m_dev);
	sbp = &mp->m_super;
	if (sbp->s_fmod==0)
		return;
	bp = bclaim(mp->m_dev, (daddr_t)SUPERI);
	sbp->s_time = timer.t_time;
	sbp->s_fmod = 0;
	kkcopy(sbp, FP_OFF(bp->b_faddr), sizeof(*sbp));
	cansuper(FP_OFF(bp->b_faddr));
	bwrite(bp, 1);
	brelease(bp);
}

/*
 * Return the mount entry for the given device.  If `f' is not set
 * and the device is read only, don't set the error status.
 */
MOUNT *
getment(dev, f)
register dev_t dev;
{
	register MOUNT *mp;

	for (mp=mountp; mp!=NULL; mp=mp->m_next) {
		if (mp->m_dev != dev)
			continue;
		if ((mp->m_flag&MFRON) != 0) {
			if (f != 0)
				u.u_error = EROFS;
			return (NULL);
		}
		return (mp);
	}
	panic("getment: dev=0x%x", dev);
}

/*
 * Allocate a new inode with the given mode.  The returned inode is locked.
 */
INODE *
ialloc(dev, mode)
dev_t dev;
unsigned mode;
{
	register struct dinode *dip;
	register struct filsys *sbp;
	register ino_t *inop;
	register ino_t ino;
	register BUF *bp;
	register daddr_t b;
	register struct dinode *dipe;
	register ino_t *inope;
	register MOUNT *mp;
	register INODE *ip;

	if ((mp=getment(dev, 1)) == NULL)
		return (NULL);
	sbp = &mp->m_super;
	for (;;) {
		lock(mp->m_ilock);
		if (sbp->s_ninode == 0) {
			ino = 1;
			inop = sbp->s_inode;
			inope = &sbp->s_inode[NICINOD];
			for (b=INODEI; b<sbp->s_isize; b++) {
				if (bad(dev, b)) {
					ino += INOPB;
					continue;
				}
				if ((bp=bread(dev, b, 1)) == NULL) {
					ino += INOPB;
					continue;
				}
				dip = FP_OFF(bp->b_faddr);
				dipe = &dip[INOPB];
				for (; dip<dipe; dip++, ino++) {
					if (dip->di_mode != 0)
						continue;
					if (inop >= inope)
						break;
					*inop++ = ino;
				}
				brelease(bp);
				if (inop >= inope)
					break;
			}
			sbp->s_ninode = inop - sbp->s_inode;
			if (sbp->s_ninode == 0) {
				sbp->s_tinode = 0;
				unlock(mp->m_ilock);
				devmsg(dev, "Out of inodes");
				u.u_error = ENOSPC;
				return (NULL);
			}
		}
		ino = sbp->s_inode[--sbp->s_ninode];
		--sbp->s_tinode;
		sbp->s_fmod = 1;
		unlock(mp->m_ilock);
		if ((ip=iattach(dev, ino)) != NULL) {
			if (ip->i_mode != 0) {
				devmsg(dev, "Inode %u busy", ino);
				idetach(ip);
				continue;
			}
			ip->i_flag = 0;
			ip->i_mode = mode;
			ip->i_nlink = 0;
			ip->i_uid = u.u_uid;
			ip->i_gid = u.u_gid;
		}
		return (ip);
	}
}

/*
 * Free the inode `ino' on device `dev'.
 */
ifree(dev, ino)
dev_t dev;
ino_t ino;
{
	register struct filsys *sbp;
	register MOUNT *mp;

	if ((mp=getment(dev, 1)) == NULL)
		return;
	lock(mp->m_ilock);
	sbp = &mp->m_super;
	sbp->s_fmod = 1;
	if (sbp->s_ninode < NICINOD)
		sbp->s_inode[sbp->s_ninode++] = ino;
	sbp->s_tinode++;
	unlock(mp->m_ilock);
}

/*
 * Free all blocks in the indirect block `b' on the device `dev'.
 * `l' is the level of indirection.
 */
indfree(dev, b, l)
dev_t dev;
daddr_t b;
register unsigned l;
{
	register int i;
	register BUF *bp;
	daddr_t * dp;
	daddr_t b1;

	if (b == 0)
		return;
	if (l-->0 && (bp=bread(dev, b, 1))!=NULL) {
		i = NBN;
		while (i-- > 0) {
			dp = FP_OFF(bp->b_faddr);

			if ((b1 = dp[i]) == 0)
				continue;
			candaddr(b1);
			if (l == 0)
				bfree(dev, b1);
			else
				indfree(dev, b1, l);
		}
		brelease(bp);
	}
	bfree(dev, b);
}

/*
 * Allocate a block from the filesystem mounted of device `dev'.
 */
daddr_t
balloc(dev)
dev_t dev;
{
	register struct filsys *sbp;
	register struct fblk *fbp;
	register daddr_t b;
	register BUF *bp;
	register MOUNT *mp;

	if ((mp=getment(dev, 1)) == NULL)
		return (0);
	lock(mp->m_flock);
	sbp = &mp->m_super;
	if (sbp->s_nfree == 0) {
enospc:
		sbp->s_nfree = 0;
		devmsg(dev, "Out of space");
		u.u_error = ENOSPC;
		b = 0;
	} else {
		sbp->s_fmod = 1;
		if ((b=sbp->s_free[--sbp->s_nfree]) == 0)
			goto enospc;
		if (sbp->s_nfree == 0) {
			if (b >= sbp->s_fsize
			 || b < sbp->s_isize
			 || (bp = bread(dev, b, 1)) == NULL) {
ebadflist:
				devmsg(dev, "Bad free list");
				goto enospc;
			}
			fbp = FP_OFF(bp->b_faddr);
			sbp->s_nfree = fbp->df_nfree;
			canshort(sbp->s_nfree);
			if ((unsigned)sbp->s_nfree > NICFREE)
				goto ebadflist;
			kkcopy(fbp->df_free, sbp->s_free, sizeof(sbp->s_free));
			canndaddr(sbp->s_free, sbp->s_nfree);
			brelease(bp);
		}
		--sbp->s_tfree;
		if (b >= sbp->s_fsize || b < sbp->s_isize)
			goto ebadflist;
	}
	unlock(mp->m_flock);
	return (b);
}

/*
 * Free the block `b' on the device `dev'.
 */
bfree(dev, b)
dev_t dev;
daddr_t b;
{
	register struct filsys *sbp;
	register struct fblk *fbp;
	register BUF *bp;
	register MOUNT *mp;

	if ((mp=getment(dev, 1)) == NULL)
		return;
	sbp = &mp->m_super;
	if (b>=sbp->s_fsize || b<sbp->s_isize) {
		devmsg(dev, "Bad block %u (free)", (unsigned)b);
		return;
	}
	lock(mp->m_flock);
	if (sbp->s_nfree == 0 || sbp->s_nfree == NICFREE) {
		bp = bclaim(dev, b);
		fbp = FP_OFF(bp->b_faddr);
		kclear(fbp, BSIZE);
		fbp->df_nfree = sbp->s_nfree;
		canshort(fbp->df_nfree);
		kkcopy(sbp->s_free, fbp->df_free, sizeof(fbp->df_free));
		canndaddr(fbp->df_free, sbp->s_nfree);
		bp->b_flag |= BFMOD;
		brelease(bp);
		sbp->s_nfree = 0;
	}
	sbp->s_free[sbp->s_nfree++] = b;
	sbp->s_tfree++;
	sbp->s_fmod = 1;
	unlock(mp->m_flock);
}

/*
 * Determine if the given block is bad.
 */
bad(dev, b)
dev_t dev;
daddr_t b;
{
	register INODE *ip;
	register BUF *bp;
	register int i;
	register int m;
	register int n;
	daddr_t l;

	if ((ip=iattach(dev, 1)) == NULL)
		panic("bad()");
	n = blockn(ip->i_size);
	if ((m=n) > ND)
		m = ND;
	for (i=0; i<m; i++) {
		--n;
		if (b == ip->i_a.i_addr[i]) {
			idetach(ip);
			return (1);
		}
	}
	l = ip->i_a.i_addr[ND];
	idetach(ip);
	if (n == 0)
		return (0);
	if ((bp=bread(dev, l, 1)) == NULL)
		return (0);
	if ((m=n) > NBN)
		m = NBN;
	for (i=0; i<m; i++) {
		l = ((daddr_t *)bp)[i];
		candaddr(l);
		if (b == l) {
			brelease(bp);
			return (1);
		}
	}
	brelease(bp);
	return (0);
}

/*
 * Canonize `n' disk addresses.
 */
canndaddr(dp, n)
register daddr_t *dp;
register int n;
{
	while (n--) {
		candaddr(*dp);
		dp++;
	}
}

/*
 * Write out an accounting record.
 */
setacct()
{
	register PROC *pp;
	struct acct acct;
	IO acctio;

	if (acctip == NULL)
		return;
	pp = SELF;
	kkcopy(u.u_comm, acct.ac_comm, 10);
	acct.ac_utime = ltoc(pp->p_utime);
	acct.ac_stime = ltoc(pp->p_stime);
	acct.ac_etime = ltoc(timer.t_time - u.u_btime);
	acct.ac_btime = u.u_btime;
	acct.ac_uid = u.u_uid;
	acct.ac_gid = u.u_gid;
	acct.ac_mem = 0;
	acct.ac_io = ltoc(u.u_block);
	acct.ac_tty = pp->p_ttdev;
	acct.ac_flag = u.u_flag;
	ilock(acctip);
	acctio.io_seek = acctip->i_size;
	acctio.io_ioc  = sizeof (acct);
	acctio.io_base = &acct;
	acctio.io_seg  = IOSYS;
	acctio.io_flag = 0;
	iwrite(acctip, &acctio);
	iunlock(acctip);
	u.u_error = 0;
}
