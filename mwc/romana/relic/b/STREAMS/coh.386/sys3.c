/* $Header: /src386/STREAMS/coh.386/RCS/sys3.c,v 2.4 93/08/09 13:36:42 bin Exp Locker: bin $ */
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
 * System calls (more filesystem related calls).
 */
#include <sys/coherent.h>
#include <sys/buf.h>
#include <sys/con.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <sys/fd.h>
#include <sys/filsys.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <sys/io.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/file.h>

/*
 * Open the file `np' with the mode `mode'.
 */
uopen(np, oflag, magic)
char *np;
{
	register int f;
	register INODE *ip;
	register fd_t fd;
	int cflag;	/* Flag is set if we create a file.  */
	IO		io;
	struct direct	dir;

	cflag = 0;	/* Nothing created so far.  */

	/*
	 * NIGEL: As reported numerous times by customers, this stupid code
	 * will create a file before looking to see whether it can open a file
	 * descriptor. In fact, any error in here will leave a new file around
	 * despite the error return.
	 *
	 * Do it right; allocate the resources first!
	 */

	if ((fd = fdalloc ()) == ERROR_FD)
		return -1;


	/* Determine read or write status for fdopen.  */

	switch (oflag & 3) {
	case O_RDONLY:
		f = IPR;
		break;
	case O_WRONLY:
		f = IPW;
		break;
	case O_RDWR:
		f = IPR|IPW;
		break;
	default:
		SET_U_ERROR( EINVAL, "bad oflag" );
		T_PIGGY( 0x10000, printf("<open: bad oflag %x>", oflag));
		goto done;
	}

	/* Process the O_CREAT flag.  */
	if ((oflag & O_CREAT) != 0) {

		io.io_seg = IOUSR;
		if (ftoi (np, 'c', & io, & dir) != 0) {
			T_PIGGY( 0x10000,
				printf("<open: bad ftoi(%s, 'c')>", np));
			goto done;
		}

		/* If it didn't exist, but its parent did, then make it.  */
		if ((ip = u.u_cdiri) == NULL) {
			if ((ip = imake ((magic & ~ IFMT) | IFREG,
					 0, & io, & dir)) == NULL) {
				T_PIGGY (0x10000, 
					printf ("<open: bad imake(%x, 0)>",
						(magic & ~ IFMT) | IFREG));
				goto done;
			}
			cflag = 1;	/* Note that we just created a file.  */
		} else {	/* The file already exists.  */
			/*
			 * Exclusive O_CREAT on existing file should fail.
			 */
			if ((oflag & O_EXCL) != 0) {
				idetach (ip);
				SET_U_ERROR (EEXIST,
					 "exclusive creat on existing file");
				goto done;
			}
			/*
			 * Do not write to a read only file system;
			 * never write to a directory;
			 * always write to block and character special devices.
			 */
			switch (ip->i_mode & IFMT) {
			case IFBLK:
			case IFCHR:
				break;

			case IFDIR:
				idetach (ip);
				SET_U_ERROR (EISDIR, "<open: EISDIR>");
				goto done;

			default:
				if (getment (ip->i_dev, 1) == NULL) {
					idetach (ip);
					SET_U_ERROR (EROFS,
						"Could not fetch mount entry");
					T_PIGGY (0x10000,
printf("<open: bad getment(ip->i_dev: %x, 1)>", ip->i_dev));
					goto done;
				}
			}
		} /* Did the file exist?  */

	} else { /* O_CREAT was not set--just reference the file.  */

		io.io_seg = IOUSR;
		if (ftoi (np, 'r', & io, & dir) != 0) {
			T_PIGGY( 0x10000, printf("<open: bad ftoi(%s, 'r')>",
				np));
			goto done;
		}
		ip = u.u_cdiri;	/* This must be the inode we wanted.  */
	}

	/*
	 * ASSERTION: We probably have an inode for an existing file.
	 * If we don't, the ip will be NULL and iaccess() will fail (as
	 * desired.)
	 */

	/*
	 * Only check permissions on a pre-existing file.
	 */
	if (0 == cflag && iaccess (ip, f) == 0) {
		idetach (ip);
		T_PIGGY (0x10000,
			 printf ("<open: bad access(ip:%x, f:%x)>", ip, f));
		goto done;
	}

	/*
	 * ASSERTION: We have an inode for a file we
	 * have valid permissions on.
	 */

	if ((ip->i_flag & IFEXCL) != 0) {
		idetach (ip);
		SET_U_ERROR (EEXIST, "open: file already open O_EXCL");
		goto done;	/* Somebody else has an exclusive open.  */
	}

	/*
	 * If requesting exclusive open, fail if someone else has it open.
	 */
	if ((oflag & O_EXCL) != 0) {
		if (ip->i_refc != 1) {
			idetach (ip);
			SET_U_ERROR (EEXIST, "<open: O_EXCL but already open>");
			goto done;
		}

		/* Mark this open inode as exclusive.  */
		ip->i_flag &= IFEXCL;
	}

	if ((oflag & O_NDELAY) != 0)
		f |= IPNDLY;
	if ((oflag & O_NONBLOCK) != 0)
		f |= IPNONBLOCK;
	if ((oflag & O_APPEND) != 0)
		f |= IPAPPEND;
	if ((oflag & O_SYNC) != 0)
		f |= IPSYNC;
	if ((oflag & O_EXCL) != 0)
		f |= IPEXCL;
	if ((oflag & O_NOCTTY) != 0)
		f |= IPNOCTTY;

	if (fdinit (fd, ip, f) < 0) {
		idetach (ip);
		T_PIGGY (0x10000,
			 printf ("<open: bad fdopen(ip: %x, f: %x>", ip, f));
		goto done;
	}

	/* If requested, truncate the file.  */
	if ((oflag & O_TRUNC) != 0 && ((ip->i_mode & IFPIPE) != IFPIPE)) {
		if (0 == cflag) {	/* No need to truncate a new file.  */
			if (iaccess (ip, IPW) != 0) {
				iclear (ip);
			} else {
				idetach (ip);
				T_PIGGY (0x10000,
				    printf("<open: No access to truncate.>"));
				goto done;
			}
		}
	}

	iunlock (ip);

done:
	return (fd = fdfinish (fd)) == ERROR_FD ? -1 : fd;
}


