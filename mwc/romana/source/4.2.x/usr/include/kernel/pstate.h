/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__KERNEL_PSTATE_H__
#define	__KERNEL_PSTATE_H__

/*
 * Process state-code type.  This is available at user level to the "ps"
 * command, but has no portable user-level use even there, as the kernel
 * also supplies "ps" with the printable character-code that corresponds to
 * the state.
 */

typedef enum {
	PS_RUN,			/* Running */
	PS_READY,		/* Ready to run */
	PS_MUTEX,		/* Blocked on a basic or read/write lock */
	PS_SLEEP,		/* Sleeping, signals do not interrupt */
	PS_WAIT,		/* Sleeping, interruptible */
	PS_STOPPED,		/* Stopped */
	PS_ZOMBIE,		/* Exiting */
	PS_MAXSTATE
} procstate_t;


/*
 * The only drawback of using enumerations is that we cannot perform useful
 * tests on them at compile-time with #if.  Here, we set a feature-test to
 * say whether the state code fits into a word (or long-word) mask so
 * that tests for state sets can have some important source-level optimization
 * performed.  Your mileage may vary, but it may be faster to use shift-and-
 * mask techniques than a cascade of equality tests.  This feature-test says
 * whether these techniques /can/ be applied to this data type.
 */

#define	_PSTATE_CAN_MASK		1

#endif	/* ! defined (__KERNEL_PSTATE_H__) */
