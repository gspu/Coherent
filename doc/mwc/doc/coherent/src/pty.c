
From mwc!gort!hal Tue Dec 20 22:14:43 1994
Return-Path: <mwc!gort!hal>
Received: from mwc by lepanto.mwc.com with uucp
	(Smail3.1.28.1 #3) id m0rKIRn-0001Z4C; Tue, 20 Dec 94 22:14 CST
Received: by mwc.com (smail2.5.3-coh) id AA07212; 21 Dec 94 03:33:22 GMT (Wed)
Received: by gort.mwc.com (smail2.5.3-coh) id AA00170; 20 Dec 94 21:29:17 CST (Tue)
Subject: new pty driver, more
To: fred@lepanto.mwc.com (Fred Butzen)
Date: Tue, 20 Dec 1994 21:29:16 +0100 (CST)
From: "Hal Snyder" <hal@gort.mwc.com>
Cc: udo
In-Reply-To: <9412201104.AA9672.V1.5@lepanto.mwc.com> from "Fred Butzen" at Dec 20, 94 11:04:05 am
Reply-To: hal@mwc.com
X-Mailer: ELM [version 2.4 PL23]
Content-Type: text
Content-Length: 10997     
Message-Id: <9412202129.AA00170@gort.mwc.com>
Status: RO

Text from Fred Butzen:

> The "more" that's included with 4.2.14 is the old 286 binary.  It core
> dumps if handed exactly two file arguments.  It works correctly in all
> other situations that I have tested; however, it fails if you try to
> search for a highlighted or underscored string, which limits its usefulness
> in displaying manual pages.  The source for this version does not work
> correctly if recompiled under Coherent 386.  Let's call this version
> "more-1".

Forget it.  Lose it to the bit bucket in the sky.

> I have hacked the above sources.  They recompile correctly under Coherent
> 4.2.14.  I've also added code to search correctly for highlighted or
> underscored strings.  I also had to hack the source slightly so that it
> would open /dev/tty to read input from the keyboard at the end of each
> page.  This version works correctly on the console, on a terminal, and in
> an "xterm" window; however, it dies with a bad file descriptor if I run it
> in an rxvt window.

Looks like a winner to me.  It is quite possible rxvt is not properly
detaching the controlling terminal.  Please try "ps alxg" on shells
running under both xterm and rxvt, and mail me the results (i.e., the
full ps listing in each case).

> (I think this is a problem with rxvt, as it has displayed
> other problems as well.)  It does display some odd behavior, though.  When
> I type a search string in a terminal, characters appear to get dropped on
> the floor unless you type *very* slowly.  And in an xterm window, when I
> type a search string, every character appears double (e.g., if you type
> "printf" you see "pprriinnttff").  So it works, but mostly.  Let's call
> this "more-2".

Please give me this set of bits.  I will beep you a new pty.c to try out
to see if any of the misfeatures go away.  See below.

> I have pulled the "more" sources off the BSD-Lite disk.  I can't compile
> these yet because they use constants that our headers don't grok.  Let's
> call this "more-3".

If the only bad constants are related to lseek, you should be able to make
the substitution by referring to our lseek man page.  I would like to see
what more-3 can do for us once you hack the constants.

Here is a new pty.c.  To use it, compile (cc -c pty.c -o Driver.o) and put
the Driver.o into /etc/conf/pty, then relink a kernel.  It might solve some
problems.
-- pty.c
/*
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
 * Revision 2.4  93/08/19  10:39:20  nigel
 * r83 ioctl (), corefile, new headers
 * 
 * Revision 2.3  93/08/19  04:03:06  nigel
 * Nigel's R83
 * 
 * Revision 2.2  93/07/26  15:32:12  nigel
 * Nigel's R80
 * 
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
 */

#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/poll.h>
#include <sys/cmn_err.h>

#include <sys/coherent.h>
#include <sys/uproc.h>
#include <sys/proc.h>
#include <sys/tty.h>		/* indirectly includes sgtty.h */
#include <sys/con.h>
#include <sys/devices.h>
#include <sys/sched.h>		/* CVTTOUT, IVTTOUT, SVTTOUT */


#define channel(dev)	(dev & 0x7F)
#define master(dev)	(dev & 0x80)


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
 * Support functions (local functions).
 */
static void ptycycle();

/* Configurable variables - see /etc/conf/pty/Space.c. */
extern int NUPTY;

static PTY * p;

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
			wakeup (& pp->p_mopen);
	}
}

