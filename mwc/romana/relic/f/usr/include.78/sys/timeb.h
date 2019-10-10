/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */

/*
 * /usr/include/sys/timeb.h
 *
 * Time buffer for ftime() call.
 * ftime() is not in BCS, but is in the extended set of calls.
 *
 * Revised: Fri Jun 11 06:42:43 1993 CDT
 */

#ifndef	 __SYS_TIMEB_H__
#define	 __SYS_TIMEB_H__

#include <sys/types.h>

#pragma align 2
struct timeb {
	time_t	time;			/* Time since 1970 */
	unsigned short millitm;		/* Milliseconds */
	short	timezone;		/* Time zone */
	short	dstflag;		/* Daylight saving time applies */
};
#pragma align

#endif /* __SYS_TIMEB_H__ */
