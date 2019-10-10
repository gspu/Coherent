#define	_DDI_DKI	1
#define	_SYSV4		1

/*
 * This file contains the stream head put and service routines.
 */

#include <common/ccompat.h>
#include <kernel/strmlib.h>
#include <sys/debug.h>
#include <sys/types.h>
#include <sys/stream.h>
#include <sys/stropts.h>
#include <sys/poll.h>
#include <sys/errno.h>
#include <sys/signal.h>
#include <stddef.h>


#define	QUEUE_POLL(q)		((shead_t *) (q)->q_ptr)->sh_pollhead


/*
 * This function processes a "set options" request for a stream.
 */

#if	__USE_PROTO__
__LOCAL__ void (STREAM_SETOPT) (queue_t * q, struct stroptions * so)
#else
__LOCAL__ void
STREAM_SETOPT __ARGS ((q, so))
queue_t	      *	q;
struct stroptions
	      *	so;
#endif
{
	shead_t	      *	sheadp;
	ulong_t		flags;
	pl_t		prev_pl;

	ASSERT (q != NULL);
	ASSERT (so != NULL);

	sheadp = (shead_t *) q->q_ptr;
	flags = so->so_flags;

	prev_pl = SHEAD_LOCK (sheadp);

	if ((flags & SO_READOPT) != 0) {
		/*
		 * Set the read option flags. If multiple of the flag values
		 * are specified, collapse to one.
		 */

		SHEAD_SRDOPT (sheadp, so->so_readopt);
	}

	if ((flags & SO_WROFF) != 0)
		sheadp->sh_wroff = so->so_wroff;

	if ((flags & SO_MREADOFF) != 0)
		sheadp->sh_flags &= ~ SH_READMSG;
	else if ((flags & SO_MREADON) != 0)
		sheadp->sh_flags |= SH_READMSG;

	if ((flags & SO_NDELOFF) != 0)
		sheadp->sh_flags &= ~ SH_NDELAY;
	else if ((flags & SO_NDELON) != 0)
		sheadp->sh_flags |= SH_NDELAY;

	if ((flags & SO_ISNTTY) != 0)
		sheadp->sh_flags &= ~ SH_TTY;
	else if ((flags & SO_ISTTY) != 0)
		sheadp->sh_flags |= SH_TTY;

	if ((flags & SO_TONSTOP) != 0)
		sheadp->sh_flags &= ~ SH_TOSTOP;
	else if ((flags & SO_TOSTOP) != 0)
		sheadp->sh_flags |= SH_TOSTOP;

	SHEAD_UNLOCK (sheadp, prev_pl);


	prev_pl = QFREEZE_TRACE (q, "STREAM_SETOPT");

	if ((flags & SO_MINPSZ) != 0)
		q->q_minpsz = so->so_minpsz;

	if ((flags & SO_MAXPSZ) != 0)
		q->q_maxpsz = so->so_maxpsz;

	QUNFREEZE_TRACE (q, prev_pl);

	QBAND_SETOPT (q, so);
}


/*
 * The read side put procedure, where data enters the stream head. Process
 * the priority messages, queue the normal ones for processes to grab.
 *
 * We use the stream head service procedure purely for doing the less time-
 * critical stuff such as waking up processes waiting for data and dealing
 * with polling.
 */

