/*
 * coh.386/sys2.c
 *
 * Filesystem related system calls.
 *
 * Revised: Mon Jul 12 08:56:23 1993 CDT
 */

#include <kernel/_sleep.h>
#include <sys/coherent.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <sys/fd.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <sys/mount.h>
#include <sys/sched.h>
#include <sys/stat.h>
#include <sys/file.h>

/*
 * Determine accessibility of the given file.
 */

uaccess(np, mode)
char *np;
register int mode;
{
	register INODE *ip;
	register int r;
	IO		io;
	struct direct	dir;

	schizo ();

	io.io_seg = IOUSR;
	r = ftoi (np, 'r', & io, & dir);

	schizo ();

	if (r)
		return;
	ip = u.u_cdiri;
	if (! iaccess (ip, mode))
		u.u_error = EACCES;
	idetach (ip);
	return 0;
}

/*
 * Schizo - swap real and effective id's.
 */
schizo()
{
	register int t;

	t = u.u_uid;
	u.u_uid = u.u_ruid;
	u.u_ruid = t;
	t = u.u_gid;
	u.u_gid = u.u_rgid;
	u.u_rgid = t;
}

/*
 * Turn accounting on or off.
 */
uacct(np)
register char *np;
{
	if (super () == 0)
		return;

	if (np == NULL) {
		if (acctip == NULL) {
			u.u_error = EINVAL;
			return;
		}

		ldetach (acctip);
		acctip = NULL;
	} else {
		INODE	      *	ip;
		IO		io;
		struct direct	dir;

		if (acctip != NULL) {
			u.u_error = EINVAL;
			return;
		}

		io.io_seg = IOUSR;
		if (ftoi (np, 'r', & io, & dir))
			return;

		ip = u.u_cdiri;
		if ((ip->i_mode & IFMT) != IFREG) {
			u.u_error = EINVAL;
			idetach (ip);
			return;
		}
		iunlock (ip);
		acctip = ip;
	}
	return 0;
}

/*
 * Set current directory.
 */
uchdir(np)
char *np;
{
	setcdir (np, & u.u_cdir);
	return 0;
}

/*
 * Given a directory name and a pointer to a working directory pointer,
 * Save the inode associated with the directory name in the working
 * directory pointer and release the old one.  This is used to change
 * working and root directories.
 */
setcdir(np, ipp)
char *np;
register INODE **ipp;
{
	register INODE *ip;
	IO		io;
	struct direct	dir;

	io.io_seg = IOUSR;
	if (ftoi (np, 'r', & io, & dir))
		return;

	ip = u.u_cdiri;
	if ((ip->i_mode & IFMT) != IFDIR) {
		u.u_error = ENOTDIR;
		idetach (ip);
		return;
	}
	if (iaccess (ip, IPE) == 0) {
		u.u_error = EACCES;
		idetach (ip);
		return;
	}
	iunlock (ip);
	ldetach (* ipp);
	* ipp = ip;
}

/*
 * Change the mode of a file.
 */
uchmod(np, mode)
char *np;
{
	register INODE *ip;
	IO		io;
	struct direct	dir;

	io.io_seg = IOUSR;
	if (ftoi (np, 'r', & io, & dir))
		return;

	ip = u.u_cdiri;
	if (owner (ip->i_uid)) {
		if (u.u_uid)
			mode &= ~ ISVTXT;
		ip->i_mode &= IFMT;
		ip->i_mode |= mode & ~ IFMT;
		icrt (ip);	/* chmod - ctime */
	}
	idetach (ip);
	return 0;
}

/*
 * Change owner and group of a file.
 */
uchown(np, uid, gid)
char *np;
{
	register INODE *ip;
	IO		io;
	struct direct	dir;

	io.io_seg = IOUSR;
	if (ftoi (np, 'r', & io, & dir))
		return;

	ip = u.u_cdiri;
	if (super ()) {
		ip->i_mode &= ~ (ISUID | ISGID);  /* clear any setuid/setgid */
		ip->i_uid = uid;
		ip->i_gid = gid;
		icrt (ip);	/* chown - ctime */
	}
	idetach (ip);
	return 0;
}

