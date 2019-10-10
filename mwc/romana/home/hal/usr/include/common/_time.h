/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__COMMON__TIME_H__
#define	__COMMON__TIME_H__

/*
 * This internal header file defines the ISO C data type "time_t".  To support
 * cross-compilation environments, it uses a variety of feature tests to
 * avoid conflict with definitions supplied by the translator headers.
 */

#include <common/feature.h>
#include <common/__time.h>

#if	__BORLANDC__

# ifndef	_TIME_T
# define	_TIME_T
typedef	__time_t	time_t;
# endif

#elif __GNUDOS__

/*
 * For DJCC v1.09, you must modify <sys/types.h> to guard against
 * multiple definitions. The header structure is hosed.
 */

# ifndef	_TIME_T
# define	_TIME_T
typedef __time_t	time_t;
# endif

#elif	__COHERENT__

typedef	__time_t	time_t;

#else

# error	time_t not known for this system

#endif


#endif	/* ! defined (__COMMON__TIME_H__) */
