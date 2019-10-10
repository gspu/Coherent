/*
 * File:	pty.c
 *
 * Purpose:	pseudoterminal device driver
 *
 *	Master devices are named /dev/pty[pqrs][0-f].
 *	Corresponding slaves are /dev/tty[pqrs][0-f].
 *
 *	Minor numbers are 0..127 assigned in increasing order,
 *	plus 128 for master device.
 *
 *	Output written to master appears as input to slave and
 *	vice versa.  Data path is:
 *
 *		app	master		slave	line	app
 *		using	device	shunt	device	disc.	using
 *		master	driver		driver	module	slave
 *
 *      slave read does ttread() which is fed by ttin()
 *	master write does ttin()
 *
 *      slave write does ttwrite() which is fed by ttout()
 *	master read does ttout()
 *
 * $Log:	pty.c,v $
 * Revision 1.4  93/04/14  10:12:10  root
 * r75
 * 
 * Revision 1.3  92/07/16  16:35:29  hal
 * Kernel #58
 * 
 * Revision 1.2  92/03/18  07:45:33  hal
 * master device polling added
 * 
 * Revision 1.1  92/03/16  12:57:31  hal
 * Initial revision
 * 
 */

/*
 * -----------------------------------------------------------------
 * Includes.
 */
#include <sys/coherent.h>
#include <sys/stat.h>
#include <sys/proc.h>
#include <sys/tty.h>		/* indirectly includes sgtty.h */
#include <sys/con.h>
#include <sys/devices.h>
#include <errno.h>
#include <poll.h>
#include <sys/sched.h>		/* CVTTOUT, IVTTOUT, SVTTOUT */

/*
 * -----------------------------------------------------------------
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */

#define channel(dev)	(dev & 0x7F)
#define master(dev)	(dev & 0x80)
#ifdef _I386
#define	EEBUSY	EBUSY
#else
#define	EEBUSY	EDBUSY
#endif

/*
 * Explanation of p_mopen values:
 * 0 - master is closed
 * 1 - master open, waiting for slave to open
 * 2 - master and slave both open
 * 3 - master open, slave has closed
 */

typedef struct pty {
	TTY p_tp;
	event_t p_iev;
	event_t p_oev;
	char p_mopen;
	char p_asleep;	/* master is asleep in read or write awaiting slave */
	char p_ttwr;	/* slave is suspended in mid ttwrite() */
} PTY;

/*
 * -----------------------------------------------------------------
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */
int nulldev();
void pollwake();

/*
 * Configuration functions (local functions).
 */
static void ptyclose();
static void ptyioctl();
static void ptyload();
static void ptyopen();
static void ptyread();
static void ptyunload();
static void ptywrite();
static void ptystart();
static int ptypoll();

/*
 * Support functions (local functions).
 */
static void ptycycle();

/*
 * -----------------------------------------------------------------
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */
int NUPTY = 8;

/*
 * Configuration table (export data).
 */
CON ptycon ={
	DFCHR|DFPOL,			/* Flags */
	PTY_MAJOR,			/* Major index */
	ptyopen,			/* Open */
	ptyclose,			/* Close */
	nulldev,			/* Block */
	ptyread,			/* Read */
	ptywrite,			/* Write */
	ptyioctl,			/* Ioctl */
	nulldev,			/* Powerfail */
	nulldev,			/* Timeout */
	ptyload,			/* Load */
	ptyunload,			/* Unload */
	ptypoll				/* Poll */
};

static PTY * p;

/*
 * -----------------------------------------------------------------
 * Code.
 */

/*
 * ptyload()
 */
static void
ptyload()
{
	int i;
	TTY * tp;

	if ((p = (PTY *)kalloc(NUPTY*sizeof(PTY))) == 0) {
		printf("ptyload: can't allocate %s pty's\n", NUPTY);
		return;
	}
	kclear(p, NUPTY*sizeof(PTY));
	for (i = 0; i < NUPTY; i++) {
		tp = &p[i].p_tp;
		tp->t_cs_sel  = cs_sel();
		tp->t_start   = ptystart;
		tp->t_param   = nulldev;
		tp->t_ddp     = (char *)i;
	}
}