/*
 * Set root directory.
 */
uchroot(np)
register char *np;
{
	if (super ())
		setcdir (np, & u.u_rdir);
	return 0;
}

/*
 * Close the given file descriptor.
 */
uclose(fd)
{
	fdclose (fd);
	return 0;
}

/*
 * Create a file with the given mode.
 */
ucreat(np, mode)
char *np;
register int mode;
{
	return uopen (np, O_WRONLY | O_CREAT | O_TRUNC, mode);
}

/*
 * Duplicate a file descriptor.
 */
udup(ofd)
{
	return fddup (ofd, 0);
}

/*
 * Given a file descriptor, return a status structure.
 */
ufstat(fd, stp)
struct stat *stp;
{
	register INODE *ip;
	register FD *fdp;
	struct stat stat;

	if ((fdp = fdget (fd)) == NULL)
		return;

	ip = fdp->f_ip;
	istat (ip, & stat);

	if (kucopy (& stat, stp, sizeof (stat)) != sizeof (stat)) {
		u.u_error = EFAULT;
		return -1;
	}
	return 0;
}

/*
 * File control.
 */

ufcntl (fd, cmd, arg)
unsigned	fd;
unsigned	cmd;
unsigned	arg;
{
	register FD * fdp;
	struct flock sfl;

	T_VLAD(2, printf("fcntl(%d,%x,%x) ", fd, cmd, arg));

	/*
	 * Validate file descriptor.
	 */

	if ((fdp = fdget (fd)) == NULL) {
		u.u_error = EBADF;
		return;
	}

	switch (cmd) {

	case F_DUPFD:
		/*
		 * Validate base file descriptor.
		 */
		if (arg >= NOFILE) {
			u.u_error = EINVAL;
			return;
		}

		/*
		 * Search for next available file descriptor.
		 */

		return fddup (fd, arg);

	case F_SETFL:
		fdp->f_flag &= ~ (IPNDLY | IPAPPEND | IPNONBLOCK);
		if (arg & O_NDELAY)
			fdp->f_flag |= IPNDLY;
		if (arg & O_APPEND)
			fdp->f_flag |= IPAPPEND;
		if (arg & O_NONBLOCK)
			fdp->f_flag |= IPNONBLOCK;

		/*
		 * Originally, this call returned the previous flag values,
		 * as permitted by the various standards. However, many
		 * programs incorrectly check for "== 0" as the return
		 * condition from this function rather than "!= -1" as they
		 * should.
		 */
		return 0;

	case F_GETFL:
		switch (fdp->f_flag & (IPR | IPW)) {
		case IPR: arg = O_RDONLY; break;
		case IPW: arg = O_WRONLY; break;
		default:  arg = O_RDWR;   break;
		}

		if ((fdp->f_flag & IPNDLY) != 0)
			arg |= O_NDELAY;

		if ((fdp->f_flag & IPAPPEND) != 0)
			arg |= O_APPEND;

		if ((fdp->f_flag & IPNONBLOCK) != 0)
			arg |= O_NONBLOCK;

		return arg;

	case F_GETLK:
	case F_SETLK:
	case F_SETLKW:
		if (ukcopy ((struct flock *) arg, & sfl,
			    sizeof (struct flock)) != sizeof (struct flock)) {
			u.u_error = EFAULT;
			return -1;
		}
		if (rlock (fdp, cmd, & sfl))
			return -1;
		if (cmd == F_GETLK &&
		    kucopy (& sfl, (struct flock *) arg,
			    sizeof (struct flock)) != sizeof (struct flock)) {
			u.u_error = EFAULT;
			return -1;
		}
		return 0;

	case F_GETFD:
		return fdgetflags (fd);

	case F_SETFD:
		return fdsetflags (fd, arg);

	default:
		T_VLAD (0x02, printf ("'fcntl - unknown cmd=%d arg=0x0%x' ",
				      cmd, arg));
		u.u_error = EINVAL;
	}
}

