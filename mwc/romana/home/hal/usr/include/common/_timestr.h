/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__COMMON__TIMESTR_H__
#define	__COMMON__TIMESTR_H__

/*
 * This internal header file defines the System V, Release 4 structure
 * "timestruc".  This structure is defined as being exported to the user
 * in <sys/time.h>, but is also required for inclusion as a member in other
 * structures such as "stat".
 *
 * BSD systems define a "struct timeval" that has a similar purpose and layout
 * to the SVR4 ABI "struct timestruc".  Users are cautioned that the "timeval"
 * structure is also not part of the iBCS2 specification, nor is it defined in
 * POSIX.
 */

#include <common/__time.h>


#if	_SYSV4 || _SYSV3
# define	__TIMESTRUC_TAG		timestruc
#else
# define	__TIMESTRUC_TAG		timeval
#endif

typedef struct __TIMESTRUC_TAG {
	__time_t	tv_sec;
	long		tv_usec;
} __timestruc_t;

#endif	/* ! defined (__COMMON__TIMESTR_H__) */

