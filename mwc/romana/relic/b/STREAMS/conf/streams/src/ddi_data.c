#define	_DDI_DKI	1
#define	_DDI_DKI_IMPL	1
#define	_SYSV4		1

/*
 * Definition for the DDI/DKI linkable version of ddi_proc ()
 */

/*
 *-IMPORTS:
 *	<common/ccompat.h>
 *		__USE_PROTO__
 *		__ARGS ()
 *	<kernel/x86lock.h>
 *		ATOMIC_STORE_UCHAR ()
 *	<kernel/ddi_lock.h>
 *		INTERNAL_LOCK
 *		poll_global_hierarchy
 *		poll_global_priority
 *		defer_hierarchy
 *		defer_priority
 *		proc_global_hierarchy
 *		proc_global_priority
 *	<sys/types.h>
 *		inb ()
 *		outb ()
 *	<sys/ksynch.h>
 *		lkinfo_t
 *		LOCK_ALLOC ()
 *	<sys/kmem.h>
 *		KM_NOSLEEP
 *		kmem_alloc ()
 *	<stddef.h>
 *		NULL
 *	<string.h>
 *		memset ()
 */

#include <common/ccompat.h>
#include <kernel/x86lock.h>
#include <kernel/ddi_lock.h>
#include <sys/types.h>
#include <sys/ksynch.h>
#include <sys/kmem.h>
#include <stddef.h>
#include <string.h>

#include <kernel/ddi_base.h>
#include <kernel/ddi_proc.h>
#include <kernel/ddi_cpu.h>
#include <kernel/ddi_glob.h>


/*
 * Here we have the actal static data declarations for the uniprocessor
 * implementation of the DDI/DKI per-CPU data. Note that under a "real"
 * system, the dynamic area would be the space left over by the fixed data up
 * to the size of a page, because that seems the best (read fastest and most
 * secure) way of doing it (by mapping each processor's private data into the
 * same address across all processors). Of course, it's questionable whether
 * any drivers *will* actually use the binary-compatible function-versions of
 * the accessors, but that's not our problem.
 *
 * Don't forget that if we *do* use the paging system for per-CPU data that
 * those pages will have to be mapped somewhere else as well, because there is
 * data like the deferred function table that the world may want controlled
 * access to.
 */

dcdata_t	__ddi_cpu_data [1];
dgdata_t	__ddi_global_data;
__LOCAL__ char	ddi_cpu_dynarea [512];


/*
 * Lock information structures.
 */

__LOCAL__ lkinfo_t poll_global_lkinfo = {
	"polling global lock", INTERNAL_LOCK
};

__LOCAL__ lkinfo_t defer_lkinfo = {
	"defer write lock", INTERNAL_LOCK
};

__LOCAL__ lkinfo_t proc_global_lkinfo = {
	"process global lock", INTERNAL_LOCK
};


/*
 *-STATUS:
 *	Local DDI/DKI extension
 *
 *-NAME:
 *	ddi_base_data		Get per-process DDI/DKI base data.
 *
 *-SYNOPSIS:
 *	#include <kernel/ddi_base.h>
 *
 *	dbdata_t * ddi_base_data (void);
 *
 *-DESCRIPTION:
 *	This function returns a base pointer to a table of information that
 *	the DDI/DKI needs to associate with a process but will not need to
 * 	access outside that process context.
 *
 *-RETURN VALUE:
 *	The base address of a per-process DDI/DKI data table entry. The
 *	value returned may be considered "constant" and memoized within a
 *	context provided that no rescheduling may occur.
 *
 *-LEVEL:
 *	Base only.
 *
 *-NOTES:
 *	This function does not sleep.
 */


#if	__USE_PROTO__
dbdata_t * (ddi_base_data) (void)
#else
dbdata_t *
ddi_base_data __ARGS (())
#endif
{
	ASSERT_BASE_LEVEL ();

	return ddi_base_data ();
}


/*
 *-STATUS:
 *	Local DDI/DKI extension
 *
 *-NAME:
 *	ddi_proc_data		Get per-process DDI/DKI global data.
 *
 *-SYNOPSIS:
 *	#include <kernel/ddi_proc.h>
 *
 *	dpdata_t * ddi_proc_data (void);
 *
 *-DESCRIPTION:
 *	This function returns a base pointer to a table of information that
 *	the DDI/DKI needs to associate with a process but may need to access
 * 	outside the process context.
 *
 *-RETURN VALUE:
 *	The base address of a per-process DDI/DKI data table entry. The
 *	value returned may be considered "constant" and memoized within a
 *	context provided that no rescheduling may occur.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	This function does not sleep.
 */


