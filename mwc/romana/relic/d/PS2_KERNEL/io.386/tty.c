/*
 * File:	$USRSRC/ttydrv/tty.c
 *
 * Purpose:	COHERENT line discipline module.
 *	This is the common part of typewriter service. It handles all device-
 *	independent aspects of a typewriter, including tandem flow control,
 *	erase and kill, stop and start, and common ioctl functions.
 *
 * $Log:	tty.c,v $
 * Revision 1.2  92/08/04  12:54:52  bin
 * upd for ker59
 * 
 * Revision 1.12  92/06/06  12:39:28  hal
 * Last before adding termio field to tty struct.
 * 
 * Revision 1.11  92/04/30  08:58:50  hal
 * Add asy.  Remove silos from tty struct.
 * 
 * Revision 1.10  92/04/13  10:14:01  hal
 * Kernel #52.
 * 
 * Revision 1.9  92/03/18  07:44:47  hal
 * ttwrite0() needed to break pty deadlock.
 * 
 * Revision 1.8  92/03/18  05:27:05  hal
 * Add ttrtp(), ttinp(), ttoutp().
 * Fix ctrl-s/ctrl-q transposition in termio routine.
 * TIOCSET[PN] now sets RAW depending on RAWIN&RAWOUT & vice versa.
 * 
 * Revision 1.7  92/02/20  20:25:36  hal
 * Minor fixes
 * 
 * Revision 1.6  92/02/20  19:43:30  piggy
 * Add "mode" arg to ttsetgrp() for NOCTTY support.
 * 
 * Revision 1.5  92/02/16  23:14:23  hal
 * Initial termio support.
 * 
 * Revision 1.4  92/02/16  18:27:33  hal
 * Binary compatibility with Sys V sgtty
 * 
 * Revision 1.3  92/02/15  15:43:46  root
 * Merge with 286 kernel tty.c.
 */

/*
 * About STOP flag bits:
 *	T_ISTOP is set when the tty module's input queue is in danger of
 *		overflow.  It is up to the device driver to check this flag
 *		and do something about it.  If ttin() is called with a
 *		character from the device while T_ISTOP is set, the  character
 *		is discarded.  T_ISTOP is cleared when the input queue is
 *		sufficiently empty.  The device driver can monitor this bit for
 *		hardware flow control.
 *	T_TSTOP is the "Tandem" flow control flag for input.  If TANDEM is set
 *		and the input queue is in danger of overflow, t_stopc is sent
 *		and T_TSTOP is set.  When the input queue is empty enough,
 *		t_startc is sent and T_TSTOP is cleared.
 *	T_STOP is the flow control bit for output.  No output will be
 *		written to the output queue while this bit is true.
 *		Except for initialization of flags in the TTY struct, by
 *		ttopen(), this bit is not written by tty.c.
 *	91/09/15 - hal
 */

/*
 * Includes.
 */
#include <sys/clist.h>
#include <sys/coherent.h>
#include <sys/con.h>
#include <sys/deftty.h>
#include <sys/io.h>
#include <sys/proc.h>
#include <sys/sched.h>
#include <sys/stat.h>
#include <sys/tty.h>
#include <errno.h>
#ifdef _I386
#include <termio.h>
#include <sys/inode.h>
#else
#endif

#ifdef TRACER
#define DUMPSGTTY(sp)	dumpsgtty(sp)

static void dumpsgtty(sp)
struct sgttyb * sp;
{
	T_HAL(2, printf("S:%x:%x ", sp->sg_ispeed, sp->sg_flags));
}
#else
#define DUMPSGTTY(sp)
#endif

/*
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */

#define	SGTTY_CPY_LEN	(sizeof (struct sgttyb))

/* NEAR_OR_FAR_CALL is for invoking t_param and t_start */
#ifdef _I386
#define	 NEAR_OR_FAR_CALL(tp_fn)  { (*tp->tp_fn)(tp); }
#define SET_HPCL { \
	if (tp->t_termio.c_cflag & HUPCL) \
		tp->t_flags |= T_HPCL; \
	else \
		tp->t_flags &= ~T_HPCL; }
#else
#define	 NEAR_OR_FAR_CALL(tp_fn)  {\
	if (tp->t_cs_sel) \
		ld_call(tp->t_cs_sel, tp->tp_fn, tp); \
	else \
		(*tp->tp_fn)(tp); }
#define SET_HPCL
#endif

/*
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */
void ttclose();
void ttflush();
void tthup();
void ttin();
int  ttinp();
void ttioctl();
void ttopen();
int  ttout();
int  ttoutp();
int  ttpoll();
void ttread();
void ttread0();
void ttsetgrp();
void ttsignal();
void ttstart();
void ttwrite();
void ttwrite0();

static void ttstash();
static void ttrtp();

