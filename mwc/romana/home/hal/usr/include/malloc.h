/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__MALLOC_H__
#define	__MALLOC_H__

/*
 * Some BSD programs use this header file to determine whether a system is
 * BSD-derived:  if present, malloc() is prototyped here; otherwise,
 * it is prototyped in <sys/malloc.h>.  NB, under COHERENT, malloc() is
 * prototyped in  <stdlib.h>.
 */

#include <stdlib.h>

#endif	/* ! defined (__MALLOC_H__) */
