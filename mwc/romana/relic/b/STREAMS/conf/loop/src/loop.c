/*
 * This loop-around driver has been transcribed verbatim from the System V,
 * Release 4 Multiprocessor "Programmer's Guide : STREAMS" manual, from
 * Chapter 4. This functions as a simple test for compatibility.
 *
 * Some minor editing has proved necessary due to this driver's use of header
 * files not documented in the DDI/DKI. The following files were either
 * #included by the original source, but are not present in this system, or
 * were never used in the example. Additional imports not present in the
 * original code are documented below.
 *
 * It may well be that the example is correct in importing these headers, and
 * the "synopsis" lines in the DDI/DKI documentation are incorrect.
 */

#define	_DDI_DKI	1
#define	_SYSV4		1

/*
 *-IMPORTS:
 *	<common/ccompat.h>
 *		__USE_PROTO__
 *		__ARGS ()
 *		__PROTO ()
 *	<sys/kmem.h>
 *		KM_NOSLEEP
 *	<sys/ksynch.h>
 *		lkinfo_t
 *		lock_t
 *		LOCK_ALLOC ()
 *		LOCK ()
 *		UNLOCK ()
 *	<sys/cmn_err.h>
 *		CE_PANIC
 *		CE_WARN
 *		cmn_err ()
 *	<sys/errno.h>
 *		ENXIO
 *		EBUSY
 *		EINVAL
 *	<stddef.h>
 *		NULL
 */

#include <common/ccompat.h>
#include <sys/kmem.h>
#include <sys/ksynch.h>
#include <sys/cmn_err.h>
#include <sys/errno.h>
#include <stddef.h>

/* #include "sys/param.h" */
/* #include "sys/sysmacros.h" */
/* #include "sys/dir.h" */
/* #include "sys/signal.h" */
/* #include "sys/user.h" */
/* #include "sys/cred.h" */

#include "sys/types.h"
#include "sys/stream.h"
#include "sys/stropts.h"
#include "sys/ddi.h"


static struct module_info minfo = {
	0xEE12, "loop", 0, INFPSZ, 512, 128
};


/*
 * The original example code contained declarations for the loop... ()
 * routines that did not even declare them as functions, but as integers. This
 * has been corrected. The original code would not have compiled with an ISO
 * C compiler.
 */

static	int	loopopen	__PROTO ((queue_t * q, dev_t * devp, int flag,
					  int sflag, cred_t * credp));
static	int	loopclose	__PROTO ((queue_t * q, int flag,
					  cred_t * credp));
static	void	loopwput	__PROTO ((queue_t * q, mblk_t * mp));
static	void	loopwsrv	__PROTO ((queue_t * q));
static	void	looprsrv	__PROTO ((queue_t * q));

static struct qinit rinit = {
	NULL, looprsrv, loopopen, loopclose, NULL, & minfo, NULL
};

static struct qinit winit = {
	loopwput, loopwsrv, NULL, NULL, NULL, & minfo, NULL
};

struct streamtab loopinfo = { & rinit, & winit, NULL, NULL };


/*
 * Import definitions from the "Space.c" file.
 */

#include "loop.h"

int	loopdevflag = 0;


/*
 * The example code did not declare this, ever.
 */

static	lkinfo_t	loop_lkinfo = {
	"Loopback driver lock"
};


/*
 * Extensions to the original code; as dicussed, there are limitations in the
 * multiprocessor code in the original example. We have extracted the code
 * that deals with the lock hierarchy into separate functions to make it
 * simpler to experiment with different multiprocessor lock strategies.
 */

#define	LOOP_HIER(idx)		(hier + 1)


/*
 * Lock a pair of loop entries.
 */

#if	__USE_PROTO__
static pl_t (LOOP_LOCK) (struct loop * loop, struct loop * other)
#else
static pl_t
LOOP_LOCK __ARGS ((loop, other))
struct loop   *	loop;
struct loop   *	other;
#endif
{
	pl_t		prev_pl;

	/*
	 * Since the entries are part of a single array object, we are
	 * guaranteed that the relational comparison of the pointers will
	 * yield valid results.
	 *
	 * The comparison ensures that we lock the entries in hierarchy order;
	 * the canonical ordering is needed to avoid deadlock.
	 */

	if (loop < other) {

		prev_pl = LOCK (loop->lck, plstr);
		(void) LOCK (other->lck, plstr);
	} else {

		prev_pl = LOCK (other->lck, plstr);
		(void) LOCK (loop->lck, plstr);
	}

        return prev_pl;
}


