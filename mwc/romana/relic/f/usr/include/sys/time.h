/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */

#ifndef	__SYS_TIME_H__
#define	__SYS_TIME_H__

#include <common/feature.h>
#include <common/__time.h>

/* Used to be "struct timeval" in Coherent prior to 4.2 */

typedef struct {
	__time_t	tv_sec;
	long		tv_usec;
} __timestruc_t;

#if	! _POSIX_SOURCE
typedef __timestruc_t	timestruc_t;
#endif

#endif	/* ! defined (__SYS_TIME_H__) */

