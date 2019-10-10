#ifndef	__SYS_KSYNCH_H__
#define	__SYS_KSYNCH_H__

/*
 * This header has been introduced to hold the definition of the kernel type
 * GATE, which was originally in <sys/types.h>. With the POSIX cleanup, it
 * has been moved here where it belongs, in anticipation of the real DDI/DKI
 * synchronization primitives.
 *
 * You don't want to know what this type means. Really.
 */

typedef	char		GATE [2];

#endif	/* ! defined (__SYS_KSYNCH_H__) */
