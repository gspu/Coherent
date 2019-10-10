/*
 * The code in this file is taken from the example dump module listing in
 * Appendix B of the STREAMS Programmer's Guide for System V, Release 4 Multi-
 * Processor (Intel Edition). The overall structure of the code has been
 * retained, with some editing to reflect local coding standards, additional
 * commentary, and to bring the code up to SVR4MP DDI/DKI standards.
 */

#define	_DDI_DKI	1
#define	_SYSV4		1

#include <common/ccompat.h>
#include <sys/inline.h>
#include <sys/file.h>
#include <sys/errno.h>
#include <stddef.h>

/*
 * DUMP Module. This module prints data and ioctls going to and from a device
 * in real time. Printout is on the console. Usage is to push it on a stream
 * between any other modules.
 *
 * It accepts two ioctls:
 *	DUMP_VERB	Verbose printing of M_DATA (default)
 *	DUMP_TERSE	Terse printing of data
 *
 * The messages it prints begin with "I:" for incoming, "O:" for outgoing
 * data. "Ci" or "Co" are for non-data (control) messages. Data is printed in
 * character or hexadecimal format delimited by {{ and }} at message
 * boundaries.
 */

#include "sys/types.h"
#include "sys/stream.h"
#include "sys/cmn_err.h"
#include "dump.h"

static struct module_info dumprinfo = {
	0x6475, "dump", 0, INFPSZ, 0, 0
};
static struct module_info dumpwinfo = {
	0x6475, "dump", 0, INFPSZ, 0, 0
};


static	int	dumpopen	__PROTO ((queue_t * q, dev_t * devp, int flag,
					  int sflag, cred_t * credp));
static	int	dumpclose	__PROTO ((queue_t * q, int flag,
					  cred_t * credp));
static	void	dumpwput	__PROTO ((queue_t * q, mblk_t * mp));
static 	void	dumprput	__PROTO ((queue_t * q, mblk_t * mp));

static struct qinit rinit = {
	dumprput, NULL, dumpopen, dumpclose, NULL, & dumprinfo, NULL
};
static struct qinit winit = {
	dumpwput, NULL, NULL, NULL, NULL, & dumpwinfo, NULL
};

struct streamtab dumpinfo = {
	& rinit, & winit, NULL, NULL
};

int		dumpdevflag = 0;


/*
 * Gather information from this data message and print it. The caller deals
 * with actually forwarding the data message.
 */

#if	__USE_PROTO__
__LOCAL__ void (dumpgather) (queue_t * q, mblk_t * mp, dm_dir_t dir)
#else
__LOCAL__ void
dumpgather __ARGS ((q, mp, dir))
queue_t	      *	q;
mblk_t	      *	mp;
dm_dir_t	dir;
#endif
{
	struct dm_str *	d = (struct dm_str *) q->q_ptr;
	pl_t		prev_pl;
	mblk_t	      *	tmp;
	unsigned long	counter;
	char		junk [2];

	/*
	 * Check state and print direction indication when it changes.
	 */

	if (d->dm_dir != dir) {

		d->dm_dir = dir;
		cmn_err (CE_CONT, dir == D_IN ? "^\nIN:" : "^\nOUT:");
	}

	if (mp->b_datap == NULL ||
	    (mp->b_rptr == mp->b_wptr && mp->b_cont == NULL)) {
		/*
		 * Zero-length messages are treated specially in most streams,
		 * note them specially.
		 */

		cmn_err (CE_CONT, "^DUMP: Zero-length data message\n");
		return;
	}

	cmn_err (CE_CONT, "^ {{");

	tmp = mp;
	counter = 0;
	junk [1] = 0;

	prev_pl = splstr ();

	do {
		unsigned char *	readp;

		if ((d->dm_flags & D_VERB) == 0) {

			counter += tmp->b_wptr - tmp->b_rptr;
			continue;
		}

		for (readp = tmp->b_rptr ; readp < tmp->b_wptr ; readp ++)
			if ((junk [0] = * readp) >= ' ' && junk [0] <= '~')
				cmn_err (CE_CONT, "^ %s", junk);
			else
				cmn_err (CE_CONT, "^ 0x%x", junk [0]);

	} while ((tmp = tmp->b_cont) != NULL &&
		 tmp->b_datap->db_type == M_DATA);


	if ((d->dm_flags & D_VERB) == 0)
		cmn_err (CE_CONT, "^ %d", counter);

	cmn_err (CE_CONT, "^ }} ");

	if (tmp != NULL)
		cmn_err (CE_CONT, "^\nDUMP: non-data b_cont\n");

	splx (prev_pl);
}


