/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__COMMON___CLOCK_H__
#define	__COMMON___CLOCK_H__

/*
 * This internal header file define the internal data type "__clock_t".
 * It resembles the ISO C data type "clock_t", but with an internal name,
 * so that headers that need this type need not import it into the user's
 * namespace.
 *
 * Several specifications  - including iBCS2 and the System V ABI - define
 * "clock_t" as an unsigned long; POSIX.1 and the C standard permit the use
 * of an unsigned type (which dramatically extends the range of times that
 * can be represented).
 *
 * Therefore, we select the type here on the basis of feature-test macros.
 */

#include <common/feature.h>

#if	_POSIX_C_SOURCE || _STDC_SOURCE || _DDI_DKI

typedef	unsigned long	__clock_t;

#else

typedef	long		__clock_t;

#endif

#endif	/* ! defined (__COMMON___CLOCK_H__) */