/*
 * Create a pipe.  Notice, we must do the IPR fdopen with IPNDLY so that
 * we don't block waiting for the writer we are about to create.  Then
 * after we are done, we ufcntl() to turn off the IPNDLY on fd1.
 */
upipe(fdp)
short fdp[2];
{
	register INODE *ip;
	register fd_t fd1;
	register fd_t fd2;

	if ((ip = pmake (0)) == NULL)
		return;
	if ((fd1 = fdopen (ip, IPR | IPNDLY)) != ERROR_FD) {
		ip->i_refc ++;
		if ((fd2 = fdopen (ip, IPW)) != ERROR_FD) {
			iunlock (ip);
			u.u_rval2 = fd2;
			ufcntl (fd1, F_SETFL, 0);
			return fd1;
		}
		-- ip->i_refc;
		iunlock (ip);
		fdclose (fd1);
		return 0;
	}
	idetach (ip);
	return 0;
}

/*
 * Read `n' bytes into the buffer `bp' from file number `fd'.
 */
uread(fd, bp, n)
char *bp;
unsigned n;
{
	T_PIGGY (0x200, printf("uread(fd: %d, bp: %x, n: %d)", fd, bp, n));
	return sysio (fd, bp, n, 0);
}


/*
 * Read or write `n' bytes from the file number `fd' using the buffer
 * `bp'.  If `do_write' is nonzero, write, else read.
 */