/*
 * Completely handle one of our ioctl () calls, including the qreply () of the
 * message.
 */

#if	__USE_PROTO__
__LOCAL__ void (dumpioc) (queue_t * q, mblk_t * mp)
#else
__LOCAL__ void
dumpioc __ARGS ((q, mp))
queue_t	      *	q;
mblk_t	      *	mp;
#endif
{
	struct iocblk *	iocp = (struct iocblk *) mp->b_rptr;
	struct dm_str *	d = (struct dm_str *) q->q_ptr;

	cmn_err (CE_CONT, "^DUMP: own ioctl is ");

	switch (iocp->ioc_cmd) {

	case DUMP_VERB:
		d->dm_flags |= D_VERB;
		cmn_err (CE_CONT, "^DUMP_VERB\n");

		mp->b_datap->db_type = M_IOCACK;
		break;

	case DUMP_TERSE:
		d->dm_flags &= ~ D_VERB;
		cmn_err (CE_CONT, "^DUMP_TERSE\n");

		mp->b_datap->db_type = M_IOCACK;
		break;

	default:
		cmn_err (CE_CONT, "^ unknown, %x\n", iocp->ioc_cmd);

		mp->b_datap->db_type = M_IOCNAK;
		break;
	}

	iocp->ioc_count = 0;
	qreply (q, mp);
}


/*
 * Display information about control messages.
 */

#if	__USE_PROTO__
__LOCAL__ void (dumpctl) (queue_t * q, mblk_t * mp, dm_dir_t dir)
#else
__LOCAL__ void
dumpctl __ARGS ((q, mp, dir))
queue_t	      *	q;
mblk_t	      *	mp;
dm_dir_t	dir;
#endif
{
	__CONST__ char * tmp = NULL;

	cmn_err (CE_CONT, "^\nC%s: M_", dir == D_IN ? "i" : "o");

	switch (mp->b_datap->db_type) {

		/* just in case */
	case M_DATA:	tmp = "DATA";	break;

	case M_READ:	tmp = "READ";	break;

	case M_IOCTL:	tmp = "IOCTL";	break;

	case M_IOCACK:	tmp = "IOCACK";	break;

	case M_IOCNAK:	tmp = "IOCNAK";	break;

	case M_IOCDATA:	tmp = "IOCDATA"; break;

	case M_CTL:	tmp = "CTL";	break;

	case M_PROTO:	tmp = "PROTO";	break;

	case M_PCPROTO:	tmp = "PCPROTO"; break;

	case M_BREAK:	tmp = "BREAK";	break;

	case M_DELAY:	tmp = "DELAY";	break;

	case M_PASSFP:	tmp = "PASSFP";	break;

	case M_SETOPTS:	tmp = "SETOPTS"; break;

	case M_ERROR:	tmp = "ERROR";	break;

	case M_HANGUP:	tmp = "HANGUP";	break;

	case M_FLUSH:	tmp = "FLUSH";	break;

	case M_COPYOUT:	tmp = "COPYOUT"; break;

	case M_COPYIN:	tmp = "COPYIN";	break;

	case M_START:	tmp = "START";	break;

	case M_STOP:	tmp = "STOP";	break;

	case M_STARTI:	tmp = "STARTI";	break;

	case M_STOPI:	tmp = "STOPI";	break;

	case M_SIG:
		cmn_err (CE_CONT, "^SIG (%d) ", * mp->b_rptr);
		break;

	case M_PCSIG:
		cmn_err (CE_CONT, "^PCSIG (%d) ", * mp->b_rptr);
		break;

	default:
		cmn_err (CE_CONT, "^Unknown! (0x%x) ", mp->b_datap->db_type);
		break;
	}

	if (tmp != NULL)
		cmn_err (CE_CONT, "^%s", tmp);

	/*
	 * Reset the direction indicator so the next data message will print
	 * the direction it is headed in.
	 */

	((struct dm_str *) q->q_ptr)->dm_dir = D_NONE;
}


/*
 * Display information about generally-known ioctl ()s. Feel free to add more
 * knowledge to this to suit your application.
 */

#if	__USE_PROTO__
__LOCAL__ void (dumpshow) (mblk_t * __NOTUSED (mp), int cmd)
#else
__LOCAL__ void
dumpshow __ARGS ((mp, cmd))
mblk_t	      *	mp;
int		cmd;
#endif
{
	switch (cmd) {

	default:
        	return;
	}
}


