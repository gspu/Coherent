#define	_DDI_DKI	1
#define	_DDI_DKI_IMPL	1
#define	_SYSV4		1

/*
 *-IMPORTS:
 *	<common/ccompat.h>
 *		__USE_PROTO__
 *		__ARGS ()
 *	<sys/debug.h>
 *		ASSERT ()
 *	<kernel/ddi_cpu.h>
 *		dcdata_t
 *		ddi_cpu_data ()
 *	<kernel/ddi_glob.h>
 *		dgdata_t
 *		ddi_global_data ()
 *	<kernel/ddi_lock.h>
 *		defer_priority
 *		defer_hierarchy
 *	<sys/ksynch.h>
 *		LOCK ()
 *		UNLOCK ()
 *	<sys/cmn_err.h>
 *		CE_WARN
 *		cmn_err ()
 *	<sys/stdlib.h>
 *		NULL
 */

#include <common/ccompat.h>
#include <kernel/ddi_cpu.h>
#include <kernel/ddi_glob.h>
#include <kernel/ddi_lock.h>
#include <sys/debug.h>
#include <sys/ksynch.h>
#include <sys/cmn_err.h>
#include <stddef.h>

#include <kernel/defer.h>


/*
 * This internal function queues a deferred function on a deferred-function
 * queue.
 */

#if	__USE_PROTO__
__LOCAL__ int (DEFER) (defer_t * deferp, __deffuncp_t funcp)
#else
__LOCAL__ int
DEFER __ARGS ((deferp, funcp))
defer_t	      *	deferp;
__deffuncp_t	funcp;
#endif
{
	pl_t		prev_pl;
	int		idx;

	ASSERT (funcp != NULL);
	ASSERT (deferp != NULL);
	ASSERT (deferp->df_tab != NULL);
	ASSERT (ATOMIC_FETCH_UCHAR (deferp->df_max) > 0);

	prev_pl = LOCK (deferp->df_wlock, defer_priority);

	/*
	 * Write the function pointer into the next free slot in the table,
	 * and then move the write pointer to the next entry in the circular
	 * buffer.
	 */

	idx = ATOMIC_FETCH_UCHAR (deferp->df_write);

	deferp->df_tab [idx ++] = funcp;

	if (idx == ATOMIC_FETCH_UCHAR (deferp->df_max))
		idx = 0;

	/*
	 * If the write pointer bumps into the read pointer, then the defer
	 * table is full and we should complain. A possible alternative
	 * strategy would be to wait for the other processor, but that is only
	 * a good idea if we have the machinery to interrupt it available,
	 * which we currently do not.
	 */

	if (idx != ATOMIC_FETCH_UCHAR (deferp->df_read)) {
		/*
		 * Store the updated write pointer and prepare to return
		 * success.
		 */

		ATOMIC_STORE_UCHAR (deferp->df_write, idx);
		idx = 0;
	} else {

		cmn_err (CE_WARN, "table overflow in DEFER ()");
		idx = 1;	/* flag failure to the caller */
	}

	UNLOCK (deferp->df_wlock, prev_pl);

	return idx;
}


/*
 *-STATUS:
 *	For the Implementors only.
 *
 *-NAME:
 *	defer_int_cpu	Defer a routine on a specified CPU.
 *
 *-SYNOPSIS:
 *	#include <kernel/defer.h>
 *
 *	int defer_int_cpu (void (* funcp) (void), processorid_t cpuid);
 *
 *-ARGUMENTS:
 *	funcp		Pointer to the function to be executed upon the given
 *			processor's return from kernel execution to user-level
 *			execution.
 *
 *	cpuid		Processor on which "funcp" is to be executed.
 *
 *-DESCRIPTION:
 *	defer_int_cpu () requests that a function be executed on the given CPU
 *	at a priority higher than any user-level code. If the given CPU is
 *	currently executing kernel code, the function will be run before user-
 *	level execution is resumed. The routine is run at a priority lower
 *	than any interrupt-level code; if the implementation permits, the
 *	routine will be run at a priority higher than base kernel code, but
 *	this is not guaranteed.
 *
 *	The implementation will make a best effort to ensure that the deferred
 *	function is executed promptly. However, some hardware configurations
 *	may not permit one CPU to interrupt another under software control, so
 *	if the specified CPU is currently running at user level it may not
 *	execute the deferred function for some time.
 *
 *-RETURN VALUE:
 *	0 is returned on success. If the specified CPU is invalid, or the
 *	deferred-function table for the specified CPU is full, then a non-zero
 *	error value is returned.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	It is currently unspecified whether the function pointed to by "funcp"
 *	is permitted to contend for locks held by the caller.
 */