/*
 * Device control information.
 */

int
uioctl (fd, cmd, argp, regsetp)
unsigned	fd;
unsigned	cmd;
__VOID__      *	argp;
gregset_t     *	regsetp;
{
	register FD *fdp;
	register INODE *ip;
	register int mode;


	T_PIGGY (0x8, printf ("uioctl(%d, 0x%x, 0x%x)", fd, cmd, argp));

	if ((fdp = fdget (fd)) == NULL)
		return;
	ip = fdp->f_ip;
	mode = ip->i_mode & IFMT;
	if (mode != IFCHR && mode != IFBLK) {
		u.u_error = ENOTTY;
		return;
	}
	dioctl (ip->i_a.i_rdev, cmd, argp, fdp->f_flag, regsetp);
	return 0;
}

/*
 * Create a link, `np2' to the already existing file `np1'.
 */

ulink (np1, np2)
char *np1;
char *np2;
{
	return do_link (np1, np2, IOUSR);
}


/*
 * Internal version of link (), used by ulink () and umkdir ().
 */

int
do_link (path1, path2, space)
char	      *	path1;
char	      *	path2;
int		space;
{
	register INODE *ip1;
	IO		io;
	struct direct	dir;

	io.io_seg = space;
	if (ftoi (path1, 'r', & io, & dir))
		return;

	ip1 = u.u_cdiri;
	if ((ip1->i_mode & IFMT) == IFDIR && super () == 0) {
		idetach (ip1);
		return;
	}

	iunlock (ip1);

	io.io_seg = space;
	if (ftoi (path2, 'c', & io, & dir)) {
		ldetach (ip1);
		return;
	}

	if (u.u_cdiri != NULL) {
		u.u_error = EEXIST;
		idetach (u.u_cdiri);
		ldetach (ip1);
		return;
	}

	if (ip1->i_dev != u.u_pdiri->i_dev) {
		u.u_error = EXDEV;
		idetach (u.u_pdiri);
		ldetach (ip1);
		return;
	}

	if (iaccess (u.u_pdiri, IPW) == 0) {
		idetach (u.u_pdiri);
		ldetach (ip1);
		return;
	}

	idirent (ip1->i_ino, & io, & dir);
	idetach (u.u_pdiri);
	ilock (ip1);

	/*
	 * idirent() can fail during iwrite. In this case we should not 
         * increase link count. 
	 * As result of this old bug, 286 mkdir utility destroys file 
	 * system when runs out of free blocks.
	 */

	if (! u.u_error)
		ip1->i_nlink ++;

	icrt (ip1);	/* link - ctime */
	idetach (ip1);
	return 0;
}


/*
 * Seek on the given file descriptor.
 */

off_t
ulseek(fd, off, w)
register off_t off;
{
	register FD *fdp;
	register INODE *ip;

	if ((fdp = fdget (fd)) == NULL)
		return;
	ip = fdp->f_ip;
	if ((ip->i_mode & IFMT) == IFPIPE) {
		u.u_error = ESPIPE;
		return;
	}
	switch (w) {
	case 0:
		break;

	case 1:
		off += fdp->f_seek;
		break;

	case 2:
		off += ip->i_size;
		break;

	default:
		u.u_error = EINVAL;
		return;
	}

	if (off < 0) {
		u.u_error = EINVAL;
		return;
	}

	fdp->f_seek = off;
	return off;
}

/*
 * Create a special file.
 */
umknod(np, mode, rdev)
char *np;
dev_t rdev;
{
	register INODE *ip;
	register int type;
	IO		io;
	struct direct	dir;

	type = mode & IFMT;
	if (type != IFPIPE && super () == 0)
		return;
	if (type != IFBLK && type != IFCHR)
		rdev = 0;

	io.io_seg = IOUSR;
	if (ftoi (np, 'c', & io, & dir))
		return;

	if ((ip = u.u_cdiri) != NULL) {
		u.u_error = EEXIST;
		idetach (ip);
		return;
	}

	if ((ip = imake (mode, rdev, & io, & dir)) != NULL)
		idetach (ip);

	return 0;
}