#if	__USE_PROTO__
dpdata_t * (ddi_proc_data) (void)
#else
dpdata_t *
ddi_proc_data __ARGS (())
#endif
{
	return ddi_proc_data ();
}


/*
 *-STATUS:
 *	Local DDI/DKI extension
 *
 *-NAME:
 *	ddi_cpu_data		Get per-CPU DDI/DKI global data.
 *
 *-SYNOPSIS:
 *	#include <kernel/ddi_cpu.h>
 *
 *	dcdata_t * ddi_cpu_data (void);
 *
 *-DESCRIPTION:
 *	This function returns a base pointer to a table of information that
 *	can be considered per-CPU DDI/DKI static data. The value returned
 *	may be considered "constant" and memoized within a context provided
 *	that no rescheduling could occur.
 *
 *-RETURN VALUE:
 *	The base address of a per-CPU DDI/DKI data table entry.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	This function does not sleep.
 */

#if	__USE_PROTO__
dcdata_t * (ddi_cpu_data) (void)
#else
dcdata_t *
ddi_cpu_data __ARGS (())
#endif
{
	return ddi_cpu_data ();
}


/*
 *-STATUS:
 *	Local DDI/DKI extension
 *
 *-NAME:
 *	ddi_global_data		Get DDI/DKI global data.
 *
 *-SYNOPSIS:
 *	#include <kernel/ddi_glob.h>
 *
 *	dgdata_t * ddi_global_data (void);
 *
 *-DESCRIPTION:
 *	This function returns a base pointer to a table of information that
 *	represents the global state of the DDI/DKI subsystem, with the
 *	possible exception of the STREAMS global state.
 *
 *-RETURN VALUE:
 *	The base address of the global DDI/DKI data table. The value returned
 *	may be considered "constant" and memoized within a context provided
 *	that no rescheduling may occur.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	This function does not sleep.
 */


#if	__USE_PROTO__
dgdata_t * (ddi_global_data) (void)
#else
dgdata_t *
ddi_global_data __ARGS (())
#endif
{
	return ddi_global_data ();
}


/*
 *-STATUS:
 *	For the Implementors only.
 *
 *-NAME:
 *	ddi_cpu_id	Determine the current CPU id.
 *
 *-SYNOPSIS:
 *	#include <kernel/ddi_cpu.h>
 *
 *	processorid_t ddi_cpu_id (void);
 *
 *-DESCRIPTION:
 *	ddi_cpu_id () allows DDI/DKI code a way of accessing the current CPU
 *	id for passing to functions such as dtimeout () or defer_int_cpu ().
 *
 *-RETURN VALUE:
 *	The id code of the CPU that the caller is executing on.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 */

#if	__USE_PROTO__
processorid_t (ddi_cpu_id) (void)
#else
processorid_t
ddi_cpu_id __ARGS (())
#endif
{
	return ddi_cpu_data ()->dc_cpuid;
}


/*
 *-STATUS:
 *	For the Implementors only.
 *
 *-NAME:
 *	ddi_cpu_alloc	Dynamically allocates per-CPU data.
 *
 *-SYNOPSIS:
 *	#include <kernel/ddi_cpu.h>
 *
 *	void * ddi_cpu_alloc (size_t size);
 *
 *-ARGUMENTS:
 *	size		The size in bytes to allocate. This value will always
 *			be rounded up to the size of an integer to ensure the
 *			alignment of the returned space. A value of 0 is not
 *			legal and will result in the function returning NULL.
 *
 *-DESCRIPTION:
 *	ddi_cpu_alloc () is for use in the DDI/DKI implementation to request
 *	per-CPU data in a flexible way. Facilities that require tables of per-
 *	CPU data should request it at startup time to allow the table sizes to
 *	be flexibly administered and also to avoid polluting the "dcdata_t"
 *	structure with information whose size cannot easily be fixed. Using
 *	this facility should allow for greater levels of internal binary
 *	compatibility by restricting the amount of change the "dcdata_t"
 *	structure can undergo between (minor) releases of the operation
 *	system.
 *
 *-RETURN VALUE:
 *	On failure, NULL is returned. On success, a pointer to the requested
 *	data is returned. The pointer returned will be aligned to the size of
 *	an integer on the target machine architecture.
 *
 *-LEVEL:
 *	Base only.
 *
 *-NOTES:
 *	Does not sleep.
 */

