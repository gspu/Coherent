/* $Header: /kernel/kersrc/coh.386/RCS/fd.c,v 1.2 92/08/04 12:30:49 bin Exp Locker: bin $ */
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
 * Revision 1.2  92/08/04  12:30:49  bin
 * changed for kernel 59
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

	if (fd>=NUFILE || (fdp=u.u_filep[fd])==NULL) {
		u.u_error = EBADF;
		return (NULL);
	}
	return (fdp);
}

/*
 * Given an inode, and a mode containing permission flags, open the
 * inode with the appropriate permissions and return a file descriptor
 * containing it.
 */
fdopen(ip, mode)
register INODE *ip;
{
	register FD **fdpp;
	register FD *fdp;

	for (fdpp=u.u_filep; fdpp<&u.u_filep[NUFILE]; fdpp++) {
		if (*fdpp != NULL)
			continue;
		if ((fdp=kalloc(sizeof(FD))) == NULL)
			return (-1);
		iopen(ip, mode);
		if (u.u_error) {
			kfree(fdp);
			return (-1);
		}
		fdp->f_flag = mode;
		fdp->f_flag2 = 0;
		fdp->f_refc = 1;
		fdp->f_seek = 0;
		fdp->f_ip = ip;
		*fdpp = fdp;
		return (fdpp-u.u_filep);
	}
	u.u_error = EMFILE;
	return (-1);
}

/*
 * Close the given file number.
 */
fdclose(fd)
register unsigned fd;
{
	register FD *fdp;
	static	FLOCK	flock = { F_UNLCK, 0, 0, 0 };

	if (fd>=NUFILE || (fdp=u.u_filep[fd])==NULL) {
		u.u_error = EBADF;
		return;
	}
	if (fdp->f_ip->i_rl != NULL)
		rlock(fdp, F_SETLK, &flock);	/* delete all record locks */
	u.u_filep[fd] = NULL;
	if (fdp->f_refc == 0)
		panic("fdclose()");
	if (--fdp->f_refc == 0) {
		iclose(fdp->f_ip);
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

	for (fdpp=u.u_filep; fdpp<&u.u_filep[NUFILE]; fdpp++) {
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

	for (fd=0; fd<NUFILE; fd++) {
		if (u.u_filep[fd] == NULL)
			continue;
		fdclose(fd);
	}
}