/*
 * ptyunload()
 */
static void
ptyunload()
{
	if (p)
		kfree(p);
	printf("pty driver unloaded\n");
}

/*
 * ptyopen()
 */
static void
ptyopen(dev, mode)
dev_t dev;
int mode;
{
	int chan = channel(dev);
	PTY * pp = p + chan;
	TTY * tp = &pp->p_tp;

	if (chan >= NUPTY) {
		u.u_error = ENXIO;
		goto open_done;
	}

	if (master(dev)){
		if (pp->p_mopen) {
			u.u_error = EEBUSY;
			goto open_done;
		}
		if (tp->t_open)
			pp->p_mopen = 2;
		else
			pp->p_mopen = 1;
		wakeup((char *)(&tp->t_open));
		ptycycle(chan);
	} else {
		tp->t_flags |= T_HOPEN | T_STOP;
		for (;;) {	/* wait for carrier */
			if (pp->p_mopen)
				break;
#ifdef _I386
			x_sleep((char *)(&tp->t_open), pritty, slpriSigCatch,
			  "ptycd");
#else
			v_sleep((char *)(&tp->t_open), CVTTOUT, IVTTOUT,
			  SVTTOUT, "ptycd");
#endif
			/* PTY driver is waiting for carrier.  */
			if (SELF->p_ssig && nondsig()) {  /* signal? */
				u.u_error = EINTR;
				tp->t_flags &= ~(T_HOPEN | T_STOP);
				goto open_done;
			}
		}
		tp->t_flags |= T_CARR;
		tp->t_flags &= ~(T_HOPEN | T_STOP);
		ttopen(tp);
		tp->t_open++;
		ttsetgrp(tp, dev, mode);
		if (pp->p_mopen == 1 || pp->p_mopen == 3)
			pp->p_mopen = 2;
	}
open_done:;
}

/*
 * ptyclose()
 */
static void
ptyclose(dev, mode)
dev_t dev;
int mode;
{
	int chan = channel(dev);
	PTY * pp = p + chan;
	TTY * tp = &pp->p_tp;

	if (chan >= NUPTY) {
		u.u_error = ENXIO;
		return;
	}

	if (master(dev)){
		if (pp->p_mopen) {
			tp->t_flags &= ~T_CARR;
			tthup(tp);
			pp->p_mopen = 0;
		}
	} else {
		if (--tp->t_open == 0) {
			ttclose(tp);
			if (pp->p_mopen == 2)
				pp->p_mopen = 3;
			wakeup(&pp->p_mopen);
		}
	}
}

/*
 * ptyread()
 */
static void
ptyread(dev, iop)
dev_t dev;
register IO * iop;
{
	int chan = channel(dev);
	PTY * pp = p + chan;
	TTY * tp = &pp->p_tp;
	int c;

	if (master(dev)){
		int char_read = 0;

		while (iop->io_ioc) {
			c = ttout(tp);
			if (c == -1) { /* nothing to fetch */
				if (char_read) {
					ttstart(tp);
					goto read_done;
				}
				if (iop->io_flag & IONDLY) {
					u.u_error = EAGAIN;
					goto read_done;
				}
				if (pp->p_mopen == 3) {
					u.u_error = EIO;
					goto read_done;
				}
				ttstart(tp);
				pp->p_asleep = 1;
#ifdef _I386
				x_sleep(&pp->p_mopen, pritty, slpriSigCatch,
				  "ptyread");
#else
				v_sleep(&pp->p_mopen, CVTTOUT, IVTTOUT,
				  SVTTOUT, "ptyread");
#endif
				/* The PTY driver is waiting for a read.  */
				if (SELF->p_ssig && nondsig()) {
					u.u_error = EINTR;
					goto read_done;
				}
			} else {
				ioputc(c, iop);
				char_read = 1;
			}
		}
read_done:;
	} else {
#if 0
		if (pp->p_asleep) {
			pp->p_asleep = 0;
			wakeup(&pp->p_mopen);
		}
		pollwake(&pp->p_oev);
		ttread(tp, iop);
#else
		pp->p_asleep = 0;	/* ttread0 will awaken the sleeper */
		ttread0(tp,iop,wakeup,&pp->p_mopen,pollwake,&pp->p_oev);
#endif
	}
}

