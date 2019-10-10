#define	_DDI_DKI	1
#define	_DDI_DKI_IMPL	1
#define	_SYSV4		1

/*
 * This file contains functions relating to the implementation of DDI/DKI
 * timeout functions.
 */
/*
 *-IMPORTS:
 *	<common/ccompat.h>
 *		__USE_PROTO__
 *		__ARGS ()
 *	<kernel/ddi_glob.h>
 *		ddi_global_data ()
 *	<kernel/ddi_cpu.h>
 *		ddi_cpu_ref ()
 *		ddi_cpu_unref ()
 *		ddi_cpu_id ()
 *	<kernel/ddi_lock.h>
 *              timeout_global_hierarchy
 *		timeout_global_priority
 *	<kernel/defer.h>
 *		defer_int_cpu ()
 *		defer_int_here ()
 *	<sys/debug.h>
 *		ASSERT ()
 *	<sys/types.h>
 *		plhi
 *		pl_t
 *		toid_t
 *		__tfuncp_t
 *	<sys/inline.h>
 *		splx ()
 *	<sys/ksynch.h>
 *		LOCK_ALLOC ()
 *		LOCK_DEALLOC ()
 *		LOCK ()
 *		UNLOCK ()
 *	<sys/kmem.h>
 *		KM_NOSLEEP
 *		kmem_alloc ()
 *		kmem_free ()
 *	<sys/cmn_err.h>
 *		CE_PANIC
 *		cmn_err ()
 */

#include <common/ccompat.h>
#include <kernel/ddi_glob.h>
#include <kernel/ddi_cpu.h>
#include <kernel/ddi_lock.h>
#include <kernel/defer.h>
#include <sys/debug.h>
#include <sys/types.h>
#include <sys/inline.h>
#include <sys/ksynch.h>
#include <sys/kmem.h>
#include <sys/cmn_err.h>

/*
 * This code doesn't (yet) use the ddi_global_data () and ddi_cpu_data ()
 * mechanisms. It will need to in order to support the dtimeout () call to
 * bind a timeout to a specific CPU. However, this has been left pending
 * investigation of how we want to specify CPU ids...
 *
 * So, at some stage the TIMEOUT_GLOBAL_... (), SCHEDULE_TIMEOUT () and
 * STREAMS_TIMEOUT () functions may be parameterized.
 */


/*
 * All the global data this file uses in one handy place.
 *
 * How to do timeouts? I've always been a big fan of delta-queues.
 * How to manage untimeout ()?
 *
 * The cancellation procedure is seriously complicated by the need for cancel
 * attempts to block until completion if the timeout has begun. What we
 * implement here is similar to a ticket lock, although we use a single global
 * basic lock to guarantee write atomicity rather than a test-and-set lock.
 *	t_ticket	t_ticket_holder
 *	   0		     0			Cell pending activation.
 *	   1		     0			Cell activated.
 *	   1		     1			Cell processed.
 *	   m		     n			m != n, cancel attempts n + 1
 *						through m pending.
 *
 * Whoever finds "t_ticket" and "t_ticket_holder" equal (to their number, of
 * course) has responsibility for deallocating the lock structure. The timeout
 * activation code always has responsibility for dequeuing the lock once it has
 * been activated; this avoids having the pending cancels rescan the list to
 * dequeue the cell, and also ensures that untimeout () calls will simply
 * return rather than joining the queue if the timeout function has finished.
 *
 * Using a simple state code might seem simpler... but we have a special
 * problem here, in that we want to return the memory occupied by the event
 * cell (and its state code) to the heap. Using the ticket system, we get
 * acknowledgement from all the contexts that were interested in the state of
 * the cell that they are no longer interested in it.
 */

struct timeout {
	timeout_t     *	t_next;
	__clock_t	t_delta;
	toid_t		t_id;
	processorid_t	t_cpu;

	atomic_uchar_t	t_ticket;		/* next ticket */
	atomic_uchar_t	t_ticket_holder;	/* who holds the ticket */

	__tfuncp_t	t_funcp;		/* function to call */
	_VOID	      *	t_arg;			/* argument for function */
	lock_t	      *	t_lockp;		/* basic lock to acquire */
	pl_t		t_pl;			/* priority level for call */
};

__LOCAL__ lkinfo_t _timeout_lkinfo = {
	"timeout global lock", INTERNAL_LOCK
};


#define	TIMEDATA()	(& ddi_global_data ()->dg_timeouts)

