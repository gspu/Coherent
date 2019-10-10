/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Structure returned by the `times' system call.
 */

#ifndef	 TIMES_H
#define	 TIMES_H	TIMES_H

struct tbuffer {
	long	 tb_utime;		/* Process user time */
	long	 tb_stime;		/* Process system time */
	long	 tb_cutime;		/* Child user time */
	long	 tb_cstime;		/* Child system time */
};

#endif
