/* $Header: /y/coh.386/RCS/fs1.c,v 1.4 92/07/16 16:33:32 hal Exp $ */
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
 * Filesystem (mostly handling of in core inodes).
 *
 * $Log:	fs1.c,v $
 * Revision 1.4  92/07/16  16:33:32  hal
 * Kernel #58
 * 
 * Revision 1.3  92/02/06  17:55:36  vlad
 * Fix typo in ialloc panic.
 * 
 * Revision 1.2  92/01/06  11:59:17  hal
 * Compile with cc.mwc.
 * 
 * Revision 1.1	88/03/24  16:13:47	src
 * Initial revision
 * 
 * 86/12/13	Allan Cornish		/usr/src/sys/coh/fs1.c
 * isync() no longer updates the disk image of a character device inode.
 *
 * 86/11/19	Allan Cornish		/usr/src/sys/coh/fs1.c
 * idirent() initializes the (new) (IO).io_flag field to 0.
 */
#include <sys/coherent.h>
#include <sys/buf.h>
#include <canon.h>
#include <sys/dir.h>
#include <errno.h>
#include <sys/filsys.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <sys/io.h>
#include <sys/mount.h>>
#include <sys/stat.h>

/*
 * Get character for `ftoi' depending on what space the characters are
 * coming from.
 */
#define ftoic(p)	(u.u_io.io_seg==IOSYS ? *p : getubd(p))

/*
 * Map the given filename to an inode.  If an error is encountered,
 * `u.u_error' is set.  `u.u_error' is always returned.  As this routine
 * needs to set several things, depending on the type of access, `t',
 * there are places in the processes' user area reserved for this routine
 * to set.  These are defined in the user process structure.  The seek
 * position is always set to the position of the directory entry of the
 * child if the child exists or the first free position if it doesn't.
 *  'r' =>  Reference.  A pointer to the child's inode is returned locked.
 *  'c' =>  Create.  If the child exists, a pointer to the inode is returned
 *          locked.  Otherwise if the parent directory exists, a pointer to
 *          the parent directory is returned locked.  Otherwise, an error.
 *  'u' =>  Unlink.  The parent directory is returned unlocked.  The child's
 *          inode number is returned.  The seek position is also set.
 */
ftoi(np, t)
char *np;
{
	register INODE *cip;
	register char *cp;
	register int c;
	register struct direct *dp;
	register BUF *bp;
	off_t cseek, fseek, s;
	int fflag, mflag;
	dev_t dev;
	ino_t ino;
	daddr_t b;

	u.u_cdirn = 0;
	u.u_cdiri = NULL;
	u.u_pdiri = NULL;
	if ((c=ftoic(np++)) != '/')
		cip = u.u_cdir;
	else {
		c = ftoic(np++);
		cip = u.u_rdir;
	}
	while (c == '/')
		c = ftoic(np++);
	ilock(cip);
	cip->i_refc++;
	if (c == '\0') {
		if (t == 'r') {
			u.u_cdiri = cip;
			return (u.u_error);
		}
		u.u_error = ENOENT;
		idetach(cip);
		return (u.u_error);
	}
	for (;;) {
		cp = u.u_direct.d_name;
		while (c!='/' && c!='\0') {
			if (cp < &u.u_direct.d_name[DIRSIZ])
				*cp++ = c;
			c = ftoic(np++);
		}
		while (c == '/')
			c = ftoic(np++);
		while (cp < &u.u_direct.d_name[DIRSIZ])
			*cp++ = '\0';
		if ((cip->i_mode&IFMT) != IFDIR)
			u.u_error = ENOTDIR;
		else
			iaccess(cip, IPE);
		if (u.u_error) {
			idetach(cip);
			return (u.u_error);
		}
		cp = u.u_direct.d_name;
		if (cip->i_ino==ROOTIN && cip->i_dev!=rootdev)
			if (*cp++=='.' && *cp++=='.' && *cp++=='\0')
				cip = ftoim(cip);
		b = 0;
		fflag = 0;
		mflag = 0;
		cseek = 0;
		s = cip->i_size;
		while (s > 0) {
			if ((bp=vread(cip, b++)) == NULL) {
				idetach(cip);
				return (u.u_error);
			}
			dp = bp->b_vaddr;
			while (dp < bp->b_vaddr+BSIZE) {
				if ((s-=sizeof(*dp)) < 0)
					break;
				if ((ino=dp->d_ino) == 0) {
					if (fflag == 0) {
						fflag++;
						fseek = cseek;
					}
				} else {
					if (direq(dp)) {
						canino(ino);
						mflag = 1;
						s = 0;
						break;
					}
				}
				cseek += sizeof(*dp);
				dp++;
			}
			brelease(bp);
		}
		dev = cip->i_dev;
		if (fflag == 0)
			fseek = cseek;
		if (mflag == 0) {
			if (c=='\0' && t=='c') {
				u.u_pdiri = cip;
				u.u_io.io_seek = fseek;
			} else {
				u.u_error = ENOENT;
				idetach(cip);
			}
			return (u.u_error);
		}
		if (c == '\0') {
			if (t == 'u') {
				u.u_cdirn = ino;
				u.u_pdiri = cip;
				u.u_io.io_seek = cseek;
				return (u.u_error);
			}
			idetach(cip);
			u.u_cdiri = iattach(dev, ino);
			return (u.u_error);
		}
		idetach(cip);
		if ((cip=iattach(dev, ino)) == NULL)
			return (u.u_error);
	}
}

