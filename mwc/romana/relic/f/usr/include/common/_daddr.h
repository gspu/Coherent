#ifndef	__COMMON__DADDR_H__
#define	__COMMON__DADDR_H__

/*
 * This internal header file is intended as the sole point of definition for
 * the SVR4 data type "daddr_t". Non-user header files should take care to
 * use only the internal type so that "daddr_t" may be hidden for compiles
 * with _POSIX_SOURCE enabled.
 */

#include <common/feature.h>
#include <common/__daddr.h>

#if	! _POSIX_SOURCE

typedef	__daddr_t	daddr_t;

#endif

#endif	/* ! defined (__COMMON__DADDR_H__) */
