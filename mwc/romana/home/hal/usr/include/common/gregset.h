/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__COMMON_GREGSET_H__
#define	__COMMON_GREGSET_H__

/*
 * An incomplete type definition you can use to point at register sets saved by
 * the kernel trap-handler or the user-level signal code.
 */

typedef	union __gregset	gregset_t;

#endif	/* ! defined (__COMMON_GREGSET_H__) */