/*
 * Given an inode which is the root of a file system, return the inode
 * on which the file system was mounted.
 */
INODE *
ftoim(ip)
register INODE *ip;
{
	register MOUNT *mp;

	for (mp=mountp; mp!=NULL; mp=mp->m_next) {
		if (mp->m_dev == ip->i_dev) {
			idetach(ip);
			ip = mp->m_ip;
			ilock(ip);
			ip->i_refc++;
			break;
		}
	}
	return (ip);
}

/*
 * Compare the string in `u.u_direct.d_name' with the name in the
 * given directory pointer.
 */
direq(dp)
struct direct *dp;
{
	register char *cp1, *cp2;
	register unsigned n;

	if (dp->d_ino == 0)
		return (0);
	cp1 = dp->d_name;
	cp2 = u.u_direct.d_name;
	n = DIRSIZ;
	do {
		if (*cp1++ != *cp2++)
			return (0);
	} while (--n);
	return (1);
}

/*
 * Make an inode of the given mode and device.  The parent directory,
 * name and such stuff is set by ftoi.
 */
INODE *
imake(mode, rdev)
unsigned mode;
dev_t rdev;
{
	register INODE *ip;

	ip = NULL;
	mode &= ~u.u_umask;
	if ((mode&ISVTXT)!=0 && super()==0)
		goto det;
	if (iaccess(u.u_pdiri, IPW) == 0)
		goto det;
	if ((ip=ialloc(u.u_pdiri->i_dev, mode)) == NULL)
		goto det;
	ip->i_nlink = 1;
	ip->i_a.i_rdev = rdev;
	idirent(ip->i_ino);
	iamc(ip);	/* creat/mknod - atime/mtime/ctime */
det:
	idetach(u.u_pdiri);
	return (ip);
}

/*
 * Write a directory entry out.  Everything necessary has been conveniently
 * set by `ftoi', except the new inode number of this directory entry.
 */
idirent(ino)
{
	u.u_direct.d_ino = ino;
	canino(u.u_direct.d_ino);
	u.u_io.io_ioc  = sizeof (struct direct);
	u.u_io.io.vbase = &u.u_direct;
	u.u_io.io_seg  = IOSYS;
	u.u_io.io_flag = 0;
	iwrite(u.u_pdiri, &u.u_io);
}