#if	__USE_PROTO__
int (defer_int_cpu) (__deffuncp_t funcp, processorid_t cpuid)
#else
int
defer_int_cpu __ARGS ((funcp, cpuid))
__deffuncp_t	funcp;
processorid_t	cpuid;
#endif
{
	dcdata_t      *	dcdatap;
	int		retval;

	ASSERT (funcp != NULL);

	if ((dcdatap = ddi_cpu_ref (cpuid)) == NULL)
		return -1;

	retval = DEFER (& dcdatap->dc_defint, funcp);

	ddi_cpu_unref (dcdatap);
	return retval;
}


/*
 *-STATUS:
 *	For the Implementors only.
 *
 *-NAME:
 *	defer_int_any	Defer a routine on an unspecified CPU.
 *
 *-SYNOPSIS:
 *	#include <kernel/defer.h>
 *
 *	int defer_int_any (void (* funcp) (void));
 *
 *-ARGUMENTS:
 *	funcp		Pointer to the function to be executed upon some
 *			processor's return from kernel execution to user-level
 *			execution.
 *
 *-DESCRIPTION:
 *	defer_int_any () requests that a function be executed at some future
 *	time convenient to the implementation. The routine will be run at a
 *	priority higher than any user-level code, and lower than any
 *	interrupt-level code; if the implementation permits, the function will
 *	be run at a priority higher than base kernel code, but this is not
 *	guaranteed.
 *
 *	The function will be executed on the first CPU available. However, a
 *	typical implementation would not interrupt user-level execution on
 *	any other CPUs merely to run deferred functions.
 *
 *-RETURN VALUE:
 *	0 is returned on success. If the global deferred-function table is
 *	full, then a non-zero error value is returned.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	It is currently unspecified whether the function pointed to by "funcp"
 *	is permitted to contend for locks held by the caller.
 */

#if	__USE_PROTO__
int (defer_int_any) (__deffuncp_t funcp)
#else
int
defer_int_any __ARGS ((funcp))
__deffuncp_t	funcp;
#endif
{
	return DEFER (& ddi_global_data ()->dg_defint, funcp);
}


/*
 *-STATUS:
 *	For the Implementors only.
 *
 *-NAME:
 *	defer_int_here	Defer a routine on the current CPU.
 *
 *-SYNOPSIS:
 *	#include <kernel/defer.h>
 *
 *	int defer_int_here (void (* funcp) (void));
 *
 *-ARGUMENTS:
 *	funcp		Pointer to the function to be executed upon this
 *			processor's return from kernel execution to user-level
 *			execution.
 *
 *-DESCRIPTION:
 *	defer_int_here () requests that a function be executed at some future
 *	time convenient to the implementation on the current CPU. The routine
 *	will be run at a priority higher than any user-level code, and lower
 *	than any interrupt-level code; if the implementation permits, the
 *	function will run at a level higher than base kernel code, but this is
 *	not guaranteed.
 *
 *-RETURN VALUE:
 *	0 is returned on success. If the current CPUs deferred-function table
 *	is full, then a non-zero error value is returned.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	It is currently unspecified whether the function pointed to by "funcp"
 *	is permitted to contend for locks held by the caller.
 */

#if	__USE_PROTO__
int (defer_int_here) (__deffuncp_t funcp)
#else
int
defer_int_here __ARGS ((funcp))
__deffuncp_t	funcp;
#endif
{
	return DEFER (& ddi_cpu_data ()->dc_defint, funcp);
}


