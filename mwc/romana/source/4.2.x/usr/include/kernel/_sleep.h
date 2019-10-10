/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__KERNEL__SLEEP_H__
#define	__KERNEL__SLEEP_H__

/*
 * Use the following type and enumeration values to test the result of a
 * kernel-internal sleep, to see what the result of the sleep was.  Making
 * this an enumeration is important for linting; the internal sleep functions
 * should be accessed though macros so that non-signalling sleeps are cast to
 * void properly.
 */

typedef enum {
	PROCESS_NORMAL_WAKE,
	PROCESS_SIGNALLED,
	PROCESS_CONTINUED
} __sleep_t;

#endif	/* ! defined (__KERNEL__SLEEP_H__) */