/*
 * Return a pointer to a locked inode in core containing the given
 * inode number and device.
 */
INODE *
iattach(dev, ino)
{
	register INODE *ip;
	register INODE *fip;
	register unsigned lrt;
	register MOUNT *mp;

	for (;;) {
		fip = NULL;
		for (ip=&inodep[NINODE-1]; ip>=inodep; --ip) {
			if (ip->i_ino==ino && ip->i_dev==dev)
				break;
			if (ip->i_refc == 0) {
				if (fip==NULL || ip->i_lrt<lrt) {
					fip = ip;
					lrt = ip->i_lrt;
				}
			}
		}
		if (ip < inodep) {
			if ((ip=fip) == NULL) {
				devmsg(dev, "Inode table overflow");
				u.u_error = ENFILE;
				return (NULL);
			}
			ilock(ip);
			if (ip->i_refc != 0) {
				iunlock(ip);
				continue;
			}
			ip->i_dev = dev;
			ip->i_ino = ino;
			ip->i_refc = 1;
			ip->i_lrt = timer.t_time;
			if (icopydm(ip) == 0) {
				ip->i_ino = 0;
				ip->i_refc = 0;
				iunlock(ip);
				return (NULL);
			}
			return (ip);
		}
		if ((ip->i_flag&IFMNT) != 0) {
			for (mp=mountp; mp!=NULL; mp=mp->m_next) {
				if (mp->m_ip == ip) {
					ino = ROOTIN;
					dev = mp->m_dev;
					break;
				}
			}
			continue;
		}
		ilock(ip);
		if (ip->i_ino!=ino || ip->i_dev!=dev) {
			iunlock(ip);
			continue;
		}
		if (ip->i_refc < 0)
			panic("ialloc(%x)", ip);
		ip->i_refc++;
		ip->i_lrt = timer.t_time;
		return (ip);
	}
}

/*
 * Given a locked inode, deaccess it.
 */
idetach(ip)
register INODE *ip;
{
	if (ilocked(ip)==0 || ip->i_refc<=0)
		panic("idetach(%p)", ip);
	if (--ip->i_refc == 0) {
#if	1
		if (ip->i_rl != NULL)
			panic("idetach(%p) with locked records", ip);
#endif
		if ((ip->i_flag&(IFACC|IFMOD|IFCRT)) != 0
		 || ip->i_nlink == 0)
			icopymd(ip);
	}
	iunlock(ip);
}

/*
 * Given a inode which isn't locked, lock it and then deaccess.
 */
ldetach(ip)
register INODE *ip;
{
	ilock(ip);
	idetach(ip);
}

/*
 * A specialized routine for finding whether the given inode may be unlinked.
 * Quite simple you say, but we already have an inode locked and could run
 * into gating problems if we were to lock another.  So we look through the
 * cache to see if the inode is there.  If it is, we can easily tell.  If it
 * isn't, `icopydm' is called with a static.  This routine is only used by
 * `uunlink'.
 */
iucheck(dev, ino)
register dev_t dev;
register ino_t ino;
{
	register INODE *ip;
	INODE inode;

	for (ip=&inodep[NINODE-1]; ip>=inodep; --ip) {
		if (ip->i_ino==ino && ip->i_dev==dev)
			break;
	}
	if (ip < inodep) {
		ip = &inode;
		ip->i_dev = dev;
		ip->i_ino = ino;
		if (icopydm(ip) == 0)
			return (0);
	}
	if ((ip->i_mode&IFMT) == IFDIR) {
		if (super() == 0)
			return (0);
	}
	return (1);
}

/*
 * Copy an inode from disk to memory performing canonization.
 */
