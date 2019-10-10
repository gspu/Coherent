/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	 __KERNEL_ALLOC_H__
#define	 __KERNEL_ALLOC_H__

#include <common/feature.h>
#include <common/ccompat.h>
#include <common/__size.h>
#include <common/__caddr.h>


/*
 * Prototypes for stuff relating to the old, broken heap manager.  Equivalent
 * to DDI/DKI kmem_... () routines.
 */

__EXTERN_C_BEGIN__

__caddr_t	palloc		__PROTO ((__size_t _size));
void		pfree		__PROTO ((__caddr_t _mem));

__VOID__      *	kalloc		__PROTO ((__size_t _size));
void		kfree		__PROTO ((__VOID__ * _mem));

void		KMEM_INIT	__PROTO ((__VOID__ * _mem, __size_t _size));

__EXTERN_C_END__

#endif	/* ! defined (__KERNEL_ALLOC_H__) */
