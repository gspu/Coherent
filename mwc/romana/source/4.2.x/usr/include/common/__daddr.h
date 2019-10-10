/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__COMMON___DADDR_H__
#define	__COMMON___DADDR_H__

/*
 * This internal header file defines the internal data type "__daddr_t".
 * It is equivalent to the SVR4 data type "daddr_t", but is given an internal
 * name so that header files can refer to it without exporting the user-level
 * type.	
 */

typedef	long		__daddr_t;

#endif	/* ! defined (__COMMON___DADDR_H__) */
