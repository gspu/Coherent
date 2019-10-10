/*
 * File:	$USRSRC/ttydrv/tty.c
 *
 * Purpose:	COHERENT line discipline module.
 *	This is the common part of typewriter service. It handles all device-
 *	independent aspects of a typewriter, including tandem flow control,
 *	erase and kill, stop and start, and common ioctl functions.
 *
 * $Log:	tty.c,v $
 * Revision 1.9  91/09/17  06:06:42  bin
 * updated by hal
 * 
 * Revision 1.8  91/09/13  18:01:39  piggy
 * Only do XON/XOFF flow control if TANDEM is set.
 * 
 * Revision 1.7  91/09/13  17:58:00  hal
 * Drop 3rd arg (was writing PSW directly from it!) for ttread/ttwrite.
 * General face lift.
 *
 *
 * Bug: no support for 8-bit characters.
 * Fix: don't strip keyboard input. 01/22/91.  (norm)
 *
 * Bug:	Switching modes between cooked and CBREAK/RAW left buffered input
 *	in the input buffer until returning to cooked mode. 05/13/91 norm
 *
 * Bug: setting speed to default in ttopen() was conditioned to
 *      use hard constants.  90/08/28.  hws
 *
 * Revision 1.5  91/06/06  18:28:53  norm
 * Restore 8-bit fix.
 *
 * Revision 1.2	89/07/17  11:51:20 	src
 * Bug:	Terminal could lock up when setting it to RAWIN mode, if
 * 	output was suspended due to X-OFF, and output data was present.
 * Fix:	Setting terminal to RAWIN mode now clears X-OFF, starts output
 * 	BEFORE waiting for output to drain.  Received signals now cause
 * 	operation to complete without waiting for drain. (ABC)
 *
 * Revision 1.1	88/03/24  16:18:12	src
 * Initial revision
 *
 * 86/12/12	Allan Cornish		/usr/src/sys/drv/tty.c
 * Added 3rd argument to ttpoll() to support non-blocking polls.
 *
 * 86/11/19	Allan Cornish		/usr/src/sys/drv/tty.c
 * Made ttread() and ttwrite() recognize the IONDLY flag in iop->io_flag.
 * wakeup() and pollwake() now have delayed invocation by defer().
 * Added poll [System V.3] capability.
 *
 * 85/06/28	Allan Cornish
 * made ttioctl() clear T_STOP flag if ISRIN.
 *
 * 85/03/04	Allan Cornish
 * made ttread()  interruptible.
 *
 * 85/03/01	Allan Cornish
 * made ttclose() interruptible.
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
#include <sys/uproc.h>
#include <errno.h>

/*
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */

/* NEAR_OR_FAR_CALL is for invoking t_param and t_start */
#define	 NEAR_OR_FAR_CALL(tp_fn)  {\
	if (tp->t_cs_sel) \
		ld_call(tp->t_cs_sel, tp->tp_fn, tp); \
	else \
		(*tp->tp_fn)(tp); }

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
void ttioctl();
void ttopen();
int  ttout();
int  ttpoll();
void ttread();
void ttsetgrp();
void ttsignal();
void ttstart();
void ttstash();
void ttwrite();

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
void ttopen(tp)
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
	if (tp->t_param != NULL) {
		NEAR_OR_FAR_CALL(t_param)
	}
}

/*
 * ttsetgrp()
 *
 *	Set process group when process does not have one.
 *	Also set up process's controlling terminal.
 */