#if	__USE_PROTO__
__VOID__ * (ddi_cpu_alloc) (size_t size)
#else
__VOID__ *
ddi_cpu_alloc __ARGS ((size))
size_t		size;
#endif
{
	dcdata_t      *	dcdatap = ddi_cpu_data ();
	__VOID__      *	alloc;

	ASSERT_BASE_LEVEL ();

	size = (size + sizeof (int) - 1) & ~ sizeof (int);

	if (size == 0 || dcdatap->dc_dynalloc + size > dcdatap->dc_dynend)
		return NULL;

	alloc = dcdatap->dc_dynalloc;
	dcdatap->dc_dynalloc += size;

        memset (alloc, 0, size);
	return alloc;
}


/*
 *-STATUS:
 *	For the Implementors only.
 *
 *-NAME:
 *	ddi_cpu_other		Return per-CPU data for other CPUs
 *
 *-SYNOPSIS:
 *	#include <kernel/ddi_cpu.h>
 *
 *	dcdata_t * ddi_cpu_ref (processorid_t cpu);
 *
 *-ARGUMENTS:
 *	cpu		ID code for processor whose data needs to be accessed.
 *
 *-DESCRIPTION:
 *	The implementations of DDI/DKI facilities may wish to write into the
 *	private data areas of other CPUs. This function provides a way of
 *	accessing that information.
 *
 *	Note that the per-CPU data area provides many conveniences to the
 *	implementor, not the least of which is latitude with synchronization,
 *	which has a data space cost if not a run-time cost, and also makes
 *	many operations considerably simpler. Access by other CPUs to per-CPU
 *	data bypasses most of the assumptions that can be conveniently made
 *	about access to this area, so users of this function are especially
 *	cautioned to use all the appropriate interlock mechanisms when writing
 *	data via the pointer this function provides.
 *
 *	There must be exactly one call to ddi_cpu_unref () for each call to
 *	ddi_cpu_ref (). This should be done as soon as possible after the
 *	call to ddi_cpu_ref () since on some architectures making this memory
 *	shared may have a serious impact on performance.
 *
 *-RETURN VALUE:
 *	NULL is returned if the cpu ID is not valid for the machine
 *	configuration. Otherwise, a pointer to the per-process data for the
 *	indicated CPU is returned.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	It is as yet uspecified whether recursive calls to ddi_cpu_ref () are
 *	permitted.
 */

#if	__USE_PROTO__
dcdata_t * (ddi_cpu_ref) (processorid_t cpu)
#else
dcdata_t *
ddi_cpu_ref __ARGS ((cpu))
processorid_t	cpu;
#endif
{
	/*
	 * Only one processor, as yet.
	 */

	return cpu == 0 ? ddi_cpu_data () : NULL;
}


/*
 *-STATUS:
 *	For the Implementors only.
 *
 *-NAME:
 *	ddi_cpu_other		Return per-CPU data for other CPUs
 *
 *-SYNOPSIS:
 *	#include <kernel/ddi_cpu.h>
 *
 *	void ddi_cpu_unref (dcdata_t * data);
 *
 *-ARGUMENTS:
 *	data		A pointer to per-CPU data obtained via the
 *			ddi_cpu_ref () function.
 *
 *-DESCRIPTION:
 *	This function is used to release a reference to per-CPU data for some
 *	CPU that was obtained by ddi_cpu_ref (). This function may perform
 *	various actions to undo any work necessary to ensure that the memory
 *	in question can be accessed by the calling CPU, such as unmapping it
 *	from the calling CPU's address space to provide maximum protection
 *	against accidental damage.
 *
 *	There must be exactly one call to ddi_cpu_unref () for each call to
 *	ddi_cpu_ref ().
 *
 *-RETURN VALUE:
 *	None.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 */

