/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__COMMON__DADDR_H__
#define	__COMMON__DADDR_H__

/*
 * This internal header file defines the SVR4 data type "daddr_t".  Non-user
 * header files should use only the internal type so that "daddr_t" can be
 * hidden for compilation with _POSIX_C_SOURCE enabled.
 */

#include <common/feature.h>
#include <common/__daddr.h>

#if	! _POSIX_C_SOURCE

typedef	__daddr_t	daddr_t;

#endif

#endif	/* ! defined (__COMMON__DADDR_H__) */
