/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__MEMORY_H__
#define	__MEMORY_H__

/*
 * Some programs request this header for the mem*() functions from the
 * Standard C library. This behavior is derived from BSD UNIX, and is
 * not portable.  We just redirect the request to <string.h> so such
 * code can still compile.
 */

#include <string.h>

#endif	/* ! defined (__MEMORY_H__) */
