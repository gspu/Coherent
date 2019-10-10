/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__COMMON_LINEAR_H__
#define	__COMMON_LINEAR_H__

/*
 * Implementation-private type for representing i386 linear memory addresses
 * for use in machine-specific data structures. Linear memory is of interest
 * mainly to debuggers (for manipulating the i386 debug registers) and to
 * paging-related code (because linear addresses need to be taken apart to
 * index the page tables).
 *
 * A representation of linear addresses as an arithmetic type is plausible, to
 * allow easy conversion to/from printable forms; however, the fact that
 * pointer arithmetic is almost never used on linear addresses makes a non-
 * scalar type more sensible, so structures contain the non-arithmetic form
 * and an arithmetic form of compatible size is provided for use in debugging
 * applications.
 */

#include <common/__types.h>

typedef	__ulong_t	__linear_arith_t;


typedef	struct __linear_address
		      *	__linear_t;

#define	__LINEAR_TO_ARITH(lin)		((__linear_arith_t) (lin))
#define	__ARITH_TO_LINEAR(arith)	((__linear_t) (arith))

#endif	/* ! defined (__COMMON_LINEAR_H__) */

