/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__COMMON___FSIZE_H__
#define	__COMMON___FSIZE_H__

/*
 * This internal header file defines the internal data type "__fsize_t".
 * This type abstracts the notion of file size for Coherent utilities to aid
 * in migration to file systems that can represent files 2^32-1 bytes.
 *
 * This definition is the basis of the Coherent data type "fsize_t", but given
 * an internal name so that headers can use it internally without exporting it
 * to the user space.
 *
 * Macros for accessing items of this type portably are not finished.
 * Users of this type should be aware that it may not remain a scalar type.
 * Use of this type in portable software is strongly discouraged.
 */

typedef	unsigned long	__fsize_t;

#endif	/* ! defined (__COMMON___FSIZE_H__) */
