/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__KERNEL_DDI_GLOB_H__
#define	__KERNEL_DDI_GLOB_H__

/*
 * This internal header file defines structures and an access procedure for
 * DDI/DKI global data that must be shared between all CPUs.  Note that this
 * may not be all the truly global context; examine the STREAMS header files
 * for additional global data specific to STREAMS.
 */

#include <common/ccompat.h>
#include <kernel/x86lock.h>
#include <kernel/_lock.h>
#include <kernel/_sv.h>
#include <kernel/_toid.h>
#include <kernel/ddi_data.h>
#include <kernel/st_alloc.h>

/*
 * Timeout events are stored on a global queue, both while they are pending
 * and once they have expired. When an event expires, it is bound to a CPU
 * (if one was not specified when the event was created) and a function to
 * process expired events on that CPU is deferred.
 */

typedef	struct timeout	timeout_t;

typedef struct tlist {
	__lock_t      *	td_lock;
	toid_t		td_id;
	timeout_t     *	td_first;
	timeout_t     *	td_run;
} timelist_t;


/*
 * Truly global data used by the DDI/DKI system (except for STREAMS global
 * data).
 */

typedef	struct ddi_global_data	dgdata_t;

struct ddi_global_data {
	/*
	 * The defer-table data is at the front of the structure because it is
	 * typically manipulated by hand-coded assembly-language routines.
	 */

	defer_t		dg_defint;		/* interrupt-level defers */
	defer_t		dg_defproc;		/* process-level defers */

	atomic_uchar_t	dg_run_bufcalls;	/* bufcall check deferred */
	atomic_uchar_t	dg_run_strsched;	/* STREAMS service deferred */
	atomic_uchar_t	dg_run_time;		/* timeout check deferred */
	atomic_uchar_t	dg_init_flag;		/* initialization spin-lock */

	unsigned long	dg_reserved_2 [4];

	__lock_t      *	dg_polllock;		/* global lock on polling */
	__lock_t      *	dg_proclock;		/* process reference count */

	timelist_t	dg_timeouts;		/* timeout data */

	__lock_t      *	dg_kmem_lock;		/* basic lock for heap */
	_ST_HEAP_CONTROL_P dg_kmem_heap;
	__sv_t	      *	dg_kmem_sv;		/*
						 * synchronization variable for
						 * waiting for free memory.
						 */
	__size_t	dg_kmem_required;	/* Level of memory required */
};


__EXTERN_C_BEGIN__

dgdata_t      *	ddi_global_data	__PROTO ((void));

__EXTERN_C_END__


extern dgdata_t		__ddi_global_data;

#define	ddi_global_data() 	(& __ddi_global_data)


#endif	/* ! defined (__KERNEL_DDI_GLOB_H__) */