#define	TIMEOUT_GLOBAL_LOCK()	\
		(ASSERT (TIMEDATA ()->td_lock != NULL), \
		 LOCK (TIMEDATA ()->td_lock, timeout_global_priority))

#define	TIMEOUT_GLOBAL_UNLOCK(p)	UNLOCK (TIMEDATA ()->td_lock, (p))



/*
 * This internal function is called whenever there are timeout routines
 * waiting to be processed by a particular CPU.
 */

#if	__USE_PROTO__
__LOCAL__ void (RUN_TIMEOUTS) (void)
#else
__LOCAL__ void
RUN_TIMEOUTS __ARGS (())
#endif
{
	pl_t		prev_pl;
	timeout_t     *	scan;
	timeout_t     *	next;
	timeout_t     *	free_list;
	processorid_t	my_id = ddi_cpu_id ();
	int		lock_failed = 0;

	prev_pl = TIMEOUT_GLOBAL_LOCK ();


	/*
	 * Run all the events that belong to this CPU.
	 */

	for (scan = TIMEDATA ()->td_run ; scan != TIMEDATA ()->td_first ;
	     scan = scan->t_next) {

		if (scan->t_cpu != my_id)
			continue;

		if (ATOMIC_FETCH_UCHAR (scan->t_ticket) > 0) {
			/*
			 * This entry is going to be deleted. Don't run it.
			 */

			continue;
		}


		/*
		 * Before we commit to running the timeout function, we need
		 * to check that the timeout function can acquire the basic
		 * lock that it needs.
		 */

		if (scan->t_lockp != NULL &&
		    TRYLOCK (scan->t_lockp, plhi) == invpl) {
			/*
			 * Try the next event cell, after we remember that an
			 * event cell failed to get a lock.
			 */

			lock_failed = 1;
			continue;
		}


		/*
		 * OK, stop other CPUs from processing this entry and make
		 * cancel requests block until we are finished.
		 */

		ATOMIC_STORE_UCHAR (scan->t_ticket, 1);

		TIMEOUT_GLOBAL_UNLOCK (scan->t_pl);


		/*
		 * Now execute the user function as requested. Note that we
		 * set the interrupt priority level to "scan->t_pl" even
		 * though we passed that value to UNLOCK (), since an
		 * implementation is permitted to ignore the "pl" argument to
		 * UNLOCK ().
		 *
		 * If we acquired a lock on behalf of the timeout function, we
		 * release it after the function runs.
		 */

		(void) splx (scan->t_pl);

		(* scan->t_funcp) (scan->t_arg);

		if (scan->t_lockp != NULL)
			UNLOCK (scan->t_lockp, prev_pl);

		(void) splx (prev_pl);


		/*
		 * Now we relock the list and mark this cell as finished. If
		 * a delete request for the cell has come in, let it proceed
		 * now that the function has been run.
		 */

		prev_pl = TIMEOUT_GLOBAL_LOCK ();

		ATOMIC_STORE_UCHAR (scan->t_ticket_holder, 1);
	}


	/*
	 * Run over the list of expired events and move any that have been
	 * completed to a work list for deletion.
	 */

	free_list = NULL;

	for (scan = TIMEDATA ()->td_run ; scan != TIMEDATA ()->td_first ;
	     scan = next) {

		/*
		 * If this event cell is in the throes of deletion, try again
		 * later. If there is an event which has not yet been run,
		 * don't clean up any more.
		 */

		if (ATOMIC_FETCH_UCHAR (scan->t_ticket_holder) !=
		    ATOMIC_FETCH_UCHAR (scan->t_ticket)) {

			lock_failed = 1;
			break;
		}

		if (ATOMIC_FETCH_UCHAR (scan->t_ticket) == 0)
			break;

		/*
		 * Move the cell to the work list.
		 */

		next = scan->t_next;
		scan->t_next = free_list;
		free_list = scan;
	}

	TIMEDATA ()->td_run = scan;


	/*
	 * If we have run (and freed) all the events that we can, clear the
	 * run flag, otherwise defer this function again by way of backing off
	 * from the locks we failed.
	 */

	if (lock_failed)
		defer_int_here (RUN_TIMEOUTS);
	else
		ATOMIC_STORE_UCHAR (ddi_cpu_data ()->dc_run_timeouts, 0);

	TIMEOUT_GLOBAL_UNLOCK (prev_pl);


	/*
	 * Free any timeout cells that we were able to reap.
	 */

	while ((scan = free_list) != NULL) {

		free_list = scan->t_next;

		kmem_free (scan, sizeof (* scan));
	}
}


