/* $Header: /ker/coh.386/RCS/fd.c,v 2.4 93/07/26 15:09:56 nigel Exp $ */
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
 * File descriptor routines.
 *
 * $Log:	fd.c,v $
 * Revision 2.4  93/07/26  15:09:56  nigel
 * Nigel's R80
 * 
 * Revision 1.5  93/04/14  10:06:26  root
 * r75
 * 
 * Revision 1.3  92/06/10  12:52:39  hal
 * First record locking changes.
 * 
 * Revision 1.2  92/01/06  11:59:07  hal
 * Compile with cc.mwc.
 * 
 * Revision 1.1	88/03/24  16:13:43	src
 * Initial revision
 * 
 */

#include <common/ccompat.h>
#include <sys/debug.h>
#include <sys/coherent.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <sys/fd.h>
#include <sys/inode.h>

/*
 * Operations for converting from tagged to untagged "pointer to system file
 * table entry" and vice versa.
 */

enum {
	TAGMASK		= sizeof (int) - 1
};

#define	MAKE_TAGGED_FD(untag,flag) \
	(ASSERT (((int) (untag) & TAGMASK) == 0), \
	 ASSERT (((flag) & ~ TAGMASK) == 0), \
	 (tagfd_t) ((unsigned long) (untag) + (flag)))

#define	GET_UNTAGGED_FD(tagged) \
	((FD *) ((unsigned long) (tagged) & ~ TAGMASK))

#define	GET_TAGGED_FLAG(tagged) \
	((int) (tagged) & TAGMASK)

/*
 * Given a file number, return the file descriptor. Now that file descriptors
 * stored in the U area have their low bits tagged to store flag information
 * that is FD-specific (gotta avoid that space crunch in the U area), this
 * interface deals with masking out any extra stuff and returning a plain
 * pointer.
 */

#if	__USE_PROTO__
FD * fdget (fd_t fd)
#else
FD *
fdget (fd)
fd_t		fd;
#endif
{
	FD	      *	fdp;

	if (fd >= NOFILE ||
	    (fdp = GET_UNTAGGED_FD (u.u_filep [fd])) == NULL) {
		u.u_error = EBADF;
		return NULL;
	}
	return fdp;
}


/*
 * Return tag bits from file descriptor.
 */

#if	__USE_PROTO__
int fdgetflags (fd_t fd)
#else
int
fdgetflags (fd)
fd_t		fd;
#endif
{
	if (fd >= NOFILE || u.u_filep [fd] == NULL) {
		u.u_error = EBADF;
		return -1;
	}
	return GET_TAGGED_FLAG (u.u_filep [fd]);
}


/*
 * This function allows clients to set the flag bits.
 */

#if	__USE_PROTO__
int fdsetflags (fd_t fd, int flags)
#else
int
fdsetflags (fd, flags)
fd_t		fd;
int		flags;
#endif
{
	if (fd >= NOFILE || u.u_filep [fd] == NULL) {
		u.u_error = EBADF;
		return -1;
	}
	if ((flags & ~ TAGMASK) != 0) {
		u.u_error = EINVAL;
		return -1;
	}

	u.u_filep [fd] = MAKE_TAGGED_FD (GET_UNTAGGED_FD (u.u_filep [fd]),
					 flags);
	return 0;
}


/*
 * This function performs something similar to the F_DUPFD function of
 * fcntl ()... this functionality is needed several places internally.
 */

#if	__USE_PROTO__
fd_t fddup (fd_t old, fd_t base)
#else
fd_t
fddup (old, base)
fd_t		old;
fd_t		base;
#endif
{
	while (base < NOFILE)
		if (u.u_filep [base] == NULL) {
			FD	      *	fdp = fdget (old);

			if (fdp == NULL)
				return ERROR_FD;
			u.u_filep [base] = MAKE_TAGGED_FD (fdp, 0);
			fdp->f_refc ++;
			return base;
		} else
			base ++;

	u.u_error = EMFILE;
	return ERROR_FD;
}


/*
 * This function finds a free slot in the process file descriptor table and
 * fills it in with a partially initialised entry.
 *
 * This function returns a file descriptor number on success, -1 on failure.
 */

