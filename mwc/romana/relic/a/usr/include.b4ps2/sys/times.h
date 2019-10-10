/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Structure returned by the `times' system call.
 */

#ifndef	 TIMES_H
#define	 TIMES_H	TIMES_H

#ifdef _I386
struct tms {
	long	 tms_utime;		/* Process user time */
	long	 tms_stime;		/* Process system time */
	long	 tms_cutime;		/* Child user time */
	long	 tms_cstime;		/* Child system time */
};
#else
struct tbuffer {
	long	 tb_utime;		/* Process user time */
	long	 tb_stime;		/* Process system time */
	long	 tb_cutime;		/* Child user time */
	long	 tb_cstime;		/* Child system time */
};
#endif

#endif
