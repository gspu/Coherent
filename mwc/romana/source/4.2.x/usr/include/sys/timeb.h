/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	 __SYS_TIMEB_H__
#define	 __SYS_TIMEB_H__

/*
 * Time buffer for ftime() call.
 * ftime() is not in iBCS, but is in the extended set of calls.
 */

#include <common/__time.h>

struct timeb {
	__time_t	time __ALIGN (2);	/* Time since 1970 */
	unsigned short millitm;			/* Milliseconds */
	short		timezone;		/* Time zone */
	short		dstflag;		/* Daylight saving flag */
#pragma	align 2
};
#pragma	align	/* control structure alignment with Coherent 'cc' */

#endif /* ! defined (__SYS_TIMEB_H__) */
