#define	_DDI_DKI	1
#define	_SYSV4		1

/*
 * This file contains the fundamental STREAMS routines defined in Appendix
 * C of the STREAMS Programmer's Guide for System V Release 4. Note that
 * additional information about the definition of these routines has been
 * taken from the System V Release 4 Multi-Processor DDI/DKI For Intel CPU's.
 *
 * Later documents significantly redefine the semantics of many STREAMS
 * routines. If there is a conflict, the normal System V Release 4 version
 * is preferred, and the conflict is noted. However, where possible the later
 * semantics have been incorporated or anticipated.
 */

/*
 *-IMPORTS:
 *	<common/ccompat.h>
 *		__USE_PROTO__
 *		__ARGS ()
 *	<kernel/defer.h>
 *		defer_int_any ()
 *	<sys/debug.h>
 *		ASSERT ()
 *	<sys/types.h>
 *		uchar_t
 *		uint_t
 *		ushort_t
 *		toid_t
 *	<sys/kmem.h>
 *		KM_NOSLEEP
 *		kmem_alloc ()
 *		kmem_free ()
 *	<sys/cmn_err.h>
 *		CE_WARN
 *		cmn_err ()
 *	<sys/strlog.h>
 *		SL_ERROR
 *		SL_TRACE
 *		SL_CONSOLE
 *		SL_NOTIFY
 *		SL_FATAL
 *		SL_WARN
 *		SL_NOTE
 *		SL_LEVEL_MASK
 *		NLOGARGS
 *		struct log_ctl
 *	<sys/errno.h>
 *		EINVAL
 *		ENOMEM
 *		EPERM
 *	<stdarg.h>
 *		va_start ()
 *		va_arg ()
 *		va_end ()
 *	<string.h>
 *		memcpy ()
 */

#include <common/ccompat.h>
#include <kernel/defer.h>
#include <sys/debug.h>
#include <sys/types.h>
#include <sys/kmem.h>
#include <sys/cmn_err.h>
#include <sys/strlog.h>
#include <sys/errno.h>
#include <stdarg.h>
#include <string.h>

#include <sys/stream.h>
#include <kernel/strmlib.h>


/*
 * The pre-SVR4 STREAMS system used a marker bit to determine when putq () and
 * putbq () should enable a queue. This flag, documented as QWANTR in the SVR4
 * STREAMS documentation, is no longer sufficient to deal with scheduling with
 * priority bands. The DDI/DKI and STREAMS documentation in SVR4 do not
 * contain detailed new rules for dealing with exactly when a queue should be
 * enabled automatically.
 *
 * The rules that we have adopted to deal with band flow control use our own
 * member of the queue structure, "q_lastband". The idea is for getq () and
 * rmvq () to record the band of the last message that was dequeued (or to
 * set QWANTR), so that when a message in a higher band is queued the stream
 * will be enabled to process it. If the last message retrieved is returned
 * to the queue because of a flow-control blockage, putq () will see that the
 * message is not a member of a higher band that the last read, and thus will
 * know not to enable the queue.
 */


/*
 * Simple helper macro: is this message a priority message?
 */

#define	IS_PRI_MSG(mp)		pcmsg ((mp)->b_datap->db_type)


/*
 * Simple realloc () function for memory allocated with kmem_alloc ()
 * (assuming that it is our kmem_alloc (), that is).
 */

#if	__USE_PROTO__
__LOCAL__ _VOID * (kmem_realloc) (_VOID * mem, size_t newsize, size_t oldsize)
#else
__LOCAL__ _VOID *
kmem_realloc __ARGS ((mem, newsize, oldsize))
_VOID	      *	mem;
size_t		newsize;
size_t		oldsize;
#endif
{
	pl_t		prev_pl;

	ASSERT (mem != NULL && newsize > 0 && oldsize > 0);

	prev_pl = LOCK (str_mem->sm_other_lock, str_other_pl);

	mem = st_realloc (str_mem->sm_other_heap, mem, newsize, oldsize);

	OTHER_ALLOCED (newsize - oldsize);

	UNLOCK (str_mem->sm_other_lock, prev_pl);

	return mem;
}


/*
 * This internal function actually implements the queue freezing; in addition,
 * it asserts that the queue is non-NULL, traces who froze the queue, and
 * so forth. If a queue monitoring facility is introduced, this would be the
 * place to do it.
 */

#if	__USE_PROTO__
pl_t (QFREEZE_TRACE) (queue_t * q, __CONST__ char * name)
#else
pl_t
QFREEZE_TRACE __ARGS ((q, name))
queue_t	      *	q;
__CONST__ char * name;
#endif
{
	if (q == NULL)
		cmn_err (CE_PANIC, "NULL queue passed to STREAMS utility %s",
			 name);

	return SFREEZE_LOCK (q, name);
}


/*
 * This internal function is similar to QFREEZE_TRACE (), but performs the
 * assertions and tracing functions for routines that require that the queue
 * already be frozen on entry.
 */

#if	__USE_PROTO__
void (QFROZEN_TRACE) (queue_t * q, __CONST__ char * name)
#else
void
QFROZEN_TRACE __ARGS ((q, name))
queue_t	      *	q;
__CONST__ char * name;
#endif
{
	if (q == NULL)
		cmn_err (CE_PANIC, "NULL queue passed to STREAMS utility %s",
			 name);

	SFREEZE_ASSERT_FROZEN (q);
}


/*
 * This internal function is for utility routines that walk over various
 * queues (and hence perform multiple freeze/unfreeze operations in local
 * procedures such as QUEUE_NEXT).
 */

#if	__USE_PROTO__
void (QUEUE_TRACE) (queue_t * q, __CONST__ char * name)
#else
void
QUEUE_TRACE __ARGS ((q, name))
queue_t	      *	q;
__CONST__ char * name;
#endif
{
	if (q == NULL)
		cmn_err (CE_PANIC, "NULL queue passed to STREAMS utility %s",
			 name);
}


/*
 * This internal function follows the singly-threaded lists of priority band
 * structures to find the "pri"th priority band attached to the queue. It
 * returns NULL if no such band has been allocated.
 *
 * The queue passed to this function must be frozen.
 */

#if	! defined (VECTOR_BANDS) || defined (VECTOR_BANDS_TEST)

#if	__USE_PROTO__
qband_t * (QUEUE_BAND) (queue_t * q, uchar_t pri)
#else
qband_t *
QUEUE_BAND __ARGS ((q, pri))
queue_t	      *	q;
uchar_t		pri;
#endif
{
	qband_t	      *	qbandp;
	uchar_t		count = pri;


	ASSERT (pri > 0);
	QFROZEN_TRACE (q, "QUEUE_BAND");

	/*
	 * Just iterate through the list of band structures.
	 */

	if ((qbandp = q->q_bandp) != NULL)
		while (-- count > 0)
			if ((qbandp = qbandp->qb_next) == NULL)
				break;

	ASSERT (pri > q->q_nband ? qbandp == NULL : qbandp != NULL);

#ifdef	VECTOR_BANDS_TEST
	/*
	 * For extra fun, we define a mode where we work everything out both
	 * ways to ensure correctness. The #undef makes all the later uses
	 * of the QUEUE_BAND name map to this function version so that the
	 * test gets included.
	 */

	ASSERT (qbandp == QUEUE_BAND (q, pri));
# undef QUEUE_BAND
#endif

	return qbandp;
}


/*
 * This internal function gets the predecessor to a band structure from a
 * queue, or NULL if there is no such predecessor.
 *
 * The definition of the "qband" structure was public from its introduction in
 * SVR4, but was made private in the SVR4 MP release. We could reasonably
 * change the definition of this structure to include a back-link, but for
 * now we'll do it the hard way and stay with the structure as defined.
 *
 * The queue passed to this function must be frozen.
 */

#if	__USE_PROTO__
qband_t * (QBAND_PREV) (queue_t * q, qband_t * qbandp)
#else
qband_t *
QBAND_PREV __ARGS ((q, qbandp))
queue_t	      *	q;
qband_t	      *	qbandp;
#endif
{
	qband_t	      *	scan;

	ASSERT (qbandp != NULL);
	QFROZEN_TRACE (q, "QBAND_PREV");

	if ((scan = q->q_bandp) == qbandp)
		return NULL;

	while (scan->qb_next != qbandp) {
		scan = scan->qb_next;

		ASSERT (scan != NULL);
	}

#ifdef VECTOR_BANDS_TEST
	ASSERT (scan == QBAND_PREV (q, qbandp));

# undef	QBAND_PREV
#endif

	return scan;
}

#endif	/* ! defined (VECTOR_BANDS) || defined (VECTOR_BANDS_TEST) */


/*
 * This local function attempts to allocate new storage for a priority-band
 * structure associated with a queue. Since for a given band "n", band
 * structures 1 through "n-1" must also be present, this function may attempt
 * to acquire several structures simultaneously.
 *
 * The caller must have the queue frozen.
 */

#if	__USE_PROTO__
__LOCAL__ qband_t * (QBAND_ALLOC) (queue_t * q, uchar_t pri)
#else
__LOCAL__ qband_t *
QBAND_ALLOC __ARGS ((q, pri))
queue_t	      *	q;
uchar_t		pri;
#endif
{
	qband_t	      *	newband;
	int		nbands;

	QFROZEN_TRACE (q, "QBAND_ALLOC");
	ASSERT (pri > 0);
	ASSERT (pri > q->q_nband);

	/*
	 * We want to allocate band structures "q->q_nband + 1" ... "pri".
	 *
	 * Since the caller has the stream frozen, we want to do this fairly
	 * quickly, so we allocate the new structures as a vector. In order
	 * to speed up other operations, we may also try to reallocate any
	 * existing information so that the entire set of band structures is
	 * kept in a single block of memory.
	 */

#ifdef VECTOR_BANDS
	if (q->q_bandp != NULL) {
		/*
		 * There are some existing band structures to be moved around
		 * (note that this will require that all the "qb_next"
		 * pointers be rethreaded).
		 */

		if ((newband = (qband_t *)
			kmem_realloc (q->q_bandp, pri * sizeof (qband_t),
				      q->q_nband * sizeof (qband_t))) == NULL)
			return NULL;

		q->q_bandp = newband;

		for (nbands = 0 ; nbands < q->q_nband ; newband ++, nbands ++)
			newband->qb_next = newband + 1;

		/*
		 * Now we have rethreaded the relocated version of the old
		 * band structures, "newband" is set correctly to initialize
		 * the new band structures.
		 *
		 * We zero the remaining memory to ease initialization.
		 */

		memset (newband, 0, (pri - q->q_nband) * sizeof (qband_t));
	} else
#endif

	if ((newband = (qband_t *)
		kmem_zalloc ((pri - q->q_nband) * sizeof (qband_t),
			     KM_NOSLEEP)) == NULL)
		return NULL;

#ifndef VECTOR_BANDS
	newband->qb_flag = QB_FIRST;

	/*
	 * Special-case update the "qb_next" pointer of the previously highest
	 * band.
	 */

	if (q->q_nbands > 0)
		QUEUE_BAND (q, q->q_nbands)->qb_next = newband;
#endif

	/*
	 * Thread the "qb_next" pointers of the new structures together.
	 */

	for (nbands = q->q_nband ; nbands < pri - 1 ; newband ++, nbands ++)
		newband->qb_next = newband + 1;

	return newband;
}


/*
 * This internal function performs an atomic read of the "q_next" member of
 * the queue passed to it, skipping over queues that are hidden from the
 * STREAMS system.
 *
 * The queue passed to this function must be frozen. After this function, the
 * passed queue is unfrozen and the returned queue is frozen (unless it is
 * NULL).
 *
 * Note that this function illustrates a problem with using a fixed-hierarchy
 * basic lock scheme; this code can't deadlock with itself because of the
 * relationship expressed by the "q_next" member. Locks are only acquired in
 * one direction along a stream axis, so each queue lock has an implicit
 * hierarchy value lower that its successor.
 */

#if	__USE_PROTO__
queue_t * (QUEUE_NEXT) (queue_t * q)
#else
queue_t *
QUEUE_NEXT __ARGS ((q))
queue_t	      *	q;
#endif
{
	QFROZEN_TRACE (q, "QUEUE_NEXT");

	do {
		queue_t	      *	next;

		if ((next = q->q_next) != NULL)
			(void) QFREEZE_TRACE (next, "QUEUE_NEXT");

		QUNFREEZE_TRACE (q, plstr);

		q = next;
	} while (q != NULL && (q->q_flag & QPROCSOFF) != 0);

	return q;
}


/*
 * This internal function attempts to schedule a queue. It will fail if the
 * queue has no service procedure or if the service procedure has been
 * disabled. It will do nothing, but return success, if the queue is currently
 * scheduled for service.
 */

#if	__USE_PROTO__
__LOCAL__ int (QUEUE_TRYSCHED) (queue_t * q)
#else
__LOCAL__ int
QUEUE_TRYSCHED __ARGS ((q))
queue_t	      *	q;
#endif
{
	QFROZEN_TRACE (q, "QUEUE_TRYSCHED");

	if (q->q_qinfo->qi_srvp == NULL || (q->q_flag & QPROCSOFF) != 0)
		return 0;

	if ((q->q_flag & QENAB) != 0 ||
	    QSCHED_SCHEDULE (q, str_mem->sm_sched) != 0)
		return 1;

	/*
	 * Mark the queue as having been scheduled, and if the STREAMS service
	 * procedure scheduler has not been deferred, do so.
	 */

	q->q_flag |= QENAB;

	if (ATOMIC_FETCH_AND_STORE_UCHAR (ddi_global_data ()->dg_run_strsched,
					  1) == 0)
		defer_int_any (RUN_STREAMS);

	return 1;
}


/*
 * QUEUE_BACKENAB () is an internal function used to unblock a queue which
 * has been blocked by flow control. When a queue with a service procedure
 * becomes full, and a queue "behind" it finds that it cannot put into it
 * with bcanput (), bcanputnext (), canput (), or canputnext (), the target
 * queue is marked so that this procedure will be called when it is ready to
 * receive more data.
 *
 * Since the "behind" queue should have a service procedure (otherwise, the
 * flow-control tests would have no point), we scan the queues behind the
 * passed queue "q" and enable the first one which has a service procedure.
 *
 * The queue passed to this function must *not* be frozen. To avoid deadlock,
 * a single function cannot hold both sides of a queue frozen; this is the
 * only function in the STREAMS library that might plausibly want to do so.
 *
 * This function is normally called as a result of action in QBAND_REDUCE ()
 * or QUEUE_REDUCE (), and under normal circumstances this would not cause an
 * inconvenience. However, the action of rvmq () means that we use a condition
 * in the "q_flags" member of the queue to indirectly schedule this function
 * at a safe time. We only need to test the flag in a path that includes one
 * of the above named functions or in user calls to unfreezestr ().
 */

#if	__USE_PROTO__
void (QUEUE_BACKENAB) (queue_t * q)
#else
void
QUEUE_BACKENAB __ARGS ((q))
queue_t	      *	q;
#endif
{
	int		done;
	queue_t	      *	other;
	pl_t		prev_pl;

	ASSERT (q != NULL);

	/*
	 * To walk backwards along the queue list, we must first move to the
	 * other kind of queue and walk along that, which effectively moves us
	 * in the reverse direction with respect to the original queue type.
	 */

	other = OTHERQ (q);

	prev_pl = QFREEZE_TRACE (other, "QUEUE_BACKENAB");

	do {
		/*
		 * Atomically read the "q_next" member of the queue.
		 */

		if ((other = QUEUE_NEXT (other)) == NULL) {
			/*
			 * Cannot search further.
			 */

			(void) splx (prev_pl);
			break;
		}


		/*
		 * To avoid deadlock, we have to unfreeze "other" before we
		 * look at freezing its partner. We ensure that it won't go
		 * away on us after we unfreeze it by incrementing the
		 * "active" count.
		 */

		other->q_active ++;

		QUNFREEZE_TRACE (other, prev_pl);


		/*
		 * We might be attempting to schedule an empty queue, which
		 * we should allow.
		 */

		q = OTHERQ (other);

		prev_pl = QFREEZE_TRACE (q, "QUEUE_BACKENAB");

		done = QUEUE_TRYSCHED (q);

		QUNFREEZE_TRACE (q, prev_pl);


		/*
		 * Now we go back and remove our reference to the partner
		 * queue after relocking it.
		 */

		prev_pl = QFREEZE_TRACE (other, "QUEUE_BACKENAB");

		ASSERT (other->q_active > 0);

		other->q_active --;

		if ((q->q_flag & QPROCSOFF) != 0 && q->q_active == 0) {
			/*
			 * Time to wake up the sleepers waiting for the put
			 * and service routines on a queue to exit. See
			 * qprocsoff () for a discussion of the locking
			 * protocol.
			 */

			(void) LOCK (str_mem->sm_proc_lock, plstr);
			SV_BROADCAST (str_mem->sm_proc_sv, 0);
			UNLOCK (str_mem->sm_proc_lock, plstr);
		}
	} while (! done);

	QUNFREEZE_TRACE (other, prev_pl);
}


/*
 * This local function is called whenever a queue's last message has been
 * dequeued, and will wake up any function waiting for the queue to drain.
 *
 * The caller must have the stream frozen.
 */

#if	__USE_PROTO__
__LOCAL__ void (QUEUE_DRAINED) (queue_t * q)
#else
__LOCAL__ void
QUEUE_DRAINED __ARGS ((q))
queue_t	      *	q;
#endif
{
	pl_t		prev_pl;

	QFROZEN_TRACE (q, "QUEUE_DRAINED");

	if ((q->q_flag & QDRAIN) != 0) {
		shead_t	      *	sheadp;

		/*
		 * Under normal circumstances, a call to SV_BROADCAST () would
		 * be all we needed to do. However, since we allow ourself to
		 * use a more primitive kind of lock that a basic lock to
		 * implement freezing a queue, the process that waits for us
		 * to drain cannot use the frozen queue as the lock to pass to
		 * SV_WAIT (). By making us wait for the lock, we make
		 * the wakeup multiprocessor-safe; otherwise, we might be able
		 * to try and wake the process before it has actually slept,
		 * causing infinite sleep.
		 *
		 * Since the synchronization variable we are going to
		 * broadcast to is part of the stream head, we ensure that
		 * this code is only executed for the module or driver which
		 * is immediately below the stream head, or the stream head
		 * write queue itself.
		 */

		ASSERT (RD (q)->q_next == NULL ||
			RD (q)->q_next->q_next == NULL);

		sheadp = (shead_t *)
			(RD (q)->q_next == NULL ? q->q_ptr :
				RD (q)->q_next->q_ptr);

		prev_pl = SHEAD_LOCK (sheadp);

		SV_BROADCAST (sheadp->sh_wait_sv, 0);

		q->q_flag &= ~ QDRAIN;

		SHEAD_UNLOCK (sheadp, prev_pl);
	}
}


/*
 * This local function updates the flow-control information for a priority
 * band when the band current level is increasing.
 *
 * The queue passed to this function must be frozen.
 */

#if	__USE_PROTO__
__LOCAL__ void (QBAND_INCREASE) (queue_t * q, qband_t * qbandp, ulong_t adjust)
#else
__LOCAL__ void
QBAND_INCREASE __ARGS ((q, qbandp, adjust))
queue_t	      *	q;
qband_t	      *	qbandp;
ulong_t		adjust;
#endif
{
	ASSERT (qbandp != NULL);
	QFROZEN_TRACE (q, "QBAND_INCREASE");

	/*
	 * If the band we are dealing with is already flow-controlled, just
	 * get out of here.
	 */

	qbandp->qb_count += adjust;

	if ((qbandp->qb_flag & QB_FULL) != 0)
		return;

	/*
	 * If we are over the high-water mark, then we must mark ourself and
	 * all the bands below us (including the normal messages) as full as
	 * well.
	 */

	if (qbandp->qb_count >= qbandp->qb_hiwat) {

		do
			qbandp->qb_flag |= QB_FULL;
		while ((qbandp = QBAND_PREV (q, qbandp)) != NULL);


		/*
		 * According to the way I read the STREAMS
		 * programmer's guide, band flow control
		 * should affect normal messages as well (ie,
		 * as if they are band 0).
		 *
		 * We deal with this as a special case.
		 */

		if (q->q_count >= q->q_hiwat)
			q->q_flag |= QFULL;
	}
}


/*
 * This local function updates the flow-control information for a priority
 * band when the band current level is decreasing.
 *
 * The queue passed to this function must be frozen.
 */

#if	__USE_PROTO__
__LOCAL__ void (QBAND_REDUCE) (queue_t * q, qband_t * qbandp, ulong_t adjust)
#else
__LOCAL__ void
QBAND_REDUCE __ARGS ((q, qbandp, adjust))
queue_t	      *	q;
qband_t	      *	qbandp;
ulong_t		adjust;
#endif
{

	ASSERT (qbandp != NULL);
	ASSERT (qbandp->qb_count >= adjust);
	QFROZEN_TRACE (q, "QBAND_REDUCE");

	/*
	 * If this band wasn't flow-controlled anyway, then we don't need to
	 * do anything other than adjust the count.
	 */

	qbandp->qb_count -= adjust;

	if ((qbandp->qb_flag & QB_FULL) == 0)
		return;


	/*
	 * If we are under the low-water mark, then we can release any pending
	 * writes to this band and our hold on lower bands if and only if the
	 * bands above us are not controlled.
	 *
	 * We can test the bands above us in one step since we require that a
	 * band which has become full also mark all lower bands as full. If
	 * our immediate upper neighbour is full then we cannot proceed.
	 */

	if (qbandp->qb_count <= qbandp->qb_lowat &&
	    (qbandp->qb_next == NULL ||
			(qbandp->qb_next->qb_flag & QB_FULL) == 0)) {

		/*
		 * We release lower bands as long as they are under their low-
		 * water marks. It seems pointless to resume bands that are
		 * too close to entering a controlled state anyway.
		 */

		do {
			qbandp->qb_flag &= ~ QB_FULL;

			if ((qbandp->qb_flag & QB_WANTW) != 0) {
				/*
				 * Propagate a back-enable request to the
				 * queue.
				 */

				qbandp->qb_flag &= ~ QB_WANTW;
				q->q_flag |= QBACK;
			}

			if ((qbandp = QBAND_PREV (q, qbandp)) == NULL) {
				/*
				 * According to the way I read the STREAMS
				 * programmer's guide, band flow control
				 * should affect normal messages as well (ie,
				 * as if they are band 0).
				 *
				 * We deal with this as a special case.
				 */

				if (q->q_count <= q->q_lowat) {
					q->q_flag &= ~ QFULL;

					if ((q->q_flag & QWANTW) != 0) {

						q->q_flag &= ~ QWANTW;
						q->q_flag |= QBACK;
					}
				}

				break;
			}
		} while (qbandp->qb_count <= qbandp->qb_lowat);
	}
}


