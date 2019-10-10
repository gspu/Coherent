/* $Header: /kernel/kersrc/coh.286/RCS/misc.c,v 1.1 92/07/17 15:18:09 bin Exp Locker: bin $ */
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
 * Miscellaneous routines.
 *
 * $Log:	misc.c,v $
 * Revision 1.1  92/07/17  15:18:09  bin
 * Initial revision
 * 
 * Revision 1.1	88/03/24  16:14:01	src
 * Initial revision
 * 
 * 87/05/08	Allan Cornish		/usr/src/sys/coh/misc.c
 * System code and data segments no longer reported in panic messages.
 *
 * 87/02/17	Allan Cornish		/usr/src/sys/coh/misc.c
 * Panic message now includes system code and data segments.
 */
#include <sys/coherent.h>
#include <acct.h>
#include <errno.h>
#include <sys/ino.h>
#include <sys/stat.h>

/*
 * Copy `n' bytes from `bp1' to `bp2'.
 */
kkcopy(bp1, bp2, n)
register char *bp1;
register char *bp2;
unsigned n;
{
	register unsigned n1;

	n1 = n;
	if (n1) {
		do {
			*bp2++ = *bp1++;
		} while (--n1);
	}
	return (n);
}

/*
 * Clear the next `n' bytes starting at `bp'.
 */
kclear(bp, n)
register char *bp;
register unsigned n;
{
	if (n) {
		do {
			*bp++ = 0;
		} while (--n);
	}
}

/*
 * Make sure we are the super user.
 */
super()
{
	if (u.u_uid) {
		u.u_error = EPERM;
		return (0);
	}
	u.u_flag |= ASU;
	return (1);
}

/*
 * Make sure we are the gived `uid' or the super user.
 */
owner(uid)
{
	if (u.u_uid == uid)
		return (1);
	if (u.u_uid == 0) {
		u.u_flag |= ASU;
		return (1);
	}
	u.u_error = EPERM;
	return (0);
}

/*
 * Panic.
 */
panic(a1)
char *a1;
{
	static panflag;

	if (panflag++ == 0) {
		printf("Panic: %r", &a1);
		putchar('\n');
		usync();
	}
	halt();
	--panflag;
}

/*
 * Print a message from a device driver.
 */
devmsg(dev, a1)
dev_t dev;
char *a1;
{
	printf("(%d,%d): %r", major(dev), minor(dev), &a1);
	printf("\n");
}
