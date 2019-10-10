/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__KERNEL__TIMERS_H__
#define	__KERNEL__TIMERS_H__

/*
 * Define access methods to a variety of notions of time.
 */

#include <common/ccompat.h>
#include <common/_time.h>


/*
 * Time of day structure.
 */

extern struct _TIME_OF_DAY {
	__time_t	t_time;		/* Time and date */
	int		t_tick;		/* Clock ticks into this second */
	int		t_zone;		/* Time zone */
	int		t_dstf;		/* Daylight saving time used */
} timer;

extern	long	 lbolt;			/* Clock ticks since system startup */

__EXTERN_C_BEGIN__

__time_t	posix_current_time	__PROTO ((void));

__EXTERN_C_END__

#endif	/* ! defined (__KERNEL__TIMERS_H__) */

