/*
 * /usr/include/sys/__clock.h
 *
 * Definition of __clock_t.
 *
 * Revised: Wed May 19 10:26:58 1993 CDT
 */
#ifndef	__SYS___CLOCK_H__
#define	__SYS___CLOCK_H__

/*
 * This internal header file is intended as the sole point of definition for
 * the internal data type "__clock_t", exactly like the ISO C data type
 * "clock_t" but with an internal same so that headers which define function
 * prototypes needing this type need not actually import the type into the
 * user's namespace.
 *
 * Several specifications including iBCS2 and the System V ABI define the
 * "clock_t" type as an unsigned long, although POSIX.1 and the C standard
 * permit the use of an unsigned type (which dramatically extends the range
 * of times that can be represented).
 *
 * Accordingly, we select the type here on the basis of feature-test macros.
 */

#if	defined(_POSIX_SOURCE) || _STDC_SOURCE || _DDI_DKI

typedef	unsigned long	__clock_t;

#else

typedef	long		__clock_t;

#endif

#endif	/* ! defined (__SYS___CLOCK_H__) */