#ifdef _I386
static void make_termio();
static void make_sg();
#else
#define make_termio(a1,a2,a3)
#define make_sg(a1,a2,a3)
#endif

/*
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */
extern	int	wakeup();
extern	void	pollwake();

/*
 * ttopen()
 *
 *	Called by driver on first open.
 *	Set up defaults.
 */
void
ttopen(tp)
register TTY *tp;
{
	tp->t_escape = 0;
	tp->t_sgttyb.sg_ispeed = tp->t_dispeed;
	tp->t_sgttyb.sg_ospeed = tp->t_dospeed;
	tp->t_sgttyb.sg_erase  = DEF_SG_ERASE;
	tp->t_sgttyb.sg_kill   = DEF_SG_KILL;
	tp->t_sgttyb.sg_flags  = DEF_SG_FLAGS;
	tp->t_tchars.t_intrc   = DEF_T_INTRC;
	tp->t_tchars.t_quitc   = DEF_T_QUITC;
	tp->t_tchars.t_startc  = DEF_T_STARTC;
	tp->t_tchars.t_stopc   = DEF_T_STOPC;
	tp->t_tchars.t_eofc    = DEF_T_EOFC;
	tp->t_tchars.t_brkc    = DEF_T_BRKC;
#ifdef _I386
	make_termio(&tp->t_sgttyb, &tp->t_tchars, &tp->t_termio);
	if (tp->t_flags & T_HPCL)
		tp->t_termio.c_cflag |= HUPCL;
	else
		tp->t_termio.c_cflag &= ~HUPCL;
	tp->t_termio.c_cflag |= (CS8|CREAD);
#endif
	if (tp->t_param)
		NEAR_OR_FAR_CALL(t_param)
}

/*
 * ttsetgrp()
 *
 *	If process is a group leader without a control terminal,
 *	make its control terminal this device.
 *
 *	If process is a group leader and this device does not have
 *	a process group, give it the group of the current process.
 */
