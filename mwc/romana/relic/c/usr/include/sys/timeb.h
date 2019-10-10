/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Time buffer.
 */

#ifndef	 TIMEB_H
#define	 TIMEB_H	TIMEB_H

#include <sys/types.h>

struct timeb {
	time_t	time;			/* Time since 1970 */
	unsigned short millitm;		/* Milliseconds */
	short	 timezone;		/* Time zone */
	short	 dstflag;		/* Daylight saving time applies */
};

#endif
