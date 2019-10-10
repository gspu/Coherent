/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__KERNEL__BUF_H__
#define	__KERNEL__BUF_H__

/*
 * A common place for declaring an incomplete type that can be used to
 * declare function prototypes for kernel functions that expect to be passed
 * a member of the COHERENT buffer cache.
 */

#include <kernel/__buf.h>

typedef	__buf_t		buf_t;

#endif	/* ! defined (__KERNEL__BUF_H__) */