#if	__USE_PROTO__
void (ddi_cpu_unref) (dcdata_t * data)
#else
void
ddi_cpu_unref __ARGS ((data))
dcdata_t      *	data;
#endif
{
	ASSERT (data != NULL && data == ddi_cpu_data ());
}


/*
 * Do what we need to do to ensure that the spl... () functions work.
 */

#include <sys/inline.h>

__EXTERN_C__
#if	__USE_PROTO__
int (INTR_INIT) (void)
#else
int
INTR_INIT __ARGS (())
#endif
{
	dcdata_t      *	dcdatap;

	/*
	 * This implementation statically allocates the per-CPU data, since
	 * deep down we really "know" that we are working on a uniprocessor.
	 * This also means we don't have to zero-fill anything.
	 */

	dcdatap = ddi_cpu_data ();
	dcdatap->dc_base_mask = (inb (__SPICM__) << 8) | inb (__PICM__);


	/*
	 * We start at interrupt level 1 so we can trap attempts to go to
	 * sleep before we are ready for that.
	 */

	dcdatap->dc_int_level = 0;
	dcdatap->dc_ipl = plbase;


	/*
	 * Set up the "dynamic" part of the structures.
	 */

	dcdatap->dc_dynalloc = ddi_cpu_dynarea;
	dcdatap->dc_dynend = ddi_cpu_dynarea + sizeof (ddi_cpu_dynarea);


	/*
	 * Carve off some of the "dynamic" space for the heirarchy-test stuff.
	 * This is done with the dynamic stuff because it seems like a really
	 * bad idea putting a table in space we want to form the basis of an
	 * internal binary standard (even though this is a very well-defined
	 * array).
	 */

	dcdatap->dc_hierarchy_cnt = (__lkhier_t *)
		ddi_cpu_alloc (sizeof (__lkhier_t) *
			       (__MAX_HIERARCHY__ - __MIN_HIERARCHY__ + 1));

	return dcdatap->dc_hierarchy_cnt == NULL;
}


/*
 * Set up one of the deferred-function tables.
 */

#if	__USE_PROTO__
__LOCAL__ int (DEFER_INIT) (defer_t * deferp, int local, int max)
#else
__LOCAL__ int
DEFER_INIT __ARGS ((deferp, local, max))
defer_t	      *	deferp;
int		local;
int		max;
#endif
{
	deferp->df_tab = (__deffuncp_t *) (local == 0 ?
		kmem_alloc (sizeof (__deffuncp_t) * max, KM_NOSLEEP) :
		ddi_cpu_alloc (sizeof (__deffuncp_t) * max));

	deferp->df_wlock = LOCK_ALLOC (defer_hierarchy, defer_priority,
				       & defer_lkinfo, KM_NOSLEEP);

	if (deferp->df_tab == NULL || deferp->df_wlock == NULL)
		return 1;

	ATOMIC_STORE_UCHAR (deferp->df_max, max);
	return 0;
}


/*
 * Set up the global and per-CPU DDI/DKI data, locks and other whatnot.
 */

__EXTERN_C__
#if	__USE_PROTO__
int (DDI_GLOB_INIT) (void)
#else
int
DDI_GLOB_INIT __ARGS (())
#endif
{
	dgdata_t      *	dgdatap = ddi_global_data ();
	dcdata_t      *	dcdatap = ddi_cpu_data ();

	/*
	 * Set up the global data table. We assume here that our global data
	 * is zeroed by default.
	 */

	if (DEFER_INIT (& dgdatap->dg_defint, 0, 25) != 0 ||
	    DEFER_INIT (& dgdatap->dg_defproc, 0, 25) != 0 ||
	    DEFER_INIT (& dcdatap->dc_defint, 1, 25) != 0 ||
	    DEFER_INIT (& dcdatap->dc_defproc, 1, 25) != 0)
		return 1;

	dgdatap->dg_polllock = LOCK_ALLOC (poll_global_hierarchy,
					   poll_global_priority,
					   & poll_global_lkinfo, KM_NOSLEEP);

	dgdatap->dg_proclock = LOCK_ALLOC (proc_global_hierarchy,
					   proc_global_priority,
					   & proc_global_lkinfo, KM_NOSLEEP);

	if (dgdatap->dg_polllock == NULL || dgdatap->dg_proclock == NULL)
		return 1;

	return 0;
}
