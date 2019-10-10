/*
 * Trivial echo driver for STREAMS testing.
 */

#define	_DDI_DKI	1
#define	_SYSV4		1

#include <common/ccompat.h>
#include <sys/stream.h>
#include <sys/stropts.h>
#include <stddef.h>


static struct module_info minfo = {
	0x1821, "echo", 0, INFPSZ, 512, 128
};

static	int	echoopen	__PROTO ((queue_t * q, dev_t * devp, int flag,
					  int sflag, cred_t * credp));
static	int	echoclose	__PROTO ((queue_t * q, int flag,
					  cred_t * credp));
static	void	echowput	__PROTO ((queue_t * q, mblk_t * mp));
static	void	echowsrv	__PROTO ((queue_t * q));
static	void	echorsrv	__PROTO ((queue_t * q));

static struct qinit rinit = {
	NULL, echorsrv, echoopen, echoclose, NULL, & minfo, NULL
};

static struct qinit winit = {
	echowput, echowsrv, NULL, NULL, NULL, & minfo, NULL
};

struct streamtab echoinfo = { & rinit, & winit, NULL, NULL };

int	echodevflag = 0;


#if	__USE_PROTO__
int echoopen (queue_t * q, dev_t * __NOTUSED (devp), int __NOTUSED (flag),
	      int __NOTUSED (sflag), cred_t * __NOTUSED (credp))
#else
int
echoopen (q, devp, flag, sflag, credp)
queue_t	      *	q;
dev_t	      *	devp;
int		flag;
int		sflag;
cred_t	      *	credp;
#endif
{
	/*
	 * Enable put and service routines for this queue pair.
	 */

	qprocson (q);
	return 0;
}


#if	__USE_PROTO__
void echowput (queue_t * q, mblk_t * mp)
#else
void
echowput (q, mp)
queue_t	      *	q;
mblk_t	      *	mp;
#endif
{
	switch (mp->b_datap->db_type) {

	case M_IOCTL:
		/*
		 * Invalid ioctl (). Setting "ioc_error" causes the
		 * ioctl () call to return that particular errno. By
		 * default, ioctl () will return EINVAL on failure.
		 */

		mp->b_datap->db_type = M_IOCNAK;
		qreply (q, mp);
		break;

	case M_FLUSH:
		/*
		 * Canonical driver flush processing.
		 */

		if ((* mp->b_rptr & FLUSHW) != 0)
			flushq (q, FLUSHALL);

		if ((* mp->b_rptr & FLUSHR) != 0)
			flushq (RD (q), FLUSHALL);

		* mp->b_rptr &= ~ FLUSHW;

		if ((* mp->b_rptr & FLUSHR) != 0)
			qreply (RD (q), mp);
		else
			freemsg (mp);
		break;

	case M_DATA:
	case M_PROTO:
	case M_PCPROTO:
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
void echowsrv (queue_t * q)
#else
void
echowsrv (q)
queue_t	      *	q;
#endif
{
	mblk_t	      *	mp;

	while ((mp = getq (q)) != NULL) {
		/*
		 * If the read side becomes full, keep the messages on this
		 * side to exert back-pressure upstream.
		 */

		if (! pcmsg (mp->b_datap->db_type) && ! canputnext (RD (q))) {

			putbq (q, mp);
			break;
		}

		putnext (RD (q), mp);
	}
}


#if	__USE_PROTO__
void echorsrv (queue_t * q)
#else
void
echorsrv (q)
queue_t	      *	q;
#endif
{
	/*
	 * Deal with being back-enabled by flow control by enabling the write
	 * side service procedure to retry sending the messages backed up
	 * there.
	 */

	qenable (WR (q));
}


#if	__USE_PROTO__
int echoclose (queue_t * q, int __NOTUSED (flag), cred_t * __NOTUSED (credp))
#else
int
echoclose (q, flag, credp)
queue_t	      *	q;
int		flag;
cred_t	      *	credp;
#endif
{
	/*
	 * Disable put and service routines for the queue pair.
	 */

	qprocsoff (q);
	return 0;
}
