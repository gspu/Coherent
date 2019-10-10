/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Kernel memory allocator.
 */

#ifndef	 __KERNEL_ALLOC_H__
#define	 __KERNEL_ALLOC_H__

#include <common/feature.h>
#include <common/ccompat.h>
#include <common/__size.h>
#include <common/__parith.h>

#define	NEXT_FIT	1

/*
 * Structure for allocator.
 */

#if	NEXT_FIT

typedef	struct _heap	heap_t;

#else

typedef	__VOID__      *	heap_t;

#endif


__EXTERN_C_BEGIN__

heap_t	      *	setarena	__PROTO ((__VOID__ * _base, __size_t _size));
__VOID__      *	alloc		__PROTO ((heap_t * _arena, __size_t _size));

__EXTERN_C_END__

#endif	/* ! defined (__KERNEL_ALLOC_H__) */