/*
 * ptywrite()
 */
static void
ptywrite(dev, iop)
dev_t dev;
register IO * iop;
{
	int chan = channel(dev);
	PTY * pp = p + chan;
	TTY * tp = &pp->p_tp;
	int c;

	if (master(dev)){
		while (iop->io_ioc) {
			if (!ttinp(tp)) {
				if (iop->io_flag & IONDLY) {
					u.u_error = EAGAIN;
					goto write_done;
				}
				if (pp->p_mopen == 3) {
					u.u_error = EIO;
					goto write_done;
				}
				pp->p_asleep = 1;
#ifdef _I386
				x_sleep(&pp->p_mopen, pritty, slpriSigCatch,
				  "ptywrite");
#else
				v_sleep(&pp->p_mopen, CVTTOUT, IVTTOUT,
				  SVTTOUT, "ptywrite");
#endif
				/* The PTY driver is waiting for a write.  */
				if (SELF->p_ssig && nondsig()) {  /* signal? */
					u.u_error = EINTR;
					goto write_done;
				}
			}
			c = iogetc(iop);
			ttin(tp, c);
		}
		wakeup(&pp->p_mopen);
	} else {
		pp->p_ttwr = 1;
		ttwrite0(tp,iop,wakeup,&pp->p_mopen,pollwake,&pp->p_iev);
		pp->p_ttwr = 0;
	}
write_done:;
}

/*
 * ptyioctl()
 */
static void
ptyioctl(dev, com, vec)
dev_t	dev;
int	com;
struct sgttyb *vec;
{
	int chan = channel(dev);
	PTY * pp = p + chan;
	TTY * tp = &pp->p_tp;

	if (master(dev)){
		u.u_error = EINVAL;
	} else {
		ttioctl(tp, com, vec);
	}
}

/*
 * ptystart()
 */
static void
ptystart(tp)
TTY * tp;
{
	int chan = (int)tp->t_ddp;
	PTY * pp = p + chan;

	if (chan >= 0 && chan < NUPTY) {
		if (pp->p_ttwr)
			wakeup(&pp->p_mopen);
	}
}

/*
 * ptypoll()
 */
static int
ptypoll(dev, ev, msec)
dev_t dev;
int ev;
int msec;
{
	int chan = channel(dev);
	PTY * pp = p + chan;
	TTY * tp = &pp->p_tp;
	int ret;

	if (master(dev)) {
		/*
		 * Priority polls not supported.
		 */
		ev &= (POLLIN | POLLOUT);

		/*
		 * Input poll with no data present.
		 */
		if ((ev & POLLIN) && (ttoutp(tp) == 0)) {

			/*
			 * Blocking input poll.
			 */
			if (msec != 0) {
				pollopen(&pp->p_iev);
			}

			/*
			 * Second look to avoid interrupt race.
			 */
			if (ttoutp(tp) == 0)
				ev &= ~POLLIN;
		}

		/*
		 * Output poll with no space.
		 */
		if ((ev & POLLOUT) && (ttinp(tp) == 0)) {

			/*
			 * Blocking output poll.
			 */
			if (msec != 0) {
				pollopen(&pp->p_oev);
			}

			/*
			 * Second look to avoid interrupt race.
			 */
			if (ttinp(tp) == 0)
				ev &= ~POLLOUT;
		}

		ret = ev;
	} else
		ret = ttpoll(tp, ev, msec);
	return ret;
}

/*
 * ptycycle()
 *
 * Do a wakeup of any sleeping pty's at regular intervals.
 */
static void
ptycycle(chan)
int chan;
{
	PTY * pp = p + chan;
	TTY * tp = &pp->p_tp;

	/*
	 * Do wakeups.
	 */
	if (pp->p_asleep || pp->p_ttwr) {
		wakeup(&pp->p_mopen);
		pollwake(&pp->p_oev);
	}

	/*
	 * Schedule next cycle.
	 */
	if (pp->p_mopen)
		timeout(&tp->t_rawtim, HZ/10, ptycycle, chan);
}
