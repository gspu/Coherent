#ifndef	__COMMON__SIZE_H__
#define	__COMMON__SIZE_H__

/*
 * This internal header file is intended as the sole point of definition for
 * the ISO C data type "size_t". To support cross-compilation environments,
 * this header file uses a variety of feature tests to avoid conflict with
 * definitions provided elsewhere by a translator's headers.
 */

#include <common/feature.h>
#include <common/__size.h>

#if	__BORLANDC__ || __COHERENT__

# ifndef	_SIZE_T
# define	_SIZE_T
typedef	__size_t	size_t;
# endif

#elif	__GNUDOS__

/*
 * For DJCC v1.09, you will have to modify <stddef.h> to prevent multiple
 * definitions of "size_t". The headers in DJCC are totally hosed. In addition,
 * GCC mandates "unsigned long" for reasons connected to built-in versions of
 * library functions.
 */


# ifndef	_SIZE_T
# define	_SIZE_T
typedef	__size_t	size_t;
# endif

#else

#error	size_t not known for this system

#endif

#endif	/* ! defined (__COMMON__SIZE_H__) */