/*
 * Code from STREAMS_TIMEOUT () to schedule a single timeout event, factored
 * out here to keep STREAMS_TIMEOUT () manageable.
 */

#if	__USE_PROTO__
__LOCAL__ __INLINE__ void SCHEDULE_TIMEOUT (timeout_t * timep)
#else
__LOCAL__ __INLINE__ void
SCHEDULE_TIMEOUT __ARGS ((timep))
timeout_t     *	timep;
#endif
{
	dcdata_t      *	dcdatap;

	if (timep->t_cpu == NOCPU)
		timep->t_cpu = ddi_cpu_id ();

	dcdatap = ddi_cpu_ref (timep->t_cpu);

	ASSERT (dcdatap != NULL);

	if (ATOMIC_FETCH_UCHAR (dcdatap->dc_run_timeouts) == 0) {
		/*
		 * Schedule the routine to actually run the timeouts.
		 */

		defer_int_cpu (RUN_TIMEOUTS, timep->t_cpu);
		ATOMIC_STORE_UCHAR (dcdatap->dc_run_timeouts, 1);
	}
}


/*
 * This internal function is called once for each clock tick of real time that
 * passes in the system overall. It may be that several instances get to run
 * simultaneously in different CPUs if things get bogged down, but the basic
 * idea is that the function gets called once per clock tick.
 */

__EXTERN_C__
#if	__USE_PROTO__
void (STREAMS_TIMEOUT) (void)
#else
void
STREAMS_TIMEOUT __ARGS (())
#endif
{
	pl_t		prev_pl;
	timeout_t     *	scan;

	prev_pl = TIMEOUT_GLOBAL_LOCK ();

	/*
	 * Now that we have the timeout queue locked, decrement the delta
	 * value of the entry at the front of the queue. After that, work out
	 * whether any entries have been triggered as a result.
	 */

	if ((scan = TIMEDATA ()->td_first) != NULL) {

		ASSERT (scan->t_delta > 0);

		if (-- scan->t_delta == 0) {

			if (TIMEDATA ()->td_run == NULL)
				TIMEDATA ()->td_run = scan;

			/*
			 * At least one entry has been set off. Loop over the
			 * entries, notifying each CPU that has an event bound
			 * to it. If an event is not bound to any CPU, bind it
			 * to the current CPU.
			 */

			do
				SCHEDULE_TIMEOUT (scan);
			while ((scan = scan->t_next) != NULL &&
			       scan->t_delta == 0);

			TIMEDATA ()->td_first = scan;
		}
	}

	TIMEOUT_GLOBAL_UNLOCK (prev_pl);
}


/*
 * This internal function factors out the common elements of timer event
 * scheduling from itimeout () and ltimeout ().
 */

#if	__USE_PROTO__
__LOCAL__ toid_t (QUEUE_TIMEOUT) (__tfuncp_t fn, _VOID * arg, __clock_t ticks,
				  lock_t * lockp, processorid_t cpu, pl_t pl)