int
sysio (fd, bp, n, do_write)
int fd;
char *bp;
unsigned n;
int do_write;
{
	register FD *fdp;
	register INODE *ip;
	register int type;
	IO		io;

	if ((fdp = fdget(fd)) == NULL)
		return 0;

	if ((fdp->f_flag & (do_write ? IPW : IPR)) == 0) {
		u.u_error = EBADF;
		return 0;
	}

	/*
	 * When reading (writing into user memory), buffer may NOT be in text
	 * segment.  When writing (reading from user memory), buffer may
	 * be in text segment.
	 */
	if (! useracc (bp, n, ! do_write)) {
		u.u_error = EFAULT;
		return 0;
	}

	ip = fdp->f_ip;
	type = ip->i_mode & IFMT;
	if (type != IFCHR)
		ilock (ip);

	/* Writes in append mode are forced to end of file. */
	if ((fdp->f_flag & IPAPPEND) != 0 && do_write)
		fdp->f_seek = ip->i_size;

	if (do_write && (ip->i_mode & IFMT) == IFREG) {
		long maxbyte = (long) u.u_bpfmax * BSIZE;
		if (maxbyte <= fdp->f_seek)
			n = 0;
		else if ((long) n > maxbyte - fdp->f_seek)
			n = (unsigned) (maxbyte - fdp->f_seek);
	}

	io.io_seg = IOUSR;
	io.io_seek = fdp->f_seek;
	io.io.vbase = bp;
	io.io_ioc  = n;
	io.io_flag = 0;

	if ((fdp->f_flag & IPNDLY) != 0)
		io.io_flag |= IONDLY;
	if ((fdp->f_flag & IPNONBLOCK) != 0)
		io.io_flag |= IONONBLOCK;

	if (do_write) {
		iwrite(ip, & io);
	} else {
		iread(ip, & io);
		iacc (ip);		/* read - atime */
	}
	n -= io.io_ioc;
	fdp->f_seek += n;

	if (type != IFCHR)
		iunlock (ip);

	/* Was this inode opened for synchronous writes?  */
	if ((fdp->f_flag & IPSYNC) != 0)
		isync (ip->i_dev);

	return n;
}


/*
 * Return a status structure for the given file name.
 */

ustat(np, stp)
char *np;
struct stat *stp;
{
	register INODE *ip;
	struct stat stat;
	IO		io;
	struct direct	dir;

	if (ftoi (np, 'r', & io, & dir) != 0)
		return;

	ip = u.u_cdiri;
	istat (ip, & stat);
	idetach (ip);

	if (kucopy (& stat, stp, sizeof (stat)) != sizeof (stat)) {
		u.u_error = EFAULT;
		return -1;
	}
	return 0;
}

/*
 * Write out all modified buffers, inodes and super blocks to disk.
 */
usync()
{
	register MOUNT *mp;
	static GATE syngate;

	lock(syngate);
	for (mp=mountp; mp!=NULL; mp=mp->m_next)
		msync(mp);
	bsync();
	unlock(syngate);
	return 0;
}

/*
 * Set the mask for file access.
 */
uumask(mask)
{
	register int omask;

	omask = u.u_umask;
	u.u_umask = mask & 0777;
	return (omask);
}

/*
 * Unmount the given device.
 */
uumount(sp)
char *sp;
{
	register INODE *ip;
	register MOUNT *mp;
	register MOUNT **mpp;
	register dev_t rdev;
	register int mode;
	IO		io;
	struct direct	dir;

	if (ftoi (sp, 'r', & io, & dir) != 0)
		return;

	ip = u.u_cdiri;
	if (iaccess (ip, IPR | IPW) == 0) {
		idetach (ip);
		return;
	}

	rdev = ip->i_a.i_rdev;
	mode = ip->i_mode;

	idetach (ip);
	if ((mode & IFMT) != IFBLK) {
		u.u_error = ENOTBLK;
		return;
	}
	for (mpp = & mountp ; (mp = * mpp) != NULL ; mpp = & mp->m_next)
		if (mp->m_dev == rdev)
			break;

	if (mp == NULL) {
		u.u_error = EINVAL;
		return;
	}

	msync (mp);
	for (ip = & inodep [NINODE - 1] ; ip >= inodep ; -- ip) {
		if (ip->i_refc > 0 && ip->i_dev == rdev) {
			u.u_error = EBUSY;
			return;
		}
	}

	for (ip = & inodep [NINODE - 1] ; ip >= inodep ; -- ip) {
		if (ip->i_dev == rdev)
			ip->i_ino = 0;
	}

	bflush (rdev);
	dclose (rdev, mp->m_flag ? IPR : IPR | IPW, DFBLK);/* NIGEL */
	*mpp = mp->m_next;
	mp->m_ip->i_flag &= ~ IFMNT;

	ldetach (mp->m_ip);
	kfree (mp);
	return 0;
}

/*
 * Unlink the given file.
 */

uunlink (np)
char *np;
{
	(void) do_unlink (np, IOUSR);
	return 0;
}