#if	__USE_PROTO__
fd_t fdalloc (void)
#else
fd_t
fdalloc ()
#endif
{
	int		i;

	for (i = 0 ; i < sizeof (u.u_filep) / sizeof (* u.u_filep) ; i ++) {
		if (u.u_filep [i] == NULL) {
			FD	      *	filep;

			if ((filep = kalloc (sizeof (FD))) == NULL) {
				/*
				 * Insufficient resources!
				 */

				u.u_error = EAGAIN;
				return ERROR_FD;
			}

			u.u_filep [i] = MAKE_TAGGED_FD (filep, 0);

			filep->f_flag = 0;
			filep->f_refc = 0;
			filep->f_seek = 0;
			filep->f_ip = NULL;

			return i;
		}
	}

	u.u_error = EMFILE;
	return ERROR_FD;
}


/*
 * This function performs the second half of the file open process by filling
 * in the inode member of the file table entry and requesting that the inode be
 * opened.
 *
 * This function returns -1 on error, 0 on success.
 */

#if	__USE_PROTO__
int fdinit (fd_t fd, INODE * ip, int mode)
#else
int
fdinit (fd, ip, mode)
fd_t		fd;
INODE	      *	ip;
int		mode;
#endif
{
	FD	      *	filep;

	if ((filep = fdget (fd)) == NULL)
		return -1;

	iopen (ip, mode);

	if (u.u_error != 0)
		return -1;

	filep->f_ip = ip;
	filep->f_flag = mode;
	filep->f_refc = 1;

	return 0;
}


/*
 * This function finalises an open; normally this does nothing, but if there
 * has been an error, this code will take care of deallocating the entry.
 *
 * This function returns the file descriptor number on success, or -1 on error.
 */

#if	__USE_PROTO__
fd_t fdfinish (fd_t fd)
#else
fd_t
fdfinish (fd)
fd_t		fd;
#endif
{
	FD	      *	filep;

	if ((filep = fdget (fd)) == NULL)
		return ERROR_FD;

	if (filep->f_refc == 0) {
		/*
		 * The open never really succeeded, release resources.
		 */

		kfree (filep);
		u.u_filep [fd] = NULL;
		fd = ERROR_FD;
	}

	return fd;
}


/*
 * Given an inode, and a mode containing permission flags, open the
 * inode with the appropriate permissions and return a file descriptor
 * containing it.
 */

#if	__USE_PROTO__
fd_t fdopen (INODE * ip, int mode)
#else
fd_t
fdopen (ip, mode)
INODE	      *	ip;
int		mode;
#endif
{
	int		fd;

	if ((fd = fdalloc ()) != ERROR_FD) {
		fdinit (fd, ip, mode);
		fd = fdfinish (fd);
	}

	return fd;
}


/*
 * Close the given file number.
 */

#if	__USE_PROTO__
void fdclose (unsigned fd)
#else
void
fdclose(fd)
unsigned	fd;
#endif
{
	register FD *fdp;
	static	struct flock	flock = { F_UNLCK, 0, 0, 0 };

	if (fd >= NOFILE ||
	    (fdp = GET_UNTAGGED_FD (u.u_filep [fd])) == NULL) {
		u.u_error = EBADF;
		return;
	}

	if (fdp->f_ip->i_rl != NULL)
		rlock (fdp, F_SETLK, & flock);	/* delete all record locks */

	u.u_filep[fd] = NULL;

	if (fdp->f_refc == 0)
		panic("fdclose()");

	if (-- fdp->f_refc == 0) {
		iclose (fdp->f_ip, fdp->f_flag);
		kfree (fdp);
	}
}


/*
 * Assuming we have made a copy of the user area, increment the reference
 * of all open files.  (used in fork).
 */

#if	__USE_PROTO__
void fdadupl (void)
#else
void
fdadupl ()
#endif
{
	int		i;

	for (i = 0 ; i < NOFILE ; i ++)
		if (u.u_filep [i] != NULL)
			GET_UNTAGGED_FD (u.u_filep [i])->f_refc ++;
}


/*
 * Close all open files in the current process.
 */

#if	__USE_PROTO__
void fdaclose (void)
#else
void
fdaclose()
#endif
{
	int		fd;

	for (fd = 0 ; fd < NOFILE ; fd ++)
		if (u.u_filep [fd] != NULL)
			fdclose (fd);
}