/*
 * Unlock a pair of loop entries.
 */

#if	__USE_PROTO__
static void (LOOP_UNLOCK) (struct loop * loop, struct loop * other,
			   pl_t prev_pl)
#else
static void
LOOP_UNLOCK __ARGS ((loop, other, prev_pl))
struct loop   *	loop;
struct loop   *	other;
pl_t		prev_pl;
#endif
{
	/*
	 * The original example always unlocked the minor devices in reverse
	 * order to the lock order. This isn't necessary; whether it's a good
	 * idea or not cannot be determined without forbidden knowledge of the
	 * implementation of the lock primitives, or rigorous profiling.
	 */

	UNLOCK (loop->lck, plstr);
	UNLOCK (other->lck, prev_pl);
}


/*
 * Init routine for the driver, called at boot time before system services are
 * initialized. This function may not call any DDI/DKI routines other than
 * those listed on the init(2D2K) manual page, because only those services
 * have been initialized.
 */

__EXTERN_C__	
#if	__USE_PROTO__
void loopinit (void)
#else
void
loopinit ()
#endif
{
	int		hier;

	/*
	 * Allocate a multiprocessor lock for each minor device.
	 *
	 * An arbitrary hierarchy is defined based on position in the
	 * "loop_loop" table, such that to lock two entries at the same time,
	 * always lock "loop_loop [n].lck" before "loop_loop [n+m].lck".
	 */

	/*
	 * This version of the loopback driver will keep the above scheme from
	 * the original code, but there are some important restrictions.
	 *
	 * This scheme restricts the number of minor devices to the range of
	 * valid hierarchy values that LOCK_ALLOC () will accept, which is a
	 * total of 32 entries. This restriction can be lifted by using a
	 * single hierarchy level and using TRYLOCK () when trying to acquire
	 * multiple locks, or simply using a single global lock.
	 *
	 * Note that the original code allocated the locks without testing the
	 * return value from LOCK_ALLOC (). Furthermore, the code passed a
	 * flag value of KM_SLEEP, which is not legal for an init routine,
	 * according to the init(D2DK) manual page.
	 */

	if (loop_cnt > 32) {

		cmn_err (CE_WARN, "Only 32 loopback entries can be provided");
		loop_cnt = 32;
	}

	for (hier = 0 ; hier < loop_cnt ; hier ++) {

		if ((loop_loop [hier].lck =
				LOCK_ALLOC (hier + 1, plstr, & loop_lkinfo,
					    KM_NOSLEEP)) ==  NULL)
			cmn_err (CE_PANIC, "Could not allocate lock in "
					   "loopinit ()");
	}
}


#if	__USE_PROTO__
int loopopen (queue_t * q, dev_t * devp, int __NOTUSED (flag), int sflag,
	      cred_t * __NOTUSED (credp))
#else
int
loopopen (q, devp, flag, sflag, credp)
queue_t	      *	q;
dev_t	      *	devp;
int		flag;
int		sflag;
cred_t	      *	credp;
#endif
{
	struct loop   *	loop;
	dev_t		newminor;

	/*
	 * If CLONEOPEN, pick a minor device number to use. Otherwise, check
	 * the minor device range.
	 */

	if (sflag == CLONEOPEN) {

		for (newminor = 0 ; newminor < loop_cnt ; newminor ++) {

			if (loop_loop [newminor].qptr == NULL)
				break;
		}
	} else
		newminor = geteminor (* devp);

	if (newminor >= loop_cnt)
		return ENXIO;

	/*
	 * Construct new device number if this is the first open.
	 */

	if (q->q_ptr != NULL)
		return 0;

	* devp = makedevice (getemajor (* devp), newminor);

	loop = & loop_loop [newminor];

	q->q_ptr = WR (q)->q_ptr = (char *) loop;
	loop->qptr = WR (q);
	loop->oqptr = NULL;


	/*
	 * Enable put and service routines for this queue pair.
	 */

	qprocson (q);
	return 0;
}