/*
 * ptyload()
 */
static void
ptyload()
{
	int i;
	TTY * tp;

	if ((p = (PTY *) kalloc (NUPTY * sizeof (PTY))) == 0) {
		cmn_err (CE_WARN, "ptyload: can't allocate %s pty's\n", NUPTY);
		return;
	}

	memset (p, 0, NUPTY * sizeof (PTY));

	for (i = 0 ; i < NUPTY ; i ++) {
		tp = & p [i].p_tp;
		tp->t_start   = ptystart;
		tp->t_param   = NULL;
		tp->t_ddp     = (char *) i;
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
	TTY * tp = & pp->p_tp;

	if (chan >= NUPTY) {
		set_user_error (ENXIO);
		return;
	}

	if (master (dev)) {
		if (pp->p_mopen) {
			set_user_error (EBUSY);
			return;
		}
		if (tp->t_open)
			pp->p_mopen = 2;
		else
			pp->p_mopen = 1;
		wakeup ((char *) & tp->t_open);
		ptycycle (chan);
		return;
	}

	/* If we get this far, we are opening a slave device. */

	tp->t_flags |= T_HOPEN | T_STOP;
	for (;;) {	/* wait for carrier */
		if (pp->p_mopen)
			break;

		/* PTY driver is waiting for carrier.  */
		if (x_sleep ((char *) & tp->t_open, pritty,
			     slpriSigCatch, "ptycd") == PROCESS_SIGNALLED) {
			set_user_error (EINTR);
			tp->t_flags &= ~(T_HOPEN | T_STOP);
			return;
		}
	}

	tp->t_flags |= T_CARR;
	tp->t_flags &= ~(T_HOPEN | T_STOP);
	ttopen (tp);

	/* Kluge to test why some X11 apps are confused. */
	/* Try forcing some kind of raw mode on first open. */
	if (tp->t_open == 0) {
		tp->t_sgttyb.sg_flags |= RAW;
		tp->t_sgttyb.sg_flags &= ~ECHO;

		tp->t_termio.c_iflag = 0;

		tp->t_termio.c_cflag &= ~ (PARODD | PARENB);
		tp->t_termio.c_cflag |= CS8 | CREAD;

		tp->t_termio.c_oflag &= ~ OPOST;

		tp->t_termio.c_lflag &=
		  ~ (ECHONL | ISIG | ICANON | XCASE | ECHO);
	}

	tp->t_open ++;
	ttsetgrp (tp, dev, mode);
	if (pp->p_mopen == 1 || pp->p_mopen == 3)
		pp->p_mopen = 2;
}

/*
 * ptyclose()
 */

static void
ptyclose (dev, mode)
dev_t dev;
int mode;
{
	int chan = channel(dev);
	PTY * pp = p + chan;
	TTY * tp = & pp->p_tp;

	if (chan >= NUPTY) {
		set_user_error (ENXIO);
		return;
	}

	if (master (dev)) {
		if (pp->p_mopen) {
			tp->t_flags &= ~ T_CARR;
			tthup (tp);
			pp->p_mopen = 0;
		}
		return;
	}

	if (-- tp->t_open == 0) {
		ttclose (tp);
		if (pp->p_mopen == 2)
			pp->p_mopen = 3;
		wakeup (& pp->p_mopen);
	}
}

/*
 * ptyread()
 */

static void
ptyread(dev, iop)
dev_t dev;
IO * iop;
{
	int chan = channel (dev);
	PTY * pp = p + chan;
	TTY * tp = & pp->p_tp;
	int c;

	if (master (dev)) {
		int char_read = 0;

		while (iop->io_ioc) {
			if ((c = ttout (tp)) == -1) {
				if (char_read) {
					ttstart (tp);
					return;
				}
				if (iop->io_flag & (IONDLY | IONONBLOCK)) {
					if (tp->t_group == 0 ||
					    (iop->io_flag & IONONBLOCK) != 0)
						set_user_error (EAGAIN);
					return;
				}
				if (pp->p_mopen == 3) {
					set_user_error (EIO);
					return;
				}
				ttstart (tp);
				pp->p_asleep = 1;
				/* The PTY driver is waiting for a read.  */
				if (x_sleep (& pp->p_mopen, pritty,
					     slpriSigCatch, "ptyread")
				    == PROCESS_SIGNALLED) {
					set_user_error (EINTR);
					return;
				}
			} else {
				ioputc (c, iop);
				char_read = 1;
			}
		}
		return;
	}

#if 0
	if (pp->p_asleep) {
		pp->p_asleep = 0;
		wakeup (& pp->p_mopen);
	}
	pollwake (& pp->p_oev);
	ttread (tp, iop);
#else
	pp->p_asleep = 0;	/* ttread0 will awaken the sleeper */
	ttread0 (tp,iop,wakeup, & pp->p_mopen, pollwake, & pp->p_oev);
#endif
}

/*
 * ptywrite()
 */

static void
ptywrite(dev, iop)
dev_t dev;
IO * iop;
{
	int chan = channel(dev);
	PTY * pp = p + chan;
	TTY * tp = &pp->p_tp;
	int c;

	if (master (dev)) {
		while (iop->io_ioc) {
			if (! ttinp (tp)) {
				if (iop->io_flag & (IONDLY | IONONBLOCK)) {
					set_user_error (EAGAIN);
					return;
				}
				if (pp->p_mopen == 3) {
					set_user_error (EIO);
					return;
				}
				pp->p_asleep = 1;

				/* The PTY driver is waiting for a write.  */
				if (x_sleep (& pp->p_mopen, pritty,
					     slpriSigCatch, "ptywrite")
				    == PROCESS_SIGNALLED) {
					set_user_error (EINTR);
					return;
				}
			}

			c = iogetc (iop);
			ttin (tp, c);
		}
		wakeup (& pp->p_mopen);
	} else {
		pp->p_ttwr = 1;
		ttwrite0 (tp, iop, wakeup, & pp->p_mopen, pollwake,
			  & pp->p_iev);
		pp->p_ttwr = 0;
	}
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
	int chan = channel (dev);
	PTY * pp = p + chan;
	TTY * tp = & pp->p_tp;

	if (master (dev)) {
		set_user_error (EINVAL);
		return;
	}

	ttioctl (tp, com, vec);
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

	if (! master (dev))
		return ttpoll (tp, ev, msec);

	/*
	 * Priority polls not supported.
	 */
	ev &= (POLLIN | POLLOUT);

	/*
	 * Input poll with no data present.
	 */
	if ((ev & POLLIN) != 0 && ttoutp(tp) == 0) {

		/*
		 * Blocking input poll.
		 */
		if (msec != 0)
			pollopen (& pp->p_iev);

		/*
		 * Second look to avoid interrupt race.
		 */

		if (ttoutp (tp) == 0)
			ev &= ~POLLIN;
	}

	/*
	 * Output poll with no space.
	 */

	if ((ev & POLLOUT) != 0 && ttinp (tp) == 0) {
		/*
		 * Blocking output poll.
		 */

		if (msec != 0)
			pollopen (& pp->p_oev);

		/*
		 * Second look to avoid interrupt race.
		 */

		if (ttinp (tp) == 0)
			ev &= ~ POLLOUT;
	}

	return ev;
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
		wakeup (& pp->p_mopen);
		pollwake (& pp->p_oev);
	}

	/*
	 * Schedule next cycle.
	 */
	if (pp->p_mopen)
		timeout (& tp->t_rawtim, HZ / 10, ptycycle, chan);
}


/*
 * Configuration table (export data).
 */

CON ptycon = {
	DFCHR | DFPOL,			/* Flags */
	PTY_MAJOR,			/* Major index */
	ptyopen,			/* Open */
	ptyclose,			/* Close */
	NULL,				/* Block */
	ptyread,			/* Read */
	ptywrite,			/* Write */
	ptyioctl,			/* Ioctl */
	NULL,				/* Powerfail */
	NULL,				/* Timeout */
	ptyload,			/* Load */
	ptyunload,			/* Unload */
	ptypoll				/* Poll */
};

-- 
Hal Snyder, M.D.		hal@mwc.com			itis
Mark Williams Company     	(708)-291-6700 (voice)		foras
60 Revere Drive, Suite 250	(708)-291-6750 (fax)		rixsatis
Northbrook, IL 60062

