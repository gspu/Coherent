/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__COMMON___OFF_H__
#define	__COMMON___OFF_H__

/*
 * This internal header file defines the internal data type "__off_t".
 * It is equivalent to the POSIX.1 data type "off_t" but is given an internal
 * name so that header files may refer to it without exporting the user-level
 * type.
 */

typedef	long		__off_t;

#endif	/* ! defined (__COMMON___OFF_H__) */
