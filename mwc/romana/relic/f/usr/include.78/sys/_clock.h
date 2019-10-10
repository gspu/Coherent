#ifndef	__SYS__CLOCK_H__
#define	__SYS__CLOCK_H__

/*
 * This internal header file is intended as the sole point of definition for
 * the ISO C data type "clock_t". To support cross-compilation environments,
 * this header file uses a variety of feature tests to avoid conflict with
 * definitions supplied by the translator headers.
 */

#include <sys/__clock.h>

#if	defined (__BORLANDC__)

# ifndef	_CLOCK_T
# define	_CLOCK_T
typedef	__clock_t	clock_t;


#define	CLK_TCK		18.2
#define	CLOCKS_PER_SEC	18.2

# endif

#elif	defined (__GNUC__)

typedef __clock_t	clock_t;

#elif	defined (__COHERENT__)

typedef __clock_t	clock_t;

#else

# error	clock_t not known for this system

#endif

#endif	/* ! defined (__SYS__CLOCK_H__) */