/*
 * Mount the device `sp' on the pathname `np'.  The flag, `f',
 * indicates that the device is to be mounted read only.
 */
umount(sp, np, f)
char *sp;
char *np;
{
	register INODE *ip;
	register MOUNT *mp;
	register dev_t rdev;
	register int mode;
	IO		io;
	struct direct	dir;

	io.io_seg = IOUSR;
	if (ftoi (sp, 'r', & io, & dir))
		return;

	ip = u.u_cdiri;
	if (iaccess (ip, IPR | IPW) == 0)
		goto err;

	mode = ip->i_mode;
	rdev = ip->i_a.i_rdev;
	if ((mode & IFMT) != IFBLK) {
		u.u_error = ENOTBLK;
		goto err;
	}

	idetach(ip);

	io.io_seg = IOUSR;
	if (ftoi (np, 'r', & io, & dir))
		return;

	ip = u.u_cdiri;

	if (iaccess (ip, IPR) == 0)
		goto err;

	if ((ip->i_mode & IFMT) != IFDIR) {
		u.u_error = ENOTDIR;
		goto err;
	}

	/* Check for current directory, open, or mount directory */

	if (ip->i_refc > 1 || ip->i_ino == ROOTIN) {
		u.u_error = EBUSY;
		goto err;
	}
	for (mp = mountp ; mp != NULL ; mp = mp->m_next) {
		if (mp->m_dev == rdev) {
			u.u_error = EBUSY;
			goto err;
		}
	}

	if ((mp = fsmount (rdev, f)) == NULL)
		goto err;
	mp->m_ip = ip;
	ip->i_flag |= IFMNT;
	ip->i_refc ++;
err:
	idetach (ip);
	return 0;
}

/*
 * Poll devices for input/output events.
 */