#if	__USE_PROTO__
__LOCAL__ void headrput (queue_t * q, mblk_t * mp)
#else
__LOCAL__ void
headrput (q, mp)
queue_t	      *	q;
mblk_t	      *	mp;
#endif
{
	shead_t	      *	sheadp = (shead_t *) q->q_ptr;
	int		tmp;
        pl_t		prev_pl;

	switch (mp->b_datap->db_type) {

	case M_PCPROTO:
		/*
		 * Basically like normal data/protocol messages, but we
		 * discard later arrivals if there is already such a message
		 * queued at the stream head.
		 */

		prev_pl = QFREEZE_TRACE (q, "headrput");

		tmp = q->q_first->b_datap->db_type == M_PCPROTO;

		QUNFREEZE_TRACE (q, prev_pl);

		if (tmp)
			break;

		/* FALL INTO */
	case M_PROTO:
	case M_DATA:
		putq (q, mp);
		mp = NULL;
		break;

	case M_PCSIG:
		/*
		 * Priority signals get generated straight away.
		 */

		SHEAD_SIGNAL (sheadp, * mp->b_rptr);
		break;

	case M_SIG:
		/*
		 * Non-priority signals are generated in-band when the signal
		 * message is dequeued by a reading process.
		 */

		putq (q, mp);
		mp = NULL;
		break;


	case M_IOCACK:
	case M_IOCNAK:
		/*
		 * Send this message to the specific waiting process based
		 * on the ID field.
		 */

		prev_pl = SHEAD_LOCK (sheadp);

		if (sheadp->sh_ioc_seq ==
			    ((struct iocblk *) mp->b_rptr)->ioc_id &&
		    sheadp->sh_ioc_msg == NULL) {
			/*
			 * Park the message and let the waiting process know
			 * about it.
			 */

			sheadp->sh_ioc_msg = mp;
			mp = NULL;
		}

		SHEAD_UNLOCK (sheadp, prev_pl);

		SHEAD_WAKE (sheadp, SH_IOCTL_WAIT);
		break;

	case M_IOCTL:
		/*
		 * Thanks to the operation of STREAMS pipes, we can see
		 * messages sent from the other end.
		 */

		mp->b_datap->db_type = M_IOCNAK;
		qreply (q, mp);
		mp = NULL;
		break;

	case M_FLUSH:
		/*
		 * Check to see what action we should perform;
		 */

		tmp = * mp->b_rptr;

		if ((tmp & FLUSHR) != 0) {

			if ((tmp & FLUSHBAND) != 0)
				flushband (q, mp->b_rptr [1], FLUSHALL);
			else
				flushq (q, FLUSHALL);

			tmp = * mp->b_rptr &= ~ FLUSHR;
		}

		if ((tmp & FLUSHW) != 0) {

			qreply (q, mp);
			mp = NULL;
		}
		break;

	case M_ERROR:
		prev_pl = SHEAD_LOCK (sheadp);

		if (mp->b_wptr > mp->b_rptr + 1) {
			/*
			 * Two-byte form of the M_ERROR message.
			 */

			if (mp->b_rptr [0] != NOERROR)
				sheadp->sh_rerrcode = mp->b_rptr [0];

			if (mp->b_rptr [1] != NOERROR)
				sheadp->sh_rerrcode = mp->b_rptr [1];
		} else {
			/*
			 * One-byte form of M_ERROR; NOERROR or 0 are not
			 * valid values for the error code.
			 */

			tmp = (uchar_t) mp->b_rptr [0];

			if (tmp == 0 || tmp == NOERROR)
				tmp = ENXIO;

			sheadp->sh_rerrcode = tmp;
			sheadp->sh_werrcode = tmp;
		}


		/*
		 * Since we already have the stream head locked, and because
		 * we want to wake up everyone, we call SV_BROADCAST ()
		 * directly rather than going through SHEAD_WAKE ().
		 */

		sheadp->sh_lock_mask &= ~ SH_WAIT_MASK;
		SV_BROADCAST (sheadp->sh_wait_sv, 0);

		SHEAD_UNLOCK (sheadp, prev_pl);

		tmp = (sheadp->sh_rerrcode != 0 ? FLUSHR : 0) |
			(sheadp->sh_werrcode != 0 ? FLUSHW : 0);

		if (tmp != 0) {

			pollwakeup (sheadp->sh_pollhead, POLLERR);
			putctl1 (W (sheadp->sh_head), M_FLUSH, tmp);
		}
		break;

	case M_HANGUP:
		prev_pl = SHEAD_LOCK (sheadp);

		sheadp->sh_flags |= SH_HANGUP;

		/*
		 * Since we already have the stream head locked, and because
		 * we want to wake up everyone, we call SV_BROADCAST ()
		 * directly rather than going through SHEAD_WAKE ().
		 */

		sheadp->sh_lock_mask &= ~ SH_WAIT_MASK;
		SV_BROADCAST (sheadp->sh_wait_sv, 0);

		SHEAD_UNLOCK (sheadp, prev_pl);

		if (sheadp->sh_controller != NULL) {
			/*
			 * If we have acquired a controlling process, send a
			 * SIGHUP to the process (and not the foreground
			 * process group).
			 */

			proc_signal (sheadp->sh_controller, SIGHUP);
		}

		break;

	case M_SETOPTS:
		STREAM_SETOPT (q, (struct stroptions *) mp->b_rptr);
		break;
	}

	if (mp != NULL)
		freemsg (mp);
}


