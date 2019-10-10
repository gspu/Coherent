#ifndef	__COMMON___TIME_H__
#define	__COMMON___TIME_H__

/*
 * This internal header file is intended as the sole point of definition for
 * the internal data type "__time_t", exactly equivalent to the ISO C data
 * type "time_t" but given an internal name so that header files may refer
 * to the type without exporting the user-level type unnecessarily.
 *
 * The System V ABI and the iBCS2 specification set define "time_t" as a long.
 * A POSIX.1 system is permitted to use "unsigned long" to extend the range of
 * time values since the POSIX.1 epoch, since conforming applications should
 * be using ((time_t) -1) to detect errors.
 *
 * For now, we resolve this conflict by use of the _POSIX_SOURCE feature-test
 * macro. If _POSIX_SOURCE is defined, then we use the "unsigned long" form as
 * the client has contracted us in a POSIX guise.
 */

#if	_POSIX_SOURCE

typedef	unsigned long	__time_t;

#else

typedef	long		__time_t;

#endif


#endif	/* ! defined (__COMMON___TIME_H__) */
