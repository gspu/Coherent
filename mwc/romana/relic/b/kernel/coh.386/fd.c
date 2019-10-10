/* $Header: /y/coh.386/RCS/fd.c,v 1.5 93/04/14 10:06:26 root Exp $ */
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
#include <sys/coherent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/fd.h>
#include <sys/inode.h>

/*
 * Given a file number, return the file descriptor.
 */
FD *
fdget(fd)
register unsigned fd;
{
	register FD *fdp;

	if (fd>=NOFILE || (fdp=u.u_filep[fd])==NULL) {
		u.u_error = EBADF;
		return (NULL);
	}
	return (fdp);
}

/*
 * NIGEL: To help fix some stupid bugs with uopen (), I have split the
 * fdopen () routine into three parts; one for resource allocation, one for
 * setting the inode member and opening the inode, and one for finishing up
 * an open.
 */

/*
 * This function finds a free slot in the process file descriptor table and
 * fills it in with a partially initialised entry.
 *
 * This function returns a file descriptor number on success, -1 on failure.
 */

int fdalloc ()
{
	int		i;

	for (i = 0 ; i < sizeof (u.u_filep) / sizeof (* u.u_filep) ; i ++) {
		if (u.u_filep [i] == NULL) {
			FD    *	filep;

			if ((filep = u.u_filep [i] = kalloc (sizeof (FD)))
					== NULL) {
				/*
				 * Insufficient resources!
				 */

				u.u_error = EAGAIN;
				return -1;
			}

			filep->f_flag = 0;
			filep->f_flag2 = 0;
			filep->f_refc = 0;
			filep->f_seek = 0;
			filep->f_ip = NULL;

			return i;
		}
	}

	u.u_error = EMFILE;
	return -1;
}

/*
 * This function performs the second half of the file open process by filling
 * in the inode member of the file table entry and requesting that the inode be
 * opened.
 *
 * This function returns -1 on error, 0 on success.
 */

int fdinit (fd, ip, mode)
int		fd;
INODE	      *	ip;
int		mode;
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

int fdfinish (fd)
int		fd;
{
	FD	      *	filep;

	if ((filep = fdget (fd)) == NULL)
		return -1;

	if (filep->f_refc == 0) {
		/*
		 * The open never really succeeded, release resources.
		 */

		kfree (filep);
		u.u_filep [fd] = NULL;
		fd = -1;
	}

	return fd;
}


/*
 * Given an inode, and a mode containing permission flags, open the
 * inode with the appropriate permissions and return a file descriptor
 * containing it.
 */
fdopen(ip, mode)
register INODE *ip;
{
	int		fd;

	if ((fd = fdalloc ()) >= 0) {
		fdinit (fd, ip, mode);
		fd = fdfinish (fd);
	}

	return fd;
}


/*
 * Close the given file number.
 */
fdclose(fd)
register unsigned fd;
{
	register FD *fdp;
	static	FLOCK	flock = { F_UNLCK, 0, 0, 0 };

	if (fd>=NOFILE || (fdp=u.u_filep[fd])==NULL) {
		u.u_error = EBADF;
		return;
	}
	if (fdp->f_ip->i_rl != NULL)
		rlock(fdp, F_SETLK, &flock);	/* delete all record locks */
	u.u_filep[fd] = NULL;
	if (fdp->f_refc == 0)
		panic("fdclose()");
	if (--fdp->f_refc == 0) {
		iclose(fdp->f_ip, fdp->f_flag);
		kfree(fdp);
	}
}

/*
 * Assuming we have made a copy of the user area, increment the reference
 * of all open files.  (used in fork).
 */
fdadupl()
{
	register FD **fdpp;
	register FD *fdp;

	for (fdpp=u.u_filep; fdpp<&u.u_filep[NOFILE]; fdpp++) {
		if ((fdp=*fdpp) == NULL)
			continue;
		fdp->f_refc++;
	}
}

/*
 * Close all open files in the current process.
 */
fdaclose()
{
	register int fd;

	for (fd=0; fd<NOFILE; fd++) {
		if (u.u_filep[fd] == NULL)
			continue;
		fdclose(fd);
	}
}
