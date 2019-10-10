/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1993 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Structure of the login records in `/etc/utmp'
 * as well as the cummulative records in
 * `/usr/adm/wtmp'.
 */

#ifndef	__UTMP_H__
#define	__UTMP_H__

#ifndef DIRSIZ
#define	DIRSIZ	14
#endif

#include <sys/types.h>

#pragma align 1
struct	utmp {
	char	ut_line[8];		/* tty name */
	char	ut_name[DIRSIZ];	/* User name */
	time_t	ut_time;		/* time signed on */
};
#pragma align

#endif