/*
 * The stream head read side service procedure exists to defer notification of
 * data arrivals at the stream head.
 */

#if	__USE_PROTO__
__LOCAL__ void headrsrv (queue_t * q)
#else
__LOCAL__ void
headrsrv (q)
queue_t	      *	q;
#endif
{
	mblk_t	      *	msg;
	pl_t		prev_pl;

	prev_pl = QFREEZE_TRACE (q, "headrsrv");

	if ((msg = q->q_first) != NULL && datamsg (msg->b_datap->db_type)) {

		if (! pcmsg (msg->b_datap->db_type)) {

			pollwakeup (QUEUE_POLL (q), POLLIN);

			pollwakeup (QUEUE_POLL (q),
				    msg->b_band == 0 ? POLLRDNORM :
				    		       POLLRDBAND);
		} else
			pollwakeup (QUEUE_POLL (q), POLLPRI);

	}

	QUNFREEZE_TRACE (q, prev_pl);

	if (msg != NULL)
		SHEAD_WAKE ((shead_t *) q->q_ptr, SH_READ_WAIT);
}


/*
 * The stream head write put procedure is not normally used; stream head
 * actions call putq () directly. However, M_FLUSH processing comes through
 * here.
 */

#if	__USE_PROTO__
__LOCAL__ void headwput (queue_t * q, mblk_t * mp)
#else
__LOCAL__ void
headwput (q, mp)
queue_t	      *	q;
mblk_t	      *	mp;
#endif
{
	if (mp->b_datap->db_type == M_FLUSH &&
	    (* mp->b_rptr & FLUSHW) != 0) {

		if ((* mp->b_rptr & FLUSHBAND) != 0)
			flushband (q, mp->b_rptr [1], FLUSHALL);
		else
			flushq (q, FLUSHALL);

		pollwakeup (QUEUE_POLL (q), POLLOUT);
		SHEAD_WAKE ((shead_t *) q->q_ptr, SH_WRITE_WAIT);
	}

	putq (q, mp);
}


/*
 * The stream head write service procedure just forwards messages on down the
 * line, checking for (band) flow control.
 */

#if	__USE_PROTO__
__LOCAL__ void headwsrv (queue_t * q)
#else
__LOCAL__ void
headwsrv (q)
queue_t	      *	q;
#endif
{
	mblk_t	      *	mp;

	pollwakeup (QUEUE_POLL (q), POLLWRBAND);

	while ((mp = getq (q)) != NULL) {

		if (pcmsg (mp->b_datap->db_type) ||
		    (mp->b_band > 0 && bcanputnext (q, mp->b_band)) ||
		    canputnext (q))
			putnext (q, mp);
		else {
			putbq (q, mp);
			return ;
		}
	}

	/*
	 * Flow control has been relieved on this queue; let the processes
	 * know about it!
	 */

	pollwakeup (QUEUE_POLL (q), POLLOUT);
	SHEAD_WAKE ((shead_t *) q->q_ptr, SH_WRITE_WAIT);
}


/*
 * since the stream head is a queue pair like any other, we need a
 * streamtab/qinit for it.
 */

__LOCAL__ struct module_info headrinfo = {
	0, "standard head", 0, 256, 512, 0
};

__LOCAL__ struct qinit defstreamread = {
	headrput, headrsrv, NULL, NULL, NULL,
	& headrinfo, NULL
}, defstreamwrite = {
	headwput, headwsrv, NULL, NULL, NULL,
	& headrinfo, NULL
};

struct streamtab headinfo = {
	& defstreamread, & defstreamwrite, NULL, NULL
};

