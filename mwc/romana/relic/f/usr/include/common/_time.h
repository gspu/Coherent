#ifndef	__COMMON__TIME_H__
#define	__COMMON__TIME_H__

/*
 * This internal header file is intended as the sole point of definition for
 * the ISO C data type "time_t". To support cross-compilation environments,
 * this header file uses a variety of feature tests to avoid conflict with
 * definitions supplied by the translator headers.
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
 * For DJCC v1.09, you will have to modify <sys/types.h> to guard against
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