#if	__USE_PROTO__
void loopwput (queue_t * q, mblk_t * mp)
#else
void
loopwput (q, mp)
queue_t	      *	q;
mblk_t	      *	mp;
#endif
{
	struct loop   *	loop;
	pl_t		prev_pl;
	int		error;

	loop = (struct loop *) q->q_ptr;

	switch (mp->b_datap->db_type) {

	case M_IOCTL: {
		struct iocblk *	iocp;

		iocp = (struct iocblk *) mp->b_rptr;

		switch (iocp->ioc_cmd) {

		case LOOP_SET: {
			int		to;	/* other minor device */
			struct loop   *	other;

			/*
			 * Sanity check: "ioc_count" contains the amount of
			 * user-supplied data, and must equal the size of an
			 * int.
			 */

			if (iocp->ioc_count != sizeof (int)) {

				error = EINVAL;
				goto iocnak;
			}


			/*
			 * Fetch other device from 2nd message block.
			 */

			to = * (int *) mp->b_cont->b_rptr;


			/*
			 * More sanity checks: the minor must be in range,
			 * open already, and both devices must be
			 * disconnected.
			 *
			 * Note that the original code contained a serious
			 * flaw in the test logic where the lock on the other
			 * minor device was taken out before the range check.
			 * In addition, the lock was
			 *	oldpri = LOCK (loop_loop [to].qptr, plstr);
			 * but it isn't legal to try locking a "queue_t *".
			 *
			 * The original code allowed a connect attempt to the
			 * same minor as the "loop" entry, which will cause
			 * deadlock.
			 */

			if (to >= loop_cnt || to < 0 ||
			    (other = & loop_loop [to]) == loop) {

				error = ENXIO;
				goto iocnak;
			}

			prev_pl = LOOP_LOCK (loop, other);

			if (other->qptr == NULL || loop->oqptr != NULL ||
			    other->oqptr != NULL) {

				LOOP_UNLOCK (loop, other, prev_pl);
				error = EBUSY;
				goto iocnak;
			}


			/*
			 * Cross-connect streams via the loop structures.
			 */

			loop->oqptr = RD (other->qptr);
			other->oqptr = RD (q);

			LOOP_UNLOCK (loop, other, prev_pl);


			/*
			 * Return a successful ioctl (). Set "ioc_count" to
			 * zero, since no data is returned.
			 */

			mp->b_datap->db_type = M_IOCACK;
			iocp->ioc_count = 0;
			break;
		    }

		default:
			error = EINVAL;
iocnak:
			/*
			 * Invalid ioctl (). Setting "ioc_error" causes the
			 * ioctl () call to return that particular errno. By
			 * default, ioctl () will return EINVAL on failure.
			 */

			mp->b_datap->db_type = M_IOCNAK;
			iocp->ioc_error = error;
		}

		qreply (q, mp);
		break;
	    }

	case M_FLUSH: {
		queue_t	      *	tempqptr;

		prev_pl = LOCK (loop->lck, plstr);

		/*
		 * The original example code flushed the read queues of this
		 * minor and any connected minor, despite the fact that there
		 * are never any messages queued there. We'll skip that part.
		 */

		if ((* mp->b_rptr & FLUSHW) != 0)
			flushq (q, FLUSHALL);

		if ((* mp->b_rptr & FLUSHR) != 0)
			if (loop->oqptr != NULL)
				flushq (WR (loop->oqptr), FLUSHALL);

		/*
		 * Now change around the flush message in a similar fashion to
		 * the midpoint of a STREAMS FIFO. The logic in the original
		 * example was broken in that it did not cope correctly with
		 * the situation where the device was not connected, it did
		 * not have a declaration for "tempqptr", and it just fell
		 * through into the data message case.
		 */

		if ((tempqptr = loop->oqptr) != NULL) {

			switch (* mp->b_rptr & ~ FLUSHBAND) {

			case FLUSHW:
			case FLUSHR:
				* mp->b_rptr ^= FLUSHR | FLUSHW;
				break;

			default:
				break;
			}
		} else {
			/*
			 * Canonical driver flush processing. We use a NULL
			 * value in "tempqptr" to flag that the message should
			 * be discarded.
			 */

			* mp->b_rptr &= ~ FLUSHW;

			tempqptr = (* mp->b_rptr & FLUSHR) != 0 ? RD (q) :
						NULL;
		}

		UNLOCK (loop->lck, prev_pl);

		if (tempqptr != NULL)
			putnext (tempqptr, mp);
		else
			freemsg (mp);

		/*
		 * The original example fell through into the data-message
		 * code, which is clearly incorrect.
		 */

		break;
	    }

	case M_DATA:
	case M_PROTO:
	case M_PCPROTO:
		/*
		 * If this stream is not connected, register an error with an
		 * M_ERROR message. Otherwise, queue the message for
		 * forwarding by the write service procedure.
		 */

		prev_pl = LOCK (loop->lck, plstr);

		error = loop->oqptr == NULL ? ENXIO : 0;

		UNLOCK (loop->lck, prev_pl);

		if (error != 0) {

			freemsg (mp);
			putnextctl1 (RD (q), M_ERROR, error);
		} else
			putq (q, mp);
		break;

	default:
		/*
		 * Discard unrecognized messages.
		 */

		freemsg (mp);
		break;
	}
}


