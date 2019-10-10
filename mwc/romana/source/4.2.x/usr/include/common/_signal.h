/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__COMMON__SIGNAL_H__
#define	__COMMON__SIGNAL_H__

#include <common/ccompat.h>
#include <common/_limits.h>
#include <common/_tricks.h>

/*
 * This header file contains a number of definitions for signal-related data
 * types used by kernel and user-level mechanisms to support the variety of
 * existing binary interfaces.
 */

/*
 * The following constant gives the number of signals for which kernel storage
 * is made available; this number is traditionally made available to user code
 * though the NSIG constant, although neither Standard C nor POSIX.1
 * applications can use it.
 */

#define	_SIGNAL_MAX		31


/*
 * Be aware that while ISO C and POSIX.1 specify the signature of a signal-
 * catching function as "void (*) (int sig)", extra parameters to the signal
 * functions are supported.
 */

typedef	void	__sighand_t	__PROTO ((__ANY_ARGS__));


/*
 * The following deals with signal actions. For maximum efficiency, we use
 * signals as collections of bit-vectors; various user-level binary
 * compatibility standards specify the lengths of the vectors, whereas the
 * kernel internally allocates exactly as much as is really needed.
 */

typedef	unsigned long		__sigmask_t;

#define	__SIGMASK_BIT		__LONG_BIT

typedef	struct {
	__sigmask_t	_sigbits [4];
} n_sigset_t;

#define	__N_SIGSET_SET(ss, value) \
		((ss)._sigbits [3] = (ss)._sigbits [2] = \
		 (ss)._sigbits [1] = (ss)._sigbits [0] = (value))

typedef	struct {
	__sigmask_t	_sigbits [1];
} o_sigset_t;

#define	__O_SIGSET_SET(ss, value)	(* (ss)._sigbits = (value))

#define	___SIGSET_LEN	__DIVIDE_ROUNDUP_CONST (_SIGNAL_MAX, __SIGMASK_BIT)
typedef struct {
	__sigmask_t	_sigbits [___SIGSET_LEN];
} __sigset_t;

#if	___SIGSET_LEN == 1
#define	___SIGSET_SET(ss, value)	(* (ss)._sigbits = (value))
#elif	___SIGSET_LEN == 2
#define	___SIGSET_SET(ss, value) \
		((ss)._sigbits [1] = (ss)._sigbits [0] = (value))
#else
#error	_SIGNAL_MAX is larger than we expect
#endif


/*
 * Signal-action flags that are relevant for all signals.
 */

enum {
	__SA_ONSTACK	= 1,
	__SA_RESETHAND	= 2,
	__SA_RESTART	= 4,
	__SA_SIGINFO	= 8,
	__SA_NODEFER	= 16
};
typedef	unsigned short	__sigactfl_t;


/*
 * Signal-action flags that are only relevant for particular signals.
 */

enum {
	__SF_NOCLDWAIT	= 1,
	__SF_NOCLDSTOP	= 2
};

typedef	unsigned short	__sigmiscfl_t;


/*
 * Some simple macros for dealing with the bit-set operations on sigset_t's.
 */

#define	__SIGSET_UNIT(ss,n)	\
	(sizeof ((ss)._sigbits) == sizeof (__sigmask_t) ? 0 : \
	 (unsigned) ((n) - 1) / (sizeof (__sigmask_t) * __CHAR_BIT))

#define	__SIGSET_MASK(n)	(1UL << ((unsigned) ((n) - 1) & \
				  (sizeof (__sigmask_t) * __CHAR_BIT - 1)))

#define	__SIGSET_ADDBIT(ss,n)	\
		((ss)._sigbits [__SIGSET_UNIT (ss, n)] |= __SIGSET_MASK (n))
#define	__SIGSET_CLRBIT(ss,n)	\
		((ss)._sigbits [__SIGSET_UNIT (ss, n)] &= ~ __SIGSET_MASK (n))
#define	__SIGSET_TSTBIT(ss,n)	\
		(((ss)._sigbits [__SIGSET_UNIT (ss, n)] & \
		 __SIGSET_MASK (n)) != 0)
#define	__SIGSET_ADDMASK(ss,n,m)	\
		((ss)._sigbits [__SIGSET_UNIT (ss, n)] |= (m))
#define	__SIGSET_CLRMASK(ss,n,m)	\
		((ss)._sigbits [__SIGSET_UNIT (ss, n)] &= ~ (m))
#define	__SIGSET_TSTMASK(ss,n,m)	\
		(((ss)._sigbits [__SIGSET_UNIT (ss, n)] & (m)) != 0)
#define	__SIGSET_FIRSTBIT(mask)		(__LEAST_BIT_ULONG (mask) + 1)


#endif	/* ! defined (__COMMON__SIGNAL_H__) */