/*
 * Internal version of unlink () called by uunlink () and umkdir ().
 */

int
do_unlink (path, space)
{
	register INODE *ip;
	register dev_t dev;
	IO		io;
	struct direct	dir;
	unsigned	olderror;

	/*
	 * We start by clearing u_error because we are called from umkdir ()
	 * in a situation where the active error number is not relevant to
	 * us. We return the old error number so that umkdir () can restore
	 * the error number it wants easily.
	 */

	olderror = u.u_error;
	u.u_error = 0;

	io.io_seg = space;
	if (ftoi (path, 'u', & io, & dir) != 0)
		return olderror;

	ip = u.u_pdiri;
	if (iaccess (ip, IPW) == 0) {
		u.u_error = EACCES;
		goto err;
	}
	dev = ip->i_dev;

	if (iucheck (dev, u.u_cdirn) == 0)
		goto err;

	idirent (0, & io, & dir);
	idetach (ip);

	if ((ip = iattach (dev, u.u_cdirn)) == NULL)
		return;

	if (ip->i_nlink > 0)
		-- ip->i_nlink;
	icrt (ip);	/* unlink - ctime */
err:
	idetach (ip);
	return olderror;
}


/*
 * Set file times.
 */

uutime(np, utime)
char *np;
time_t utime [2];
{
	register INODE *ip;
	struct {
		time_t		_time [2];
	} stime;
	IO		io;
	struct direct	dir;

	if (ftoi (np, 'r', & io, & dir) != 0)
		return;

	ip = u.u_cdiri;
	if (owner (ip->i_uid)) {
		iamc (ip);	/* utime - atime/mtime/ctime */
		if (utime != NULL) {
			if (ukcopy (utime, & stime,
				    sizeof (stime)) != sizeof (stime)) {
				u.u_error = EFAULT;
			} else {
				ip->i_atime = stime._time [0];
				ip->i_mtime = stime._time [1];
			}
		}
	}
	idetach (ip);
	return 0;
}

/*
 * Write `n' bytes from buffer `bp' on file number `fd'.
 */
uwrite(fd, bp, n)
char *bp;
unsigned n;
{
	return sysio (fd, bp, n, 1);
}

/*
 *
 * int
 * useracc(base, count, writeUsr) -- determine user accessibility
 * caddr_t base;
 * int count;
 * int writeUsr;
 *
 *	Input:	base  = offset in user data space of the region to be accessed.
 *		count = size of access region in bytes.
 *		writeUsr = 0 if read access to be checked, else write
 *
 *	Action:	Verify user has desired access mode into specified region.
 *
 *	Return:	0 = permission denied.
 *		1 = access allowed.
 *
 *	Notes:	Mode is ignored for now, but is required for compatibility
 *		with System V, and future protected mode extensions.
 */
int
useracc(base, count, writeUsr)
register char *base;
int writeUsr, count;
{
	if (base + count >= base) {
		int		ret;

		ret = accdata (base, count) || accstack (base, count) ||
			accShm (base, count);
		if (! writeUsr)
			ret = ret || acctext (base, count);

		return ret;
	}
	return 0;
}


/*
 * strUserAcc(str, writeUsr) - Check user accessibility of 0 terminated string.
 *
 * char	*str;		null-terminated string,
 * int	writeUsr;	0 if read access to be checked, else write.
 *
 * Returns string size on success (without 0), -1 otherwise.
 *
 * It is interface to useracc() when count is not known.
 */
int strUserAcc(str, writeUsr)
char	*str;
int	writeUsr;
{
	register char	*ch;

	if (! useracc (str, 1, writeUsr))
		return -1;

	for (ch = str ; * ch != 0 ; ch ++) 
		if (! useracc (ch + 1, 1, writeUsr)) 
			return -1;

	return ch - str;
}

/*
 * "Safe" ukcopy and kucopy - use useracc to check user address supplied.
 */
int
kucopyS(kernel, user, n)
{
	if (useracc (user, n, 1))
		return kucopy (kernel, user, n);
	else {
		u.u_error = EFAULT;
		return 0;
	}
}

int
ukcopyS(user, kernel, n)
{
	if (useracc (user, n, 0))
		return ukcopy (user, kernel, n);
	else {
		u.u_error = EFAULT;
		return 0;
	}
}