void ttsetgrp(tp, ctdev, mode)
register TTY *tp;
dev_t ctdev;
int mode;
{
	register PROC *pp;

	pp = SELF;
#ifdef _I386
	if (pp->p_group == pp->p_pid && 0 == (mode & IPNOCTTY)) {
#else
	if (pp->p_group == pp->p_pid) {
#endif
		if (pp->p_ttdev == NODEV)
			pp->p_ttdev = ctdev;
		if (tp->t_group == 0)
			tp->t_group = pp->p_pid;
	}
}

/*
 * ttyclose()
 *
 *	Called by driver on the last close.
 *	Wait for all pending output to go out.
 *	Kill input.
 */
void ttclose(tp)
register TTY *tp;
{
	register int s;

	while (tp->t_oq.cq_cc) {
		s = sphi();
		if (tp->t_oq.cq_cc) {
			tp->t_flags |= T_DRAIN;
			v_sleep((char *)&tp->t_oq, CVTTOUT, IVTTOUT, SVTTOUT,
				"ttydrain");
			/* The line discipline is waiting for the tty to drain.  */
		}
		spl(s);
		if (SELF->p_ssig && nondsig())
			break;
	}
	ttflush(tp);
	tp->t_flags = tp->t_group = 0;
}

/*
 * ttread()
 *
 *	The read routine for a tty device driver will call this function.
 *
 *	Move data from tp->t_iq to io segment iop.
 *	Number of characters to copy is in iop->ioc.
 *
 *	In cooked mode, copy up to the first newline or break character, or
 *	until the count runs out.
 *	In CBREAK or RAW modes, return when count runs out or when input clist
 *	is empty and we're returning at least one byte.
 */

void ttread(tp, iop)
register TTY *tp;
register IO *iop;
{
	ttread0(tp, iop, 0, 0, 0, 0);
}

/*
 * ttread0()
 *
 *	Move data from user (in IO struct) to clists.
 *	Do wakeups on functions supplied when read is blocked or completed.
 */
void ttread0(tp, iop, func1, arg1, func2, arg2)
register TTY *tp;
register IO *iop;
int (*func1)(), arg1, (*func2)(), arg2;
{
	register int c;
	int o;
	int sioc = iop->io_ioc;  /* number of bytes to read */

	while (iop->io_ioc) {
		o = sphi();
		while ((c = getq(&tp->t_iq)) < 0) {
			if ((tp->t_flags & T_CARR) == 0) {
			   u.u_error = EIO;  /* error since no carrier */
			   spl(o);
			   return;
			}

			/* If we're in CBREAK or RAW mode, and we don't */
			/* have the special "blocking read" bit set for */
			/* these modes, and we read at least one byte   */
		        /* of input, return immediately, since we have  */
			/* run out of characters from the clist.	*/

			if (ISBBYB && ((tp->t_flags & T_BRD) == 0)
			   && iop->io_ioc < sioc) {
			   spl(o);
			   return;
			}

			/*
			 * Non-blocking reads.
			 * Tell user process to try again later.
			 */
			if (iop->io_flag & IONDLY) {
				u.u_error = EAGAIN;
				spl(o);
				return;
			}

			tp->t_flags |= T_INPUT;  /* wait for more data */
			if (func1)
				(*func1)(arg1);
			if (func2)
				(*func2)(arg2);
			v_sleep((char *)&tp->t_iq, CVTTIN, IVTTIN, SVTTIN,
				"ttywait");
			/* The line discipline is waiting for more data.  */

			if (SELF->p_ssig && nondsig()) {
				if (iop->io_ioc == sioc)
					u.u_error = EINTR;
				spl(o);
				return;
			}
		}
		/*
		 * Flow control - can we turn on input from the driver yet?
		 */
		if (tp->t_iq.cq_cc <= ILOLIM) {
			if (tp->t_flags & T_ISTOP)
				tp->t_flags &= ~T_ISTOP;
			if (ISTAND && (tp->t_flags&T_TSTOP)) {
				tp->t_flags &= ~T_TSTOP;
				while (putq(&tp->t_oq, startc) < 0) {
					ttstart(tp);
					waitq();
				}
				ttstart(tp);
			}
		}
		spl(o);
		if (!ISBBYB && ISEOF)
			goto read_done;
		if (ioputc(c, iop) < 0)
			goto read_done;
		if (!ISBBYB && (c=='\n' || ISBRK))
			goto read_done;
	}
read_done:
	if (func1)
		(*func1)(arg1);
	if (func2)
		(*func2)(arg2);
}

/*
 * ttwrite()
 *
 *	Write routine.
 */
void ttwrite(tp, iop)
register TTY *tp;
register IO *iop;
{
	ttwrite0(tp, iop, 0, 0, 0, 0);
}

/*
 * ttwrite0()
 *
 *	Move data from user (in IO struct) to clists.
 *	Do wakeups on functions supplied when write is blocked or completed.
 */
void ttwrite0(tp, iop, func1, arg1, func2, arg2)
register TTY *tp;
register IO *iop;
int (*func1)(), arg1, (*func2)(), arg2;
{
	register int c;
	int o;

	/*
	 * Non-blocking writes which can fit.
	 * NOTE: exhaustion of clists can still cause blocking writes.
	 */
	if ((iop->io_flag & IONDLY) && (OHILIM >= iop->io_ioc)) {

		/*
		 * No room.
		 */
		if (tp->t_oq.cq_cc >= OHILIM-iop->io_ioc) {
			u.u_error = EAGAIN;
			return;
		}
	}

	while ((c = iogetc(iop)) >= 0) {
		if ((tp->t_flags & T_CARR) == 0) {
			u.u_error = EIO;  /* error since no carrier */
			return;
		}
		o = sphi();
		while (tp->t_oq.cq_cc >= OHILIM) {
			ttstart(tp);
			if (tp->t_oq.cq_cc < OHILIM)
				break;
			tp->t_flags |= T_HILIM;
			if (func1)
				(*func1)(arg1);
			if (func2)
				(*func2)(arg2);
			v_sleep((char *)&tp->t_oq, CVTTOUT, IVTTOUT, SVTTOUT,
				"ttyoq");
			/*
			 * The line discipline is waiting for an output
			 * queue to drain.
			 */
			if (SELF->p_ssig && nondsig()) {
				u.u_error = EINTR;
				spl(o);
				return;
			}
		}
		while (putq(&tp->t_oq, c) < 0) {
			ttstart(tp);
			waitq();
		}
		spl(o);
	}
	o = sphi();
	ttstart(tp);
	spl(o);
	if (func1)
		(*func1)(arg1);
	if (func2)
		(*func2)(arg2);
}

/*
 * ttioctl()
 *
 *	This routine handles common typewriter ioctl functions.
 *	Note that flushing the stream now means drain the output
 *	and clear the input.
 */
void ttioctl(tp, com, vec)
register TTY *tp;
int com;
register struct sgttyb *vec;
{
	register int	flush = 0;
	register int	drain = 0;
	int s;
	int rload = 0;
	int was_bbyb;

	/*
	 * Keep sgttyb, t_chars, AND termio structs for each tty device.
	 *
	 * TCSET* writes a new termio and converts so as to update
	 * sgttyb and t_chars as well.
	 *
	 * TIOCSET[NP] writes new sgttyb and converts so as to update termio.
	 *
	 * TIOCSETC writes new t_chars and converts so as to update termio.
	 */
	switch (com) {
#ifdef _I386
	case TCGETA:
		kucopy(&tp->t_termio, vec, sizeof(struct termio));
		break;
	case TCSETA:
		ukcopy(vec, &tp->t_termio, sizeof(struct termio));
		was_bbyb = ISBBYB;	/* previous mode */
		make_sg(vec, &tp->t_sgttyb, &tp->t_tchars);
		SET_HPCL;
		++rload;
		if (!was_bbyb && ISBBYB)
			ttrtp(tp);
		break;
	case TCSETAW:
		ukcopy(vec, &tp->t_termio, sizeof(struct termio));
		make_sg(vec, &tp->t_sgttyb, &tp->t_tchars);
		SET_HPCL;
		++drain;	  /* delay for output */
		++rload;
		break;
	case TCSETAF:
		ukcopy(vec, &tp->t_termio, sizeof(struct termio));
		make_sg(vec, &tp->t_sgttyb, &tp->t_tchars);
		SET_HPCL;
	        ++flush;          /* flush input */
		++drain;	  /* delay for output */
		++rload;
		break;
#endif
	case TIOCQUERY:
		kucopy(&tp->t_iq.cq_cc, vec, sizeof(int));
		break;
	case TIOCGETP:
		kucopy(&tp->t_sgttyb, vec, SGTTY_CPY_LEN);
		break;
	case TIOCSETP:
		DUMPSGTTY(&tp->t_sgttyb);
	        ++flush;          /* flush input */
		++drain;	  /* delay for output */
		++rload;
		ukcopy(vec, &tp->t_sgttyb, SGTTY_CPY_LEN);
		make_termio(&tp->t_sgttyb, &tp->t_tchars, &tp->t_termio);
		break;
	case TIOCSETN:
		DUMPSGTTY(&tp->t_sgttyb);
		was_bbyb = ISBBYB;	/* previous mode */
		++rload;
		ukcopy(vec, &tp->t_sgttyb, SGTTY_CPY_LEN);
		make_termio(&tp->t_sgttyb, &tp->t_tchars, &tp->t_termio);
		if (!was_bbyb && ISBBYB)
			ttrtp(tp);
		break;
	case TIOCGETC:
		kucopy(&tp->t_tchars, vec, sizeof (struct tchars));
		break;
	case TIOCSETC:
		++rload;
		++drain;
		ukcopy(vec, &tp->t_tchars, sizeof (struct tchars));
		make_termio(&tp->t_sgttyb, &tp->t_tchars, &tp->t_termio);
		break;
	case TIOCEXCL:
		s = sphi();
		tp->t_flags |= T_EXCL;
		spl(s);
		break;
	case TIOCNXCL:
		s = sphi();
		tp->t_flags &= ~T_EXCL;
		spl(s);
		break;
	case TIOCHPCL:		/* set hangup on last close */
		s = sphi();
		tp->t_flags |= T_HPCL;
		spl(s);
#ifdef _I386
		tp->t_termio.c_cflag |= HUPCL;
#endif
		break;
	case TIOCCHPCL:		/* don't hangup on last close */
		if (!super())   /* only superuser may do this */
			u.u_error = EPERM;        /* not su */
		else {
			s = sphi();
			tp->t_flags &= ~T_HPCL;   /* turn off hangup bit */
			spl(s);
#ifdef _I386
			tp->t_termio.c_cflag &= ~HUPCL;
#endif
		}
		break;
	case TIOCGETTF:		/* get tty flag word */
		kucopy(&tp->t_flags, (unsigned *) vec, sizeof(unsigned));
		break;
#ifdef _I386
	case TCFLSH:	/* sleazy - should look at 2nd arg but don't yet */
#endif
	case TIOCFLUSH:
		++flush;        /* flush both input and output */
/*		++drain;	Why? - hws - 91/11/22	*/
		break;
	case TIOCBREAD:		/* blocking read for CBREAK/RAW mode */
		s = sphi();
		tp->t_flags |= T_BRD;
		spl(s);
		break;
	case TIOCCBREAD:	/* turn off CBREAK/RAW blocking read mode */
		s = sphi();
		tp->t_flags &= ~T_BRD;
		spl(s);
		break;
	/*
	 * The following is a hack so that the process group for /dev/console
	 * contains the current login shell running on it.
	 * Only expect /etc/init to use this ugliness.
	 */
	case TIOCSETG:
		if (super())
			tp->t_group = SELF->p_group;
		break;
	default:
		u.u_error = EINVAL;
	}

	/*
	 * T_STOP is set under two conditions:
	 * - a modem control device is awaiting carrier
	 * - a stopc (usually Ctrl-S) character was received.
	 *
	 * If ioctl just put device into RAWIN mode, make sure device
	 * is not still waiting for startc.
	 */
	if (ISRIN && (tp->t_flags & T_STOP) && !(tp->t_flags & T_HOPEN)) {
		s = sphi();
		tp->t_flags &= ~T_STOP;
		ttstart(tp);
		spl(s);
	}

	/*
	 * Wait for output to drain, or signal to arrive.
	 */
	if (drain) {
		while (tp->t_oq.cq_cc) {
			s = sphi();
			tp->t_flags |= T_DRAIN;
			spl(s);
			v_sleep((char *)&tp->t_oq, CVTTOUT, IVTTOUT, SVTTOUT,
				"ttyiodrn");
			/* A TIOC has asked for tty output to drain.  */
			if (SELF->p_ssig && nondsig())
				break;
		}
	}

	/*
	 * Flush input.
	 */
	if (flush)
		ttflush(tp);

	/*
	 * Re-initialize hardware.
	 */
	if (rload) {
		if ((tp->t_sgttyb.sg_flags & RAWIN)
		  && (tp->t_sgttyb.sg_flags & RAWOUT))
			tp->t_sgttyb.sg_flags |= RAW;
		if (tp->t_sgttyb.sg_flags & RAW)
			tp->t_sgttyb.sg_flags |= (RAWIN|RAWOUT);
		if (tp->t_param)
			NEAR_OR_FAR_CALL(t_param)
	}
}

/*
 * ttpoll()
 *
 *	Polling routine.
 *	[System V.3 Compatible]
 */
int ttpoll(tp, ev, msec)
register TTY * tp;
int ev;
int msec;
{
	/*
	 * Priority polls not supported.
	 */
	ev &= ~POLLPRI;

	/*
	 * Input poll with no data present.
	 */
	if ((ev & POLLIN) && (tp->t_iq.cq_cc == 0)) {

		/*
		 * Blocking input poll.
		 */
		if (msec)
			pollopen(&tp->t_ipolls);

		/*
		 * Second look to avoid interrupt race.
		 */
		if (tp->t_iq.cq_cc == 0)
			ev &= ~POLLIN;
	}

	/*
	 * Output poll with no space.
	 */
	if ((ev & POLLOUT) && (tp->t_oq.cq_cc >= OLOLIM)) {

		/*
		 * Blocking output poll.
		 */
		if (msec)
			pollopen(&tp->t_opolls);

		/*
		 * Second look to avoid interrupt race.
		 */
		if (tp->t_oq.cq_cc >= OLOLIM)
			ev &= ~POLLIN;
	}

	if (((ev & POLLIN) == 0) && ((tp->t_flags & T_CARR) == 0))
		ev |= POLLHUP;

	return ev;
}

/*
 * ttout()
 *
 *	Pull a character from the output queues of the typewriter.
 *	Doing fills, newline insert, tab expansion, etc.
 *
 *	If the stream is empty return a -1.
 *	Called at high priority.
 */
int ttout(tp)
register TTY *tp;
{
	register int c;

	if (tp->t_nfill) {
		--tp->t_nfill;
		c = tp->t_fillb;
	} else if (tp->t_flags & T_INL) {
		tp->t_flags &= ~T_INL;
		c = '\n';
	} else {
		if ((c=getq(&tp->t_oq)) < 0)
			return -1;
		if (!ISROUT) {
			if (c=='\n' && ISCRMOD) {
				tp->t_flags |= T_INL;
				c = '\r';
			} else if (c=='\t' && ISXTABS) {
				tp->t_nfill = ~(tp->t_hpos|~07);
				tp->t_fillb = ' ';
				c = ' ';
			}
		}
	}
	if (!ISROUT) {
		if (c == '\b') {
			if (tp->t_hpos)
				--tp->t_hpos;
		} else if (c == '\r')
			tp->t_hpos = 0;
		else if (c == '\t')
			tp->t_hpos = (tp->t_hpos|07) + 1;
#if NOT_8_BIT
		else if (c >= ' ' && c <= '~')
#else
		else if ((c >= ' ' && c <= '~') || (c >= 0200 && c <= 0376))
#endif
			++tp->t_hpos;
	}
	return c;
}

/*
 * ttin()
 *
 *	Pass a character to the device independent typewriter routines.
 *	Handle erase and kill, tandem flow control, and other magic.
 *	Often called at high priority from the driver's interrupt routine.
 */
void
ttin(tp, c)
register TTY *tp;
register int c;
{
	int dc, i, n;
	int s;

	if (!ISRIN) {
#if NOT_8_BIT
		c &= 0177;
#endif
		if (ISINTR) {
			ttsignal(tp, SIGINT);
			goto ttin_ret;
		}
		if (ISQUIT) {
			ttsignal(tp, SIGQUIT);
			goto ttin_ret;
		}
	}
	if (tp->t_flags & T_ISTOP)
		goto ttin_ret;
	if (!ISRIN) {
		if (c=='\r' && ISCRMOD)
			c = '\n';
		if (tp->t_escape) {
			if (c == ESC)
				++tp->t_escape;
			else {
				if (ISERASE || ISKILL) {
					c |= 0200;
					--tp->t_escape;
				}
				while (tp->t_escape && tp->t_ibx<NCIB-1) {
					tp->t_ib[tp->t_ibx++] = ESC;
					--tp->t_escape;
				}
				ttstash(tp, c);
			}
			if (ISECHO) {
#if NOT_8_BIT
				putq(&tp->t_oq, c&0177);
#else
				putq(&tp->t_oq, c); /* no strip for 8-bit */
#endif
				ttstart(tp);
			}
			goto ttin_ret;
		}
		if (ISERASE && !ISCBRK) {
			while (tp->t_escape && tp->t_ibx<NCIB-1) {
				tp->t_ib[tp->t_ibx++] = ESC;
				--tp->t_escape;
			}
			if (tp->t_ibx == 0)
				goto ttin_ret;
			dc = tp->t_ib[--tp->t_ibx];
			if (ISECHO) {
				if (!ISCRT)
					putq(&tp->t_oq, c);
				/* don't erase for bell, null, or rubout */
#if NOT_8_BIT
				else if (((c = dc&0177) == '\007')
					|| c == 0 || c == 0177)
#else
				else if (((c = dc) == '\007')
					|| c == 0 || c == 0177 || c == 0377)
#endif
				        goto ttin_ret;
				else if (c != '\b' && c != '\t') {
					putq(&tp->t_oq, '\b');
					putq(&tp->t_oq,  ' ');
					putq(&tp->t_oq, '\b');
				} else if (c == '\t') {
					n = tp->t_opos + tp->t_escape;
					for (i=0; i<tp->t_ibx; ++i) {
						c = tp->t_ib[i];
#if NOT_8_BIT
						if (c & 0200) {
							++n;
							c &= 0177;
						}
#endif
						if (c == '\b')
							--n;
						else {
							if (c == '\t')
								n |= 07;
							++n;
						}
					}
					while (n++ < tp->t_hpos)
						putq(&tp->t_oq, '\b');
				}
#if NOT_8_BIT
				if (dc & 0200) {
					if ((dc&0177) != '\b')
						putq(&tp->t_oq, '\b');
					putq(&tp->t_oq,  ' ');
					putq(&tp->t_oq, '\b');
				}
#endif
				ttstart(tp);
			}
			goto ttin_ret;
		}
		if (ISKILL && !ISCBRK) {
			tp->t_ibx = 0;
			tp->t_escape = 0;
			if (ISECHO) {
				if (c < 0x20) {
					putq(&tp->t_oq, '^');
					c += 0x40;
				}
				putq(&tp->t_oq, c);
				putq(&tp->t_oq, '\n');
				ttstart(tp);
			}
			goto ttin_ret;
		}
	}
	if (ISBBYB) {
		putq(&tp->t_iq, c);
		if (tp->t_flags & T_INPUT) {
			s = sphi();
			tp->t_flags &= ~T_INPUT;
			spl(s);
			defer(wakeup, (char *) &tp->t_iq);
		}
		if (tp->t_ipolls.e_procp) {
			tp->t_ipolls.e_procp = 0;
			defer(pollwake, (char *) &tp->t_ipolls);
		}
	} else {
		if (tp->t_ibx == 0)
			tp->t_opos = tp->t_hpos;
		if (c == ESC)
			++tp->t_escape;
		else
			ttstash(tp, c);
	}
	if (ISECHO) {
		if (ISRIN || !ISEOF) {
			putq(&tp->t_oq, c);
			ttstart(tp);
		}
	}
	if ((n=tp->t_iq.cq_cc)>=IHILIM) {
		s = sphi();
		tp->t_flags |= T_ISTOP;
		spl(s);
	} else if (ISTAND && (tp->t_flags&T_TSTOP)==0 && n>=ITSLIM) {
		s = sphi();
		tp->t_flags |= T_TSTOP;
		spl(s);
		putq(&tp->t_oq, stopc);
		ttstart(tp);
	}

ttin_ret:
	return;
}

/*
 * ttstash()
 *
 *	Cooked mode.
 *	Put character in the buffer and check for end of line.
 *	Only a legal end of line can take the last character position.
 *
 *	Only called from ttin(), and ttin() is called at high priority.
 */
static void ttstash(tp, c)
register TTY *tp;
{
	register char *p1, *p2;

	if (c=='\n' || ISEOF || ISBRK) {
		p1 = &tp->t_ib[0];
		p2 = &tp->t_ib[tp->t_ibx];
		*p2++ = c;			/* Always room */
		while (p1 < p2)
#if NOT_8_BIT
			putq(&tp->t_iq, (*p1++)&0177);
#else
			putq(&tp->t_iq, (*p1++));
#endif
		tp->t_ibx = 0;
		tp->t_escape = 0;

		if (tp->t_flags & T_INPUT) {
			tp->t_flags &= ~T_INPUT;
			defer(wakeup, (char *) &tp->t_iq);
		}

		if (tp->t_ipolls.e_procp) {
			tp->t_ipolls.e_procp = 0;
			defer(pollwake, (char *) &tp->t_ipolls);
		}

	} else if (tp->t_ibx < NCIB-1)
		tp->t_ib[tp->t_ibx++] = c;
}

/*
 * ttstart()
 *
 *	Start output on a tty.
 *	Duck out if stopped.  Do wakeups.
 */
void ttstart(tp)
register TTY *tp;
{
	register int n;
	int s;

	n = tp->t_flags;
	if (n & T_STOP)
		goto stdone;

	if ((n&T_DRAIN) && tp->t_oq.cq_cc==0
	   && (n&T_INL)==0 && tp->t_nfill==0) {
		s = sphi();
		tp->t_flags &= ~T_DRAIN;
		spl(s);
		defer(wakeup, (char *) &tp->t_oq);
		goto stdone;
	}

	NEAR_OR_FAR_CALL(t_start)

	if (tp->t_oq.cq_cc > OLOLIM)
		goto stdone;

	if (n & T_HILIM) {
		s = sphi();
	   	tp->t_flags &= ~T_HILIM;
		spl(s);
		defer(wakeup, (char *) &tp->t_oq);
	}

	if (tp->t_opolls.e_procp) {
		tp->t_opolls.e_procp = 0;
		defer(pollwake, (char *) &tp->t_opolls);
	}
stdone:
	return;
}

/*
 * ttflush()
 *
 *	Flush a tty.
 *	Called to clear out queues.
 */
void ttflush(tp)
register TTY *tp;
{
	int s;

	clrq(&tp->t_iq);
	clrq(&tp->t_oq);

	if (tp->t_flags & T_INPUT) {
		defer(wakeup, (char *) &tp->t_iq);
	}

	if (tp->t_flags & (T_DRAIN|T_HILIM)) {
		defer(wakeup, (char *) &tp->t_oq);
	}

	if (tp->t_ipolls.e_procp) {
		tp->t_ipolls.e_procp = 0;
		defer(pollwake, (char *) &tp->t_ipolls);
	}

	if (tp->t_opolls.e_procp) {
		tp->t_opolls.e_procp = 0;
		defer(pollwake, (char *) &tp->t_opolls);
	}

	tp->t_ibx = 0;
	tp->t_escape = 0;
	s = sphi();
	tp->t_flags &= T_SAVE;  /* reset most flag bits */
	spl(s);
}

/*
 * ttsignal()
 *
 *	Send a signal to every process in the given process group.
 */
void
ttsignal(tp, sig)
TTY *tp;
int sig;
{
	register int g;
	register PROC *pp;

	g = tp->t_group;
	if (g == 0)
		goto sigdone;
	ttflush(tp);
	pp = &procq;
	while ((pp=pp->p_nforw) != &procq)
		if (pp->p_group == g) {
			sendsig(sig, pp);
		}
sigdone:
	return;
}

/*
 * tthup()
 *
 *	Flag hangup internally to force errors on tty read/write, flush tty,
 *	then send hangup signal.
 */
void tthup(tp)
register TTY *tp;
{
	ttflush(tp);
	ttsignal(tp, SIGHUP);
}

#ifdef _I386
/*
 * Convert from sgttyb and tchars structs to termio.
 */
static void
make_termio(sgp, tcp, trp)
struct sgttyb * sgp;
struct tchars * tcp;
struct termio * trp;
{
	trp->c_cc[VINTR] = tcp->t_intrc;
	trp->c_cc[VQUIT] = tcp->t_quitc;
	trp->c_cc[VEOF ] = tcp->t_eofc;
	trp->c_cc[VEOL ] = '\n';
	trp->c_cc[VERASE] = sgp->sg_erase;
	trp->c_cc[VKILL ] = sgp->sg_kill;

	trp->c_iflag = BRKINT | ISTRIP | IXON | IGNPAR | INPCK | ISTRIP;
	trp->c_oflag = OPOST;
	trp->c_cflag &= (CSIZE|HUPCL|CLOCAL|CREAD);
	trp->c_lflag = ICANON | ISIG | ECHONL | ECHOK;

	if (sgp->sg_flags & TANDEM)
		trp->c_iflag |= IXOFF;

	if (sgp->sg_flags & CRMOD)
		trp->c_iflag |= ICRNL;

	if (sgp->sg_flags & LCASE) {
		trp->c_lflag |= XCASE;
		trp->c_iflag |= IUCLC;
		trp->c_oflag |= OLCUC;
	}

	if (sgp->sg_flags & RAWIN)
		trp->c_iflag &= ~IXON;

	if (sgp->sg_flags & RAWOUT)
		trp->c_oflag &= ~OPOST;

	if (sgp->sg_flags & XTABS)
		trp->c_oflag |= XTABS;

	if (sgp->sg_flags & CRMOD)
		trp->c_oflag |= ONLCR;

	if (sgp->sg_flags & (EVENP|ODDP)) {
		trp->c_cflag |= PARENB;
		if (sgp->sg_flags & ODDP)
			trp->c_cflag |= PARODD;
	}
	trp->c_cflag |= sgp->sg_ispeed;

	if (sgp->sg_flags & CRT)
		trp->c_lflag |= ECHOE;

	if (sgp->sg_flags & RAWIN)
		trp->c_lflag &= ~(ISIG|ICANON);

	if (sgp->sg_flags & CBREAK)
		trp->c_lflag &= ~ICANON;

	if (sgp->sg_flags & ECHO)
		trp->c_lflag |= ECHO;
}

/*
 * Convert from termio struct to sgttyb and tchars.
 */
static void
make_sg(trp, sgp, tcp)
struct termio * trp;
struct sgttyb * sgp;
struct tchars * tcp;
{
	T_HAL(1, { printf("T:%x:%x:%x:%x ", trp->c_iflag, trp->c_oflag, \
	  trp->c_cflag, trp->c_lflag);});
	tcp->t_intrc = trp->c_cc[VINTR];
	tcp->t_quitc = trp->c_cc[VQUIT];
	tcp->t_startc= '\021';		/* Ctrl-Q */
	tcp->t_stopc = '\023';		/* Ctrl-S */
	tcp->t_eofc  = trp->c_cc[VEOF];
	tcp->t_brkc  = -1;

	sgp->sg_erase  = trp->c_cc[VERASE];
	sgp->sg_kill   = trp->c_cc[VKILL ];
	sgp->sg_ispeed = trp->c_cflag & CBAUD;
	sgp->sg_ospeed = sgp->sg_ispeed;
	sgp->sg_flags  = RAWIN | RAWOUT | CBREAK;

	if (trp->c_lflag & ECHO)
		sgp->sg_flags |= ECHO;

	if (trp->c_lflag & ECHOE)
		sgp->sg_flags |= CRT;

	if ( (trp->c_lflag & XCASE)
	  || (trp->c_oflag & OLCUC)
	  || (trp->c_iflag & IUCLC))
		sgp->sg_flags |= LCASE;

	if (trp->c_iflag & IXOFF)
		sgp->sg_flags |= TANDEM;

	if (trp->c_iflag & ICRNL)
		sgp->sg_flags |= CRMOD;

	if (trp->c_oflag & ONLCR)
		sgp->sg_flags |= CRMOD;

	if (trp->c_oflag & OPOST)
		sgp->sg_flags &= ~RAWOUT;

	if (trp->c_oflag & XTABS)
		sgp->sg_flags |= XTABS;

	if (trp->c_cflag & PARENB) {
		if (trp->c_cflag & PARODD)
			sgp->sg_flags |= ODDP;
		else
			sgp->sg_flags |= EVENP;
	}

	if (trp->c_lflag & ISIG)
		sgp->sg_flags &= ~RAWIN;

	if (trp->c_lflag & ICANON)
		sgp->sg_flags &= ~CBREAK;
}
#endif

/*
 * ttrtp()
 *
 * Recover contents of typeahead when changing modes.
 * Called for ioctls of TIOCSETP and TCSETA,
 * when going from not byte-by-byte input to BBYB.
 */
static void
ttrtp(tp)
TTY * tp;
{
	register char	*p1, *p2;

	if (tp->t_ibx) {
		p1 = &tp->t_ib[0];
		p2 = &tp->t_ib[tp->t_ibx];
		while (p1 < p2) {
#if NOT_8_BIT
			putq(&tp->t_iq, (*p1++) & 0177);
#else
			putq(&tp->t_iq, (*p1++));
#endif
		}
		tp->t_ibx = 0;
	}
}

/*
 * ttinp()
 *
 * Return nonzero if ttin() may be called to send data for pickup by ttread(),
 * or 0 if not.
 */
int
ttinp(tp)
TTY * tp;
{
	return ((tp->t_flags&T_ISTOP) == 0);
}

/*
 * ttoutp()
 *
 * Return nonzero if ttout() may be called to fetch data stored by ttwrite(),
 * or 0 if not.
 */
int
ttoutp(tp)
TTY * tp;
{
	return (tp->t_nfill || (tp->t_flags&T_INL) || tp->t_oq.cq_cc);
}