/*
 * This local function factors out the computation of the total number of
 * data bytes in a message from the routines that manipulate flow-control
 * parameters. Since this is often the only loop in such routines, factoring
 * this out might help inlining in some cases.
 */

#if	__USE_PROTO__
__LOCAL__ ulong_t (MSG_SIZE) (mblk_t * mp)
#else
__LOCAL__ ulong_t
MSG_SIZE __ARGS ((mp))
mblk_t	      *	mp;
#endif
{
	ulong_t		total = 0;

	ASSERT (mp != NULL);

	do
		total += mp->b_wptr - mp->b_rptr;
	while ((mp = mp->b_cont) != NULL);

	return total;
}


/*
 * This internal function wraps up the check for whether a newly queued
 * message is of sufficient priority to cause the queue to be enabled.
 *
 * The queue passed to this function must be frozen.
 */

#if	__USE_PROTO__
__LOCAL__ int (QUEUE_CHECK_SCHED) (queue_t * q, mblk_t * mp,
				    qband_t * qbandp)
#else
__LOCAL__ int
QUEUE_CHECK_SCHED __ARGS ((q, mp, qbandp))
queue_t	      *	q;
mblk_t	      *	mp;
qband_t	      *	qbandp;
#endif
{
	ulong_t		msgsize;

	QFROZEN_TRACE (q, "QUEUE_CHECK_SCHED");

	if ((q->q_flag & QNOENB) == 0 &&
	    ((q->q_flag & QWANTR) != 0 || q->q_lastband < mp->b_band)) {
		/*
		 * The new message is part of a higher-priority band than the
		 * last message that was retrieved, so we need to enable the
		 * queue.
		 */

		q->q_lastband = mp->b_band;
		(void) QUEUE_TRYSCHED (q);
	}


	/*
	 * As a convenience to the callers, all of whom are queueing the new
	 * message, update the flow control parameters.
	 */

	msgsize = MSG_SIZE (mp);

	if (mp->b_band > 0) {

		ASSERT (qbandp != NULL);

		QBAND_INCREASE (q, qbandp, msgsize);

		return 1;
	} else {
		/*
		 * Just deal with the base flow-control stuff.
		 */

		if ((q->q_count += msgsize) > q->q_hiwat)
			q->q_flag |= QFULL;

		return 0;
	}
}


/*
 * This internal function collects the details of dequeuing a message from
 * a priority-band structure.
 *
 * The queue on which "mp" was queued must be frozen.
 */

#if	__USE_PROTO__
__LOCAL__ void (QBAND_DEQUEUE) (qband_t * qbandp, mblk_t * mp)
#else
__LOCAL__ void
QBAND_DEQUEUE __ARGS ((qbandp, mp))
qband_t	      *	qbandp;
mblk_t	      *	mp;
#endif
{
	ASSERT (qbandp != NULL);
	ASSERT (mp != NULL);
	ASSERT (mp->b_band > 0);

	if (qbandp->qb_first == mp) {

		if (qbandp->qb_last == mp) {
			/*
			 * The band has become empty.
			 */

			ASSERT (mp->b_next == NULL ||
				mp->b_next->b_band < mp->b_band);

			qbandp->qb_last = qbandp->qb_first = NULL;
		} else {
			/*
			 * There is more data in the band.
			 */

			ASSERT (mp->b_next != NULL);
			ASSERT (mp->b_next->b_band == mp->b_band);

			qbandp->qb_first = mp->b_next;
		}
	} else if (qbandp->qb_last == mp) {
		/*
		 * The band is non-empty, but we have to move the end up.
		 */

		ASSERT (mp->b_prev != NULL);
		ASSERT (mp->b_prev->b_band == mp->b_band);

		qbandp->qb_last = mp->b_prev;
	}
}


/*
 * This internal function tests to see whether a stream wanting to write to
 * this queue should block to honour the flow-control scheme. The return
 * value is 1 if the caller can write to this queue, 0 otherwise.
 *
 * The queue passed to this function must be frozen.
 */

#if	__USE_PROTO__
__LOCAL__ int (QBAND_CANPUT) (queue_t * q, uchar_t pri)
#else
__LOCAL__ int
QBAND_CANPUT __ARGS ((q, pri))
queue_t	      *	q;
uchar_t		pri;
#endif
{
	qband_t	      *	qbandp;

	QFROZEN_TRACE (q, "QBAND_CANPUT");

	if (pri > 0) {

		qbandp = QUEUE_BAND (q, pri);

		if (qbandp != NULL && (qbandp->qb_flag & QB_FULL) != 0) {
			/*
			 * We must set the following flag to indicate that
			 * back-enabling is desired for this priority band.
			 *
			 * We'll set the global QWANTW flag as well, to make
			 * testing simpler for qprocson ().
			 */

			qbandp->qb_flag |= QB_WANTW;
			q->q_flag |= QWANTW;
			return 0;
		}
	} else
		if ((q->q_flag & QFULL) != 0) {
			/*
			 * We must set the following flag to indicate that the
			 * queue has a writer who has made a failed write
			 * attempt.
			 */

			q->q_flag |= QWANTW;
			return 0;
		}

	return 1;
}


/*
 * This internal function collects the non-priority-band flow-control actions
 * from flushq () and getq ().
 *
 * The queue passed to this function must be frozen.
 */

#if	__USE_PROTO__
__LOCAL__ void (QUEUE_REDUCE) (queue_t * q, ulong_t adjust)
#else
__LOCAL__ void
QUEUE_REDUCE __ARGS ((q, adjust))
queue_t	      *	q;
ulong_t		adjust;
#endif
{
	QFROZEN_TRACE (q, "QUEUE_REDUCE");

	ASSERT (q->q_count >= adjust);

	/*
	 * If the queue was not previously full, then we don't need to do
	 * anything other than tweak the count.
	 */

	q->q_count -= adjust;

	if ((q->q_flag & QFULL) != 0 && q->q_count <= q->q_lowat) {

		q->q_flag &= ~ QFULL;

		if ((q->q_flag & QWANTW) != 0) {

			q->q_flag &= ~ QWANTW;
			q->q_flag |= QBACK;
		}
	}
}


/*
 * This function contains part of the M_SETOPTS processing; specifically, here
 * we deal with changing the watermarks of a band. The caller should test the
 * QBACK flag when unfreezing the stream.
 */

#if	__USE_PROTO__
void (QBAND_SETOPT) (queue_t * q, struct stroptions * so)
#else
void
QBAND_SETOPT __ARGS ((q, so))
queue_t	      *	q;
struct stroptions
	      *	so;
#endif
{
	qband_t	      *	qbandp;
	pl_t		prev_pl;

	prev_pl = QFREEZE_TRACE (q, "QBAND_SETOPTS");

	if ((so->so_flags & (SO_LOWAT | SO_HIWAT)) == 0)
		return;

	if ((so->so_flags & SO_BAND) != 0 && so->so_band > 0) {

		if ((qbandp = QUEUE_BAND (q, so->so_band)) == NULL &&
		    (qbandp = QBAND_ALLOC (q, so->so_band)) == NULL) {
			/*
			 * We cannot allocate space for the band accounting
			 * structures. Give up.
			 */

			QUNFREEZE_TRACE (q, prev_pl);
			return;
		}

		if ((so->so_flags & SO_LOWAT) != 0) {

			qbandp->qb_lowat = so->so_lowat;
			QBAND_REDUCE (q, qbandp, 0L);
		}

		if ((so->so_flags & SO_HIWAT) != 0) {

			qbandp->qb_hiwat = so->so_hiwat;
			QBAND_INCREASE (q, qbandp, 0L);
		}
	} else {

		if ((so->so_flags & SO_LOWAT) != 0) {

			q->q_lowat = so->so_lowat;
			QUEUE_REDUCE (q, 0L);
		}

		if ((so->so_flags & SO_HIWAT) != 0) {

			q->q_hiwat = so->so_hiwat;
			if (q->q_count > q->q_hiwat)
				q->q_flag |= QFULL;
		}
	}


	/*
	 * Since we are in a path with QBAND_REDUCE (), check for QBACK before
	 * unfreezing the stream.
	 */

	{
		unsigned long	back;

		if ((back = q->q_flag & QBACK) != 0)
			q->q_flag &= ~ QBACK;

		QUNFREEZE_TRACE (q, prev_pl);

		if (back)
			QUEUE_BACKENAB (q);
	}
}


/*
 * This internal function is used by bufcall () and esbbcall () to queue a
 * STREAMS event cell on the queue for eventual processing once memory
 * becomes available.
 */

#if	__USE_PROTO__
__LOCAL__ toid_t (STORE_EVENT) (sevent_t * seventp, int pri)
#else
__LOCAL__ toid_t
STORE_EVENT __ARGS ((seventp, pri))
sevent_t      *	seventp;
int		pri;
#endif
{
	pl_t		prev_pl;
	selist_t      *	selistp;
	sevent_t      *	sprev;

	/*
	 * Let's lock the list that we are going to thread the new event on.
	 */

	selistp = & str_mem->sm_bcevents [MAP_PRI_LEVEL (pri)];

	prev_pl = SELIST_LOCK (selistp);


#if _TOID_MEMBER
	/*
	 * Before we perform the insertion of the event cell, we use the above
	 * lock to make our ID code generation multiprocessor-safe.
	 */

	selistp->sl_id = (((seventp->se_id = selistp->sl_id) +
				TOID_INCREMENT) % TOID_MODULUS);

	ASSERT (seventp->se_id != 0);
	ASSERT (TOID_TO_PRI (seventp->se_id) == pri);
#endif

	/*
	 * RESEARCH NOTE: There doesn't seem to be any compelling reason to
	 * choose a particular policy for managing event cells. Try getting
	 * some data of performance effects for low->high, high->low and FIFO
	 * queueing.
	 */
#if _FIFO_BUFCALL
	/*
	 * FIFO queueing is nice and simple.
	 */

	if ((sprev = selistp->sl_tail) == NULL) {

		ASSERT (selistp->sl_head == NULL);

		selistp->sl_head = selistp->sl_tail = seventp;
		seventp->se_prev = seventp->se_next = NULL;
	} else {

		sprev->se_next = seventp;
		seventp->se_prev = sprev;
		seventp->se_next = NULL;
		selistp->sl_tail = seventp;
	}
#else
	/*
	 * Now insert the event cell in the event list, keeping it sorted from
	 * low size to high size.
	 */

	{
		sevent_t      *	sscan;

		for (sscan = selistp->sl_head, sprev = NULL ; sscan != NULL ;
		     sscan = (sprev = sscan)->se_next) {

			if (sscan->se_size > seventp->se_size)
				break;
		}

		if (sprev == NULL)
			selistp->sl_head = seventp;
		else
			sprev->se_next = seventp;

		if (sscan != NULL)
			sscan->se_prev = seventp;

		seventp->se_prev = sprev;
		seventp->se_next = sscan;
	}
#endif

	SELIST_UNLOCK (selistp, prev_pl);

#if _TOID_MEMBER
	return seventp->se_id;
#endif
}


/*
 * This local function factors out the common code in put () and
 * QUEUE_PUTNEXT () [used to implement the putnext (), putnextctl () and
 * similar functions].
 *
 * The queue must be frozen on entry to this function.
 */

#if	__USE_PROTO__
__LOCAL__ void (QUEUE_PUT) (queue_t * q, mblk_t * mp, pl_t prev_pl)
#else
__LOCAL__ void
QUEUE_PUT __ARGS ((q, mp, prev_pl))
queue_t	      *	q;
mblk_t	      *	mp;
pl_t		prev_pl;
#endif
{
	QFROZEN_TRACE (q, "QUEUE_PUT");

	q->q_active ++;

	if (q->q_qinfo->qi_mstat != NULL)
		q->q_qinfo->qi_mstat->ms_pcnt ++;

	QUNFREEZE_TRACE (q, prev_pl);


	(* q->q_qinfo->qi_putp) (q, mp);


	(void) QFREEZE_TRACE (q, "QUEUE_PUT");

	q->q_active --;

	if ((q->q_flag & QPROCSOFF) != 0 && q->q_active == 0) {
		/*
		 * Time to wake up the sleepers waiting for the put and
		 * service routines on a queue to exit. See qprocsoff () for
		 * a discussion of the locking protocol.
		 */

		(void) LOCK (str_mem->sm_proc_lock, plstr);
		SV_BROADCAST (str_mem->sm_proc_sv, 0);
		UNLOCK (str_mem->sm_proc_lock, plstr);
	}
}


/*
 * This local function factors out most of the interesting part of putnext ()
 * into a common block that can be called from any of the related group of
 * functions putnext (), putnextctl (), putnextctl1 (), and qreply ().
 */

#if	__USE_PROTO__
__LOCAL__ void (QUEUE_PUTNEXT) (queue_t * q, mblk_t * mp)
#else
__LOCAL__ void
QUEUE_PUTNEXT __ARGS ((q, mp))
queue_t	      *	q;
mblk_t	      *	mp;
#endif
{
	pl_t		prev_pl;

	/*
	 * We call QUEUE_NEXT () to find the next queue on the stream that has
	 * not been disabled via qprocsoff (). It may be that there is no
	 * such queue. This can happen on a uniprocessor or multiprocessor if
	 * a device's interrupt routine begins generating data before the
	 * queue open () entry point has issued qprocson (). Since this is
	 * really an error, we complain about it.
	 */

	prev_pl = QFREEZE_TRACE (q, "QUEUE_PUTNEXT");

	q = QUEUE_NEXT (q);

	if (q != NULL) {
		/*
		 * We have found a queue we are allowed to put to.
		 */

		QUEUE_PUT (q, mp, prev_pl);
	} else {
		/*
		 * We have run off the end of the stream... we can
		 * either wait, put the message anyway, or discard
		 * the message. Either way, a warning is appropriate.
		 */

		cmn_err (CE_WARN, "putnext () ran off end of stream");
		freemsg (mp);
	}

	QUNFREEZE_TRACE (q, prev_pl);
}


/*
 * This internal function is similar in spirit to QUEUE_PUTNEXT (), above, but
 * instead factors out the process of looking for a queue with a service
 * procedure that can be enabled from the bcanputnext () and canputnext ()
 * routines.
 *
 * Determines whether the caller can legitimately put downstream in the
 * indicated priority band.
 *
 * The queue passed to this function cannot be frozen.
 */

#if	__USE_PROTO__
__LOCAL__ int (QBAND_SRVNEXT) (queue_t * q, uchar_t pri)
#else
__LOCAL__ int
QBAND_SRVNEXT __ARGS ((q, pri))
queue_t	      *	q;
uchar_t		pri;
#endif
{
	int		retval;
	pl_t		prev_pl;

	/*
	 * Scan through the stream for a queue with a service procedure that
	 * we can enable.
	 */

	prev_pl = QFREEZE_TRACE (q, "QBAND_SRVNEXT");

	for (;;) {

		q = QUEUE_NEXT (q);

		if (q == NULL) {
			/*
			 * We have run off the end of the stream, so return
			 * true to the caller.
			 */

			retval = 1;
			break;

		} else if (q->q_qinfo->qi_srvp != NULL) {

			retval = QBAND_CANPUT (q, pri);
			break;
		}

		/*
		 * No service procedure, try next queue in line.
		 */
	}

	QUNFREEZE_TRACE (q, prev_pl);


	/*
	 * If there are no queues with service procedures, the caller is
	 * allowed to put by default; the special case above is an exception.
	 */

	return retval;
}


/*
 * This function deals with allocating and initializing a message block from
 * the message memory; unlike allocb (), an extra parameter can be used to
 * cause this function to block if there is insufficient memory available to
 * satisfy the request immediately.
 */

#if	__USE_PROTO__
mblk_t * (MSGB_ALLOC) (size_t size, int pri, int flag)
#else
mblk_t *
MSGB_ALLOC __ARGS ((size, pri, flag))
size_t		size;
int		pri;
int		flag;
#endif
{
	mblk_t	      *	mblkp;

	ASSERT (flag == KM_SLEEP || flag == KM_NOSLEEP);
	ASSERT (pri == BPRI_LO || pri == BPRI_MED || pri == BPRI_HI);

	if ((mblkp = STRMEM_ALLOC (MSGB_SIZE (size), pri, flag)) != NULL) {
		/*
		 * Note that we don't bother setting up the b_prev and
		 * b_next members of the message block.
		 */

		mblkp->b_cont = NULL;
		mblkp->b_datap = MB_TO_DB (mblkp);
		mblkp->b_rptr = DB_TO_DATA (mblkp->b_datap);
		mblkp->b_wptr = mblkp->b_rptr;

		/*
		 * We specially flag this message block as part of
		 * a triple. While there are other ways we could test
		 * this, for now we permit the possibility that a
		 * client might change the attachments of data blocks
		 * and message blocks perversely.
		 */

		mblkp->b_flag = MSGTRIPLE;
		mblkp->b_band = 0;

		mblkp->b_datap->db_base = mblkp->b_rptr;
		mblkp->b_datap->db_lim = mblkp->b_rptr + size;
		mblkp->b_datap->db_type = M_DATA;
		mblkp->b_datap->db_ref = 1;

		mblkp->b_datap->db_frtnp = NULL;
	}

	return mblkp;
}


/*
 * This local helper function factors out some code in strlog () common to
 * building the "struct log_ctl" headers for the log messages.
 */

#if	__USE_PROTO__
__LOCAL__ mblk_t * (STRLOG_MAKE) (short mid, short sid, char level,
				  ushort_t flags, ulong_t seq, ushort_t whoami,
				  int * copies, int * failures, mblk_t * data)
#else
__LOCAL__ mblk_t *
STRLOG_MAKE __ARGS ((mid, sid, level, flags, seq, whoami, copies, failures, data))
short		mid;
short		sid;
char		level;
ushort_t	flags;
ulong_t		seq;
ushort_t	whoami;
int	      *	copies;
int	      *	failures;
mblk_t	      *	data;
#endif
{
	mblk_t	      *	ctl;
	struct log_ctl * lcp;
	int		alloc_pri;
	int		log_pri;

	if ((flags & whoami) == 0)
		return NULL;

	/*
	 * Work out an allocation priority for the M_PROTO message block that
	 * will contain the "log_ctl" description structure, and also
	 * work out a priority for the "mwc_pri" member of the "log_ctl"
	 * structure.
	 *
	 * This code is pretty funky; the real rules might be much simpler,
	 * but this is what I get from the docs. The "log_pri" rules go from
	 * low to high priority, so this code should match the order defined
         * for the 
	 */

	if (whoami == SL_ERROR || (flags & SL_FATAL) != 0)
		alloc_pri = BPRI_HI;
	else if ((flags & SL_WARN) != 0)
		alloc_pri = BPRI_MED;
	else
		alloc_pri = BPRI_LO;


	/*
	 * if (whoami == SL_CONSOLE)
	 */
	log_pri = LOG_INFO;

	if ((flags & SL_NOTE) != 0)
		log_pri = LOG_NOTICE;

	if (whoami == SL_TRACE)
		log_pri = LOG_DEBUG;

	if ((flags & SL_WARN) != 0)
		log_pri = LOG_WARNING;

	if (whoami == SL_ERROR)
		log_pri = LOG_ERR;

	if ((flags & SL_FATAL) != 0)
		log_pri = LOG_CRIT;


	/*
	 * Allocate the necessary structures and fill them in.
	 */

	if ((ctl = MSGB_ALLOC (sizeof (struct log_ctl), alloc_pri,
			       KM_NOSLEEP)) == NULL) {

		(* failures) ++;
		return NULL;	/* return failure */
	}

	if (* copies == 0)
		ctl->b_cont = data;
	else if ((ctl->b_cont = dupb (data)) == NULL) {

		freeb (ctl);

		(* failures) ++;
		return NULL;
	}

	(* copies) ++;

	ctl->b_datap->db_type = M_PROTO;
	lcp = (struct log_ctl *) ctl->b_rptr;
	ctl->b_wptr = (uchar_t *) (lcp + 1);

	lcp->mwc_mid = mid;
	lcp->mwc_sid = sid;
	lcp->level = level;
	lcp->flags = flags;
	lcp->mwc_seqno = seq;
	lcp->mwc_pri = log_pri | LOG_KERN;

	return ctl;
}


/*
 * This function factors out common code from putbq () and putq () for finding
 * the insertion point for a message. The original routines differed only in
 * whether the search was for the end of a band or the front of a band. By
 * passing in an appropriately adjusted band parameter, the same code can be
 * made to do double duty.
 *
 * The queue passed to this function must be frozen.
 */