int
upoll(pollfds, npoll, msec)
struct pollfd * pollfds;
unsigned long npoll;
int msec;
{
	register struct pollfd * pollp;	/* current poll pointer		 */
	register FD *	fdp;		/* current file descriptor ptr	 */
	auto	 int	fd;		/* current file descriptor	 */
	auto	 int	rev;		/* last event report received	 */
	auto	 int	nev;		/* number non-zero event reports */
	auto	 int	i;
	char * cp;
	int ret = -1;

	/*
	 * Validate number of polls.
	 */
	if ((npoll < 0) || (npoll > NOFILE)) {
		u.u_error = EINVAL;
		goto poll_done;
	}

	/*
	 * If there are any fd's to poll
	 *   validate address of polling information.
	 * npoll of 0 is legal, allows user a short delay.
	 */
	if (npoll)
		if ((pollfds == NULL)
		  || !useracc(pollfds, npoll*sizeof(struct pollfd), 1)) {
			u.u_error = EFAULT;
			goto poll_done;
		}

	for (;;) {
		/*
		 * Service each poll in turn.
		 */
		for (nev=0, i=npoll, pollp = pollfds; i > 0; --i, pollp++) {

			/*
			 * Fetch file descriptor.
			 */
			fd = getuwd(&pollp->fd);

			/*
			 * Ignore negative file descriptors.
			 */
			if (fd < 0) {
				rev = 0;
				goto remember;
			}

#if 1
			/*
			 * Ignore file descriptors that are too large.
			 */
			if (fd >= NOFILE) {
				rev = 0;
				goto remember;
			}
#else
/* For now, msg polling is deleted. */
			/*
			 * Poll message queue.
			 */
			if (fd >= NOFILE) {
				rev = msgpoll(fd, getusd(&pollp->events), msec);
				goto remember;
			}
#endif

			/*
			 * Validate file descriptor.
			 */
			if ((fdp = fdget (fd)) == NULL) {
				rev = POLLNVAL;
				goto remember;
			}

			switch ( fdp->f_ip->i_mode & IFMT ) {
			case IFCHR:
				rev = dpoll(fdp->f_ip->i_a.i_rdev,
					getusd(&pollp->events)&0xffff, msec);
				break;
			case IFPIPE:
				rev = ppoll(fdp->f_ip,
					getusd(&pollp->events)&0xffff, msec);
				break;
			default:
				printf("polling illegal dev: fd=%d mode=%x\n",
					fd, fdp->f_ip->i_mode);
				rev = POLLNVAL;
				break;
			}

			/*
			 * Remember reponses.
			 */
remember:
			cp = (char *)(&pollp->revents);
			putusd(cp, rev);

			/*
			 * Record number of non-zero responses.
			 */
			if (rev)
				nev++;
		}

		/*
		 * Non-blocking poll or poll response received.
		 */
		if ( (nev != 0) || (msec == 0) ) {
			pollexit();
			ret = nev;
			goto poll_done;
		}

		/*
		 * Schedule wakeup timer if positive delay interval given
 		 * and the timer is not currently set.
		 */
		if ( (msec > 0) && (cprocp->p_polltim.t_func == NULL) ) {
			/*
			 * Convert milliseconds to clock ticks.
			 */
			msec += (1000 / HZ) - 1;
			msec /= (1000 / HZ);
			timeout(&cprocp->p_polltim, msec,
				 wakeup, &cprocp->p_polls);
		}

		/*
		 * Wake for polled event, poll timeout, or signal.
		 */

		{
			__sleep_t	sleep;

			sleep = x_sleep (& cprocp->p_polls, pritty,
					 slpriSigCatch, "poll");

			/*
			 * Terminate event monitoring.
			 */

			pollexit();

			/*
			 * Signal woke us up.
			 */

			if (sleep == PROCESS_SIGNALLED) {
				u.u_error = EINTR;
				goto poll_done;
			}
		}

		/*
 		 * We were woken up by timeout wakeup.
 		 */
		if ( (msec > 0) && (cprocp->p_polltim.t_lbolt <= lbolt) ) {
			ret = 0;
			goto poll_done;
		}
	}

poll_done:
	/*
	 * Cancel timeout
 	 */
	if ( (msec > 0) && (cprocp->p_polltim.t_func != NULL) )
		timeout(&cprocp->p_polltim, 0, NULL, NULL);

	return ret;
}

/*
 * Suspend execution for a short interval.
 *
 * Return the number of milliseconds actually slept.
 * Shares use of cprocp->p_polltim with upoll().
 */
int
unap(msec)
int msec;
{
	int ret, lbolt0;
	int ticksToWait, ticksWaited;

	if (msec <= 0)
		return 0;

	/*
	 * Convert milliseconds to clock ticks.
	 *
	 * Wait for at least the specified number of milliseconds.
	 * For 100 Hz clock, if nap is for 11 msec, timeout is for 2 ticks.
	 */
	ticksToWait = ((msec * HZ) + 999) / 1000;
	timeout (& cprocp->p_polltim, ticksToWait, wakeup, & cprocp->p_polls);

	/*
	 * Wake for timeout or signal.
	 */
	lbolt0 = lbolt;
	if (x_sleep (& cprocp->p_polls, pritty, slpriSigCatch,
		     "nap") == PROCESS_SIGNALLED) {
		/*
		 * Signal woke us up.
		 */
		u.u_error = EINTR;
		goto napDone;
	} else {
		/*
		 * We were awakened by a timeout.
		 * Return number of milliseconds actually waited.
		 */
		ticksWaited = lbolt - lbolt0;
		if (ticksWaited >= 0)
			ret = (ticksWaited * 1000) / HZ;
		else
			ret = 0;
		goto napDone;
	}

napDone:
	/*
	 * Cancel timeout
 	 */
	timeout (& cprocp->p_polltim, 0, NULL, NULL);

	return ret;
}