icopydm(ip)
register INODE *ip;
{
	register struct dinode *dip;
	register BUF *bp;
	register ino_t ino;
	struct dinode dinode;
	vaddr_t v;

	ip->i_flag = 0;
	ino = ip->i_ino;

	if ((bp=bread(ip->i_dev, (daddr_t)iblockn(ino), 1)) == NULL)
		return (0);

	dip = &dinode;
	v = (char *)((struct dinode *)bp->b_vaddr + iblocko(ino));
	kkcopy( v, dip, sizeof(dinode));
	brelease(bp);
	ip->i_mode = dip->di_mode;
	canshort(ip->i_mode);
	ip->i_nlink = dip->di_nlink;
	canshort(ip->i_nlink);
	ip->i_uid = dip->di_uid;
	canshort(ip->i_uid);
	ip->i_gid = dip->di_gid;
	canshort(ip->i_gid);
	ip->i_size = dip->di_size;
	cansize(ip->i_size);

	switch (ip->i_mode&IFMT) {
	case IFBLK:
	case IFCHR:
		ip->i_a.i_rdev = dip->di_a.di_rdev;
		candev(ip->i_a.i_rdev);
		break;
	case IFREG:
	case IFDIR:
		l3tol(ip->i_a.i_addr, dip->di_a.di_addb, NADDR);
		break;
	case IFPIPE:
		l3tol(ip->i_pipe, dip->di_addp, ND);
		ip->i_pnc = dip->di_pnc;
		canint(ip->i_pnc);
		ip->i_prx = dip->di_prx;
		canint(ip->i_prx);
		ip->i_pwx = dip->di_pwx;
		canint(ip->i_pwx);
		break;
	default:
		kclear(&ip->i_a, sizeof(ip->i_a));
		break;
	}

	ip->i_atime = dip->di_atime;
	cantime(ip->i_atime);
	ip->i_mtime = dip->di_mtime;
	cantime(ip->i_mtime);
	ip->i_ctime = dip->di_ctime;
	cantime(ip->i_ctime);
	ip->i_rl = NULL;
	return (1);
}

/*
 * Copy an inode from memory back on to disk performing canonization.
 */
icopymd(ip)
register INODE *ip;
{
	register struct dinode *dip;
	register BUF *bp;
	register ino_t ino;
	struct dinode dinode;
	vaddr_t v;

	if (getment(ip->i_dev, 0) == NULL)
		return;

	ino = ip->i_ino;
	if (ip->i_refc==0 && ip->i_nlink==0 && ino!=BADFIN && ino!=ROOTIN) {
		iclear(ip);
		ip->i_lrt = 0;
		ip->i_mode = 0;
		ifree(ip->i_dev, ino);
	}

	dip = &dinode;
	dip->di_mode = ip->i_mode;
	canshort(dip->di_mode);
	dip->di_nlink = ip->i_nlink;
	canshort(dip->di_nlink);
	dip->di_uid = ip->i_uid;
	canshort(dip->di_uid);
	dip->di_gid = ip->i_gid;
	canshort(dip->di_gid);
	dip->di_size = ip->i_size;
	cansize(dip->di_size);

	switch (ip->i_mode&IFMT) {
	case IFBLK:
	case IFCHR:
		dip->di_a.di_rdev = ip->i_a.i_rdev;
		candev(dip->di_a.di_rdev);
		break;
	case IFREG:
	case IFDIR:
		ltol3(dip->di_addr, ip->i_a.i_addr, NADDR);
		break;
	case IFPIPE:
		ltol3(dip->di_addp, ip->i_pipe, ND);
		dip->di_pnc = ip->i_pnc;
		canshort(dip->di_pnc);
		dip->di_prx = ip->i_prx;
		canshort(dip->di_prx);
		dip->di_pwx = ip->i_pwx;
		canshort(dip->di_pwx);
		break;
	default:
		kclear(&dip->di_a, sizeof(dip->di_a));
		break;
	}

	dip->di_atime = ip->i_atime;
	cantime(dip->di_atime);
	dip->di_mtime = ip->i_mtime;
	cantime(dip->di_mtime);
	dip->di_ctime = ip->i_ctime;
	cantime(dip->di_ctime);

	if ((bp=bread(ip->i_dev, (daddr_t)iblockn(ino), 1)) == NULL)
		return;

	v = (char *)((struct dinode *)bp->b_vaddr + iblocko(ino));
	kkcopy(dip, v, sizeof(dinode));
	bp->b_flag |= BFMOD;
	brelease(bp);
	ip->i_flag &= ~(IFACC|IFMOD|IFCRT);
}

