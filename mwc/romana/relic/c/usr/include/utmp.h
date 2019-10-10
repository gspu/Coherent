/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Structure of the login records in `/etc/utmp'
 * as well as the cummulative records in
 * `/usr/adm/wtmp'.
 */

#ifndef	UTMP_H
#define	UTMP_H	UTMP_H

#ifndef DIRSIZ
#define	DIRSIZ	14
#endif

#include <sys/types.h>

struct	utmp {
	char	ut_line[8];		/* tty name */
	char	ut_name[DIRSIZ];	/* User name */
	time_t	ut_time;		/* time signed on */
};

#endif