#else
__LOCAL__ toid_t
QUEUE_TIMEOUT __ARGS ((fn, arg, ticks, lockp, cpu, pl))
__tfuncp_t	fn;
_VOID	      *	arg;
__clock_t	ticks;
lock_t	      *	lockp;
processorid_t	cpu;
pl_t		pl;
#endif
{
	pl_t		prev_pl;
	timeout_t     *	timep;
	timeout_t     *	scan;
	timeout_t     *	prev;

	if ((timep = (timeout_t *) kmem_alloc (sizeof (* timep),
					       KM_NOSLEEP)) == NULL)
		return 0;

	timep->t_funcp = fn;
	timep->t_arg = arg;
	timep->t_pl = pl;
	timep->t_lockp = lockp;
	timep->t_cpu = cpu;
	ATOMIC_STORE_UCHAR (timep->t_ticket, 0);
	ATOMIC_STORE_UCHAR (timep->t_ticket_holder, 0);

	if (ticks == 0)
		ticks = 1;

	/*
	 * Having created and mostly filled in the timeout structure, we now
	 * lock the delta-queue and try and find the place where our structure
	 * needs to be inserted.
	 *
	 * We also use the delta-queue lock to protect our access to the
	 * timeout ID generator. Since it is in theory possible to wrap around
	 * the ID space, the delta-queue walk will look for a duplicate ID.
	 * We make the simplifying assumption that anything that has lived
	 * that long isn't going to be cancelled anytime soon, so that it
	 * suffices to protect the newer entry from having it's cancel code
	 * shadowed.
	 */

	prev_pl = TIMEOUT_GLOBAL_LOCK ();

id_scan:
	if (TIMEDATA ()->td_id == 0)
		TIMEDATA ()->td_id = 1;

	timep->t_id = TIMEDATA ()->td_id ++;

	/*
	 * We have to deal with the fact that the true first element in the
	 * timeout delta-queue may be represented by either of two pointers.
	 * Since timeouts that have been scheduled to run have a zero
	 * "t_delta", searching those entries won't disrupt the delta-queue,
	 * and until they have been dequeued their timeout id's are still
	 * valid.
	 */

	if ((scan = TIMEDATA ()->td_run) == NULL)
		scan = TIMEDATA ()->td_first;

	for (prev = NULL ; scan != NULL ; scan = (prev = scan)->t_next) {

		if (scan->t_id == timep->t_id)
			goto id_scan;

		if (ticks < scan->t_delta) {
			/*
			 * OK, we have found our spot. Now we insert our new
			 * cell in front of the one we have just found.
			 */

			scan->t_delta -= ticks;
			break;
		}

		ticks -= scan->t_delta;
	}

	timep->t_delta = ticks;
	timep->t_next = scan;

	if (scan == TIMEDATA ()->td_first)
		TIMEDATA ()->td_first = timep;

	if (prev != NULL)
		prev->t_next = timep;

	TIMEOUT_GLOBAL_UNLOCK (prev_pl);

	return timep->t_id;
}


/*
 * This internal function is a direct equivalent to itimeout (), with the
 * following additional behaviour; the caller supplies the address of a basic
 * lock which this code will attempt to acquire before running the timeout
 * function. If the lock cannot be acquired immediately, the next timeout
 * function will be considered.
 *
 * This behaviour allows untimeout () to be safely used to cancel the timeout
 * request while holding the basic lock that this function will attempt to
 * acquire.
 *
 * A "timeout" value of NULL yields identical behaviour to itimeout ().
 */

#if	__USE_PROTO__
toid_t (ltimeout) (__tfuncp_t fn, _VOID * arg, __clock_t ticks,
		   lock_t * lockp, pl_t pl)
