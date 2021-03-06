/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__KERNEL__DEFER_H__
#define	__KERNEL__DEFER_H__

/*
 * This header file defines internal data type "deffuncp_t".  This definition
 * is imported by several kernel headers that should otherwise remain
 * disjoint.
 */

#include <common/ccompat.h>

typedef	void	     (*	__deffuncp_t)	__PROTO ((void));

#endif	/* ! defined (__KERNEL__DEFER_H__) */
