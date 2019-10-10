/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	 __SYS_ACCT_H__
#define	 __SYS_ACCT_H__

#include <common/__time.h>
#include <common/_uid.h>

typedef	unsigned short	comp_t;


struct acct {
	char		ac_comm [10];	/* Command name */
	comp_t		ac_utime;	/* User time */
	comp_t		ac_stime;	/* System time */
	comp_t		ac_etime;	/* Elapsed time */
	__time_t	ac_btime;	/* Beginning time of process */
	o_uid_t		ac_uid;		/* User id */
	o_gid_t		ac_gid;		/* Group id */
	short		ac_mem;		/* Average memory usage */
	comp_t		ac_io;		/* Number of disk I/O blocks */
	o_dev_t		ac_tty;		/* Control typewriter */
	char		ac_flag;	/* Accounting flag */
};

/*
 * Flags (ac_flag).
 */
#define AFORK	01			/* Execute fork, but not exec */
#define ASU	02			/* Used super user privileges */

#endif