#else
toid_t
ltimeout __ARGS ((fn, arg, ticks, lockp, pl))
__tfuncp_t	fn;
_VOID	      *	arg;
__clock_t	ticks;
lock_t	      *	lockp;
pl_t		pl;
#endif
{
	ASSERT (fn != (__tfuncp_t) NULL);
	ASSERT (lockp != NULL);

	return QUEUE_TIMEOUT (fn, arg, ticks, lockp, NOCPU, pl);
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	itimeout	Execute a function after a specified length of time.
 *
 *-SYNOPSIS:
 *	#include <sys/types.h>
 *
 *	toid_t itimeout (void (* fn) (), void * arg, long ticks, pl_t pl);
 *
 *-ARGUMENTS:
 *	fn		Function to execute when the time interval expires.
 *
 *	arg		Argument to the function.
 *
 *	ticks		Number of clock ticks to wait before the function is
 *			called.
 *
 *	pl		The interrupt priority level at which the function
 *			will be called. "pl" must specify a priority level
 *			greater than or equal to "pltimeout"; thus, "plbase"
 *			cannot not be used. See LOCK_ALLOC () for a list of
 *			values for "pl".
 *
 *-DESCRIPTION:
 *	itimeout () causes the function specified by "fn" to be called after
 *	the time interval specified by "ticks", at the interrupt priority
 *	specified by "pl". "arg" will be passed as the only argument to
 *	function "fn". The itimeout () call returns immediately without
 *	waiting for the specified function to execute.
 *
 *	The length of time before the function is called is not guaranteed to
 *	be exactly equal to the requested time, but will be at least "ticks-1"
 *	clock ticks in length. The function specified by "fn" must neither
 *	sleep nor reference process context.
 *
 *-RETURN VALUE:
 *	If the function specfied by "fn" is successfully scheduled,
 *	itimeout () returns a non-zero identifier that can be passed to
 *	untimeout () to cancel the request. If the function could not be
 *	scheduled, itimeout () returns a value of 0.
 *
 *-LEVEL:
 *	Base or Interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *	Drivers should be careful to cancel any pending itimeout () functions
 *	that access data structures before these structures are de-initialized
 *	or deallocated.
 *
 *	After the time interval has expired, "fn" only runs if the processor
 *	is at base level. Otherwise, "fn" is deferred until sometime in the
 *	near future.
 *
 *	If itimeout () is called holding a lock that is contended for by "fn",
 *	the caller must hold the lock at a processor level greater than the
 *	base processor level.
 *
 *	A "ticks" argument of 0 has the same effect as a "ticks" argument of
 *	1. Both will result in an approximate wait of between 0 and 1 tick
 *	(possibly longer).
 *
 *-SEE ALSO:
 *	LOCK_ALLOC (), untimeout ()
 */

#if	__USE_PROTO__
toid_t (itimeout) (__tfuncp_t fn, _VOID * arg, __clock_t ticks, pl_t pl)
#else
toid_t
itimeout __ARGS ((fn, arg, ticks, pl))
__tfuncp_t    *	fn;
_VOID	      *	arg;
__clock_t	ticks;
pl_t		pl;
#endif
{
	ASSERT (fn != (__tfuncp_t) NULL);

	return QUEUE_TIMEOUT (fn, arg, ticks, NULL, NOCPU, pl);
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	untimeout	Cancel previous timeout request.
 *
 *-SYNOPSIS:
 *	#include <sys/types.h>
 *
 *	void untimeout (toid_t id);
 *
 *-ARGUMENTS:
 *	id		Identifier returned from a previous call to
 *			itimeout ().
 *
 *-DESCRIPTION:
 *	untimeout () cancels a previous timeout request. If the untimeout ()
 *	is called while the function is running, then untimeout () will not
 *	return until the function has completed. The function that runs as a
 *	result of itimeout () cannot use untimeout () to cancel itself.
 *
 *-RETURN VALUE:
 *	None.
 *
 *-LEVEL:
 *	Base or interrupt, with the following exception; the untimeout () can
 *	only be performed from interrupt levels less than, or equal to, the
 *	level specified when the function was scheduled.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may not
 *	be held across calls to this function if these locks are contended by
 *	the function being scheduled.
 *
 *-SEE ALSO:
 *	delay (), itimeout (), unbufcall ()
 */

#if	__USE_PROTO__
void (untimeout) (toid_t id)
#else
void
untimeout __ARGS ((id))
toid_t		id;
#endif
{
	pl_t		prev_pl;
	timeout_t     *	scan;

	prev_pl = TIMEOUT_GLOBAL_LOCK ();

	for (scan = TIMEDATA ()->td_first ; scan != NULL ;
	     scan = scan->t_next) {
		unsigned short	ticket;

		if (scan->t_id != id)
			continue;

		/*
		 * Take a ticket, and wait for the cell's event routine to
		 * finish. If it was not running before we took a ticket, it
		 * never will.
		 */

		ticket = ATOMIC_FETCH_UCHAR (scan->t_ticket);
		ATOMIC_STORE_UCHAR (scan->t_ticket, ticket + 1);

		scan->t_cpu = ddi_cpu_id ();

		if (ticket != ATOMIC_FETCH_UCHAR (scan->t_ticket_holder)) {
			/*
			 * Wait for the event routine to complete.
			 */

			TIMEOUT_GLOBAL_UNLOCK (prev_pl);

			while (ATOMIC_FETCH_UCHAR (scan->t_ticket_holder)
			       != ticket)
				; /* DO NOTHING */


			prev_pl = TIMEOUT_GLOBAL_LOCK ();
		}


		/*
		 * Pass it on to the next guy.
		 */

		ATOMIC_STORE_UCHAR (scan->t_ticket_holder, ticket + 1);
	}

	TIMEOUT_GLOBAL_UNLOCK (prev_pl);
}


/*
 * Set up the timeout globals. This function is called via the 'mdevice' init
 * function table.
 */

__EXTERN_C__
#if	__USE_PROTO__
int (timeout_init) (void)
#else
int
timeout_init __ARGS (())
#endif
{
	TIMEDATA ()->td_lock = LOCK_ALLOC (timeout_global_hierarchy,
					   timeout_global_priority,
					   & _timeout_lkinfo, KM_SLEEP);

	return TIMEDATA ()->td_lock == NULL;
}
