/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__COMMON___IOVEC_H__
#define	__COMMON___IOVEC_H__

/*
 * This internal header file defines the structure "iovec",
 * which is used as a scatter/gather I/O fragment description.
 *
 * Several user-level and DDI/DKI headers export this structure and possibly
 * an associated typedef; this header contains the single underlying
 * definition.
 */

#include <common/__caddr.h>

typedef	struct iovec	__iovec_t;

struct iovec {
	__caddr_t	iov_base;	/* base address of storage area */
	int		iov_len;	/* size of the storage area in bytes */
};

#endif	/* ! defined (__COMMON___IOVEC_H__) */