/*
 * Copy all relevant inodes out on device `dev'.
 */
isync(dev)
register dev_t dev;
{
	register INODE *ip;

	for (ip=&inodep[NINODE-1]; ip>=inodep; --ip) {
		if (ip->i_refc == 0)
			continue;
		if (ip->i_dev != dev)
			continue;
		if ( (ip->i_mode & IFMT) == IFCHR )
			continue;
		if ((ip->i_flag&(IFACC|IFMOD|IFCRT)) == 0)
			continue;
		icopymd(ip);
	}
}

/*
 * Clear the given inode and all space associated with it.
 */
iclear(ip)
register INODE *ip;
{
	register int n;
	register daddr_t b;

	switch (ip->i_mode&IFMT) {
	case IFPIPE:
		ip->i_pnc = 0;
		ip->i_prx = 0;
		ip->i_pwx = 0;
		n = ND;
		break;
	case IFDIR:
	case IFREG:
		n = NADDR;
		break;
	default:
		return;
	}
	while (n > ND) {
		if ((b=ip->i_a.i_addr[--n]) != 0)
			indfree(ip->i_dev, b, 1+n-ND);
	}
	while (n > 0) {
		if ((b=ip->i_a.i_addr[--n]) != 0)
			bfree(ip->i_dev, b);
	}
	ip->i_size = 0;
	kclear(ip->i_a.i_addr, sizeof(ip->i_a.i_addr));
	iamc(ip);	/* creat/pipe - atime/mtime/ctime */
}

/*
 * Copy the appropriate information from the inode to the stat buffer.
 */
istat(ip, sbp)
register INODE *ip;
register struct stat *sbp;
{
	sbp->st_dev = ip->i_dev;
	sbp->st_ino = ip->i_ino;
	sbp->st_mode = ip->i_mode;
	sbp->st_nlink = ip->i_nlink;
	sbp->st_uid = ip->i_uid;
	sbp->st_gid = ip->i_gid;
	sbp->st_rdev = NODEV;
	sbp->st_size = ip->i_size;
	sbp->st_atime = ip->i_atime;
	sbp->st_mtime = ip->i_mtime;
	sbp->st_ctime = ip->i_ctime;
	switch (ip->i_mode&IFMT) {
	case IFBLK:
	case IFCHR:
		sbp->st_rdev = ip->i_a.i_rdev;
		sbp->st_size = 0;
		break;
	case IFPIPE:
		sbp->st_size = ip->i_pnc;
		break;
	}
}

/*
 * See if it is possible to access the given inode with the bits in
 * the given mode.
 * If the mode includes writing, and i_refc is > 1, then check for
 * shared text problems.
 */
iaccess(ip, mode)
register INODE *ip;
register int mode;
{
	if ((imode(ip, u.u_uid, u.u_gid)&mode) != mode) {
		u.u_error = EACCES;
		return (0);
	}
	if ((mode&IPW) != 0 && ip->i_refc > 1 && sbusy(ip)) {
		u.u_error = ETXTBSY;
		return (0);
	}
	return (1);
}

/*
 * Get the maximum allowable mode on a file.
 */
imode(ip, uid, gid)
register INODE *ip;
{
	if (uid == 0)
		return (IPR|IPW|IPE);
	if (uid == ip->i_uid)
		return ((ip->i_mode>>6)&07);
	if (gid == ip->i_gid)
		return ((ip->i_mode>>3)&07);
	return (ip->i_mode&07);
}