/*
 * Open a dump filter.
 */

#if	__USE_PROTO__
int dumpopen (queue_t * q, dev_t * __NOTUSED (devp), int flag,
	      int __NOTUSED (sflag), cred_t * __NOTUSED (credp))
#else
int dumpopen (q, devp, flag, sflag, credp)
queue_t	      *	q;
dev_t	      *	devp;
int		flag;
int		sflag;
cred_t	      *	credp;
#endif
{
	int		i;

	if (q->q_ptr != NULL) {

		cmn_err (CE_CONT, "^DUMP: re-open slot %d\n",
			 (struct dm_str *) q->q_ptr - dm_users);

		if ((flag & FNDELAY) != 0)
			cmn_err (CE_CONT, "^DUMP: reopen, NDELAY set\n");
		if ((flag & FNONBLOCK) != 0)
			cmn_err (CE_CONT, "^DUMP: reopen, NONBLOCK set\n");

		qprocson (q);
		return 0;
	}

	/*
	 * Search for a slot to use. We depend on the single-threading of
	 * driver open routines to protect this code, and the complementary
	 * code in dumpclose ()
	 */

	for (i = 0 ; i < dm_ucnt ; i ++) {

		if (dm_users [i].dm_use == D_FREE) {

			dm_users [i].dm_use = D_USED;
			dm_users [i].dm_dir = D_NONE;
			dm_users [i].dm_flags = D_VERB;

			WR (q)->q_ptr = q->q_ptr = (_VOID *) & dm_users [i];

			cmn_err (CE_CONT, "^DUMP: flag = %d\n", flag);

			if ((flag & FNDELAY) != 0)
				cmn_err (CE_CONT, "^DUMP: open, NDELAY set\n");
			if ((flag & FNONBLOCK) != 0)
				cmn_err (CE_CONT, "^DUMP: open, NONBLOCK set\n");

			qprocson (q);
			return 0;
		}
	}

        return EAGAIN;
}


/*
 * Close an entry and deallocate the control slot.
 */

#if	__USE_PROTO__
int (dumpclose) (queue_t * q, int __NOTUSED (flag),
		 cred_t * __NOTUSED (credp))
#else
int
dumpclose (q, flag, credp)
queue_t	      *	q;
int		flag;
cred_t	      *	credp;
#endif
{
	struct dm_str *	d = (struct dm_str *) q->q_ptr;

	d->dm_use = D_FREE;
	d->dm_flags = 0;

	WR (q)->q_ptr = q->q_ptr = NULL;

	cmn_err (CE_CONT, "^\nDUMP: end trace\n");
	qprocsoff (q);
	return 0;
}


/*
 * Put procedure for write side of module. Gathers data from all passing
 * M_DATA messages. Calls routine to handle ioctl calls.
 */

#if	__USE_PROTO__
void dumpwput (queue_t * q, mblk_t * mp)
#else
void
dumpwput (q, mp)
queue_t	      *	q;
mblk_t	      *	mp;
#endif
{
	if (mp->b_datap->db_type == M_IOCTL) {
		struct iocblk *	iocp = (struct iocblk *) mp->b_rptr;
		struct dm_str *	d = (struct dm_str *) q->q_ptr;

		if ((iocp->ioc_cmd & DUMPIOC) == DUMPIOC) {

			dumpioc (q, mp);
			return;
		}

		cmn_err (CE_CONT, "^Co: M_IOCTL %x, cnt %d ",
			 iocp->ioc_cmd, iocp->ioc_count);

		if ((d->dm_flags & D_VERB) != 0 && mp->b_cont)
			dumpshow (mp->b_cont, iocp->ioc_count);

		d->dm_dir = D_NONE;

	} else if (mp->b_datap->db_type == M_DATA)
		dumpgather (q, mp, D_OUT);
	else
		dumpctl (q, mp, D_OUT);

	putnext (q, mp);
}


/*
 * Read side put procedure, simply discriminates between upwards-bound control
 * and data.
 */

#if	__USE_PROTO__
void dumprput (queue_t * q, mblk_t * mp)
#else
void
dumprput (q, mp)
queue_t	      *	q;
mblk_t	      *	mp;
#endif
{
	if (mp->b_datap->db_type == M_DATA)
		dumpgather (q, mp, D_IN);
	else
		dumpctl (q, mp, D_IN);

	putnext (q, mp);
}