void ttsetgrp(tp, ctdev)
register TTY *tp;
dev_t ctdev;
{
	register PROC *pp;

	pp = SELF;
	if (pp->p_group == 0) {
		if (tp->t_group == 0)
			tp->t_group = pp->p_pid;
		pp->p_group = tp->t_group;
	}
	if (pp->p_ttdev == NODEV)
		pp->p_ttdev = ctdev;
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

	while (tp->t_oq.cq_cc != 0) {
		s = sphi();
		if (tp->t_oq.cq_cc != 0) {
			tp->t_flags |= T_DRAIN;
			sleep((char *)&tp->t_oq, CVTTOUT, IVTTOUT, SVTTOUT);
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
	register c;
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
			if ( iop->io_flag & IONDLY ) {
				u.u_error = EAGAIN;
				spl(o);
				return;
			}

			tp->t_flags |= T_INPUT;  /* wait for more data */
			sleep((char *)&tp->t_iq, CVTTIN, IVTTIN, SVTTIN);

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
			if ((tp->t_flags&T_ISTOP) != 0)
				tp->t_flags &= ~T_ISTOP;
			if ((tp->t_flags&T_TSTOP) != 0) {
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
			return;
		if (ioputc(c, iop) < 0)
			return;
		if (!ISBBYB && (c=='\n' || ISBRK))
			return;
	}
}

/*
 * ttwrite()
 *
 *	Write routine.
 *	Transfer stuff to the character list.
 */
void ttwrite(tp, iop)
register TTY *tp;
register IO *iop;
{
	register c;
	int o;

	/*
	 * Non-blocking writes which can fit.
	 * NOTE: exhaustion of clists can still cause blocking writes.
	 */
	if ( (iop->io_flag & IONDLY) && (OHILIM >= iop->io_ioc) ) {

		/*
		 * No room.
		 */
		if ( tp->t_oq.cq_cc >= OHILIM-iop->io_ioc ) {
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
			sleep((char *)&tp->t_oq, CVTTOUT, IVTTOUT, SVTTOUT);
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
register struct sgttyb *vec;
{
	register int	flush = 0;
	register int	drain = 0;
	register char	*p1, *p2;
		 int    rload = 0;
		 int	was_bbyb = 0;

	switch (com) {
	case TIOCQUERY:
		kucopy(&tp->t_iq.cq_cc, vec, sizeof(int));
		break;
	case TIOCGETP:
		kucopy(&tp->t_sgttyb, vec, sizeof (struct sgttyb));
		break;
	case TIOCSETP:
	        ++flush;          /* flush input */
		++drain;	  /* delay for output */
		++rload;
		ukcopy(vec, &tp->t_sgttyb, sizeof (struct sgttyb));
		break;
	case TIOCSETN:
		was_bbyb = ISBBYB;	/* previous mode */
		++rload;
		ukcopy(vec, &tp->t_sgttyb, sizeof (struct sgttyb));
		if (!was_bbyb && ISBBYB && tp->t_ibx != 0) {
			p1 = &tp->t_ib[0];
			p2 = &tp->t_ib[tp->t_ibx];
			while (p1 < p2)
#if NOT_8_BIT
				putq(&tp->t_iq, (*p1++) & 0177);
#else
				putq(&tp->t_iq, (*p1++));
#endif
			tp->t_ibx = 0;
		}
		break;
	case TIOCGETC:
		kucopy(&tp->t_tchars, vec, sizeof (struct tchars));
		break;
	case TIOCSETC:
		++rload;
		++drain;
		ukcopy(vec, &tp->t_tchars, sizeof (struct tchars));
		break;
	case TIOCEXCL:
		tp->t_flags |= T_EXCL;
		break;
	case TIOCNXCL:
		tp->t_flags &= ~T_EXCL;
		break;
	case TIOCHPCL:		/* set hangup on last close */
		tp->t_flags |= T_HPCL;
		break;
	case TIOCCHPCL:		/* don't hangup on last close */
		if (!super())   /* only superuser may do this */
		   u.u_error = EPERM;        /* not su */
		else
 	   	   tp->t_flags &= ~T_HPCL;   /* turn off hangup bit */
		break;
	case TIOCGETTF:		/* get tty flag word */
		kucopy(&tp->t_flags, (unsigned *) vec, sizeof(unsigned));
		break;
	case TIOCFLUSH:
		++flush;        /* flush both input and output */
		++drain;
		break;
	case TIOCBREAD:		/* blocking read for CBREAK/RAW mode */
		tp->t_flags |= T_BRD;
		break;
	case TIOCCBREAD:	/* turn off CBREAK/RAW blocking read mode */
		tp->t_flags &= ~T_BRD;
		break;
	default:
		u.u_error = EINVAL;
	}

	/*
	 * Ensure output is enabled BEFORE waiting for output to drain.
	 */
	if ( (ISRIN) && (tp->t_flags & T_STOP) ) {
		tp->t_flags &= ~T_STOP;
		ttstart( tp );
	}

	/*
	 * Wait for output to drain, or signal to arrive.
	 */
	if (drain != 0) {
		while (tp->t_oq.cq_cc != 0) {
			tp->t_flags |= T_DRAIN;
			sleep((char *)&tp->t_oq, CVTTOUT, IVTTOUT, SVTTOUT);
			if (SELF->p_ssig && nondsig())
				break;
		}
	}

	/*
	 * Flush input.
	 */
	if (flush != 0)
		ttflush(tp);

	/*
	 * Re-initialize hardware.
	 */
	if ( (rload != 0) && (tp->t_param != NULL) )
		NEAR_OR_FAR_CALL(t_param)
}

/*
 * ttpoll()
 *
 *	Polling routine.
 *	[System V.3 Compatible]
 */
int ttpoll( tp, ev, msec )
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
	if ( (ev & POLLIN) && (tp->t_iq.cq_cc == 0) ) {

		/*
		 * Blocking input poll.
		 */
		if ( msec != 0 )
			pollopen( &tp->t_ipolls );

		/*
		 * Second look to avoid interrupt race.
		 */
		if ( tp->t_iq.cq_cc == 0 )
			ev &= ~POLLIN;
	}

	/*
	 * Output poll with no space.
	 */
	if ( (ev & POLLOUT) && (tp->t_oq.cq_cc >= OLOLIM) ) {

		/*
		 * Blocking output poll.
		 */
		if ( msec != 0 )
			pollopen( &tp->t_opolls );

		/*
		 * Second look to avoid interrupt race.
		 */
		if ( tp->t_oq.cq_cc >= OLOLIM )
			ev &= ~POLLIN;
	}

	if ( ((ev & POLLIN) == 0) && ((tp->t_flags & T_CARR) == 0) )
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
	register c;

	if (tp->t_nfill) {
		--tp->t_nfill;
		c = tp->t_fillb;
	} else if ((tp->t_flags&T_INL) != 0) {
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
 *	Called at high priority from  the driver's interrupt processor.
 */
void ttin(tp, c)
register TTY *tp;
register c;
{
	int dc, i, n;

	if (!ISRIN) {
#if NOT_8_BIT
		c &= 0177;
#endif
		if (ISINTR) {
			ttsignal(tp, SIGINT);
			return;
		}
		if (ISQUIT) {
			ttsignal(tp, SIGQUIT);
			return;
		}

		/*
		 * Only do flow control if TANDEM is set.
		 */
		if (ISTAND) {
			if (ISSTOP) {
				if ((tp->t_flags&T_STOP) == 0)
					tp->t_flags |= T_STOP;
				return;
			}
			if (ISSTART) {
				tp->t_flags &= ~T_STOP;
				ttstart(tp);
				return;
			}
		}
	}
	if ((tp->t_flags&T_ISTOP) != 0)
		return;
	if (!ISRIN) {
		if (c=='\r' && ISCRMOD)
			c = '\n';
		if (tp->t_escape != 0) {
			if (c == ESC)
				++tp->t_escape;
			else {
				if (ISERASE || ISKILL) {
					c |= 0200;
					--tp->t_escape;
				}
				while (tp->t_escape!=0 && tp->t_ibx<NCIB-1) {
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
			return;
		}
		if (ISERASE && !ISCBRK) {
			while (tp->t_escape!=0 && tp->t_ibx<NCIB-1) {
				tp->t_ib[tp->t_ibx++] = ESC;
				--tp->t_escape;
			}
			if (tp->t_ibx == 0)
				return;
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
				        return;
				else if (c != '\b' && c != '\t') {
					putq(&tp->t_oq, '\b');
					putq(&tp->t_oq,  ' ');
					putq(&tp->t_oq, '\b');
				} else if (c == '\t') {
					n = tp->t_opos + tp->t_escape;
					for (i=0; i<tp->t_ibx; ++i) {
						c = tp->t_ib[i];
#if NOT_8_BIT
						if ((c&0200) != 0) {
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
				if ((dc&0200) != 0) {
					if ((dc&0177) != '\b')
						putq(&tp->t_oq, '\b');
					putq(&tp->t_oq,  ' ');
					putq(&tp->t_oq, '\b');
				}
#endif
				ttstart(tp);
			}
			return;
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
			return;
		}
	}
	if (ISBBYB) {
		putq(&tp->t_iq, c);
		if ((tp->t_flags&T_INPUT) != 0) {
			tp->t_flags &= ~T_INPUT;
			defer( wakeup, (char *) &tp->t_iq );
		}
		if ( tp->t_ipolls.e_procp ) {
			tp->t_ipolls.e_procp = 0;
			defer( pollwake, (char *) &tp->t_ipolls );
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
	if ((n=tp->t_iq.cq_cc)>=IHILIM)
		tp->t_flags |= T_ISTOP;
	else if (ISTAND && (tp->t_flags&T_TSTOP)==0 && n>=ITSLIM) {
		tp->t_flags |= T_TSTOP;
		putq(&tp->t_oq, stopc);
		ttstart(tp);
	}
}

/*
 * ttstash()
 *
 *	Cooked mode.
 *	Put character in the buffer and check for end of line.
 *	Only a legal end of line can take the last character position.
 */
void ttstash(tp, c)
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

		if ( tp->t_flags & T_INPUT ) {
			tp->t_flags &= ~T_INPUT;
			defer( wakeup, (char *) &tp->t_iq );
		}

		if ( tp->t_ipolls.e_procp ) {
			tp->t_ipolls.e_procp = 0;
			defer( pollwake, (char *) &tp->t_ipolls );
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

	n = tp->t_flags;
	if ( n & T_STOP )
		return;

	if ((n&T_DRAIN)!=0 && tp->t_oq.cq_cc==0
	   && (n&T_INL)==0 && tp->t_nfill==0)
	{	tp->t_flags &= ~T_DRAIN;
		defer( wakeup, (char *) &tp->t_oq );
		return;
	}

	NEAR_OR_FAR_CALL(t_start)

	if ( tp->t_oq.cq_cc > OLOLIM )
		return;

	if ( n & T_HILIM ) {
	   	tp->t_flags &= ~T_HILIM;
		defer( wakeup, (char *) &tp->t_oq );
	}

	if ( tp->t_opolls.e_procp ) {
		tp->t_opolls.e_procp = 0;
		defer( pollwake, (char *) &tp->t_opolls );
	}
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
	clrq(&tp->t_iq);
	clrq(&tp->t_oq);

	if ( tp->t_flags & T_INPUT )
		defer( wakeup, (char *) &tp->t_iq );

	if ( tp->t_flags & (T_DRAIN|T_HILIM) )
		defer( wakeup, (char *) &tp->t_oq );

	if ( tp->t_ipolls.e_procp != 0 ) {
		tp->t_ipolls.e_procp = 0;
		defer( pollwake, (char *) &tp->t_ipolls );
	}

	if ( tp->t_opolls.e_procp != 0 ) {
		tp->t_opolls.e_procp = 0;
		defer( pollwake, (char *) &tp->t_opolls );
	}

	tp->t_ibx = 0;
	tp->t_escape = 0;
	tp->t_flags &= T_SAVE;  /* reset most flag bits */
}

/*
 * ttsignal()
 *
 *	Send a signal to every process in the given process group.
 */
void ttsignal(tp, sig)
TTY *tp;
int sig;
{
	register int g;
	register PROC *pp;

	g = tp->t_group;
	if (g == 0)
		return;
	ttflush(tp);
	pp = &procq;
	while ((pp=pp->p_nforw) != &procq)
		if (pp->p_group == g)
			sendsig(sig, pp);
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
	tp->t_flags &= ~T_CARR;  /* indicate no carrier */
	ttflush(tp);
	ttsignal(tp, SIGHUP);
}
