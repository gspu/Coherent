/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Time buffer.
 */

#ifndef	 __SYS_OLDTIMEB_H__
#define	 __SYS_OLDTIMEB_H__

#include <sys/types.h>

#pragma align 2
struct timeb {
	long	time;			/* Time since 1970 */
	unsigned short millitm;		/* Milliseconds */
	short	timezone;		/* Time zone */
	short	dstflag;		/* Daylight saving time applies */
};
#pragma align

#endif