#if	__USE_PROTO__
__LOCAL__ void (QUEUE_PLACE_MSG) (queue_t * q, mblk_t * mp, uchar_t band)
#else
__LOCAL__ void
QUEUE_PLACE_MSG __ARGS ((q, mp, band))
queue_t	      *	q;
mblk_t	      *	mp;
uchar_t		band;
#endif
{
	mblk_t	      *	scan;

	/*
	 * The first stage is to skip over all the high-priority messages, and \
	 * then to find the right band location.
	 */

	for (scan = q->q_first ; scan != NULL ; scan = scan->b_next)
		if (! IS_PRI_MSG (scan))
			break;

	while (scan != NULL && scan->b_band > band)
		scan = scan->b_next;

	/*
	 * Now we know where the message goes, do it.
	 */

	if ((mp->b_next = scan) == NULL) {

		if ((mp->b_prev = q->q_last) == NULL)
			q->q_first = mp;
		else
			mp->b_prev->b_next = mp;

		q->q_last = mp;
	} else {

		if ((mp->b_prev = scan->b_prev) == NULL)
			q->q_first = mp;
		else
			mp->b_prev->b_next = mp;

		scan->b_prev = mp;
	}
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	adjmsg		Trim bytes from a message
 *
 *-SYNOPSIS:
 *	#include <sys/streams.h>
 *
 *	int adjmsg (mblk_t * mp, int len);
 *
 *-ARGUMENTS:
 *	mp		Pointer to the message to be trimmed.
 *
 *	len		The number of bytes to be removed.
 *
 *-DESCRIPTION:
 *	adjmsg () removes bytes from a message. |"len"| (the absolute value of
 *	"len") specifies the how many bytes are to be removed. If "len" is
 *	greater than 0, bytes are removed from the head of the message. If
 *	"len" is less than 0, bytes are removed from the tail. adjmsg () fails
 *	if |"len"| is greater than the number of bytes in "mp". If "len" spans
 *	more than one message block in the message, the message blocks must be
 *	the same type, or else adjmsg () will fail.
 *
 *-RETURN VALUE:
 *	If the message can be trimmed successfully, 1 is returned. Otherwise,
 *	0 is returned.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *	If "len" is greater than the amount of data in a single message block,
 *     	that message block is not freed. Rather, it is left linked in the
 *	message, and its read and write pointers are set equal to each other,
 *	indicating no data present in the block.
 *
 *-SEE ALSO:
 *	msgb
 */

#if	__USE_PROTO__
int (adjmsg) (mblk_t * mp, int len)
#else
int
adjmsg __ARGS ((mp, len))
mblk_t	      *	mp;
int		len;
#endif
{
	uchar_t		msgtype = mp->b_datap->db_type;
	int		result = 1;

	ASSERT (mp != NULL);

	/*
	 * The DDI/DKI does not specify whether the message blocks are to be
	 * left unmodified in the case of failure. For now, we interpret the
	 * semantics of the routine such that a failure leaves all the
	 * requested data removed, with the return value supplying an
	 * indication that overflow occurred.
	 */

	if (len >= 0) {
		/*
		 * Remove bytes forward from the head of the message.
		 */

		while (len != 0) {
			mblk_t	      *	next;

			if (mp->b_wptr - mp->b_rptr >= len) {
				/*
				 * We are only comsuming part of the message.
				 */

				mp->b_rptr += len;

				return 1;
			} else if ((next = mp->b_cont) == NULL) {
				/*
				 * All the message blocks in the message have
				 * been totally consumed, so return failure.
				 */

				mp->b_rptr = mp->b_wptr;

				return 0;
			}

			/*
			 * This message block has been consumed, so leave it
			 * empty.
			 */

			len -= mp->b_wptr - mp->b_rptr;

			mp->b_wptr = mp->b_rptr = mp->b_datap->db_base;

			mp = next;

			if (mp->b_datap->db_type != msgtype)
				return 0;	/* wrong types */
		}

	} else {
		mblk_t	      *	span = mp;
		int		size = 0;

		/*
		 * Since message blocks only have forward links, we have to
		 * employ some subterfuge to implement this efficiently. We
		 * first scan forward to discover the size and start of the
		 * last span of message blocks with the same type.
		 */

		for (;;) {
			size += mp->b_wptr - mp->b_rptr;

			if ((mp = mp->b_cont) == NULL)
				break;

			if (mp->b_datap->db_type != msgtype) {
				/*
				 * The type has changed, so we reset our count
				 * and start pointers to begin a new span.
				 */

				size = 0;
				msgtype = mp->b_datap->db_type;
				span = mp;
			}
		}


		if (size < len) {
			/*
			 * The user has requested too many bytes be removed,
			 * so we return a failure indication and remove as
			 * many as we are able.
			 */

			result = 0;
			len = size;
		}


		/*
		 * Now we have the last span, we can scan forward for the
		 * point where we must begin emptying messages.
		 */

		while (size > 0) {

			size -= span->b_wptr - span->b_rptr;

			if (size < len) {
				/*
				 * The count of bytes remaining in the span
				 * is less than the threshold, so we start to
				 * remove data.
				 */

				span->b_wptr -= (len - size);

				len = size;
			}

			span = span->b_next;
		}
	}

	return result;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	allocb		Allocate a message block
 *
 *-SYNOPSIS:
 *	#include <sys/types.h>
 *	#include <sys/stream.h>
 *
 *	mblk_t * allocb (int size, uint_t pri);
 *
 *-ARGUMENTS:
 *	size		The number of bytes in the message block.
 *
 *	pri		Priority of the request. This can take on one of three
 *			values: BPRI_LO, BPRI_MED, or BPRI_HI.
 *
 *-DESCRIPTION:
 *	allocb () tries to allocate a STREAMS message block. Buffer allocation
 *	fails only when the system is out of memory. If no buffer is
 *	available, the bufcall () function can help a module recover from an
 *	allocation failure.
 *
 *	The "pri" argument is a hint to the allocator indicating how badly the
 *	message is needed. BPRI_LO should be used for normal data allocations,
 *	BPRI_MED should be used for other non-critical allocations. BPRI_HI
 *	should be used for allocations that absolutely must succeed, even
 *	though success is not guaranteed. Some implementations may choose to
 *	ignore this parameter.
 *
 *-RETURN VALUE:
 *	If successful, allocb () returns a pointer to the allocated message
 *	block of type M_DATA (defined in <sys/stream.h>). If a block cannot be
 *	allocated, a NULL pointer is returned.
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
 *-SEE ALSO:
 *	bufcall (), esballoc (), esbbcall (), freeb (), msgb
 */

#if	__USE_PROTO__
mblk_t * (allocb) (int size, uint_t pri)
#else
mblk_t *
allocb __ARGS ((size, pri))
uint_t		size;
uint_t		pri;
#endif
{
	ASSERT (size >= 0);
	ASSERT (ATOMIC_FETCH_UCHAR (str_mem->sm_init) &&
		str_mem->sm_msg_lock != NULL);

	/*
	 * Since we allocate things in triples, "allocb (0)" could be legal if
	 * we wanted. There aren't any compelling reasons either way, but for
	 * simplicity I'll permit it.
	 */

#if 0
	if (size == 0)
		return NULL;
#endif

	return MSGB_ALLOC (size, pri, KM_NOSLEEP);
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	bcanput		Test for flow control in specified priority band.
 *
 *-SYNOPSIS:
 *	#include <sys/types.h>
 *	#include <sys/stream.h>
 *
 *	int bcanput (queue_t * q, uchar_t pri);
 *
 *-ARGUMENTS:
 *	q		Pointer to the message queue.
 *
 *	pri		Message priority.
 *
 *-DESCRIPTION:
 *	bcanput () tests if there is room for a message in priority band "pri"
 *	of the queue pointed to by "q". The queue _must_ have a service
 *	procedure.
 *
 *	If "pri" is 0, the bcanput () call is equivalent to a call to
 *	canput ().
 *
 *	It is possible because of race conditions to test for room using
 *	bcanput () and get an indication that there is room for a message, and
 *	then have the queue fill up before subsequently enqueuing the message,
 *	causing a violation of flow control. This is not a problem, since the
 *	violation of flow control in this case is bounded.
 *
 *-RETURN VALUE:
 *	bcanput () returns 1 if a message of priority "pri" can be placed on
 *	the queue. 0 is returned if a message of priority "pri" cannot be
 *	enqueued because of flow control within the priority band.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	The driver writer is responsible for both testing a queue with
 *	bcanput () and refraining from placing a message if bcanput () fails.
 *
 *	The caller cannot have the stream frozen [see freezestr ()] when
 *	calling this function.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *	The "q" argument may not reference "q_next" (for example, an argument
 *	of "q->q_next" is erroneous on a multiprocessor and is disallowed by
 *	the DDI/DKI). "bcanputnext (q)" is provided as a multiprocessor-safe
 *	equivalent to the common call "bcanput (q->q_next)", which is no
 *	longer allowed [see bcanputnext ()].
 *
 *-SEE ALSO:
 *	bcanputnext (), canput (), canputnext (), putbq (), putnext ()
 */

#if	__USE_PROTO__
int (bcanput) (queue_t * q, uchar_t pri)
#else
int
bcanput __ARGS ((q, pri))
queue_t	      *	q;
uchar_t		pri;
#endif
{
	pl_t		prev_pl;
	int		result = 1;

	prev_pl = QFREEZE_TRACE (q, "bcanput");

	result = QBAND_CANPUT (q, pri);

	QUNFREEZE_TRACE (q, prev_pl);

	return result;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	bcanputnext	Test for flow control in specified priority band.
 *
 *-SYNOPSIS:
 *	#include <sys/types.h>
 *	#include <sys/stream.h>
 *
 *	int bcanputnext (queue_t * q, uchar_t pri);
 *
 *-ARGUMENTS:
 *	q		Pointer to the message queue.
 *
 *	pri		Message priority.
 *
 *-DESCRIPTION:
 *	bcanputnext () searches through the stream (starting at "q->q_next")
 *	until it finds a queue containing a service routine, or until it
 *	reaches the end of the stream. If found, the queue containing the
 *	service routine is tested to see if a message in priority band "pri"
 *	can be enqueued. If the band is full, bcanputnext () marks the queue
 *	to automatically back-enable the caller's service routine when the
 *	amount of data in messages on the queue has reached its low water
 *	mark.
 *
 *	If "pri" is 0, the bcanputnext () call is equivalent to a call to
 *	canputnext ().
 *
 *	It is possible because of race conditions to test for room using
 *	bcanputnext () and get an indication that there is room for a message,
 *	and then have the queue fill up before subsequently enqueuing the
 *	message, causing a violation of flow control. This is not a problem,
 *	since the violation of flow control in this case is bounded.
 *
 *-RETURN VALUE:
 *	bcanputnext () returns 1 if a message of priority "pri" can be sent in
 *	the stream, or 0 if the stream is flow-controlled. If bcanputnext ()
 *	reaches the end of the stream without finding a queue with a service
 *	procedure, then it returns 1.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	The driver writer is responsible for both testing a queue with
 *	bcanputnext () and refraining from placing a message if bcanputnext ()
 *	fails.
 *
 *	The caller cannot have the stream frozen [see freezestr ()] when
 *	calling this function.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *	The "q" argument may not reference "q_next" (for example, an argument
 *	of "q->q_next" is erroneous on a multiprocessor and is disallowed by
 *	the DDI/DKI). "bcanputnext (q)" is provided as a multiprocessor-safe
 *	equivalent to the common call "bcanput (q->q_next)", which is no
 *	longer allowed [see bcanputnext ()].
 *
 *-SEE ALSO:
 *	bcanput (), canput (), canputnext (), putbq (), putnext ()
 */

#if	__USE_PROTO__
int (bcanputnext) (queue_t * q, uchar_t pri)
#else
int
bcanputnext __ARGS ((q, pri))
queue_t	      *	q;
uchar_t		pri;
#endif
{
	QUEUE_TRACE (q, "bcanputnext");

	return QBAND_SRVNEXT (q, pri);
}


/*
 * Note: backq () is not present in the System V DDI/DKI For Intel Processors
 * with Multiprocessing reference, and is listed in one of the appendices of
 * that volume as having been removed in the R3.2 to R4 transition. However,
 * the function is present in the regular generic System V DDI/DKI.
 */

#if 0
/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	backq		Get pointer to the queue behind the current queue.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	queue_t * backq (queue_t * q);
 *
 *-ARGUMENTS:
 *	q		Pointer to the current queue.
 *
 *-DESCRIPTION:
 *	backq () returns a pointer to the queue preceding "q". If "q" is a
 *	read queue, backq () returns a pointer to the queue downstream from
 *	"q", unless it is the stream end. If "q" is a write queue, backq ()
 *	returns a pointer to the next queue upstream from "q", unless it is
 *	the stream head.
 *
 *-RETURN VALUE:
 *	If successful, backq () returns a pointer to the queue preceding the
 *	current queue. Otherwise, it returns NULL.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	The caller cannot have the stream frozen [see freezestr ()] when
 *	calling this function.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *	If the case of a STREAMS-based fifo, this function takes no special
 *	action at the midpoint of the stream where the read and write sides
 *	interchange.
 */

#if	__USE_PROTO__
queue_t * (backq) (queue_t * q)
#else
queue_t *
backq __ARGS ((q))
queue_t	      *	q;
#endif
{
	q = OTHERQ (q);

	q = QUEUE_NEXT (q);

	if (q == NULL)
		return NULL;

	return OTHERQ (q);
}
#endif


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	bufcall		Call a function when a buffer becomes available.
 *
 *-SYNOPSIS:
 *	#include <sys/types.h>
 *	#include <sys/stream.h>
 *
 *	toid_t bufcall (uint_t size, int pri, void (* func) (), long arg);
 *
 *-ARGUMENTS:
 *	size		Number of bytes in the buffer to be allocated (from
 *			the failed allocb () request).
 *
 *	pri		Priority of the allocb () allocation request (BPRI_LO,
 *			BPRI_MED, or BPRI_HI).
 *
 *	func		Fuction or driver routine to be called when a buffer
 *			becomes available.
 *
 *	arg		Argument to the function to be called when a buffer
 *			becomes available.
 *
 *-DESCRIPTION:
 *	bufcall () serves as a timeout call of indeterminate length. When a
 *	buffer allocation request fails, bufcall () can be used to schedule
 *	the routine "func" to be called with the argument "arg" when a buffer
 *	of at least "size" bytes becomes available.
 *
 *	When "func" runs, all interrupts from STREAMS devices will be blocked
 *	on the processor on which it is running. "func" will have no user
 *	context and may not call any function that sleeps.
 *
 *-RETURN VALUE:
 *	If successful, bufcall () returns a non-zero value that identifies the
 *	scheduling request. This non-zero identifier may be passed to
 *	unbufcall () to cancel the request. If any failure occurs, bufcall ()
 *	returns 0.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *	Even when "func" is called, allocb () can still fail if another
 *	module or driver had allocated the memory before "func" was able to
 *	call allocb ().
 *
 *-SEE ALSO:
 *	allocb (), esballoc (), esbbcall (), itimeout (), unbufcall ().
 */

#if	__USE_PROTO__
toid_t (bufcall) (uint_t size, int pri, ...)
#else
toid_t
bufcall __ARGS ((size, pri))
uint_t		size;
int		pri;
#endif
{
	se_funcptr_t	func;
	long		arg;
	va_list		arglist;
	sevent_t      *	seventp;


	/*
	 * Before we do anything, we fetch the two other arguments. We use
	 * the variable-argument function convention to get around problems
	 * with implicit conversions introduced by having a prototype for
	 * this function visible.
	 *
	 * This is a generic problem with callbacks, since the shape of a
	 * function type includes the shapes of the function arguments, and
	 * because we want to pass some argument to the user-supplied function
	 * without knowing its type (and hence, we must not cause any implicit
	 * conversions on its value).
	 */

	va_start (arglist, pri);

	func = va_arg (arglist, se_funcptr_t);
	arg = va_arg (arglist, long);

	va_end (arglist);


	/*
	 * It may be that NULL is defined in such a way that it is not
	 * meaningful to compare it with a function pointer, so we just cast
	 * a zero to get the right kind of effect.
	 *
	 * We add some extra assertions below to check that "long" is of
	 * sufficient size to contain all likely parameter data items. For
	 * C, "likely" means the fundamental types, which has to include
	 * pointers to functions, pointers to void, and also pointers to
	 * structures since under C++ there is currently no guarantee to match
	 * the ISO C requirement that a pointer of type "void *" be able to
	 * hold the value of any pointer type without loss of information.
	 * [We don't consider member pointers here; they are pathological]
	 */

	ASSERT (sizeof (se_funcptr_t) <= sizeof (long) &&
		sizeof (_VOID *) <= sizeof (long) &&
		sizeof (selist_t *) <= sizeof (long));

	ASSERT (func != (se_funcptr_t) 0);
	ASSERT (pri == BPRI_LO || pri == BPRI_HI || pri == BPRI_LO);


	/*
	 * Here we *could* lock and test the available space in the STREAMS
	 * message heap to see whether sufficient space to satisfy the
	 * request has become available between the failed allocb () and
	 * the bufcall () request.
	 *
	 * The question is whether the extra time that the heap is locked is
	 * worth overcoming a delay for a freemsg () to trigger the bufcall ()
	 * later, especially given the fact that we'll have to unlock the
	 * heap to call the user's callback (introducing the opportunity for
	 * the newly-discovered space to vanish).
	 *
	 * All in all, once things get to the bufcall () stage there seems
	 * little reason to keep pushing the system, so we don't bother.
	 */

	/*
	 * First off, let's get ourselves an event cell. This is mutually
	 * dependent with the ID generation policy; if ID codes are stored in
	 * the event cells, we are free to allocate and discard from a general
	 * memory pool; if ID codes are not stored, then they must reflect
	 * some persistent attribute of the cell, which usually implies that
	 * cells come from some nonshrinking managed pool.
	 *
	 * Of course, a generative ID code scheme requires extra locking in
	 * the absence of atomic FETCH_AND_INCREMENT () operations. In this
	 * implementation, we get around this by using the list-head locks
	 * to guard the counter operations. [This has interesting implications
	 * for the design of a generic list-manipulation facility.] 
	 */

#if _TOID_MEMBER
	if ((seventp = (sevent_t *) kmem_alloc (sizeof (sevent_t),
						KM_NOSLEEP)) == NULL)
		return 0;
#else
#error	Need an allocator for event cells that matches the ID policy
#endif

	/*
	 * Fill in the event cell. Note that the size member includes the
	 * memory space required for the message block and data block!
	 */

	seventp->se_arg = arg;
	seventp->se_func = func;
	seventp->se_size = MSGB_SIZE (size);


	return STORE_EVENT (seventp, pri);
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	canput		Test for room in a message queue.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	int canput (queue_t * q);
 *
 *-ARGUMENTS:
 *	q		Pointer to the message queue.
 *
 *-DESCRIPTION:
 *	canput () tests if there is room for a message in the queue pointed to
 *	by "q". The queue _must_ have a service procedure.
 *
 *	It is possivle because of race conditions to test for room using
 *	canput () and get an indication that there is room for a message, and
 *	then have the queue fill up before subsequently enqueuing the message,
 *	causing a violation of flow control. This is not a problem, since the
 *	violation of flow control in this case is bounded.
 *
 *-RETURN VALUE:
 *	canput () returns 1 if a message can be placed on the queue. 0 is
 *	returned if a message cannot be enqueued because of flow control.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	The driver writer is responsible for both testing a queue with
 *	canput () and refraining from placing a message if canput () fails.
 *
 *	The caller cannot have the stream frozen [see freezestr ()] when
 *	calling this function.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *	The "q" argument may not reference "q_next" (for example, an argument
 *	of "q->q_next" is erroneous on a multiprocessor and is disallowed by
 *	the DDI/DKI). "canputnext (q)" is provided as a multiprocessor-safe
 *	equivalent to the common call "canput (q->q_next)", which is no
 *	longer allowed [see canputnext ()].
 *
 *-SEE ALSO:
 *	bcanputnext (), bcanput (), canputnext (), putbq (), putnext ()
 */

#if	__USE_PROTO__
int (canput) (queue_t * q)
#else
int
canput __ARGS ((q))
queue_t	      *	q;
#endif
{
	pl_t		prev_pl;
	int		result = 1;

	prev_pl = QFREEZE_TRACE (q, "canput");

	if ((q->q_flag & QFULL) != 0) {
		/*
		 * We must set the following flag to indicate that the
		 * queue has a writer who has made a failed write
		 * attempt.
		 */

		q->q_flag |= QWANTW;

		result = 0;
	}

	QUNFREEZE_TRACE (q, prev_pl);

	return result;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	canputnext	Test for flow control in a stream.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	int bcanputnext (queue_t * q);
 *
 *-ARGUMENTS:
 *	q		Pointer to the message queue.
 *
 *-DESCRIPTION:
 *	canputnext () searches through the stream (starting at "q->q_next")
 *	until it finds a queue containing a service routine, or until it
 *	reaches the end of the stream. If found, the queue containing the
 *	service routine is tested to see if there is room for a message in the
 *	queue. If the band is full, canputnext () marks the queue to
 *	automatically back-enable the caller's service routine when the amount
 *	of data in messages on the queue has reached its low water mark.
 *
 *	It is possible because of race conditions to test for room using
 *	canputnext () and get an indication that there is room for a message,
 *	and then have the queue fill up before subsequently enqueuing the
 *	message, causing a violation of flow control. This is not a problem,
 *	since the violation of flow control in this case is bounded.
 *
 *-RETURN VALUE:
 *	canputnext () returns 1 if a message can be sent in the stream, or 0
 *	if the stream is flow-controlled. If canputnext () reaches the end of
 *	the stream without finding a queue with a service procedure, then it
 *	returns 1.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	The driver writer is responsible for both testing a queue with
 *	bcanputnext () and refraining from placing a message if bcanputnext ()
 *	fails.
 *
 *	The caller cannot have the stream frozen [see freezestr ()] when
 *	calling this function.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *	The "q" argument may not reference "q_next" (for example, an argument
 *	of "q->q_next" is erroneous on a multiprocessor and is disallowed by
 *	the DDI/DKI). "bcanputnext (q)" is provided as a multiprocessor-safe
 *	equivalent to the common call "bcanput (q->q_next)", which is no
 *	longer allowed [see bcanputnext ()].
 *
 *-SEE ALSO:
 *	bcanput (), bcanputnext (), canput (), putbq (), putnext ()
 */

#if	__USE_PROTO__
int (canputnext) (queue_t * q)
#else
int
canputnext __ARGS ((q))
queue_t	      *	q;
#endif
{
	QUEUE_TRACE (q, "canputnext");

	/*
	 * Rather than depend on canput () above, we'll use the band-capable
	 * function QBAND_SRVNEXT () and depend on its ability to handle
	 * band 0. The mechanics of iterating through a stream are much more
	 * complex now than in the old SVR3.2 days due to both freezestr ()
	 * and qprocsoff ().
	 */

	return QBAND_SRVNEXT (q, 0);
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	copyb		Copy a message block.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	mblk_t * copyb (mblk_t * bp);
 *
 *-ARGUMENTS:
 *	bp		Pointer to the message block from which data are
 *			copied.
 *
 *-DESCRIPTION:
 *	copyb () allocates a new message block, and copies into it the data
 *	from the block pointed to by "bp". The new block will be at least as
 *	large as the block being copied. The "b_rptr" and "b_wptr" members of
 *	the message block pointed to by "bp" are used to determine how many
 *	bytes to copy.
 *
 *-RETURN VALUE:
 *	If successful, copyb () returns a pointer to the newly allocated
 *	message block containing the copied data. Otherwise, it returns a NULL
 *	pointer.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 *	allocb (), copymsg (), msgb
 */

#define	ALLOW_EMPTY_COPIES

#if	__USE_PROTO__
mblk_t * (copyb) (mblk_t * bp)
#else
mblk_t *
copyb __ARGS ((bp))
mblk_t	      *	bp;
#endif
{
	mblk_t	      *	newmsg;
	size_t		size;

	ASSERT (bp != NULL && bp->b_datap != NULL);

	/*
	 * What to do if there is no real data in the original message?
	 *
	 * Copy it anyway, I guess.
	 */

	size = bp->b_wptr - bp->b_rptr;

	if (size == 0)
		return MSGB_ALLOC (0, BPRI_LO, KM_NOSLEEP);


	/*
	 * In this implementation we only allocate exactly as much space as
	 * we require (thus opening the zero-length issue). It is possible to
	 * interpret the wording of the manual-page as implying that the size
	 * of the new block is at least the size of the old block including
	 * unused space, but that doesn't seem reasonable. I read that
	 * requirement as merely pointing out that the allocator can give
	 * more memory to the new block that is strictly necessary.
	 */

	if ((newmsg = MSGB_ALLOC (size, BPRI_LO, KM_NOSLEEP)) != NULL) {

		newmsg->b_datap->db_type = bp->b_datap->db_type;

		memcpy (newmsg->b_rptr, bp->b_rptr, size);

		newmsg->b_wptr += size;
	}

	return newmsg;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	copymsg		Copy a message.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	mblk_t * copymsg (mblk_t * mp);
 *
 *-ARGUMENTS:
 *	mp		Pointer to the message to be copied.
 *
 *-DESCRIPTION:
 *	copymsg () forms a new message by allocating message blocks, copies
 * 	the conents of the message referred to by "mp" (using the copyb ()
 *	function) and returns a pointer to the new message.
 *
 *-RETURN VALUE:
 *	If successful, copymsg () returns a pointer to the new message.
 *	Otherwise, it returns a NULL pointer.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 *	allocb (), copyb (), msgb
 */

#if	__USE_PROTO__
mblk_t * (copymsg) (mblk_t * mp)
#else
mblk_t *
copymsg __ARGS ((mp))
mblk_t	      *	mp;
#endif
{
	mblk_t	      *	oldscan;
	mblk_t	      *	msg;
	mblk_t	      *	newblk;

	ASSERT (mp != NULL && mp->b_datap != NULL);

	/*
	 * Rather than simply layering on top of copyb (), we can make this
	 * function be more friendly in situations where memory is scarce by
	 * trying the allocations before the copies, so that we don't spend
	 * time copying data that is going to be discarded.
	 */

	if ((newblk = msg = MSGB_ALLOC (mp->b_wptr - mp->b_rptr, BPRI_LO,
					KM_NOSLEEP)) == NULL)
		return NULL;

	for (oldscan = mp->b_cont ; oldscan != NULL ;
	     newblk = newblk->b_cont, oldscan = oldscan->b_cont) {

		if ((newblk->b_cont =
				MSGB_ALLOC (oldscan->b_wptr - oldscan->b_rptr,
					    BPRI_LO, KM_NOSLEEP)) == NULL) {
			freemsg (msg);
			return NULL;
		}
	}

	/*
	 * Now we have the memory, do the copy. Like copyb (), we have have to
	 * remember to dance around zero-length blocks in the original, which
	 * we will be preserve as copied zero-length blocks.
	 */

	oldscan = mp;
	newblk = msg;

	do {
		size_t		size;

		ASSERT (oldscan != NULL);

		if ((size = oldscan->b_wptr - oldscan->b_rptr) > 0)
			memcpy (newblk->b_rptr, oldscan->b_rptr, size);
		newblk->b_wptr += size;

		oldscan = oldscan->b_cont;
	} while ((newblk = newblk->b_cont) != NULL);

	ASSERT (oldscan == NULL);

#if 0
	if ((msg = newblk = copyb (mp)) != NULL)
		while ((mp = mp->b_cont) != NULL)
			if ((newblk = (newblk->b_cont = copyb (mp))) == NULL) {
				/*
				 * If a given block in the message cannot be
				 * copied, undo all the work done so far.
				 */

				freemsg (msg);
				return NULL;
			}
#endif

	return msg;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	datamsg		Test whether a message is a data message.
 *
 *-SYNOPSIS:
 *	#include <sys/types.h>
 *	#include <sys/stream.h>
 *	#include <sys/ddi.h>
 *
 *	int datamsg (uchar_t type);
 *
 *-ARGUMENTS:
 *	type		The type of message to be tested. The "db_type" field
 *			of the "datab" structure contains the message type.
 *			This field may be accessed through the message block
 *			using "mp->b_datap->db_type".
 *
 *-DESCRIPTION:
 *	The datamsg () function tests the type of message to determine if it
 *	is a data message type (M_DATA, M_DELAY, M_PROTO or M_PCPROTO).
 *
 *-RETURN VALUE:
 *	datamsg () returns 1 if the message is a data message and 0 if the
 *	message is any other type.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 *	allocb (), datab, msgb, messages
 */

#if	__USE_PROTO__
int (datamsg) (uchar_t type)
#else
int
datamsg __ARGS ((type))
uchar_t		type;
#endif
{
	return datamsg (type);		/* appeal to the macro version */
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	dupb		Duplicate a message block.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	mblk_t * dupb (mblk_t * bp);
 *
 *-ARGUMENTS:
 *	bp		Pointer to the message block to be duplicated.
 *
 *-DESCRIPTION:
 *	dupb () creates a new message block structure to reference the message
 *	block pointed to by "bp". Unlike copyb (), dupb () does not copy the
 *	information in the data block, but creates a new structure to point to
 *	it.
 *
 *-RETURN VALUE:
 *	If successful, dupb () returns a pointer to the new message block.
 *	Otherwise, it returns a NULL pointer.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 *	copyb (), dupmsg (), datab, msgb
 */

#if	__USE_PROTO__
mblk_t * (dupb) (mblk_t * bp)
#else
mblk_t *
dupb __ARGS ((bp))
mblk_t	      *	bp;
#endif
{
	mblk_t	      *	newblk;

	ASSERT (bp != NULL);

	if ((newblk = STRMEM_ALLOC (sizeof (mblk_t), BPRI_HI,
				    KM_NOSLEEP)) != NULL) {
		/*
		 * Note that we do not initialize the "b_prev" and "b_next"
		 * members, and that the "b_cont" member is NULL rather than
		 * a copy of the original.
		 */

		newblk->b_cont = NULL;
		newblk->b_datap = bp->b_datap;
		newblk->b_rptr = bp->b_rptr;
		newblk->b_wptr = bp->b_wptr;
		++ newblk->b_datap->db_ref;
	}

	return newblk;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	dupmsg		Duplicate a message.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	mblk_t * dupmsg (mblk_t * mp);
 *
 *-ARGUMENTS:
 *	mp		Pointer to the message to be duplicated.
 *
 *-DESCRIPTION:
 *	dupmsg () forms a new message by duplicating the message blocks in the
 *	message pointed to by "mp" and linking them via their "b_cont"
 *	pointers.
 *
 *-RETURN VALUE:
 *	If successful, dupmsg () returns a pointer to the new message.
 *	Otherwise it returns a NULL pointer.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 *	copyb (), copymsg (), dupb (), datab, msgb
 */

#if	__USE_PROTO__
mblk_t * (dupmsg) (mblk_t * mp)
#else
mblk_t *
dupmsg __ARGS ((mp))
mblk_t	      *	mp;
#endif
{
	mblk_t	      *	msg;
	mblk_t	      *	newblk;

	ASSERT (mp != NULL);

	if ((msg = newblk = dupb (mp)) != NULL)
		while ((mp = mp->b_cont) != NULL)
			if ((newblk = (newblk->b_cont = dupb (mp))) == NULL) {
				/*
				 * Discard the work we have done so far.
				 */

				freemsg (msg);
				return NULL;
			}

	return msg;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	enableok	Enable a queue to be serviced.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *	#include <sys/ddi.h>
 *
 *	void enableok (queue_t * q);
 *
 *-ARGUMENTS:
 *	q		Pointer to the queue.
 *
 *-DESCRIPTION:
 *	The enableok () function allows the service routine of the queue
 *	pointed to by "q" to be rescheduled for service. It cancels the effect
 *	of a previous use of the noenable () function on "q".
 *
 *-RETURN VALUE:
 *	None.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	The caller cannot have the stream frozen [see freezestr ()] when
 *	calling this function.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 *	noenable (), qenable (), queue
 */

#if	__USE_PROTO__
void (enableok) (queue_t * q)
#else
void
enableok __ARGS ((q))
queue_t	      *	q;
#endif
{
	pl_t		prev_pl;

	prev_pl = QFREEZE_TRACE (q, "enableok");

	q->q_flag &= ~ QNOENB;

	QUNFREEZE_TRACE (q, prev_pl);
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	esballoc	Allocate a message block using an externally supplied
 *			buffer.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	mblk_t * esballoc (uchar_t * base, int size, int pri,
 *			   frtn_t * fr_rtnp);
 *
 *-ARGUMENTS:
 *	base		Address of driver-supplied data buffer.
 *
 *	size		Number of bytes in data buffer.
 *
 *	pri		Priority of allocation request (used to allocate the
 *			message and data blocks). Valid values are BPRI_LO,
 *			BPRI_MED, and BPRI_HI.
 *
 *	fr_rtnp		Pointer to the free-routine data structure.
 *
 *-DESCRIPTION:
 *	esballoc () creates a STREAMS message and attaches a driver-supplied
 *	data buffer in place of a STREAMS data buffer. It allocates a message
 *	and data block header only. The driver-supplied data buffer, pointed
 *	to by "base", is used as the data buffer for the message.
 *
 *	When freeb () is called to free the message, on the last reference to
 *	the message the driver's free-routine specified by the "free_func"
 *	field in the "free_rtn" structure is called with one argument
 *	(specified by the "free_arg" field) to free the data buffer.
 *
 *	Instead of requiring a specific number of arguments, the "free_arg"
 *	field is defined as type "char *". This way, the driver can pass a
 *	pointer to a structure if more than one argument is needed.
 *
 *	When the "free_func" function runs, interrupts from all STREAMS
 *	devices will be blocked. It has no user context and may not call any
 *	routine that sleeps. The function may not access any dynamically
 *	allocated data structures that might no longer exist when it runs.
 *
 *-RETURN VALUE:
 *	On success, a pointer to the newly allocated message block is
 *	returned. On failure, a NULL pointer is returned.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 *	allocb (), freeb (), free_rtn
 */

#if	__USE_PROTO__
mblk_t * (esballoc) (uchar_t * base, int size, int pri, frtn_t * fr_rtnp)
#else
mblk_t *
esballoc __ARGS ((base, size, pri, fr_rtnp))
uchar_t	      *	base;
int		size;
int		pri;
frtn_t	      *	fr_rtnp;
#endif
{
	mblk_t	      *	mblkp;

	ASSERT (base != NULL);
	ASSERT (size >= 0);
	ASSERT (pri == BPRI_LO || pri == BPRI_HI || pri == BPRI_LO);
	ASSERT (ATOMIC_FETCH_UCHAR (str_mem->sm_init) &&
		str_mem->sm_msg_lock != NULL);

	/*
	 * Is it permissible for a driver to specify a NULL fr_rtnp?
	 *
	 * I think that it seems entirely logical, but neither the DDI/DKI
	 * nor the STREAMS Programmer's Guide say anything about this.
	 */

	if ((mblkp = STRMEM_ALLOC (MSGB_SIZE (0), pri, KM_NOSLEEP)) != NULL) {
		/*
		 * Note that we don't bother setting up the "b_prev" and
		 * "b_next" members of the message block.
		 */

		mblkp->b_cont = NULL;
		mblkp->b_datap = MB_TO_DB (mblkp);
		mblkp->b_rptr = base;
		mblkp->b_wptr = base;

		/*
		 * We specially flag this message block as part of
		 * a triple. While there are other ways we could test
		 * this, for now we permit the possibility that a
		 * client might change the attachments of data blocks
		 * and message blocks perversely.
		 */

		mblkp->b_flag = MSGTRIPLE;
		mblkp->b_band = 0;

		mblkp->b_datap->db_base = base;
		mblkp->b_datap->db_lim = base + size;
		mblkp->b_datap->db_type = M_DATA;
		mblkp->b_datap->db_ref = 1;

		mblkp->b_datap->db_frtnp = fr_rtnp;
	}

	return mblkp;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	esbbcall	Call a function when an externally-supplied buffer
 *			can be allocated.
 *
 *-SYNOPSIS:
 *	#include <sys/types.h>
 *	#include <sys/stream.h>
 *
 *	toid_t esbbcall (int pri, int (* func) (), long arg);
 *
 *-ARGUMENTS:
 *	pri		Priority of the esballoc () allocation request
 *			(BPRI_LO, BPRI_MED or BPRI_HI).
 *
 *	func		Function to be called when a buffer becomes available.
 *
 *	arg		Argument to the function to be called when a buffer
 *			becomes available.
 *
 *-DESCRIPTION:
 *	esbbcall (), like bufcall (), serves as a timeout call of
 *	indeterminate duration. If esballoc () is unable to allocate a message
 *	block header and a data block header to go with the externally
 *	supplied data buffer, esbbcall () can be used to schedule the routine
 *	"func", to be called with the argument "arg" when memory becomes
 *	available.
 *
 *	When "func" runs, all interrupts from STREAMS devices will be blocked
 *	on the processor on which it is running. "func" will have no user
 *	context and may not call any function that sleeps.
 *
 *-RETURN VALUE:
 *	If successful, esbbcall () returns a non-zero value that identifies
 *	the scheduling request. This non-zero identifier may be passed to
 *	unbufcall () to cancel the request. If any failure occurs, esbbcall ()
 *	returns 0.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *	Even when "func" is called, esballoc () can still fail if another
 *	module or driver had allocated the memory before "func" was able to
 *	call esballoc ().
 *
 *-SEE ALSO:
 *	allocb (), bufcall (), esballoc (), itimeout (), unbufcall ()
 */

#if	__USE_PROTO__
toid_t (esbbcall) (int pri, ...)
#else
toid_t
esbbcall __ARGS ((pri))
int		pri;
#endif
{
	se_funcptr_t	func;
	long		arg;
	va_list		arglist;
	sevent_t      *	seventp;


	/*
	 * Before we do anything, we fetch the two other arguments. We use
	 * the variable-argument function convention to get around problems
	 * with implicit conversions introduced by having a prototype for
	 * this function visible.
	 *
	 * This is a generic problem with callbacks, since the shape of a
	 * function type includes the shapes of the function arguments, and
	 * because we want to pass some argument to the user-supplied function
	 * without knowing its type (and hence, we must not cause any implicit
	 * conversions on its value).
	 */

	va_start (arglist, pri);

	func = va_arg (arglist, se_funcptr_t);
	arg = va_arg (arglist, long);

	va_end (arglist);


	/*
	 * It may be that NULL is defined in such a way that it is not
	 * meaningful to compare it with a function pointer, so we just cast
	 * a zero to get the right kind of effect.
	 *
	 * We add some extra assertions below to check that "long" is of
	 * sufficient size to contain all likely parameter data items. For
	 * C, "likely" means the fundamental types, which has to include
	 * pointers to functions, pointers to void, and also pointers to
	 * structures since under C++ there is currently no guarantee to match
	 * the ISO C requirement that a pointer of type "void *" be able to
	 * hold the value of any pointer type without loss of information.
	 * [We don't consider member pointers here; they are pathological]
	 */

	ASSERT (sizeof (se_funcptr_t) <= sizeof (long) &&
		sizeof (_VOID *) <= sizeof (long) &&
		sizeof (selist_t *) <= sizeof (long));

	ASSERT (func != (se_funcptr_t) 0);
	ASSERT (pri == BPRI_LO || pri == BPRI_HI || pri == BPRI_LO);


	/*
	 * Here we *could* lock and test the available space in the STREAMS
	 * message heap to see whether sufficient space to satisfy the
	 * request has become available between the failed allocb () and
	 * the bufcall () request.
	 *
	 * The question is whether the extra time that the heap is locked is
	 * worth overcoming a delay for a freemsg () to trigger the bufcall ()
	 * later, especially given the fact that we'll have to unlock the
	 * heap to call the user's callback (introducing the opportunity for
	 * the newly-discovered space to vanish).
	 *
	 * All in all, once things get to the bufcall () stage there seems
	 * little reason to keep pushing the system, so we don't bother.
	 */

	/*
	 * First off, let's get ourselves an event cell. This is mutually
	 * dependent with the ID generation policy; if ID codes are stored in
	 * the event cells, we are free to allocate and discard from a general
	 * memory pool; if ID codes are not stored, then they must reflect
	 * some persistent attribute of the cell, which usually implies that
	 * cells come from some nonshrinking managed pool.
	 *
	 * Of course, a generative ID code scheme requires extra locking in
	 * the absence of atomic FETCH_AND_INCREMENT () operations. In this
	 * implementation, we get around this by using the list-head locks
	 * to guard the counter operations. [This has interesting implications
	 * for the design of a generic list-manipulation facility.]
	 */

#if _TOID_MEMBER
	if ((seventp = (sevent_t *) kmem_alloc (sizeof (sevent_t),
						KM_NOSLEEP)) == NULL)
		return 0;
#else
#error	Need an allocator for event cells that matches the ID policy
#endif

	seventp->se_arg = arg;
	seventp->se_func = func;
	seventp->se_size = MSGB_SIZE (0);

	return STORE_EVENT (seventp, pri);
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	flushband	Flush messages in a specified priority band.
 *
 *-SYNOPSIS:
 *	#include <sys/types.h>
 *	#include <sys/stream.h>
 *
 *	void flushband (queue_t * q, uchar_t pri, int flag);
 *
 *-ARGUMENTS:
 *	q		Pointer to the queue.
 *
 *	pri		Priority band of messages to be flushed.
 *
 *	flag		Determines messages to flush. Valid "flag" values are:
 *
 *			  FLUSHDATA	Flush only data messages (types
 *					M_DATA, M_DELAY, M_PROTO, and
 *					M_PCPROTO).
 *
 *			  FLUSHALL	Flush all messages.
 *
 *-DESCRIPTION:
 *	The flushband () function flushes messages associated with the
 *	priority band specified by "pri". If "pri" is 0, only normal and high
 *	priority messages are flushed. Otherwise, messages are flushed from
 *	the band "pri" according to the value of "flag".
 *
 *-RETURN VALUE:
 *	None.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	The caller cannot have the stream frozen [see freezestr ()] when
 *	calling this function.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 *	put (), flushq (), queue
 */

#if	__USE_PROTO__
void (flushband) (queue_t * q, uchar_t pri, int flag)
#else
void
flushband __ARGS ((q, pri, flag))
queue_t	      *	q;
uchar_t		pri;
int		flag;
#endif
{
	pl_t		prev_pl;
	qband_t	      *	qbandp;
	mblk_t	      *	mp;
	mblk_t	      *	next;
	ulong_t		flushsize;

	ASSERT (flag == FLUSHDATA || flag == FLUSHALL);

	if (pri == 0) {
		/*
		 * Since the priority-band flush algorithm is necessarily
		 * different from the regular one (due to flow control
		 * handling), we'll just forward this on to the regular
		 * version.
		 */

		flushq (q, flag);
		return;
	}

	prev_pl = QFREEZE_TRACE (q, "flushband");

	if ((qbandp = QUEUE_BAND (q, pri)) != NULL &&
	    (mp = qbandp->qb_first) != NULL) {

		flushsize = 0;

		do {
			/*
			 * Read the "b_next" member now in case we unlink this message
			 * from the queue.
			 */

			next = mp->b_next;

			ASSERT (mp->b_band == pri);

			if (flag == FLUSHALL || datamsg (mp->b_datap->db_type)) {

				if (mp->b_prev == NULL)
					q->q_first = next;
				else
					mp->b_prev->b_next = next;

				if (next == NULL)
					q->q_last = mp->b_prev;
				else
					next->b_prev = mp->b_prev;

				QBAND_DEQUEUE (qbandp, mp);


				/*
				 * Free the message, accumulating the total
				 * size of the data referred to by the
				 * message.
				 */

				flushsize += MSG_SIZE (mp);
				freemsg (mp);
			}
		} while ((mp = next) != qbandp->qb_last);


		/*
		 * Here we update the flow control parameters for the band
		 * now that we have accumulated all the necessary changes.
		 */

		QBAND_REDUCE (q, qbandp, flushsize);
	}


	/*
	 * Since we are in a path with QBAND_REDUCE (), check for QBACK before
	 * unfreezing the stream.
	 */

	{
		unsigned long	back;

		if ((back = q->q_flag & QBACK) != 0)
			q->q_flag &= ~ QBACK;

		QUNFREEZE_TRACE (q, prev_pl);

		if (back)
			QUEUE_BACKENAB (q);
	}
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	flushq		Flush messages on a queue.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	void flushq (queue_t * q, int flag);
 *
 *-ARGUMENTS:
 *	q		Pointer to the queue.
 *
 *	flag		Determines messages to flush. Valid "flag" values are:
 *
 *			  FLUSHDATA	Flush only data messages (types
 *					M_DATA, M_DELAY, M_PROTO, and
 *					M_PCPROTO).
 *
 *			  FLUSHALL	Flush all messages.
 *
 *-DESCRIPTION:
 *	flushq () frees messages on a queue by calling freemsg () for each
 *	message. If the queue's count falls below the low water mark and
 *	someone wants to write to the queue, the nearest upstream or
 *	downstream (as approriate) service procedure is enabled.
 *
 *-RETURN VALUE:
 *	None.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	The caller cannot have the stream frozen [see freezestr ()] when
 *	calling this function.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 *	put (), flushband (), freemsg (), putq (), queue
 */

#if	__USE_PROTO__
void (flushq) (queue_t * q, int flag)
#else
void
flushq __ARGS ((q, flag))
queue_t	      *	q;
int		flag;
#endif
{
	pl_t		prev_pl;
	mblk_t	      *	mp;
	mblk_t	      *	next;
	ulong_t		flushsize;

	ASSERT (flag == FLUSHDATA || flag == FLUSHALL);

	flushsize = 0;

	prev_pl = QFREEZE_TRACE (q, "flushq");

	/*
	 * High-priority messages are flushed as well as low-priority messages
	 * so we could presumably split this function into a two-part scan
	 * since the high-priority messages are at the front of the message
	 * list while the low-priority messages are at the end.
	 *
	 * However, this function is not called very often, and we expect
	 * band usage to be rare.
	 */

	for (mp = q->q_first ; mp != NULL ; mp = next) {
		/*
		 * Read the "b_next" member now in case we unlink this message
		 * from the queue.
		 */

		next = mp->b_next;

		if (mp->b_band == 0 &&
		    (flag == FLUSHALL || datamsg (mp->b_datap->db_type))) {

			if (mp->b_prev == NULL)
				q->q_first = next;
			else
				mp->b_prev->b_next = next;

			if (next == NULL)
				q->q_last = mp->b_prev;
			else
				next->b_prev = mp->b_prev;

			/*
			 * If the message is high-priority, then we skip the
			 * accumulation of the size.
			 */

			if (IS_PRI_MSG (mp)) {
				freemsg (mp);
				continue;
			}

			/*
			 * Free the message, accumulating the total size of
			 * the data referred to by the message.
			 */

			flushsize += MSG_SIZE (mp);
			freemsg (mp);
		}
	}

	QUEUE_REDUCE (q, flushsize);


	/*
	 * Since we are in a path with QUEUE_REDUCE (), check for QBACK before
	 * unfreezing the stream.
	 */

	{
		unsigned long	back;

		if ((back = q->q_flag & QBACK) != 0)
			q->q_flag &= ~ QBACK;

		QUNFREEZE_TRACE (q, prev_pl);

		if (back)
			QUEUE_BACKENAB (q);
	}
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	freeb		Free a message block.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	void freeb (mblk_t * bp);
 *
 *-ARGUMENTS:
 *     bp		Pointer to the message block to be deallocated.
 *
 *-DESCRIPTION:
 *	freeb () deallocates a message block. If the reference count of the
 *	"db_ref" member of the "datab" structure is greater than 1, freeb ()
 *	decrements the count and returns. Otherwise, if "db_ref" equals 1, it
 *	deallocates the message block and the corresponding data block and the
 *	corresponding data block and buffer.
 *
 *	If the data buffer to be freed was allocated with esballoc (), the
 *	driver is notified that the attached data buffer needs to be freed by
 *	calling the free-routine [see free_rtn] associated with the data
 *	buffer. Once this is accomplished, freeb () releases the STREAMS
 *	resources associated with the buffer.
 *
 *-RETURN VALUE:
 *	None.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 *	allocb (), dupb (), esballoc (), datab, free_rtn, msgb
 */

#if	__USE_PROTO__
void (freeb) (mblk_t * bp)
#else
void
freeb __ARGS ((bp))
mblk_t	      *	bp;
#endif
{
	size_t		size;
	pl_t		prev_pl;
	dblk_t	      *	datap;
	frtn_t	      *	frtnp = NULL;


	ASSERT (bp != NULL && bp->b_datap != NULL);
	ASSERT (bp->b_datap->db_ref > 0);

	datap = bp->b_datap;

	/*
	 * It is expected that almost all of the calls to freeb () will result
	 * in some memory being returned to the free pool, so to save space
	 * and ensure correctness we lock the heap now.
	 *
	 * This has the advantage of serializing some of the tests below for
	 * dealing with which parts of a message triple are free. Since the
	 * message and data parts of a triple could be being freed
	 * simultaneously by different contexts, this will guarantee that the
	 * data will be freed by exactly one of the contexts.
	 *
	 * Another note about memory management: if the message and other
	 * heaps are not kept separate, then some extra work needs to be done
	 * to wake up processes sleeping for memory and for calling the
	 * scheduled buffer events. Essentially, the systems need to map to
	 * some common code that correctly prioritizes access and runs at an
	 * appropriate time (possibly when some processor is about to return
	 * to user mode from the kernel, or is totally idle).
	 */

	prev_pl = LOCK (str_mem->sm_msg_lock, str_msg_pl);

	-- datap->db_ref;

	if (datap->db_ref > 0) {
		/*
		 * Ok, we don't need to free the data block and buffer, but
		 * we may still need to free the message block. This gets a
		 * little convoluted since we might be deallocating only part
		 * of a structure that was actually allocated as a whole by
		 * allocb () or esballoc ().
		 */

		if ((bp->b_flag & MSGTRIPLE) == 0) {

			STRMEM_FREE (bp, sizeof (mblk_t));
			goto free_done;
		}

		if ((datap = MB_TO_DB (bp))->db_ref > 0) {
			/*
			 * The data block part of the triple is not free,
			 * so we set a flag (tested when the data block is
			 * freed).
			 */

			bp->b_flag |= MSGFREE;

			goto free_done;
		}

		/*
		 * If the test above was false, it arranged for the message
		 * block to be associated with its triple partner so that
		 * we can proceed with the freeing process.
		 */

	} else if ((bp->b_flag & MSGTRIPLE) == 0) {
		/*
		 * The message block isn't from the same triple as the data
		 * block. We free the message block and then see whether the
		 * message block from the triple with the data block is free.
		 */


		STRMEM_FREE (bp, sizeof (mblk_t));

		bp = DB_TO_MB (datap);

		if ((bp->b_flag & MSGFREE) == 0) {
			/*
			 * The data block part of the triple is free but the
			 * message block isn't. The triple will be freed when
			 * the message block is freed.
			 */

			goto free_done;
		}
	}


	/*
	 * Now we deal with freeing the data portion of the message. First off
	 * we check whether or not the data block holds a user-supplied buffer
	 * or not.
	 */

	if (datap->db_base != DB_TO_DATA (datap)) {
		/*
		 * Calling the user-supplied free function now would be a
		 * really bad idea, since while all the conditions that are
		 * guaranteed to true while the function is called are true,
		 * we are still holding the lock on the heap. We record the
		 * information we need and do the job later.
		 */

		ASSERT (datap->db_frtnp != NULL);

		frtnp = datap->db_frtnp;
		size = 0;

	} else {
		ASSERT (datap->db_frtnp == NULL);

		size = datap->db_lim - datap->db_base;
	}


	/*
	 * At this point we know we have a message block and a data block to
	 * free, plus "size" bytes of data buffer.
	 */

	STRMEM_FREE (bp, MSGB_SIZE (size));

free_done:
	UNLOCK (str_mem->sm_msg_lock, plstr);

	/*
	 * Here we call any deferred user data buffer free function
	 */

	if (frtnp != NULL)
		(* frtnp->free_func) (frtnp->free_arg);

	(void) splx (prev_pl);
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	freemsg		Free a message.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	void freemsg (mblk_t * mp);
 *
 *-ARGUMENTS:
 *	mp		Pointer to the message to be freed.
 *
 *-DESCRIPTION:
 *	freemsg () frees all message blocks, data blocks, and data buffers
 *	associated with the message pointed to by "mp". freemsg () walks down
 *	the "b_cont" list [see msgb], calling freeb () for every message block
 *	in the message.
 *
 *-RETURN VALUE:
 *	None.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 *	freeb (), msgb
 */

#if	__USE_PROTO__
void (freemsg) (mblk_t * mp)
#else
void
freemsg __ARGS ((mp))
mblk_t	      *	mp;
#endif
{
	mblk_t	      *	next;

	ASSERT (mp != NULL);

	do {
		next = mp->b_cont;
		freeb (mp);
	} while ((mp = next) != NULL);
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	freezestr	Freeze the state of a stream.
 *
 *-SYNOPSIS:
 *	#include <sys/types.h>
 *	#include <sys/stream.h>
 *
 *	pl_t freezestr (queue_t * q);
 *
 *-ARGUMENTS:
 *	q		Pointer to a message queue.
 *
 *-DESCRIPTION:
 *	freezestr () sets the interrupt priority to "plstr" (if the current
 *	level is lower than "plstr" and the implementation requires that
 *	interrupts be blocked while the stream is frozen) and freezes the
 *	state of the stream containing the queue specified by "q". Freezing
 *	the stream prevents any further entries into open, close, put or
 *	service procedures on the stream and prevents any messages from being
 *	taken on or taken off any queues in the stream (except by the caller
 *	of freezestr ()). Freezing the stream does not automatically stop all
 *	functions that are running within the stream; functions will continue
 *	to run until they attempt to perform some operation which changes the
 *	state of the stream, at while point they will be forced to wait for
 *	the stream to be unfrozen by a call to unfreezestr ().
 *
 *	Drivers and modules must freeze the stream while they manipulate its
 *	queues directly. This includes searching the queues and for the
 *	duration of any calls to insq (), rmvq (), strqset (), and strqget ().
 *
 *-RETURN VALUE:
 *	freezestr () returns the previous interrupt priority level which is
 *	typically used in a subsequent call to unfreezestr ().
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Calling freezestr () to freeze a stream that is already frozen by the
 *	caller will result in deadlock.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *	freezestr () should be used sparingly as it is rarely necessary to
 *	freeze a stream (most modules do not need to manipulate their queues
 *	directly) and freezing a stream can have a significant negative impact
 *	on performance.
 *
 *-SEE ALSO:
 *	unfreezestr ()
 */

#if	__USE_PROTO__
pl_t (freezestr) (queue_t * q)
#else
pl_t
freezestr __ARGS ((q))
queue_t	      *	q;
#endif
{
	return QFREEZE_TRACE (q, "freezestr");
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	getq		Get the next message from a queue.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	mblk_t * getq (queue_t * q);
 *
 *-ARGUMENTS:
 *	q		Pointer to the queue from which the message is to be
 *			retrieved.
 *
 *-DESCRIPTION:
 *	getq () is used by service routines to retrieve queue messages. It
 *	gets the next available message from the top of the queue pointed to
 *	by "q". getq () handles flow control, restarting I/O that was blocked
 *	as needed.
 *
 *-RETURN VALUE:
 *	If there is a message to retrieve, getq () returns a pointer to it. If
 *	no message is queued, getq () returns a NULL pointer.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	The caller cannot have the stream frozen [see freezestr ()] when
 *	calling this function.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 *	bcanput (), canput (), putbq (), putq (), qenable (), rmvq ()
 */

#if	__USE_PROTO__
mblk_t * (getq) (queue_t * q)
#else
mblk_t *
getq __ARGS ((q))
queue_t	      *	q;
#endif
{
	pl_t		prev_pl;
	mblk_t	      *	mp;

	prev_pl = QFREEZE_TRACE (q, "getq");

	if ((mp = q->q_first) != NULL) {
		ulong_t		msgsize;

		/*
		 * There is a message. Dequeue it and adjust the flow-control
		 * parameters appropriately depending on whether the message
		 * is a priority message and also on the band of the message.
		 */

		if ((q->q_first = mp->b_next) == NULL) {

			q->q_last = NULL;
			QUEUE_DRAINED (q);
		} else
			mp->b_next->b_prev = NULL;

		/*
		 * Record the message band for the putq () enabling mechanism.
		 */

		q->q_lastband = mp->b_band;


		/*
		 * If the message is a priority-band message, we have to
		 * finish up the dequeueing operation by adjusting the
		 * "qb_first" and "qb_last" members of the "qband" structure.
		 *
		 * We do this below with the band flow-control management.
		 */

		ASSERT (mp->b_datap != NULL);

		if (! IS_PRI_MSG (mp)) {

			msgsize = MSG_SIZE (mp);

			if (mp->b_band > 0) {
				qband_t	      *	qbandp = QUEUE_BAND (q, mp->b_band);

				ASSERT (qbandp->qb_first == mp);

				QBAND_DEQUEUE (qbandp, mp);

				QBAND_REDUCE (q, qbandp, msgsize);
			} else
				QUEUE_REDUCE (q, msgsize);
		}
	} else {
		/*
		 * The queue is empty; we set a flag to indicate to putq ()
		 * that it should enable the queue when a message is put.
		 */

		q->q_flag |= QWANTR;
	}


	/*
	 * Since we are in a path with QUEUE_REDUCE (), check for QBACK before
	 * unfreezing the stream.
	 */

	{
		unsigned long	back;

		if ((back = q->q_flag & QBACK) != 0)
			q->q_flag &= ~ QBACK;

		QUNFREEZE_TRACE (q, prev_pl);

		if (back)
			QUEUE_BACKENAB (q);
	}

	return mp;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	insq		Insert a message into a queue.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	int insq (queue_t * q, mblk_t * emp, mblk_t * nmp);
 *
 *-ARGUMENTS:
 *	q		Pointer to the queue containing message "emp".
 *
 *	emp		Pointer to the existing message before which the new
 *			message is to be inserted.
 *
 *	nmp		Pointer to the new message to be inserted.
 *
 *-DESCRIPTION:
 *	insq () inserts a message into a queue. The message to be inserted,
 *	"nmp", is placed in the queue pointer to by "q", immediately before
 *	the message "emp". If "emp" is NULL, the new message is placed at the
 *	end of the queue. All flow control parameters are updated. The service
 *	procedure is scheduled to run unless disabled by a previous call to
 *	noenable ().
 *
 *	Messages are ordered in the queue based on their priority. If an
 *	attempt is made to insert a message out of order in the queue, then
 *	"nmp" is not enqueued.
 *
 *-RETURN VALUE:
 *	If "nmp" was successfully enqueued, insq () returns 1. Otherwise,
 *	insq () returns 0.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	The caller must have the stream frozen [see freezestr ()] when calling
 *	this function.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *	The insertion can fail if there is not enough memory to allocate the
 *	accounting data structures used with messages whose priority bands are
 *	greater than zero.
 *
 *	If "emp" is non-NULL, it must point to a message in the queue pointed
 *	to by "q", or a system panic could result.
 *
 *-SEE ALSO:
 *	freezestr (), getq (), putbq (), putq (), rmvq (), unfreezestr ()
 */

#if	__USE_PROTO__
int (insq) (queue_t * q, mblk_t * emp, mblk_t * nmp)
#else
int
insq __ARGS ((q, emp, nmp))
queue_t	      *	q;
mblk_t	      *	emp;
mblk_t	      *	nmp;
#endif
{
	qband_t	      *	qbandp;

	ASSERT (nmp != NULL);
	QFROZEN_TRACE (q, "insq");


	/*
	 * The code for high-priority messages has been factored out into a
	 * completely separate path since the legality tests are incompatible
	 * with the regular sequence, and in addition because such messages do
	 * not have their size accumulated in the flow control-parameters at
	 * all, at all. The execution path here is torturous enough without
	 * folding all the checks into a sequence with a common exit.
	 */

	if (IS_PRI_MSG (nmp)) {
		/*
		 * Since putq () is defined as forcing b_band to 0 for high-
		 * priority messages, we do the same.
		 */

		nmp->b_band = 0;

		if ((nmp->b_next = emp) == NULL) {

			if ((nmp->b_prev = q->q_last) == NULL) {

				q->q_first = nmp;
			} else {

				if (! IS_PRI_MSG (nmp->b_prev))
					return 0;

				nmp->b_prev->b_next = nmp;
			}

			q->q_last = nmp;
		} else {

			if ((nmp->b_prev = emp->b_prev) == NULL) {

				q->q_first = nmp;
			} else {

				if (! IS_PRI_MSG (nmp->b_prev))
					return 0;

				nmp->b_prev->b_next = nmp;
			}

			emp->b_prev = nmp;

		}


		/*
		 * A high-priority message causes a queue to be scheduled,
		 * even if a noenable () has been done. If we cannot schedule
		 * the queue for some reason (it is disabled with qprocsoff ()
		 * or has no service routine) that is not our concern.
		 */

		(void) QUEUE_TRYSCHED (q);
		return 1;
	}


	qbandp = NULL;		/* paranoia */

	if (nmp->b_band > 0 &&
	    (qbandp = QUEUE_BAND (q, nmp->b_band)) == NULL &&
	    (qbandp = QBAND_ALLOC (q, nmp->b_band)) == NULL) {
		/*
		 * Return failure if we were unable to allocate a necessary
		 * extra band structure.
		 */

		return 0;
	}


	if ((nmp->b_next = emp) == NULL) {

		if ((nmp->b_prev = q->q_last) == NULL) {

			q->q_first = nmp;
		} else {

			if (nmp->b_prev->b_band < nmp->b_band)
				return 0;

			nmp->b_prev->b_next = nmp;

		}

		q->q_last = nmp;

	} else {
		/*
		 * The check against "emp" isn't sufficient, since this
		 * request could be attempting to insert a message in the
		 * middle of a sequence of messages with a lower band.
		 */

		if (emp->b_band > nmp->b_band || IS_PRI_MSG (emp))
			return 0;

		if ((nmp->b_prev = emp->b_prev) == NULL) {

			q->q_first = nmp;
		} else {

			if (nmp->b_prev->b_band < nmp->b_band)
				return 0;

			emp->b_prev->b_next = nmp;
		}

		emp->b_prev = nmp;
	}


	/*
	 * Now update the flow control information and priority-band pointers
	 * after possibly scheduling the queue.
	 */

	if (QUEUE_CHECK_SCHED (q, nmp, qbandp)) {
		/*
		 * Keep the band pointers updated.
		 */

		if (qbandp->qb_first == emp)
			qbandp->qb_first = nmp;

		if (qbandp->qb_last == NULL || (qbandp->qb_last->b_next == emp))
			qbandp->qb_last = nmp;
	}

	return 1;			/* success ! */
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	linkb		Concatenate two message blocks.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	void linkb (mblk_t * mp1, mblk_t * mp2);
 *
 *-ARGUMENTS:
 *	mp1		Pointer to the message block to which "mp2" is to be
 *			added.
 *
 *	mp2		Pointer to the message to be added.
 *
 *-DESCRIPTION:
 *	linkb () appends the message "mp2" to the tail of message "mp1". The
 *	continuation pointer "b_cont" of the last message block in the first
 *	message is set to point to the second message.
 *
 *-RETURN VALUE:
 *	None.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 *	unlinkb (), msgb
 */

#if	__USE_PROTO__
void (linkb) (mblk_t * mp1, mblk_t * mp2)
#else
void
linkb __ARGS ((mp1, mp2))
mblk_t	      *	mp1;
mblk_t	      *	mp2;
#endif
{
	ASSERT (mp1 != NULL && mp2 != NULL);

	while (mp1->b_cont != NULL)
		mp1 = mp1->b_cont;

	mp1->b_cont = mp2;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	msgdsize	Return number of bytes of data in a message.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	int msgdsize (mblk_t * mp);
 *
 *-ARGUMENTS:
 *	mp		Pointer to the message to be evaluated.
 *
 *-DESCRIPTION:
 *	msgdsize () counts the number of bytes of data in the message pointed
 *	to by "mp". Only bytes included in message blocks of type "M_DATA" are
 *	included in the count.
 *
 *-RETURN VALUE:
 *	The number of bytes of data in the message.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 *	msgb
 */

#if	__USE_PROTO__
int (msgdsize) (mblk_t * mp)
#else
int
msgdsize __ARGS ((mp))
mblk_t	      *	mp;
#endif
{
	int	sum = 0;

	ASSERT (mp != NULL);

	do
		if (mp->b_datap->db_type == M_DATA)
			sum += mp->b_wptr - mp->b_rptr;
	while ((mp = mp->b_cont) != NULL);

	return sum;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	msgpullup	Concatenate bytes in a message,
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	mblk_t * msgpullup (mblk_t * mp, int len);
 *
 *-ARGUMENTS:
 *	mp		Pointer to the message whose blocks are to be
 *			concatenated.
 *
 *	len		Number of bytes to concatenate,
 *
 *-DESCRIPTION:
 *	msgpullup () concatenates and aligns the first "len" data bytes of the
 *	message pointed to by "mp", copying the data into a new message. The
 *	original message is unaltered. If "len" equals -1, all data are
 *	concatenated. If "len" bytes of the same message type cannot be found,
 *	msgpullup () fails and returns NULL.
 *
 *-RETURN VALUE:
 *	On success, a pointer to the new message is returned; on failure a
 *	NULL pointer is returned.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 *	allocb (), msgb
 */

#if	__USE_PROTO__
mblk_t * (msgpullup) (mblk_t * mp, int len)
#else
mblk_t *
msgpullup __ARGS ((mp, len))
mblk_t	      *	mp;
int		len;
#endif
{
	size_t		size;
	uchar_t		msgtype;
	mblk_t	      *	scan;

	ASSERT (mp != NULL);
	ASSERT (mp->b_datap != NULL);

	/*
	 * Something that is now particularly well-specified about this new
	 * routine (intended to replace pullupmsg () from SVR3.2 STREAMS and
	 * the SVR4 DDI/DKI) is what happens to the message data in the
	 * original message *after* the "len" bytes to be pulled up?
	 *
	 * Basically, without a usage example (which I haven't been able to
	 * find in the new SVR4 MP STREAMS programmer's guide) I can't tell
	 * whether the remaining data in the message should be duplicated for
	 * the new message or not. In order to make this routine a closer
	 * functional replacement for pullupmsg (), I'm tempted to say "yes",
	 * but it could be that the new message is a single standalone message
	 * block. Hence, I'm putting in a switch to select between the two
	 * interpretations I consider most likely.
	 *
	 * This is particularly important for "msgpullup (mp, 0)", which I'd
	 * like to make an error.
	 */

#define	MSGPULLUP_DUPMSG

	if (len == 0) {
#ifdef	MSGPULLUP_DUPMSG
		return dupmsg (mp);
#else
		return MSG_ALLOC (0, BPRI_MED, KM_NOSLEEP);
#endif
	}

	/*
	 * Begin by calculating the amount of data of the same message type
	 * that is present in the message.
	 */

	msgtype = mp->b_datap->db_type;
	size = 0;
	scan = mp;

	do
		size += scan->b_wptr - scan->b_rptr;
	while ((scan = scan->b_cont) != NULL &&
	       scan->b_datap->db_type == msgtype);

	if (len == -1)
		len = size;
	else if (len > size)
		return NULL;

	/*
	 * Note that it may be reasonable to interpret the definition of this
	 * function as implying that data can be shared between the old and
	 * new message blocks if the old data block does not need to be
	 * altered (ie, if the previous data was aligned and contiguous up to
	 * the length "len").
	 */

/* #define	MSGPULLUP_SHARE */

#ifdef	MSGPULLUP_SHARE
	if ((mp->b_wptr - mp->b_rptr) >= len &&
	    ((ulong_t) mp->b_rptr) & (sizeof (int) - 1)) == 0)
		return dupmsg (mp);
#endif	/* defined (MSGPULLUP_SHARE) */


	if ((scan = MSGB_ALLOC (len, BPRI_MED, KM_NOSLEEP)) == NULL)
		return NULL;

	/*
	 * There are some message attributes other than data that need to be
	 * copied from the source to the new message.
	 */

	scan->b_band = mp->b_band;
	scan->b_flag |= mp->b_flag & ~ MSGMASK_SYSTEM;


	/*
	 * Now transfer the data from the old space to the new space.
	 *
	 * More unspecified behaviour deals with how zero-length blocks are
	 * to be treated. The following loop is coded specifically to skip
	 * over zero-length message blocks in the source that follow the
	 * "len" copied bytes of data.
	 */

#define	MSGPULLUP_DUPMSG

	while (mp != NULL) {
		size_t		blklen = mp->b_wptr - mp->b_rptr;

		if (blklen > 0)
			memcpy (scan->b_wptr, mp->b_rptr, blklen);

		scan->b_wptr += blklen;
		len -= blklen;


		if (blklen > len) {
			/*
			 * Copy a partial block. Since this must also be the
			 * last message block in the source message whose
			 * data are being moved to the new message, here is a
			 * good place to decide on the dispensation of the
			 * remaining data.
			 */

#ifdef	MSGPULLUP_DUPMSG
			if ((scan->b_cont = dupmsg (mp)) == NULL) {
				/*
				 * The remaining data could not be duplicated,
				 * so we have to fail the call overall.
				 */

				freeb (scan);
				return NULL;
			}
#endif
			break;
		}

		mp = mp->b_cont;
	}


	return scan;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	noenable	Prevent a queue from being scheduled.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	void noenable (queue_t * q);
 *
 *-ARGUMENTS:
 *	q		Pointer to the queue.
 *
 *-DESCRIPTION:
 *	The noenable () function prevents the service routine of the queue
 *	pointed to by "q" from being scheduled for service by insq (),
 *	putbq (), or putq () when enqueuing a message that is not a high
 *	priority message. This restriction can be lifted with the enableok ()
 *	function.
 *
 *	noenable () does not prevent the queue's service routine from being
 *	scheduled when a high priority message is enqueued, or by an explicit
 *	call to qenable ().
 *
 *-RETURN VALUE:
 *	None.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	The caller cannot have the stream frozen [see freezestr ()] when
 *	calling this function.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 *	enableok (), insq (), putbq (), putq (), qenable (), queue
 */

#if	__USE_PROTO__
void (noenable) (queue_t * q)
#else
void
noenable __ARGS ((q))
queue_t	      *	q;
#endif
{
	pl_t		prev_pl;

	prev_pl = QFREEZE_TRACE (q, "noenable");

	q->q_flag |= QNOENB;

	QUNFREEZE_TRACE (q, prev_pl);
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	OTHERQ		Get pointer to queue's partner queue.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	queue_t * OTHERQ (queue_t * q);
 *
 *-ARGUMENTS:
 *	q		Pointer to the queue.
 *
 *-DESCRIPTION:
 *	The OTHERQ () function returns a pointer to the other of the two
 *	queue structures that make up an instance of a STREAMS module or
 *	driver. If "q" points to the read queue the write queue will be
 *	returned, and vice versa.
 *
 *-RETURN VALUE:
 *	OTHERQ () returns a pointer to the queue's partner.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 *	RD (), WR ()
 */

#if	__USE_PROTO__
queue_t * (OTHERQ) (queue_t * q)
#else
queue_t *
OTHERQ __ARGS ((q))
queue_t	      *	q;
#endif
{
	return OTHERQ (q);
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	pcmsg		Test whether a message is a priority control message.
 *
 *-SYNOPSIS:
 *	#include <sys/types.h>
 *	#include <sys/stream.h>
 *	#include <sys/ddi.h>
 *
 *	int pcmsg (uchar_t type);
 *
 *-ARGUMENTS:
 *	type		The type of message to be tested.
 *
 *-DESCRIPTION:
 *	The pcmsg () function tests the type of message to determine if it is
 *	a priority control message (also known as a high priority message).
 *	The "db_type" field of the "datab" structure contains the message
 *	type. This field may be accessed through the message block using
 *	"mp->b_datap->db_type".
 *
 *-RETURN VALUE:
 *	pcmsg () returns 1 if the message is a priority control message and
 *	0 if the message is any other type.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 *	allocb (), datab, msgb, messages
 */

#if	__USE_PROTO__
int (pcmsg) (uchar_t type)
#else
int
pcmsg __ARGS ((type))
uchar_t		type;
#endif
{
	return pcmsg (type);
}


/*
 *-STATUS:
 *	Compatibility (pre-MP DDI/DKI)
 *
 *-NAME:
 *	pullupmsg	Concatenate bytes in a message.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	int pullupmsg (mblk_t * mp, int len);
 *
 *-ARGUMENTS:
 *	mp		Pointer to the message whose blocks are to be
 *			concatenated.
 *
 *	len		Number of bytes to concatenate.
 *
 *-DESCRIPTION:
 *	pullupmsg () tries to combine multiple data blocks into a single
 *	block. pullupmsg () concatenates and aligns the first "len" data bytes
 *	of the message pointed to by "mp". If "len" equals -1, all data is
 *	concatenated. If "len" bytes of the same message type cannot be found,
 *	pullupmsg () fails and returns 0.
 *
 *-RETURN VALUE:
 *	On success, 1 is returned; on failure, 0 is returned.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *	This function is provided for compatibility with versions of the
 *	DDI/DKI prior to the System V, Release 4 Multiprocessor edition. Calls
 *	to this function should be replaced by calls to the msgpullup ()
 *	function instead.
 *
 *-SEE ALSO:
 *	allocb (), msgpullup ()
 */

#if	__USE_PROTO__
int (pullupmsg) (mblk_t * mp, int len)
#else
int
pullupmsg __ARGS ((mp, len))
mblk_t	      *	mp;
int		len;
#endif
{
	mblk_t	      *	newmsg;

	ASSERT (mp != NULL);
	ASSERT (mp->b_datap != NULL);

	/*
	 * If the first block of the old message is sufficiently large to
	 * encompass the pullup request, then we need do no work. It is not
	 * clear whether the new msgpullup () function allows this simple
	 * optimisation, so we test for this here to guarantee it.
	 */

	if (((mp->b_wptr - mp->b_rptr) > len ||
		(len == -1 && mp->b_cont == NULL)) &&
	    ((ulong_t) mp->b_rptr & (sizeof (int) - 1)) == 0) {
		/*
		 * Do nothing and return success. Note that the alignment test
		 * above is specific to the i386 implementation.
		 */

		return 1;
	}


	/*
	 * Now use the new msgpullup () function to perform the harder task
	 * of concatenating the message, and then perform some subterfuge to
	 * ensure that the new message replaces the old message's storage.
	 *
	 * This requires some delicate manipulations to avoid disrupting
	 * the flags and whatnot that support the message-triple storage
	 * system.
	 */

	if ((newmsg = msgpullup (mp, len)) == NULL)
		return 0;


	{
		dblk_t	      *	temp = mp->b_datap;
		mp->b_datap = newmsg->b_datap;
		newmsg->b_datap = temp;
	} {
		mblk_t	      *	temp = mp->b_cont;
		mp->b_cont = newmsg->b_cont;
		newmsg->b_cont = temp;
	}

	mp->b_rptr = newmsg->b_rptr;
	mp->b_wptr = newmsg->b_wptr;

	/*
	 * Since we exchanged the "b_datap" and "b_cont" members of the
	 * message blocks, freeing the "new" message pointer will free those
	 * elements of the original message that have been made redundant by
	 * the pull-up operation, leaving the original message block intact
	 * but pointing to the rearranged data.
	 */

	freemsg (newmsg);
	return 1;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	put		Call a put procedure.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	void put (queue_t * q, mblk_t * mp);
 *
 *-ARGUMENTS:
 *	q		Pointer to a message queue.
 *
 *	mp		Pointer to the message block being passed.
 *
 *-DESCRIPTION:
 *	put () calls the "put" procedure for the queue specified by "q",
 *	passing it the arguments "q" and "mp". It is typically used by a
 *	driver or module to call its own "put" procedure so that the proper
 *	accounting is done in the stream.
 *
 *-RETURN VALUE:
 *	None.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	The caller cannot have the stream frozen [see freezestr ()] when
 *	calling this function.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *	DDI/DKI conforming drivers and modules are no longer permitted to call
 *	"put" procedures directly, but must call through the appropriate
 *	STREAMS utility function - for example, put (), putnext (), putctl (),
 *	putnextctl (), or qreply (). "put (q, mp)" is provided as a DDI/DKI-
 *	conforming equivalent to a direct call to a "put" procedure, which is
 *	no longer allowed.
 *
 *-SEE ALSO:
 *	putctl (), putctl1 (), putnext (), putnextctl (), putnextctl1 (),
 *	qreply ()
 */

#if	__USE_PROTO__
void (put) (queue_t * q, mblk_t * mp)
#else
void
put __ARGS ((q, mp))
queue_t	      *	q;
mblk_t	      *	mp;
#endif
{
	pl_t		prev_pl;

	ASSERT (mp != NULL);
	ASSERT (mp->b_datap != NULL);

	/*
	 * If a message is put to a queue that either has no put procedure at
	 * all, or has been disabled with qprocsoff (), then we discard the
	 * message and return without doing anything.
	 */

	prev_pl = QFREEZE_TRACE (q, "put");

	if ((q->q_active == 0 && (q->q_flag & QPROCSOFF) != 0) ||
	    q->q_qinfo->qi_putp == NULL)
		cmn_err (CE_WARN, "put () to disabled queue/NULL putp");
	else
		QUEUE_PUT (q, mp, prev_pl);

	QUNFREEZE_TRACE (q, prev_pl);
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	putbq		Place a message at the head of a queue.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	int putbq (queue_t * q, mblk_t * mp);
 *
 *-ARGUMENTS:
 *	q		Pointer to the queue.
 *
 *	mp		Pointer to the message.
 *
 *-DESCRIPTION:
 *	putbq () puts a message back at the head of the queue. If messages of
 *	a higher priority are present on the queue, then "bp" is placed at the
 *	head of its corresponding priority band.
 *
 *	All flow control parameters are updated. The queue's service routine
 *	is scheduled if it has not been disabled by a previous call to
 *	noenable ().
 *
 *	putbq () is usually called when bcanputnext () or canputnext ()
 *	determines that the message cannot be passed on to the next stream
 *	component.
 *
 *-RETURN VALUE:
 *	putbq () returns 1 on success and 0 on failure.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	The caller cannot have the stream frozen [see freezestr ()] when
 *	calling this function.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *	putbq () can fail if there is not enough memory to allocate the
 *	accounting data structures used with messages whose priority bands are
 *	greater than zero.
 *
 *	High priority messages should never be put back on a queue from within
 *	a service routine.
 *
 *-SEE ALSO:
 *	bcanputnext (), canputnext (), getq (), insq (), putq (), rmvq (),
 *	msgb, queue
 */

#if	__USE_PROTO__
int (putbq) (queue_t * q, mblk_t * mp)
#else
int
putbq __ARGS ((q, mp))
queue_t	      *	q;
mblk_t	      *	mp;
#endif
{
	pl_t		prev_pl;
	qband_t	      *	qbandp;

	ASSERT (mp != NULL);
	ASSERT (mp->b_datap != NULL);
	ASSERT (! pcmsg (mp->b_datap->db_type));

	prev_pl = QFREEZE_TRACE (q, "putbq");

	/*
	 * The code for high-priority messages has been factored out into a
	 * completely separate path because such messages do not have their
	 * size accumulated in the flow control-parameters at all, at all, and
	 * because the location where such messages are to be queued is
	 * determined in one step without search.
	 */

	if (IS_PRI_MSG (mp)) {
		/*
		 * Since putq () is defined as forcing b_band to 0 for high-
		 * priority messages, we do the same.
		 */

		if ((mp->b_next = q->q_first) == NULL)
			q->q_last = mp;
		else
			mp->b_next->b_prev = mp;

		mp->b_prev = NULL;
		mp->b_band = 0;
		q->q_first = mp;


		/*
		 * A high-priority message causes a queue to be scheduled
		 * even if a noenable () has been performed. For putbq (),
		 * this is true whether or not the condition set by getq ()
		 * returning null is true or not, which is the reason for the
		 * proscription against calling this routine for a high-
		 * priority message within a service procedure.
		 */

		/*
		 *!!! We need help from the STREAMS service-routine scheduler
		 *!!! to build a testable assertion for detecting when we are
		 *!!! called within the context of a service routine.
		 */

		(void) QUEUE_TRYSCHED (q);
		goto alldone;
	}


	qbandp = NULL;		/* paranoia */

	if (mp->b_band > 0 &&
	    (qbandp = QUEUE_BAND (q, mp->b_band)) == NULL &&
	    (qbandp = QBAND_ALLOC (q, mp->b_band)) == NULL)
		return 0;
	else if (QUEUE_CHECK_SCHED (q, mp, qbandp)) {
		/*
		 * Deal with band pointer maintenance now.
		 */

		if ((mp->b_next = qbandp->qb_first) != NULL) {
			/*
			 * We directly know where we want to put the message.
			 */

			if ((mp->b_prev = mp->b_next->b_prev) == NULL)
				q->q_first = mp;
			else
				mp->b_prev->b_next = mp;

			mp->b_next->b_prev = mp;

			qbandp->qb_first = mp;

			goto alldone;
		}


		/*
		 * This is the only message in this band. We will need to
		 * search for the correct position to queue this message.
		 */

		qbandp->qb_last = qbandp->qb_first = mp;
	}


	/*
	 * Now use a brute-force search for the position. We could use the
	 * band structures to speed this up, but we anticipate that there will
	 * be few band or high-priority messages, so that a brute-force search
	 * will be suitably short.
	 */

	QUEUE_PLACE_MSG (q, mp, mp->b_band);

alldone:
	QUNFREEZE_TRACE (q, prev_pl);
	return 1;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	putctl		Send a control message to a queue.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	int putctl (queue_t * q, int type);
 *
 *-ARGUMENTS:
 *	q		Pointer to the queue to which the message is to be
 *			sent.
 *
 *	type		Message type (must be control).
 *
 *-DESCRIPTION:
 *	putctl () tests the "type" argument to make sure a data type has not
 *	been specified, and then attempts to allocate a message block.
 *	putctl () fails if "type" is M_DATA, M_PROTO, or M_PCPROTO, or if a
 *	message block cannot be allocated. If successful, putctl () calls the
 *	"put" routine of the queue pointed to by "q", passing it the allocated
 *	message.
 *
 *-RETURN VALUE:
 *	On success, 1 is returned. Otherwise, if "type" is a data type, or if
 *	a message block cannot be allocated, 0 is returned.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	The caller cannot have the stream frozen [see freezestr ()] when
 *	calling this function.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *	The "q" argument to putctl () and putnextctl () may not reference
 *	"q_next" (eg, an argument of "q->q_next" is erroneous on a
 *	multiprocessor and is disallowed by the DDI/DKI).
 *	"putnextctl (q, type)" is provided as a multiprocessor-safe equivalent
 *	to the common call "putctl (q->q_next, type)" which is no longer
 *	allowed.
 *
 *-SEE ALSO:
 *	put (), putctl1 (), putnextctl (), putnextctl1 ()
 */

#if	__USE_PROTO__
int (putctl) (queue_t * q, int type)
#else
int
putctl __ARGS ((q, type))
queue_t	      *	q;
int		type;
#endif
{
	mblk_t	      *	ctlmsg;

	QUEUE_TRACE (q, "putctl");

	/*
	 * We cannot use datamsg () to test the type because datamsg () is
	 * specified as testing for M_DATA in addition to M_DATA, M_PROTO, and
	 * M_PCPROTO.
	 */

	if ((type & (M_PRI - 1)) <= M_PROTO ||
	    (ctlmsg = MSGB_ALLOC (0, BPRI_HI, KM_NOSLEEP)) == NULL)
		return 0;

	ctlmsg->b_datap->db_type = type;
	put (q, ctlmsg);

	return 1;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	putctl1		Send a control message with a one-byte parameter to a
 *			queue.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	int putctl1 (queue_t * q, int type, int param);
 *
 *-ARGUMENTS:
 *	q		Pointer to the queue to which the message is to be
 *			sent.
 *
 *	type		Message type (must be control).
 *
 *	param		One-byte parameter.
 *
 *-DESCRIPTION:
 *	putctl1 (), like putctl (), tests the "type" argument to make sure a
 *	data type has not been specified, and attempts to allocate a message
 *	block. The "param" parameter can be used, for example, to specify the
 *	signal number when an "M_PCSIG" message is being sent. putctl1 ()
 *	fails if "type" is M_DATA, M_PROTO, or M_PCPROTO, or if a message
 *	block cannot be allocated. If successful, putctl1 () calls the "put"
 *	routine of the queue pointed to by "q", passing it the allocated
 *	message.
 *
 *-RETURN VALUE:
 *	On success, 1 is returned. Otherwise, if "type" is a data type, or if
 *	a message block cannot be allocated, 0 is returned.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	The caller cannot have the stream frozen [see freezestr ()] when
 *	calling this function.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *	The "q" argument to putctl1 () and putnextctl1 () may not reference
 *	"q_next" (eg, an argument of "q->q_next" is erroneous on a
 *	multiprocessor and is disallowed by the DDI/DKI).
 *	"putnextctl1 (q, type)" is provided as a multiprocessor-safe
 *	equivalent to the common call "putctl1 (q->q_next, type)" which is no
 *	longer allowed.
 *
 *-SEE ALSO:
 *	put (), putctl (), putnextctl (), putnextctl1 ()
 */

#if	__USE_PROTO__
int (putctl1) (queue_t * q, int type, int param)
#else
int
putctl1 __ARGS ((q, type, param))
queue_t	      *	q;
int		type;
int		param;
#endif
{
	mblk_t	      *	ctlmsg;

	QUEUE_TRACE (q, "putctl1");

	/*
	 * We cannot use datamsg () to test the type because datamsg () is
	 * specified as testing for M_DATA in addition to M_DATA, M_PROTO, and
	 * M_PCPROTO.
	 */

	if ((type & (M_PRI - 1)) <= M_PROTO ||
	    (ctlmsg = MSGB_ALLOC (1, BPRI_HI, KM_NOSLEEP)) == NULL)
		return 0;

	ctlmsg->b_datap->db_type = type;
	* ctlmsg->b_wptr ++ = (unsigned char) param;

	put (q, ctlmsg);

	return 1;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	putnext		Send a message to the next queue.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	int putnext (queue_t * q, mblk_t * mp);
 *
 *-ARGUMENTS:
 *	q		Pointer to the queue from which the message "mp" will
 *			be sent.
 *
 *	mp		Pointer to the message to be passed.
 *
 *-DESCRIPTION:
 *	The putnext () function is used to pass a message to the "put" routine
 *	of the next queue ("q->q_next") in the stream.
 *
 *-RETURN VALUE:
 *	Ignored.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	The caller cannot have the stream frozen [see freezestr ()] when
 *	calling this function.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 *	putnextctl (), putnextctl1 ()
 */

#if	__USE_PROTO__
int (putnext) (queue_t * q, mblk_t * mp)
#else
int
putnext __ARGS ((q, mp))
queue_t	      *	q;
mblk_t	      *	mp;
#endif
{
	QUEUE_TRACE (q, "putnext");

	QUEUE_PUTNEXT (q, mp);

	return 0;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	putnextctl	Send a control message to a queue.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	int putnextctl (queue_t * q, int type);
 *
 *-ARGUMENTS:
 *	q		Pointer to the queue from which the message is to be
 *			sent.
 *
 *	type		Message type (must be control type).
 *
 *-DESCRIPTION:
 *	putnextctl () tests the "type" argument to make sure a data type has
 *	been specified, and then attempts to allocate a message block.
 *	putnextctl () fails if "type" is M_DATA, M_PROTO, or M_PCPROTO, or
 *	if a message block cannot be allocated. If successful, putnextctl ()
 *	calls the "put" procedure of the queue pointed to by "q->q_next",
 *	passing it the allocated message.
 *
 *-RETURN VALUE:
 *	Upon successful completion, putnextctl () returns 1. If "type" is a
 *	data type, or if a message block cannot be allocated, 0 is returned.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	The caller cannot have the stream frozen [see freezestr ()] when
 *	calling this function.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *	The "q" argument to putctl () and putnextctl () may not reference
 *	"q_next" (eg, an argument of "q->q_next" is erroneous on a
 *	multiprocessor and is disallowed by the DDI/DKI).
 *	"putnextctl (q, type)" is provided as a multiprocessor-safe equivalent
 *	to the common call "putctl (q->q_next, type)" which is no longer
 *	allowed.
 *
 *-SEE ALSO:
 *	put (), putctl (), putctl1 (), putnextctl1 ()
 */

#if	__USE_PROTO__
int (putnextctl) (queue_t * q, int type)
#else
int
putnextctl __ARGS ((q, type))
queue_t	      *	q;
int		type;
#endif
{
	mblk_t	      *	ctlmsg;

	QUEUE_TRACE (q, "putnextctl");

	/*
	 * We cannot use datamsg () to test the type because datamsg () is
	 * specified as testing for M_DATA in addition to M_DATA, M_PROTO, and
	 * M_PCPROTO.
	 */

	if ((type & (M_PRI - 1)) <= M_PROTO ||
	    (ctlmsg = MSGB_ALLOC (0, BPRI_HI, KM_NOSLEEP)) == NULL)
		return 0;

	ctlmsg->b_datap->db_type = type;

	QUEUE_PUTNEXT (q, ctlmsg);

	return 1;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	putnextctl1	Send a control message with a one-byte parameter to a
 *			queue.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	int putctl1 (queue_t * q, int type, int param);
 *
 *-ARGUMENTS:
 *	q		Pointer to the queue from which the message is to be
 *			sent.
 *
 *	type		Message type (must be control).
 *
 *	param		One-byte parameter.
 *
 *-DESCRIPTION:
 *	putnextctl1 () tests the "type" argument to make sure a data type has
 *	not been specified, and attempts to allocate a message block.
 *	putnext ctl1 () fails if "type" is M_DATA, M_PROTO, or M_PCPROTO, or
 *	if a message block cannot be allocated. If successful, putctl1 ()
 *	calls the "put" routine of the queue pointed to by "q->q_next",
 *	passing it the allocated message with the one byte parameter specified
 *	by "param".
 *
 *-RETURN VALUE:
 *	Upon successful completion, putnextctl1 () returns 1. If "type" is a
 &	data type, or if a message block cannot be allocated, 0 is returned.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	The caller cannot have the stream frozen [see freezestr ()] when
 *	calling this function.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *	The "q" argument to putctl1 () and putnextctl1 () may not reference
 *	"q_next" (eg, an argument of "q->q_next" is erroneous on a
 *	multiprocessor and is disallowed by the DDI/DKI).
 *	"putnextctl1 (q, type)" is provided as a multiprocessor-safe
 *	equivalent to the common call "putctl1 (q->q_next, type)" which is no
 *	longer allowed.
 *
 *-SEE ALSO:
 *	put (), putctl (), putctl1 (), putnextctl ()
 */

#if	__USE_PROTO__
int (putnextctl1) (queue_t * q, int type, int param)
#else
int
putnextctl1 __ARGS ((q, type, param))
queue_t	      *	q;
int		type;
int		param;
#endif
{
	mblk_t	      *	ctlmsg;

	QUEUE_TRACE (q, "putnextctl1");

	/*
	 * We cannot use datamsg () to test the type because datamsg () is
	 * specified as testing for M_DATA in addition to M_DATA, M_PROTO, and
	 * M_PCPROTO.
	 */

	if ((type & (M_PRI - 1)) <= M_PROTO ||
	    (ctlmsg = MSGB_ALLOC (1, BPRI_HI, KM_NOSLEEP)) == NULL)
		return 0;

	ctlmsg->b_datap->db_type = type;
	* ctlmsg->b_wptr ++ = (unsigned char) param;

	QUEUE_PUTNEXT (q, ctlmsg);

	return 1;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	putq		Put a message to a queue.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	int putq (queue_t * q, mblk_t  * mp);
 *
 *-ARGUMENTS:
 *	q		Pointer to the queue.
 *
 *	mp		Pointer to the message.
 *
 *-DESCRIPTION:
 *	putq () is used to put messages on a queue after the "put" routine has
 *	finished processing the message. The message is placed after any other
 *	messages of the same priority, and flow control parameters are
 *	updated. The queue's service routine is scheduled if it has not been
 *	disabled by a previous call to noenable ().
 *
 *-RETURN VALUE:
 *	putq () returns 1 on success and 0 on failure.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	The caller cannot have the stream frozen [see freezestr ()] when
 *	calling this function.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *	putq () can fail if there is not enough memory to allocate the
 *	accounting data structures used with messages whose priority bands are
 *	greater than zero.
 *
 *-SEE ALSO:
 *	getq (), insq (), putbq (), rmvq (), msgb, queue
 */

#if	__USE_PROTO__
int (putq) (queue_t * q, mblk_t * mp)
#else
int
putq __ARGS ((q, mp))
queue_t	      *	q;
mblk_t	      *	mp;
#endif
{
	pl_t		prev_pl;
	qband_t	      *	qbandp;

	ASSERT (mp != NULL);
	ASSERT (mp->b_datap != NULL);

	prev_pl = QFREEZE_TRACE (q, "putq");

	if (IS_PRI_MSG (mp)) {
		/*
		 * Since putq () is defined as forcing b_band to 0 for high-
		 * priority messages, we do the same. In addition, we also
		 * always schedule the queue. No flow-control parameters are
		 * updated for high-priority messages.
		 */

		mp->b_band = 0;
		(void) QUEUE_TRYSCHED (q);

		/*
		 * We need to flow into the code that finds the correct
		 * insertion point for high-priority and band messages.
		 */
	} else if (qbandp = NULL, mp->b_band > 0 &&
		   (qbandp = QUEUE_BAND (q, mp->b_band)) == NULL &&
		   (qbandp = QBAND_ALLOC (q, mp->b_band)) == NULL)
		return 0;
	else if (QUEUE_CHECK_SCHED (q, mp, qbandp)) {
		/*
		 * Deal with band pointer maintenance now.
		 */

		if ((mp->b_next = qbandp->qb_first) != NULL) {
			/*
			 * We directly know where we want to put the message.
			 */

			if ((mp->b_prev = mp->b_next->b_prev) == NULL)
				q->q_first = mp;
			else
				mp->b_prev->b_next = mp;

			mp->b_next->b_prev = mp;

			qbandp->qb_first = mp;

			goto alldone;
		}

		/*
		 * This is the only message in this band. We will need to
		 * search for the correct position to queue this message.
		 */

		qbandp->qb_last = qbandp->qb_first = mp;
	} else {
		/*
		 * Since we will always be inserting at the end of the queue,
		 * we directly know where we will be inserting the new
		 * message.
		 */

		mp->b_next = NULL;

		if ((mp->b_prev = q->q_last) == NULL)
			q->q_first = mp;
		else
			mp->b_prev->b_next = mp;

		q->q_last = mp;

		goto alldone;
	}


	/*
	 * Since we want to insert after other messages with the same band
	 * number, we adjust the band down by one. This introduces a special
	 * case for high-priority messages, which we want to queue before all
	 * normal messages. Since we define high-priority messages as having
	 * a band number of 0, subtracting one makes the band value wrap
	 * around, yielding the desired behaviour.
	 */

	QUEUE_PLACE_MSG (q, mp, mp->b_band - 1);

alldone:
	QUNFREEZE_TRACE (q, prev_pl);
	return 1;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	qenable		Schedule a queue's service routine to be run.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	void qenable (queue_t * q);
 *
 *-ARGUMENTS:
 *	q		Pointer to the queue.
 *
 *-DESCRIPTION:
 *	qenable () puts the queue pointed to by "q" on the linked list of
 *	STREAMS routines that are ready to be called by the STREAMS scheduler.
 *	qenable () works regardless of whether the service routine has been
 *	disabled by a previous call to noenable ().
 *
 *-RETURN VALUE:
 *	None.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	The caller cannot have the stream frozen [see freezestr ()] when
 *	calling this function.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 *	enableok (), noenable (), queue
 */

#if	__USE_PROTO__
void (qenable) (queue_t * q)
#else
void
qenable __ARGS ((q))
queue_t	      *	q;
#endif
{
	pl_t		prev_pl;

	prev_pl = QFREEZE_TRACE (q, "qenable");

	(void) QUEUE_TRYSCHED (q);

	QUNFREEZE_TRACE (q, prev_pl);
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	qprocsoff	Disable put and service procedures.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	void qprocsoff (queue_t * rq);
 *
 *-ARGUMENTS:
 *	rq		Pointer to a read queue.
 *
 *-DESCRIPTION:
 *	qprocsoff () disables the "put" and "service" routines of the driver
 *	or module whose read queue is pointed to by "rq". When the routines
 *	are disabled in a module, messages flow around the module as if it
 *	were not present in the stream.
 *
 *	qprocsoff () must be called by the "close" routine of a driver or
 *	module before deallocating any resources on which the driver/module's
 *	put and service procedures depend.
 *
 *	qprocsoff () will remove the queue's service routines from the list of
 *	service routines to be run and waits until any concurrent "put" or
 &	"service" routines are finished.
 *
 *-RETURN VALUE:
 *	None.
 *
 *-LEVEL:
 *	Base level only.
 *
 *-NOTES:
 *	May sleep.
 *
 *	The caller cannot have the stream frozen [see freezestr ()] when
 *	calling this function.
 *
 *	Driver-defined basic locks and read/write locks may not be held across
 *	calls to this function.
 *
 *	Driver-defined sleep locks may be held across calls to this function.
 *
 *-SEE ALSO:
 *	qprocson ()
 */

#if	__USE_PROTO__
void (qprocsoff) (queue_t * rq)
#else
void
qprocsoff __ARGS ((rq))
queue_t	      *	rq;
#endif
{
	pl_t		prev_pl;
	int		dosleep;
	queue_t	      *	wq;


try_rprocsoff:
	prev_pl = QFREEZE_TRACE (rq, "qprocsoff");

	rq->q_flag |= QPROCSOFF;
	if ((dosleep = rq->q_active > 0) != 0)
		(void) LOCK (str_mem->sm_proc_lock, plstr);

	QUNFREEZE_TRACE (rq, prev_pl);

	if (dosleep) {
		SV_WAIT (str_mem->sm_proc_sv, prilo, str_mem->sm_proc_lock);

		goto try_rprocsoff;
	}

	/*
	 * Now do the same for the write side.
	 */

	ASSERT ((rq->q_flag & QREADR) != 0);

	wq = W (rq);

try_wprocsoff:
	prev_pl = QFREEZE_TRACE (wq, "qprocsoff");

	wq->q_flag |= QPROCSOFF;
	if ((dosleep = wq->q_active > 0) != 0)
		(void) LOCK (str_mem->sm_proc_lock, plstr);


	QUNFREEZE_TRACE (wq, prev_pl);

	if (dosleep) {
		SV_WAIT (str_mem->sm_proc_sv, prilo, str_mem->sm_proc_lock);

		goto try_wprocsoff;
	}
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	qprocson	Enable put and service routines.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	void qprocson (queue_t * rq);
 *
 *-ARGUMENTS:
 *	rq		Pointer to a read queue.
 *
 *-DESCRIPTION:
 *	qprocson () enables the "put" and "service" routines of the driver or
 *	module whose read queue is pointed to by "rq". Prior to the call to
 *	qprocson (), the put and service routines of a newly pushed module or
 *	driver are disabled. For the module, messages flow around it as if it
 *	were not present in the stream.
 *
 *	qprocson () must be called by the first open of a module or driver
 *	after allocation and initialization of any resources on which the put
 *	and service routines depend.
 *
 *-RETURN VALUE:
 *	None.
 *
 *-LEVEL:
 *	Base level only.
 *
 *-NOTES:
 *	May sleep.
 *
 *	The caller cannot have the stream frozen [see freezestr ()] when
 *	calling this function.
 *
 *	Driver-defined basic locks and read/write locks may not be held across
 *	calls to this function.
 *
 *	Driver-defined sleep locks may be held across calls to this function.
 *
 *-SEE ALSO:
 *	qprocsoff ()
 */

#if	__USE_PROTO__
void (qprocson) (queue_t * rq)
#else
void
qprocson __ARGS ((rq))
queue_t	      *	rq;
#endif
{
	pl_t		prev_pl;
	queue_t	      *	wq;

	/*
	 * Actually, this implementation of qprocsoff () busy-waits rather
	 * than sleeping, but don't count on that.
	 */

	prev_pl = QFREEZE_TRACE (rq, "qprocson");

	ASSERT ((rq->q_flag & QREADR) != 0);

	wq = W (rq);

	(void) QFREEZE_TRACE (wq, "qprocson");

	rq->q_flag &= ~ QPROCSOFF;
	wq->q_flag &= ~ QPROCSOFF;

	ASSERT (rq->q_active == 0 && wq->q_active == 0);


	/*
	 * If there are any messages on the queues, the service procedures
	 * should be run. If the QWANTW flag is set, we'll back-enable the
	 * queues.
	 */

	if (rq->q_first != NULL && (rq->q_flag & QNOENB) != 0)
		(void) QUEUE_TRYSCHED (rq);

	if (wq->q_first != NULL && (wq->q_flag & QNOENB) != 0)
		(void) QUEUE_TRYSCHED (wq);

	if ((rq->q_flag & QWANTW) != 0) {

		rq->q_flag &= ~ QWANTW;
		QUEUE_BACKENAB (rq);
	}

	if ((wq->q_flag & QWANTW) != 0) {

		wq->q_flag &= ~ QWANTW;
		QUEUE_BACKENAB (wq);
	}

	QUNFREEZE_TRACE (wq, plstr);
	QUNFREEZE_TRACE (rq, prev_pl);
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	qreply		Send a message in the opposite direction on a stream.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	void qreply (queue_t * q, mblk_t * mp);
 *
 *-ARGUMENTS:
 *	q		Pointer to the queue from which the message is being
 *			sent.
 *
 *	mp		Pointer to the message to be sent in the opposite
 *			direction.
 *
 *-DESCRIPTION:
 *	qreply () sends a message in the opposite direction from that which
 *	"q" is pointing. It calls the OTHERQ () function to find "q"'s
 *	partner, and passes the message by calling the "put" routine of the
 *	next queue in the stream after "q"'s partner.
 *
 *-RETURN VALUE:
 *	None.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	The caller cannot have the stream frozen [see freezestr ()] when
 *	calling this function.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 */

#if	__USE_PROTO__
void (qreply) (queue_t * q, mblk_t * mp)
#else
void
qreply __ARGS ((q, mp))
queue_t	      *	q;
mblk_t	      *	mp;
#endif
{
	/*
	 * The DDI/DKI says the caller cannot have the stream frozen, but we
	 * don't actually need to do anything that would freeze the stream.
	 * The proscription arises because we need to freeze the other queue
	 * temporarily, and for one context to freeze both sides of a stream
	 * may induce deadlock.
	 */

#ifndef	NDEBUG
	pl_t		prev_pl;

	prev_pl = QFREEZE_TRACE (q, "qreply");
	QUNFREEZE_TRACE (q, prev_pl);
#endif

	ASSERT (mp != NULL);
	ASSERT (mp->b_datap != NULL);

	q = OTHERQ (q);

	QUEUE_PUTNEXT (q, mp);
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	qsize		Find the number of messages on a queue.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	int qsize (queue_t * q);
 *
 *-ARGUMENTS:
 *	q		Pointer to the queue to be evaluated.
 *
 *-DESCRIPTION:
 *	qsize () evaluates the queue pointed to by "q" and returns the number
 *	of messages it contains.
 *
 *-RETURN VALUE:
 *	If there are no messages on the queue, "qsize" returns 0. Otherwise,
 *	it returns the number of messages on the queue.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	The caller cannot have the stream frozen [see freezestr ()] when
 *	calling this function.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 *	msgb, queue
 */

#if	__USE_PROTO__
int (qsize) (queue_t * q)
#else
int
qsize __ARGS ((q))
queue_t	      *	q;
#endif
{
	pl_t		prev_pl;
	mblk_t	      *	scan;
	int		count = 0;

	prev_pl = QFREEZE_TRACE (q, "qsize");

	for (scan = q->q_first ; scan != NULL ; scan = scan->b_next)
		count ++;

	QUNFREEZE_TRACE (q, prev_pl);

	return count;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	RD		Get a pointer to the read queue.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	queue_t * RD (queue_t * q);
 *
 *-ARGUMENTS:
 *	q		Pointer to the queue whose read queue is to be
 *			returned.
 *
 *-DESCRIPTION:
 *	The RD () function accepts a queue pointer as an argument and returns
 *	a pointer to the read queue of the same module or driver.
 *
 *-RETURN VALUE:
 *	The pointer to the read queue.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 *	OTHERQ (), WR ()
 */

#if	__USE_PROTO__
queue_t * (RD) (queue_t * q)
#else
queue_t *
RD __ARGS ((q))
queue_t	      *	q;
#endif
{
	QUEUE_TRACE (q, "RD");

	return RD (q);
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	rmvb		Remove a message block from a message.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	mblk_t * rmvb (mblk_t * mp, mblk_t * bp);
 *
 *-ARGUMENTS:
 *	mp		Message from which a message block is to be removed.
 *
 *	bp		Message block to be removed.
 *
 *-DESCRIPTION:
 *	rmvb () removes a message block ("bp") from a message ("mp"), and
 *	returns a pointer to the altered message. The message block is not
 *	freed, merely removed from the message. It is the caller's
 *	responsibility to free the message block.
 *
 *-RETURN VALUE:
 *	If successful, a pointer to the message (minus the removed block) is
 *	returned. If "bp" was the only block in the message before rmvb ()
 *	was called, NULL is returned. If the designated message block ("bp")
 *	was not in the message, -1 is returned.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 */

#if	__USE_PROTO__
mblk_t * (rmvb) (mblk_t * mp, mblk_t * bp)
#else
mblk_t *
rmvb __ARGS ((mp, bp))
mblk_t	      *	mp;
mblk_t	      *	bp;
#endif
{
	mblk_t	      *	scan;

	ASSERT (mp != NULL);
	ASSERT (bp != NULL);

	if (mp == bp)
		return mp->b_cont;

	for (scan = mp ; scan != NULL ; scan = scan->b_cont)
		if (scan->b_cont == bp) {

			scan->b_cont = bp->b_cont;
			return mp;
		}

	return (mblk_t *) -1;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	rmvq		Remove a message from a queue.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	void rmvq (queue_t * q, mblk_t * mp);
 *
 *-ARGUMENTS:
 *	q		Pointer to the queue containing the message to be
 *			removed.
 *
 *	mp		Pointer to the message to be removed.
 *
 *-DESCRIPTION:
 *	rmvq () removes a message from a queue. A message can be removed from
 *	anywhere in a queue. To prevent modules and drivers from having to
 *	deal with the internals of message linkage on a queue, either rmvq ()
 *	or getq () should be used to remove a message from a queue.
 *
 *-RETURN VALUE:
 *	None.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	The caller must have the stream frozen [see freezestr ()] when calling
 *	this function.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *	"mp" must point to an existing message in the queue pointed to by "q",
 *	or a system panic will occur.
 *
 *-SEE ALSO:
 *	freezestr (), getq (), insq (), unfreezestr ().
 */

#if	__USE_PROTO__
void (rmvq) (queue_t * q, mblk_t * mp)
#else
void
rmvq __ARGS ((q, mp))
queue_t	      *	q;
mblk_t	      *	mp;
#endif
{
	ASSERT (mp != NULL);

	QFROZEN_TRACE (q, "rmvq");

	/*
	 * First, simply dequeue the message based on mp's "b_next" and
	 * "b_prev" members.
	 */

	if (mp->b_next == NULL) {

		ASSERT (q->q_last == mp);
		if ((q->q_last = mp->b_prev) == NULL)
			QUEUE_DRAINED (q);
	} else
		mp->b_next->b_prev = mp->b_prev;

	if (mp->b_prev == NULL) {

		ASSERT (q->q_first == mp);
		q->q_first = mp->b_next;

		/*
		 * Since we are dequeueing a message from the front of the
		 * queue, record the message band.
		 */

		q->q_lastband = mp->b_band;
	} else
		mp->b_prev->b_next = mp->b_next;

	/*
	 * Now we adjust the flow-control parameters and band information.
	 */

	if (! IS_PRI_MSG (mp)) {
		ulong_t		msgsize = MSG_SIZE (mp);

		if (mp->b_band > 0) {
			qband_t	      *	qbandp = QUEUE_BAND (q, mp->b_band);

			ASSERT (qbandp->qb_first == mp);

			QBAND_DEQUEUE (qbandp, mp);

			QBAND_REDUCE (q, qbandp, msgsize);
		} else
			QUEUE_REDUCE (q, msgsize);
	}
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	SAMESTR		Test if next queue is same type.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	int SAMESTR (queue_t * q);
 *
 *-ARGUMENTS:
 *	q		Pointer to the queue.
 *
 *-DESCRIPTION:
 *	The SAMESTR () function is used to see if the next queue in a stream
 *	(if it exists) is the same type as the current queue (that is, both
 *	are read queues or both are write queues). This can be used to
 *	determine the point in a STREAMS-based pipe where a read queue is
 *	linked to a write queue.
 *
 *-RETURN VALUE:
 *	SAMESTR () returns 1 if the next queue is the same type as the current
 *	queue. It returns 0 if the next queue does not exist or if it is not
 *	the same type.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	The caller cannot have the stream frozen [see freezestr ()] when
 *	calling this function.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *	The argument "q" may not reference "q_next" (for example, an argument
 *	of "q->q_next" is erroneous on a multiprocessor and is disallowed by
 *	the DDI/DKI).
 *
 *-SEE ALSO:
 *	OTHERQ ()
 */

#if	__USE_PROTO__
int (SAMESTR) (queue_t * q)
#else
int
SAMESTR __ARGS ((q))
queue_t	      *	q;
#endif
{
	int		retval;
	pl_t		prev_pl;

	prev_pl = QFREEZE_TRACE (q, "SAMESTR");

	retval = q->q_next == NULL ? 0 :
		   (q->q_next->q_flag & QREADR) == (q->q_flag & QREADR);

	QUNFREEZE_TRACE (q, prev_pl);

	return retval;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	strlog		Submit messages to the log driver.
 *
 *-SYNOPSIS:
 *	#include <sys/types.h>
 *	#include <sys/stream.h>
 *	#include <sys/strlog.h>
 *	#include <sys/log.h>	
 *
 *	int strlog (short mid, short sid, char level, uchar_t flags,
 *		    char * fmt, ...);
 *
 *-ARGUMENTS:
 *	mid		Identification number of the module or driver
 *			submitting the message.
 *
 *	sid		Identification number for a particular minor device.
 *
 *	flags		Bitmask of flags indicating message purpose. Valid
 *			flags are:
 *			  SL_ERROR	Message is for error logger.
 *			  SL_TRACE	Message is for tracing.
 *			  SL_CONSOLE	Message is for console logger.
 *			  SL_NOTIFY	If SL_ERROR is also set, mail copy of
 *					message to system administrator.
 *			  SL_FATAL	Modifier indicating error is fatal.
 *			  SL_WARN	Modifier indicating error is a
 *					warning.
 *			  SL_NOTE	Modifier indicating error is a notice.
 *
 *	fmt		printf () style format string. %s, %e, %g and %G
 *			formats are not allowed.
 *
 *	...		Zero or more arguments to printf () (maximum of
 *			NLOGARGS, currently three).
 *
 *-DESCRIPTION:
 *	strlog () submits formatted messages to the "log" driver. The messages
 *	can be retrieved with the getmsg () system call. The "flags" argument
 *	specifies the type of the message and where it is to be sent.
 *
 *-RETURN VALUE:
 *	strlog () returns 0 if the message is not seen by all the readers, 1
 *	otherwise.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 *	log (7) in the "Programmer's Guide: STREAMS"
 */

#if	__USE_PROTO__
int (strlog) (short mid, short sid, char level, ushort_t flags, char * fmt,
	      ...)
#else
int
strlog __ARGS ((mid, sid, level, flags, fmt))
short		mid;
short		sid;
char		level;
ushort_t	flags;
char	      *	fmt;
#endif
{
	mblk_t	      *	data;
	ulong_t		err_seq, trc_seq, con_seq;
	mblk_t	      *	errmsg, * trcmsg, * conmsg;
	int		failures;
	int		copies;
	int		pri;

# define	SL_ALL	(SL_ERROR | SL_TRACE | SL_CONSOLE | SL_NOTIFY |\
			 SL_FATAL | SL_WARN | SL_NOTE)

	ASSERT ((flags & SL_ALL) != 0);
	ASSERT ((flags & ~ SL_ALL) == 0);
	ASSERT (fmt != NULL);

	/*
	 * The first thing we do is assign a sequence number to this log item
	 * so that we can correctly detect when log items have been discarded
	 * due to lack of available resources. In addition, there are several
	 * sequence-number spaces available, depending on the destination of
	 * the log message. We acquire numbers from all the spaces now, and
	 * write them into the actual log messages later.
	 */

	{
		pl_t		prev_pl;
		int		temp;

		prev_pl = LOCK (str_mem->sm_seq_lock, plstr);

		err_seq = trc_seq = con_seq = 0;	/* paranoia */

		if ((flags & SL_ERROR) != 0)
			err_seq = str_mem->sm_err_seq ++;
		if ((flags & SL_TRACE) != 0)
			trc_seq = str_mem->sm_trc_seq ++;
		if ((flags & SL_CONSOLE) != 0)
			con_seq = str_mem->sm_con_seq ++;

		temp = str_mem->sm_log_rq == NULL;

		UNLOCK (str_mem->sm_seq_lock, prev_pl);


		/*
		 * If the log driver isn't installed yet, don't bother with
		 * the rest of this routine.
		 */

		if (temp)
			return 0;
	}

	/*
	 * While the documentation for this function doesn't spell it out,
	 * the log (7) driver traffics in error, trace and console messages,
	 * with the others being variations on that theme.
	 *
	 * Somehow the "flags" have to be mapped into a priority/facility code
	 * according to some rules that are alluded to in the log (7) manual
	 * pages.
	 */

	{
		size_t		size;
		va_list		args;
		uchar_t	      *	dest;
		int		temp;

		if ((flags & (SL_ERROR | SL_FATAL)) != 0)
			pri = BPRI_HI;
		else if ((flags & SL_WARN) != 0)
			pri = BPRI_MED;
		else
			pri = BPRI_LO;


		/*
		 * The data portion of a STREAMS logger message contains the
		 * unexpanded text of the "fmt" string plus NLOGARGS worth of
		 * data containing any extra arguments packed after the string
		 * aligned to the next word address.
		 *
		 * We'll also need at least one control message section, which
		 * we can copy as necessary later on.
		 */
		/*
		 * ALIGNMENT-DEPENDENT CODE.
		 */
		size = ((strlen (fmt) + 4) & ~ (sizeof (int) - 1)) +
			NLOGARGS * sizeof (ulong_t);

		if ((data = MSGB_ALLOC (size, pri, KM_NOSLEEP)) == NULL) {
			/*
			 * Return a failure indication. This is not a major
			 * problem, as the gaps in the sequence space tell a
			 * story...
			 */

			return 0;
		}

		/*
		 * Copy the format string and the argument data to the log
		 * buffer.
		 */

		dest = data->b_rptr;
		while ((* dest ++ = * (uchar_t *) fmt ++) != 0)
			;
		dest += (sizeof (int) - 1) - (((unsigned long) dest - 1) &
					      (sizeof (int) - 1));

		va_start (args, fmt);
		for (temp = 0 ; temp < NLOGARGS ; temp ++)
			* ((ulong_t *) dest) ++ = va_arg (args, ulong_t);
		va_end (args);

		data->b_wptr += size;

		ASSERT (data->b_wptr == dest);
	}


	/*
	 * Now send the log request to the read side of the log driver. This
	 * is how we distinguish kernel log requests from user log requests,
	 * since the user requests will arrive on the write side as usual.
	 *
	 * Since the various destinations for log messages expect to see
	 * messages with sequence numbers tailored to them, we create
	 * additional copies of the log message as needed. For this to work,
	 * we work out the copies first, and then send them.
	 *
	 * This would be a lot easier if C had nested functions as standard.
	 * Sadly, only GNU does this now.
	 */

	failures = 0;
	copies = 0;

	errmsg = STRLOG_MAKE (mid, sid, level, flags, err_seq, SL_ERROR,
			      & copies, & failures, data);

	trcmsg = STRLOG_MAKE (mid, sid, level, flags, trc_seq, SL_TRACE,
			      & copies, & failures, data);

	conmsg = STRLOG_MAKE (mid, sid, level, flags, con_seq, SL_CONSOLE,
			      & copies, & failures, data);

	if (errmsg != NULL)
		put (str_mem->sm_log_rq, errmsg);
	if (trcmsg != NULL)
		put (str_mem->sm_log_rq, trcmsg);
	if (conmsg != NULL)
		put (str_mem->sm_log_rq, conmsg);

	if (copies == 0)
		freemsg (data);

	return failures == 0;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	strqget		Get information about a queue or band of the queue.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	int strqget (queue_t * q, qfields_t what, uchar_t pri, long * valp);
 *
 *-ARGUMENTS:
 *	q		Pointer to the queue.
 *
 *	what		The field of the queeu about which to return
 *			information. Valid values are:
 *
 *			QHIWAT	High water mark of the specified priority
 *				band.
 *
 *			QLOWAT	Low water mark of the specified priority band.
 *
 *			QMAXPSZ	Maximum packet size of the specified priority
 *				band.
 *
 *			QMINPSZ	Minimum packet size of the specified priority
 *				band.
 *
 *			QCOUNT	Number of bytes of data in messages in the
 *				specified priority band.
 *
 *			QFIRST	Pointer to the first message in the specified
 *				priority band.
 *
 *			QLAST	Pointer to the last message in the specified
 *				priority band.
 *
 *			QFLAG	Flags for the specified priority band.
 *
 *	pri		Priority band of the queue about which to obtain
 *			information.
 *
 *	valp		Pointer to the memory locatation where the value is
 *			to be stored.
 *
 *-DESCRIPTION:
 *	strqget () gives drivers and modules a way to get information about
 *	a queue or a particular priority band of a queue without directly
 *	accessing STREAMS data structures.
 *
 *-RETURN VALUE:
 *	On success, 0 is returns. An error number is returned on failure. The
 *	actual value of the requested field is returned through the reference
 *	parameter "valp".
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	The caller must have the stream frozen [see freezestr ()] when calling
 *	this function.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 *	freezestr (), strqset (), unfreezestr (), queue
 */

#if	__USE_PROTO__
int (strqget) (queue_t * q, qfields_t what, uchar_t pri, long * valp)
#else
int
strqget __ARGS ((q, what, pri, valp))
queue_t	      *	q;
qfields_t	what;
uchar_t		pri;
long	      *	valp;
#endif
{
	QFROZEN_TRACE (q, "strqget");

	if (pri > 0)
		switch (what) {

		case QHIWAT:
			* valp = q->q_hiwat;
			break;

		case QLOWAT:
			* valp = q->q_lowat;
			break;

		case QMAXPSZ:
			* valp = q->q_maxpsz;
			break;

		case QMINPSZ:
			* valp = q->q_minpsz;
			break;

		case QCOUNT:
			* valp = q->q_count;
			break;

		case QFIRST:
			* valp = (long) q->q_first;
			break;

		case QLAST:
			* valp = (long) q->q_last;
			break;

		case QFLAG:
			* valp = q->q_flag;
			break;

		default:
			return EINVAL;
		}
	else {
		qband_t	     *	qbandp;

		if ((qbandp = QUEUE_BAND (q, pri)) == NULL &&
		    (qbandp = QBAND_ALLOC (q, pri)) == NULL)
			return ENOMEM;

		switch (what) {

		case QHIWAT:
			* valp = qbandp->qb_hiwat;
			break;

		case QLOWAT:
			* valp = qbandp->qb_lowat;
			break;

		case QCOUNT:
			* valp = qbandp->qb_count;
			break;

		case QFIRST:
			* valp = (long) qbandp->qb_first;
			break;

		case QLAST:
			* valp = (long) qbandp->qb_last;
			break;

		case QFLAG:
			* valp = qbandp->qb_flag;
			break;

		default:
			return EINVAL;
		}
	}

	return 0;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	strqset		Change information about a queue or band of the queue.
 *
 *-SYNOPSIS:
 *	#include <sys/types.h>
 *	#include <sys/stream.h>
 *
 *	int strqset (queue_t * q, qfields_t what, uchar_t pri, long val);
 *
 *-ARGUMENTS:
 *	q		Pointer to the queue.
 *
 *	what		The field of the queue to change. Value values are:
 *
 *			QHIWAT	High water mark of the specified priority
 *				band.
 *
 *			QLOWAT	Low water mark of the specified priority band.
 *
 *			QMAXPSZ	Maximum packet size of the specified priority
 *				band.
 *
 *			QMINPSZ	Minimum packet size of the specified priority
 *				band.
 *
 *	pri		Priority band of the queue to be changed.
 *
 *	val		New value for the field to be changed.
 *
 *-DESCRIPTION:
 *	strqset () gives drivers and modules a way to change information about
 *	a queue or a particular priority band of a queue without directly
 *	accessing STREAMS data structures.
 *
 *-RETURN VALUE:
 *	On success, 0 is returned. An error number is returned on failure.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	The caller must have the stream frozen [see freezestr ()] when calling
 *	this function.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 */

#if	__USE_PROTO__
int (strqset) (queue_t * q, qfields_t what, uchar_t pri, long val)
#else
int
strqset __ARGS ((q, what, pri, val))
queue_t	      *	q;
qfields_t	what;
uchar_t		pri;
long		val;
#endif
{
	QFROZEN_TRACE (q, "strqset");

	if (pri > 0)
		switch (what) {

		case QHIWAT:
			q->q_hiwat = val;
			break;

		case QLOWAT:
			q->q_lowat = val;
			break;

		case QMAXPSZ:
			q->q_maxpsz = val;
			break;

		case QMINPSZ:
			q->q_minpsz = val;
			break;

		case QCOUNT:
		case QFIRST:
		case QLAST:
		case QFLAG:
			return EPERM;

		default:
			return EINVAL;
		}
	else {
		qband_t	      *	qbandp;

		if ((qbandp = QUEUE_BAND (q, pri)) == NULL &&
		    (qbandp = QBAND_ALLOC (q, pri)) == NULL)
			return ENOMEM;

		switch (what) {

		case QHIWAT:
			qbandp->qb_hiwat = val;
			break;

		case QLOWAT:
			qbandp->qb_lowat = val;
			break;

		case QCOUNT:
		case QFIRST:
		case QLAST:
		case QFLAG:
			return EPERM;

		default:
			return EINVAL;
		}
	 }

	 return 0;
}


/*
 *-STATUS:
 *	Compatibility (pre-MP DDI/DKI)
 *
 *-NAME:
 *	testb		Check for an available buffer.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	int testb (int size, int pri);
 *
 *-ARGUMENTS:
 *	size		Size of the requested buffer.
 *
 *	pri		Priority of the allocb () request.
 *
 *-DESCRIPTION:
 *	testb () checks to see if an allocb () call is likely to succeed if
 *	a buffer of "size" bytes at priority "pri" is requested. Even if
 *	testb () returns successfully, the call to allocb () can fail.
 *
 *-RETURN VALUE:
 *	Returns 1 if a buffer of the requested size is available, and 0 if
 *	one is not.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *	This function is provided purely as a porting convenience for
 *	developers working with drivers developed under earlier releases of
 *	the System V DDI/DKI or under STREAMS from System V, Release 3. Calls
 *	to this function should be replaced with calls to functions that do
 *	the real work.
 *
 *-SEE ALSO:
 *	allocb (), bufcall ().
 */

#if	__USE_PROTO__
int (testb) (int size, int pri)
#else
int
testb __ARGS ((size, pri))
int		size;
int		pri;
#endif
{
	pl_t		prev_pl;
	int		return_val;

	/*
	 * This function is one of the most braindead peices of STREAMS. It
	 * may not have been initially, but by the SVR4 DDI/DKI, it was
	 * *totally* useless. The example code given in that DDI/DKI issue is
	 * so bad it's unbelievable, and they admit it too...
	 */

	ASSERT (size > 0);
	ASSERT (pri == BPRI_LO || pri == BPRI_HI || pri == BPRI_LO);

	pri = MAP_PRI_LEVEL (pri);
	size = MSGB_SIZE (pri);


	/*
	 * Lock the basic lock protecting access to the memory pool
	 * and attempt to acquire the memory we desire.
	 */

	prev_pl = LOCK (str_mem->sm_msg_lock, str_msg_pl);

	/*
	 * Before allocating any memory, we check to see that it makes sense
	 * to give out that memory to the given priority level.
	 */

	return_val = str_mem->sm_used + size < str_mem->sm_max [pri] &&
			st_maxavail (str_mem->sm_msg_heap) >= size;

	UNLOCK (str_mem->sm_msg_lock, prev_pl);

	return return_val;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	unbufcall	Cancel a pending bufcall () request.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	void unbufcall (toid_t id);
 *
 *-ARGUMENTS:
 *	id		Identifier returned from bufcall () or esbbcall ().
 *
 *-DESCRIPTION:
 *	unbufcall () cancels a pending bufcall () or esbbcall () request. The
 *	argument "id" is a non-zero identifier for the request to be
 *	cancelled. "id" is returned from the bufcall () or esbbcall ()
 *	function used to issue the request.
 *
 *-RETURN VALUE:
 *	None.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 *	bufcall (), esbbcall ()
 */

#if	__USE_PROTO__
void (unbufcall) (toid_t id)
#else
void
unbufcall __ARGS ((id))
toid_t		id;
#endif
{
	pl_t		prev_pl;
	selist_t      *	selistp;
	sevent_t      *	sscan;
	sevent_t      *	sprev;

	ASSERT (id != 0);

	/*
	 * Let's lock the list that we are going to search for the new event
	 * on.
	 */

	selistp = & str_mem->sm_bcevents [MAP_PRI_LEVEL (TOID_TO_PRI (id))];

	prev_pl = SELIST_LOCK (selistp);

	for (sscan = selistp->sl_head, sprev = NULL ; sscan != NULL ;
	     sscan = (sprev = sscan)->se_next) {
		/*
		 * If we find it, dequeue it.
		 */

		if (sscan->se_id == id) {
			if (sprev == NULL)
				selistp->sl_head = sscan->se_next;
			else
				sprev->se_next = sscan->se_next;
#if _FIFO_BUFCALL
			if (selistp->sl_tail == sscan) {

				ASSERT (sscan->se_next == NULL);
				selistp->sl_tail = sprev;
			} else
				ASSERT (sscan->se_next != NULL);
#endif
			break;
		}
	}

	SELIST_UNLOCK (selistp, prev_pl);
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	unfreezestr	Unfreeze the state of a stream.
 *
 *-SYNOPSIS:
 *	#include <sys/types.h>
 *	#include <sys/stream.h>
 *
 *	void unfreezestr (queue_t * q, pl_t prev_pl);
 *
 *-ARGUMENTS:
 *	q		Pointer to a message queue.
 *
 *	pl		The interrupt priority level to be set (if the
 *			implementation requires that interrupts be blocked in
 *			order to prevent deadlock) after unfreezing the
 *			stream. See LOCK_ALLOC () for a list of valid values
 *			for "pl". "pl" should be the value that was returned
 *			from the corresponding call to freezestr () unless the
 *			caller has a specific need to set some other interrupt
 *			priority level. Although portable drivers must always
 *			specify an appropriate "pl" argument, implementations
 *			which do not require that the interrupt priority be
 *			raised while the stream is frozen may choose to ignore
 *			this argument.
 *
 *-DESCRIPTION:
 *	unfreezestr () unfreezes the state of the stream containing the queue
 *	specified by "q", and sets the interrupt priority level to the value
 *	specified by "pl". Unfreezing the state of the stream allows
 *	continuation of all activities that were forced to wait while the
 *	stream was frozen.
 *
 *-RETURN VALUE:
 *	None.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	The caller must have the stream frozen [see freezestr ()] when calling
 *	this function.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 *	freezestr ()
 */

#if	__USE_PROTO__
void (unfreezestr) (queue_t * q, pl_t pl)
#else
void
unfreezestr __ARGS ((q, pl))
queue_t	      *	q;
pl_t		pl;
#endif
{
	unsigned long	back;

	QFROZEN_TRACE (q, "unfreezestr");

	/*
	 * Since the caller might have used rmvq () to cause a condition where
	 * queues behind the current one need back-enabling, test for this.
	 */

	if ((back = q->q_flag & QBACK) != 0)
		q->q_flag &= ~ QBACK;

	QUNFREEZE_TRACE (q, pl);

	if (back)
		QUEUE_BACKENAB (q);
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	unlinkb		Remove a message block from the head of a message.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	mblk_t * unlinkb (mblk_t * mp);
 *
 *-ARGUMENTS:
 *	mp		Pointer to the message.
 *
 *-DESCRIPTION:
 *	unlinkb () removes the first message block from the message pointed to
 *	by "mp". The removed message block is not freed. It is the caller's
 *	responsibility to free it.
 *
 *-RETURN VALUE:
 *	unlinkb () returns a pointer to the remainder of the message after the
 *	first message block has been removed. If there is only one message
 *	block in the message, NULL is returned.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 *	linkb ()
 */

#if	__USE_PROTO__
mblk_t * (unlinkb) (mblk_t * mp)
#else
mblk_t *
unlinkb __ARGS ((mp))
mblk_t	      *	mp;
#endif
{
	mblk_t	      *	retval;

	ASSERT (mp != NULL);

	retval = mp->b_cont;
	mp->b_cont = NULL;

	return retval;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	WR		Get a pointer to the write queue.
 *
 *-SYNOPSIS:
 *	#include <sys/stream.h>
 *
 *	queue_t * WR (queue_t * q);
 *
 *-ARGUMENTS:
 *	q		Pointer to the queue whose write queue is to be
 *			returned.
 *
 *-DESCRIPTION:
 *	The WR () function accepts a queue pointer as an argument and returns
 *	a pointer to the write queue of the same module.
 *
 *-RETURN VALUE:
 *	To pointer to the write queue.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *-SEE ALSO:
 *	OTHERQ (), RD ().
 */

#if	__USE_PROTO__
queue_t * (WR) (queue_t * q)
#else
queue_t *
WR __ARGS ((q))
queue_t	      *	q;
#endif
{
	QUEUE_TRACE (q, "WR");

	return WR (q);
}