/*
 *-STATUS:
 *	For the Implementors only.
 *
 *-NAME:
 *	defer_proc_cpu	Defer a routine on a specified CPU.
 *
 *-SYNOPSIS:
 *	#include <kernel/defer.h>
 *
 *	int defer_proc_cpu (void (* funcp) (void), processorid_t cpuid);
 *
 *-ARGUMENTS:
 *	funcp		Pointer to the function to be executed upon the given
 *			processor's return from kernel execution to user-level
 *			execution.
 *
 *	cpuid		Processor on which "funcp" is to be executed.
 *
 *-DESCRIPTION:
 *	defer_proc_cpu () requests that a function be executed on the given CPU
 *	at a priority higher than any user-level code. If the given CPU is
 *	currently executing kernel code, the function will be run before user-
 *	level execution is resumed.
 *
 *	The implementation will make a best effort to ensure that the deferred
 *	function is executed promptly. However, some hardware configurations
 *	may not permit one CPU to interrupt another under software control, so
 *	if the specified CPU is currently running at user level it may not
 *	execute the deferred function for some time.
 *
 *-RETURN VALUE:
 *	0 is returned on success. If the specified CPU is invalid, or the
 *	deferred-function table for the specified CPU is full, then a non-zero
 *	error value is returned.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	It is currently unspecified whether the function pointed to by "funcp"
 *	is permitted to contend for locks held by the caller.
 */

#if	__USE_PROTO__
int (defer_proc_cpu) (__deffuncp_t funcp, processorid_t cpuid)
#else
int
defer_proc_cpu __ARGS ((funcp, cpuid))
__deffuncp_t	funcp;
processorid_t	cpuid;
#endif
{
	dcdata_t      *	dcdatap;
	int		retval;

	ASSERT (funcp != NULL);

	if ((dcdatap = ddi_cpu_ref (cpuid)) == NULL)
		return -1;

	retval = DEFER (& dcdatap->dc_defproc, funcp);

	ddi_cpu_unref (dcdatap);
	return retval;
}


/*
 *-STATUS:
 *	For the Implementors only.
 *
 *-NAME:
 *	defer_proc_any	Defer a routine on an unspecified CPU.
 *
 *-SYNOPSIS:
 *	#include <kernel/defer.h>
 *
 *	int defer_proc_any (void (* funcp) (void));
 *
 *-ARGUMENTS:
 *	funcp		Pointer to the function to be executed upon some
 *			processor's return from kernel execution to user-level
 *			execution.
 *
 *-DESCRIPTION:
 *	defer_proc_any () requests that a function be executed at some future
 *	time convenient to the implementation. The routine will be run at a
 *	priority higher than any user-level code, and lower than any
 *	interrupt-level code or kernel code.
 *
 *	The function will be executed on the first CPU available. However, a
 *	typical implementation would not interrupt user-level execution on
 *	any other CPUs merely to run deferred functions.
 *
 *-RETURN VALUE:
 *	0 is returned on success. If the global deferred-function table is
 *	full, then a non-zero error value is returned.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	It is currently unspecified whether the function pointed to by "funcp"
 *	is permitted to contend for locks held by the caller.
 */

#if	__USE_PROTO__
int (defer_proc_any) (__deffuncp_t funcp)
#else
int
defer_proc_any __ARGS ((funcp))
__deffuncp_t	funcp;
#endif
{
	return DEFER (& ddi_global_data ()->dg_defproc, funcp);
}


/*
 *-STATUS:
 *	For the Implementors only.
 *
 *-NAME:
 *	defer_proc_here	Defer a routine on the current CPU.
 *
 *-SYNOPSIS:
 *	#include <kernel/defer.h>
 *
 *	int defer_proc_here (void (* funcp) (void));
 *
 *-ARGUMENTS:
 *	funcp		Pointer to the function to be executed upon this
 *			processor's return from kernel execution to user-level
 *			execution.
 *
 *-DESCRIPTION:
 *	defer_proc_here () requests that a function be executed at some futur
 *	time convenient to the implementation on the current CPU. The routine
 *	will be run at a priority higher than any user-level code, and lower
 *	than any interrupt-level code or kernel code.
 *
 *-RETURN VALUE:
 *	0 is returned on success. If the current CPUs deferred-function table
 *	is full, then a non-zero error value is returned.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	It is currently unspecified whether the function pointed to by "funcp"
 *	is permitted to contend for locks held by the caller.
 */

#if	__USE_PROTO__
int (defer_proc_here) (__deffuncp_t funcp)
#else
int
defer_proc_here __ARGS ((funcp))
__deffuncp_t	funcp;
#endif
{
	return DEFER (& ddi_cpu_data ()->dc_defproc, funcp);
}
