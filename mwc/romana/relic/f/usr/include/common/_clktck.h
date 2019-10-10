#ifndef	__COMMON__CLKTCK_H__
#define	__COMMON__CLKTCK_H__

/*
 * This internal header file is intended as the sole point of definition for
 * the ISO C constant "CLOCKS_PER_SEC" and the related POSIX.1 constant
 * CLK_TCK.
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
