/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__COMMON__CLKTCK_H__
#define	__COMMON__CLKTCK_H__

/*
 * This internal header file defines the ISO C constant "CLOCKS_PER_SEC"
 * and the related POSIX.1 constant CLK_TCK.
 */

#include <common/feature.h>

#if	__BORLANDC__

#define	CLK_TCK			18.2
#define	CLOCKS_PER_SEC		18.2

#elif	__GNUDOS__

#define	CLK_TCK			18.2
#define	CLOCKS_PER_SEC		18.2

#elif	__COHERENT__ && ! _SYSV4

#define	CLK_TCK			100
#define	CLOCKS_PER_SEC		100

#elif	__COHERENT__ && _SYSV4

#include <common/ccompat.h>

#define	CLK_TCK			1000000
#define	CLOCKS_PER_SEC		__sysconf (3)

__EXTERN_C__	long		__sysconf	PROTO ((int _name));

#else

# error The clock rate is not known for this system

#endif


#endif	/* ! defined (__COMMON__CLKTCK_H__) */
