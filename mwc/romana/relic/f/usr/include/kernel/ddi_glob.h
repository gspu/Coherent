#ifndef	__KERNEL_DDI_GLOB_H__
#define	__KERNEL_DDI_GLOB_H__

/*
 * This internal header file defines structures and an access procedure for
 * DDI/DKI global data that must be shared between all CPUs. Note that this
 * may not be all the truly global context; examine the STREAMS header files
 * for additional global data specific to STREAMS.
 */

/*
 *-IMPORTS:
 *	<common/ccompat.h>
 *		__EXTERN_C_BEGIN__
 *		__EXTERN_C_END__
 *		__PROTO ()
 *	<kernel/_lock.h>
 *		lock_t
 *	<kernel/_toid.h>
 *		toid_t
 *	<kernel/x86lock.h>
 *		atomic_uchar_t
 */

#include <common/ccompat.h>
#include <kernel/x86lock.h>
#include <kernel/_lock.h>
#include <kernel/_toid.h>
#include <kernel/ddi_data.h>


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

struct ddi_global_data {
	/*
	 * The defer-table data is at the front of the structure because it is
	 * typically manipulated by hand-coded assembly-language routines.
	 */

	defer_t		dg_defint;		/* interrupt-level defers */
	defer_t		dg_defproc;		/* process-level defers */

	__lock_t      *	dg_polllock;		/* global lock on polling */
	__lock_t      *	dg_proclock;		/* process reference count */

	timelist_t	dg_timeouts;		/* timeout data */

	atomic_uchar_t	dg_run_bufcalls;	/* bufcall check deferred */
	atomic_uchar_t	dg_run_strsched;	/* STREAMS service deferred */
	atomic_uchar_t	dg_run_time;		/* timeout check deferred */
};

typedef	struct ddi_global_data	dgdata_t;


__EXTERN_C_BEGIN__

dgdata_t      *	ddi_global_data	__PROTO ((void));

__EXTERN_C_END__


extern dgdata_t		__ddi_global_data;

#define	ddi_global_data() 	(& __ddi_global_data)


#endif	/* ! defined (__KERNEL_DDI_GLOB_H__) */
