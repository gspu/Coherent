/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Accounting structure.
 */
#ifndef	 ACCT_H
#define	 ACCT_H	ACCT_H

#include <sys/types.h>

struct acct {
	char	ac_comm[10];		/* Command name */
	comp_t	ac_utime;		/* User time */
	comp_t	ac_stime;		/* System time */
	comp_t	ac_etime;		/* Elapsed time */
	time_t	ac_btime;		/* Beginning time of process */
	short	ac_uid;			/* User id */
	short	ac_gid;			/* Group id */
	short	ac_mem;			/* Average memory usage */
	comp_t	ac_io;			/* Number of disk I/O blocks */
	dev_t	ac_tty;			/* Control typewriter */
	char	ac_flag;		/* Accounting flag */
};

/*
 * Flags (ac_flag).
 */
#define AFORK	01			/* Execute fork, but not exec */
#define ASU	02			/* Used super user privileges */

#endif