#if	__USE_PROTO__
void loopwsrv (queue_t * q)
#else
void
loopwsrv (q)
queue_t	      *	q;
#endif
{
	mblk_t	      *	mp;
	struct loop   *	loop;
	queue_t	      *	tmpqptr;
	pl_t		prev_pl;

	loop = (struct loop *) q->q_ptr;

	while ((mp = getq (q)) != NULL) {

		prev_pl = LOCK (loop->lck, plstr);

		tmpqptr = loop->oqptr;

		UNLOCK (loop->lck, prev_pl);

		/*
		 * The original example code implicitly assumed that the loop
		 * was connected at this point. It also would never pass a
		 * priority message to the other side, and on flow control or
		 * a priority message would never unlock the minor device.
		 *
		 * Note that the disconnect handling in the original example
		 * is broken, and this is only a partial fix.
		 */

		if (tmpqptr == NULL) {

			freemsg (mp);
			putnextctl1 (RD (q), M_ERROR, EINVAL);
			continue;
		}

		if (! pcmsg (mp->b_datap->db_type) &&
		    ! canputnext (tmpqptr)) {

			putbq (q, mp);
			break;
		}

		putnext (tmpqptr, mp);
	}
}


#if	__USE_PROTO__
void looprsrv (queue_t * q)
#else
void
looprsrv (q)
queue_t	      *	q;
#endif
{
	struct loop   *	loop;
	pl_t		prev_pl;

	/*
	 * Deal with being back-enabled by flow control by enabling the write
	 * side service procedure to retry sending the messages backed up
	 * there.
	 */

	loop = (struct loop *) q->q_ptr;

	prev_pl = LOCK (loop->lck, plstr);

	if (loop->oqptr != NULL)
		qenable (WR (loop->oqptr));

	UNLOCK (loop->lck, prev_pl);
}


#if	__USE_PROTO__
int loopclose (queue_t * q, int __NOTUSED (flag), cred_t * __NOTUSED (credp))
#else
int
loopclose (q, flag, credp)
queue_t	      *	q;
int		flag;
cred_t	      *	credp;
#endif
{
	struct loop   *	loop;

	/*
	 * Disable put and service routines for the queue pair. This, plus the
	 * fact that open and close are single-threaded, makes any further
	 * locking of this "loop_loop []" entry unnecessary.
	 */

	qprocsoff (q);

	loop = (struct loop *) q->q_ptr;
	loop->qptr = NULL;


	/*
	 * If we are connected to another stream, break the linkage and send a
	 * hangup message. The original example didn't bother performing any
	 * locking for the disconnect, which is incorrect, because put and
	 * service routines for the other minor device could still be running.
	 */

	if (loop->oqptr != NULL) {
		struct loop   *	other = (struct loop *) loop->oqptr->q_ptr;
		pl_t		prev_pl;

		prev_pl = LOCK (other->lck, plstr);

		if (other->oqptr == WR (q)) {

			putnextctl (loop->oqptr, M_HANGUP);
			other->oqptr = loop->oqptr = NULL;
		}

		UNLOCK (other->lck, prev_pl);
	}

        return 0;
}
