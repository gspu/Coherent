#ifndef	__KERNEL__SLEEP_H__
#define	__KERNEL__SLEEP_H__

/*
 * The following type and enumeration values should be used to test the result
 * of a kernel-internal sleep to see what the result of the sleep was. Making
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

