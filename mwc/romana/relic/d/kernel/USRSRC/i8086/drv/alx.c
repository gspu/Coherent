/* (-lgl
 * 	COHERENT Driver Kit Version 1.1.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 *
 * $Log:	alx.c,v $
 * Revision 1.12  91/08/06  10:32:23  bin
 * 2nd rts/cts version form hal
 * 
 * Revision 1.7  91/08/02  07:59:27  hal
 * Raw input silo uses last slot for byte count.
 * Modem control now gives RTS/CTS flow control:
 * 	CTS is checked 10x/sec (probably not often enough).
 * 	On Rx_INT, check T_ISTOP and rawin silo levels; drop RTS if need to.
 * 
 * Revision 1.6	91/04/03  18:55:07	root
 * alxclose():  do closing state machine BEFORE dropping control lines.
 * alxioctl():  save and restore interrupt enable register.
 * alxopen():   wait for pending last close (fixes SLOW port bug).
 * This version needs al.h 1.3 or later.
 *
 * Revision 1.5	91/04/02  17:53:37	root
 * save MSR delta status; add MS_INTR handling; use al.h 1.2
 *
 * Revision 1.4	91/02/22  09:21:17	root
 * alxintr():  replace repeated use of ALPORT macro by local variable
 *
 * Revision 1.3	91/02/21  14:58:16	root
 * Fix unconditional "hupcl" bug in 3.1.0 version.
 *
 * Revision 1.2	91/02/21  11:21:40	hal
 * Used in COH Release 3.1.0 - add COM3/COM4 and polling
 *
 * Revision 1.1	91/02/21  11:08:31	hal
 * Used in COH Release 3.0.0 - no COM3/COM4
 *
 -lgl) */
/*
 * Shared parts of IBM async port drivers.
 */
#include <sys/coherent.h>
#include <sys/i8086.h>
#include <sys/al.h>
#include <sys/con.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/tty.h>
#include <sys/uproc.h>
#include <sys/timeout.h>
#include <sys/clist.h>
#include <sys/ins8250.h>
#include <sys/sched.h>

#define ALPORT	(((COM_DDP *)(tp->t_ddp))->port)
#define AL_TIM	(((COM_DDP *)(tp->t_ddp))->tim)
#define AL_NUM	(((COM_DDP *)(tp->t_ddp))->com_num)
#define AL_MSR_DELTAS	(((COM_DDP *)(tp->t_ddp))->msr_deltas)
#define AL_H_CLOSE	(((COM_DDP *)(tp->t_ddp))->h_close)

#define DTRTMOUT  3	/* DTR timeout interval in seconds for close */
#define	IENABLE	(IE_RxI+IE_TxI+IE_LSI+IE_MSI)

/*
 * For rawin silo (see ktty.h), use last element of si_buf to count the number
 * of characters in the silo.
 */
#define SILO_CHAR_COUNT	si_buf[SI_BUFSIZ-1]
#define SILO_HIGH_MARK	(SI_BUFSIZ-SI_BUFSIZ/4)
#define MAX_SILO_INDEX	(SI_BUFSIZ-2)
#define MAX_SILO_CHARS	(SI_BUFSIZ-1)

int	al_sg_set = 0;
int	al_sg_clr = 0;
static int poll_divisor;  /* set by set_poll_rate(), read by alxclk() */

/*
 * functions herein
 */
int	alxopen();
int	alxclose();
int	alxtimer();
int	alxioctl();
int	alxparam();
int	alxcycle();
int	alxstart();
int	alxbreak();
int	alxintr();
static	int alxclk();
static	set_poll_rate();

/*
 * Baud rate table and polling rate table.
 * Indexed by ioctl bit rates.
 */
int albaud[] ={
	0,				/* 0 */
	2304,				/* 50 */
	1536,				/* 75 */
	1047,				/* 110 */
	857,				/* 134.5 */
	768,				/* 150 */
	576,				/* 200 */
	384,				/* 300 */
	192,				/* 600 */
	96,				/* 1200 */
	64,				/* 1800 */
	58,				/* 2000 */
	48,				/* 2400 */
	32,				/* 3600 */
	24,				/* 4800 */
	16,				/* 7200 */
	12,				/* 9600 */
	6,				/* 19200 */
	0,				/* EXTA */
	0				/* EXTB */
};

/*
 *	alp_rate[] is tied to albaud[] - it gives the minimum polling
 *	rate for the corresponding port speed; it must be a multiple
 *	of 100 (system clock Hz) and >= baud/6
 */
int alp_rate[] ={			/* baud/6 or zero */
	0,				/* 0 */
	1*HZ,				/* 50 */
	1*HZ,				/* 75 */
	1*HZ,				/* 110 */
	1*HZ,				/* 134.5 */
	1*HZ,				/* 150 */
	1*HZ,				/* 200 */
	1*HZ,				/* 300 */
	1*HZ,				/* 600 */
	2*HZ,				/* 1200 */
	3*HZ,				/* 1800 */
	4*HZ,				/* 2000 */
	4*HZ,				/* 2400 */
	6*HZ,				/* 3600 */
	8*HZ,				/* 4800 */
	12*HZ,				/* 7200 */
	16*HZ,				/* 9600 */
	0,				/* 19200 */
	0,				/* EXTA */
	0				/* EXTB */
};

/*
 *	the following is for debug only
 */
#if 0
#define CDUMP(text)	cdump(text);

cdump(message)
char *message;
{
	int i, b;

	for (i = 0; i < NUM_AL_PORTS; i++) {
		b = ((COM_DDP *)(tp_table[i]->t_ddp))->port;
		printf("%x:%x:%x:%x ", i+1, b, inb(b+MCR), inb(b+IER));
	}
	printf("poll=%d ", poll_rate);
	printf("%s\n", message);
}
#else
#define CDUMP(text)
#endif

/*
 * alxopen()
 */
alxopen(dev, mode, tp, irqtty)
dev_t	dev;
int	mode;
register TTY	*tp, **irqtty;
{
	register int	s;
	register int	b;
	register int	minor_h;  /* minor device number including high bit */
	unsigned char	msr;

	minor_h = minor(dev);     /* complete minor number */

	b = ALPORT;

	if (inb(b+IER) & ~IENABLE) { /* chip not found */
		u.u_error = ENXIO;
		return;
	}

	if ((tp->t_flags & T_EXCL) && !super()) {
		u.u_error = ENODEV;
		return;
	}

	if (drvl[major(dev)].d_time != 0) {	/* Modem settling */
		u.u_error = EDBUSY;
		return;
	}

	/*
	 * Can't open a polled port if another driver is using polling.
	 */
	if (dev & CPOLL && poll_owner & ~ POLL_AL) {
		u.u_error = EDBUSY;
		return;
	}

	/*
	 * exclusion conditions:
	 *	can't have same port polled and IRQ at once
	 *	can't have both com[13] or both com[24] IRQ at once
	 */
	if (dev & CPOLL) {
		if (com_usage[AL_NUM] == COM_IRQ) {
			u.u_error = EDBUSY;
			return;
		}
	} else {
		if (com_usage[AL_NUM] == COM_POLLED
		   || com_usage[AL_NUM ^ 2] == COM_IRQ) {
			u.u_error = EDBUSY;
			return;
		}
	}

	if (tp->t_open == 0) {        /* not already open */
		/*
		 * Wait for pending last close (if any) to finish.
		 */
		while (AL_H_CLOSE) {
   	  		sleep((char *)(&AL_H_CLOSE), CVTTOUT, IVTTOUT,
				SVTTOUT);
		}
		s = sphi();
		/*
		 * Raise basic modem control lines even if modem
		 * control hasn't been specified.
		 * MC_OUT2 turns on NON-open-collector IRQ line from the UART.
		 * since we can't have two UART's on same IRQ with MC_OUT2 on
		 */
		if (dev & CPOLL) {
			outb(b+MCR, MC_RTS|MC_DTR);
		} else {
			*irqtty = tp_table[AL_NUM];
			outb(b+MCR, MC_RTS|MC_DTR|MC_OUT2);
		}

		outb(b+IER, IENABLE);        /* enable interrupts */

		if ((minor_h & NMODC) == 0) {	/* want modem control? */
			tp->t_flags |= T_MODC | T_HOPEN | T_STOP;
			while (1) {	/* wait for carrier */
				msr = inb(b+MSR);
				AL_MSR_DELTAS |= msr;
				if (msr & MS_RLSD)
					break;
	   	  		sleep((char *)(&tp->t_open), CVTTOUT, IVTTOUT,
					SVTTOUT);	/* wait for carrier */
		 		if (SELF->p_ssig && nondsig()) {  /* signal? */
					outb(b+MCR, inb(b+MCR)&MC_OUT2);
	                    		/*
					 * make sure port is hungup
					 * disable all ints except for TxI
					 */
			    		outb(b+IER, IE_TxI);
					u.u_error = EINTR;
					spl(s);
					return;
				}
			}
			tp->t_flags &= ~T_HOPEN; /* no longer hanging in open */
			if (msr & MS_CTS)
				tp->t_flags &= ~T_STOP;
		} else {
			tp->t_flags &= ~T_MODC;		/* no modem control */
		}
	        tp->t_flags |= T_CARR;			/* carrier on */
		ttopen(tp);				/* stty inits */

		/*
		 * Allow custom modification of defaults.
		 */
		tp->t_sgttyb.sg_flags |=  al_sg_set;
		tp->t_sgttyb.sg_flags &= ~al_sg_clr;
		alxparam(tp);
		spl(s);
	} else {			        /* already open */
		if ((minor_h & NMODC) == 0) {	/* want modem control? */
	   	    if ((tp->t_flags & T_MODC)==0) { /* already not modem control? */
			u.u_error = ENODEV;     /* yes, don't allow open */
			return;
		    }
		} else {		         /* don't want modem control */
			if (tp->t_flags & T_MODC) { /* already modem control? */
				u.u_error = ENODEV; /* yes, don't allow open */
				return;
			}
		}
	}
	tp->t_open++;
	ttsetgrp(tp, dev);

	/*
	 * now that we've successfully opened, designate port as
	 * polled or interrupt driven to avoid future conflicts
	 */
	if (dev & CPOLL) {
		com_usage[AL_NUM] = COM_POLLED;
		set_poll_rate();
	} else {				/* irq-driven port */
		com_usage[AL_NUM] = COM_IRQ;
	}

	CDUMP((dev&CPOLL)?"open polled":"open irq")
}

/*
 * alxclose()
 *
 *	Called at high priority on last close for the device.
 */
alxclose(dev, mode, tp)
dev_t	dev;
int	mode;
TTY	*tp;
{
	register unsigned holdflags;
	register int b;
	int state, maj;
	int s;

	/*
	 * Called at high priority by alclose after al_buff is drained
	 */
	holdflags = tp->t_flags;	/* save flags */
	AL_H_CLOSE = 1;			/* disallow reopen til done closing */
	ttclose(tp);			/* clear flags */
	b = ALPORT;
	/*
	 * ttclose() only emptied the output queue tp->t_oq;
	 * now wait for the silo tp->rawout to empty
	 * and allow a delay for the UART on-chip xmit buffer to empty
	 * state 2: waiting for silo to empty
	 * state 1: stalling so UART can empty xmit buffer
	 * state 0: done!  ok to shut off IRQ for this chip by clearing MC_OUT2
	 */
	state = 2;
	while (state) {
		timeout(&AL_TIM, 10, wakeup, (int)&AL_TIM);
		sleep((char *)&AL_TIM, CVTTOUT, IVTTOUT, SVTTOUT);
		if (tp->t_rawout.si_ix == tp->t_rawout.si_ox  && state)
			state--;
	}

	/*
	 * If not hanging in open
	 */
	if ((holdflags & T_HOPEN) == 0) {
		/*
		 * Disable all ints except TxI
		 */
		outb(b+IER, IE_TxI);
	} else {
		/*
		 * Flags for first open
		 */
		tp->t_flags = T_MODC | T_HOPEN;
	}

	/*
	 * If hupcls
	 */
	if (holdflags & T_HPCL) {
		/*
		 * Hangup port
		 */
		s = sphi();
		outb(b+MCR, inb(b+MCR)&MC_OUT2);
		spl(s);

		/*
		 * Hold dtr low for timeout
		 */
		maj = major(dev);
		drvl[maj].d_time = 1;
		sleep((char *)&drvl[maj].d_time, CVTTOUT, IVTTOUT, SVTTOUT);
		drvl[maj].d_time = 0;
	}
	s = sphi();
	outb(b+MCR, inb(b+MCR)&(~MC_OUT2));
	spl(s);
	com_usage[AL_NUM] = COM_UNUSED;
	set_poll_rate();
	AL_H_CLOSE = 0;		/* allow reopen - done closing */
	wakeup((char *)&AL_H_CLOSE);
	CDUMP("closed")
}

/*
 * Common c_timer routine for async ports.
 */
alxtimer(dev)
dev_t dev;
{
	if (++drvl[major(dev)].d_time > DTRTMOUT)
		wakeup((char *)&drvl[major(dev)].d_time);
}


/*
 * Common c_ioctl routine for async ports.
 */
alxioctl(dev, com, vec, tp)
dev_t	dev;
struct sgttyb *vec;
register TTY 	*tp;
{
	register int	s, b;
	int stat1, stat2;
	unsigned char	msr;
	unsigned char ier_save;

	s = sphi();
	b = ALPORT;
	ier_save=inb(b+IER);
	stat1 = inb(b+MCR);		/* get current MCR register status */
	stat2 = inb(b+LCR);		/* get current LCR register status */

	switch(com) {
	case TIOCSBRK:			/* set BREAK */
		outb(b+LCR, stat2|LC_SBRK);
		break;
	case TIOCCBRK:			/* clear BREAK */
		outb(b+LCR, stat2 & ~LC_SBRK);
		break;
	case TIOCSDTR:			/* set DTR */
		outb(b+MCR, stat1|MC_DTR);
		break;
	case TIOCCDTR:			/* clear DTR */
		outb(b+MCR, stat1 & ~MC_DTR);
		break;
	case TIOCSRTS:			/* set RTS */
		outb(b+MCR, stat1|MC_RTS);
		break;
	case TIOCCRTS:			/* clear RTS */
		outb(b+MCR, stat1 & ~MC_RTS);
		break;
	case TIOCRSPEED:		/* set "raw" I/O speed divisor */
		outb(b+LCR, stat2|LC_DLAB);  /* set speed latch bit */
		outb(b+DLL, (unsigned) vec);
		outb(b+DLH, (unsigned) vec >> 8);
		outb(b+LCR, stat2);       /* reset latch bit */
		break;
	case TIOCWORDL:		/* set word length and stop bits */
		outb(b+LCR, ((stat2&~0x7) | ((unsigned) vec & 0x7)));
		break;
	case TIOCRMSR:		/* get CTS/DSR/RI/RLSD (MSR) */
		msr = inb(b+MSR);
		AL_MSR_DELTAS |= msr;
		stat1 = msr >> 4;
		kucopy(&stat1, (unsigned *) vec, sizeof(unsigned));
		break;
	default:
		ttioctl(tp, com, vec);
	}
	outb(b+IER, ier_save);
	spl(s);
}

alxparam(tp)
TTY	*tp;
{
	register int	b;
	register int	baud;
	int s;

	b = ALPORT;

	/*
	 * error if input speed not the same as output speed
	 */
	if (tp->t_sgttyb.sg_ispeed!=tp->t_sgttyb.sg_ospeed) {
		u.u_error = ENODEV;
		return;
 	}

	if ((baud = albaud[tp->t_sgttyb.sg_ispeed]) == 0) {
		if (tp->t_flags & T_MODC) {  /* modem control? */
			tp->t_flags &= ~T_CARR;  /* indicate no carrier */
			s = sphi();
			outb(b+MCR, inb(b+MCR) & MC_OUT2); /* hangup */
			spl(s);
		}
	}

	if (baud) {
		unsigned char ier_save;

		s=sphi();
		ier_save=inb(b+IER);	/* some chips need this */
		outb(b+LCR, LC_DLAB);
		outb(b+DLL, baud);
		outb(b+DLH, baud >> 8);
		switch (tp->t_sgttyb.sg_flags & (EVENP|ODDP|RAW)) {
		case EVENP:
			outb(b+LCR, LC_CS7 + LC_PARENB + LC_PAREVEN);
			break;

		case ODDP:
			outb(b+LCR, LC_CS7 + LC_PARENB);
			break;

		default:
			outb(b+LCR, LC_CS8);
			break;
		}
		outb(b+IER, ier_save);
		spl(s);
	}
	set_poll_rate();
}

/*
 * Middle level processor.
 *
 *	Invoked 10 times per second.
 *	Checks modem status for loss of carrier.
 *	Tranfers rawin buffer [from intr level] to canonical input queue.
 *	Transfers output queue to rawout buffer [for intr level].
 */
alxcycle(tp)
register TTY * tp;
{
	register int b;
	register int n;
	unsigned char msr, mcr;
	int s;

	/*
	 * Check modem status every ten clock ticks.
	 *
	 * Modem status interrupts were not enabled due to 8250 hardware bug.
	 * Enabling modem status and receive interrupts may cause lockup
	 * on older parts.
	 *
	 * Also check if input silo is nearly full in case we need RTS
	 * flow control.
	 */
	if (tp->t_flags & T_MODC) {

		/*
		 * Get status
		 */
		msr = inb(ALPORT+MSR);
		AL_MSR_DELTAS |= msr;

		/*
		 * Carrier changed.
		 */
		if (AL_MSR_DELTAS & MS_DRLSD) {
			AL_MSR_DELTAS &= ~MS_DRLSD;
			/*
			 * wakeup open
			 */
			if (tp->t_open == 0) {
				wakeup((char *)(&tp->t_open));
			}

			/*
			 * carrier off?
			 */
			else if ((msr & MS_RLSD) == 0) {
				/*
				 * clear carrier flag; send hangup signal
				 */
				s = sphi();
				tp->t_rawin.si_ox = tp->t_rawin.si_ix;
				tp->t_rawin.SILO_CHAR_COUNT = 0;
				spl(s);
				tthup(tp);
			}
		}

		/*
		 * CTS changed.
		 */
		if (AL_MSR_DELTAS & MS_DCTS) {
			AL_MSR_DELTAS &= ~MS_DCTS;
			if (msr & MS_CTS)
				tp->t_flags &= ~T_STOP;
			else
				tp->t_flags |= T_STOP;
		}

		/*
		 * If input silo not nearly full, assert RTS.
		 */
		if (tp->t_rawin.SILO_CHAR_COUNT <= SILO_HIGH_MARK) {
			mcr = inb(ALPORT+MCR);
			if ((mcr & MC_RTS) == 0) {
printf("%x RTS ON\n", ALPORT);
				outb(ALPORT+MCR, mcr | MC_RTS);
			}
		}
	}

	/*
	 * Empty raw input buffer.
	 * If modem control enabled and tt input queue is at high limit
	 * (e.g., tp->t_iq.cq_cc >= IHILIM), don't copy from rawin silo
	 * to tt input queue.
	 */
	if (!(tp->t_flags & T_MODC) || !(tp->t_flags & T_ISTOP))
		while (tp->t_rawin.SILO_CHAR_COUNT > 0) {
			ttin(tp, tp->t_rawin.si_buf[tp->t_rawin.si_ox]);
			if (tp->t_rawin.si_ox < MAX_SILO_INDEX)
				tp->t_rawin.si_ox++;
			else
				tp->t_rawin.si_ox = 0;
			tp->t_rawin.SILO_CHAR_COUNT--;
		}

	/*
	 * Calculate free output slot count.
	 */
	n  = sizeof(tp->t_rawout.si_buf) - 1;
	n += tp->t_rawout.si_ox - tp->t_rawout.si_ix;
	n %= sizeof(tp->t_rawout.si_buf);

	/*
	 * Fill raw output buffer.
	 */
	while ((--n >= 0) && ((b = ttout(tp)) >= 0)) {
		tp->t_rawout.si_buf[ tp->t_rawout.si_ix ] = b;
		if (tp->t_rawout.si_ix >= sizeof(tp->t_rawout.si_buf) - 1)
			tp->t_rawout.si_ix = 0;
		else
			tp->t_rawout.si_ix++;
	}

	/*
	 * (Re)start output, wake sleeping processes, etc.
	 */
	ttstart(tp);

	/*
	 * Schedule next cycle.
	 */
	timeout(&tp->t_rawtim, HZ/10, alxcycle, tp);
}

/*
 * Serial Transmit Start Routine.
 */
alxstart(tp)
register TTY * tp;
{
	register int b;
	register int s;
	extern alxbreak();

	/*
	 * Read line status register AFTER disabling interrupts.
	 */
	s = sphi();
	b = inb(ALPORT+LSR);

	/*
	 * Process break indication.
	 * NOTE: Break indication cleared when line status register was read.
	 */
	if (b & LS_BREAK)
		defer(alxbreak, tp);

	/*
	 * Transmitter is empty, output data is pending.
	 */
	if ((b & LS_TxRDY) && (tp->t_rawout.si_ix != tp->t_rawout.si_ox)) {
		outb(	ALPORT+DREG,
			tp->t_rawout.si_buf[ tp->t_rawout.si_ox ]);
		if (++tp->t_rawout.si_ox >= sizeof(tp->t_rawout.si_buf))
			tp->t_rawout.si_ox = 0;
	}
	spl(s);
}

/*
 * Serial Received Break Handler.
 */
alxbreak(tp)
TTY * tp;
{
	ttsignal(tp, SIGINT);
}

/*
 * Serial Interrupt Handler.
 */
alxintr(tp)
register TTY * tp;
{
	register int	b;
	int port = ALPORT;
	unsigned char mcr;

rescan:
	switch (inb(port+IIR)) {

	case LS_INTR:
		if (inb(port+LSR) & LS_BREAK)
			defer(alxbreak, tp);
		goto rescan;

	case Rx_INTR:
		b = inb(port+DREG);
		if (tp->t_open == 0)
			goto rescan;
		/*
		 * Must recognize XOFF quickly to avoid transmit overrun.
		 * Recognize XON here as well to avoid race conditions.
		 */
		if ((tp->t_sgttyb.sg_flags & RAWIN) == 0) {
			/*
			 * XOFF.
			 */
			if (tp->t_tchars.t_stopc == (b & 0177)) {
				tp->t_flags |= T_STOP;
				goto rescan;
			}

			/*
			 * XON.
			 */
			if (tp->t_tchars.t_startc == (b & 0177)) {
				tp->t_flags &= ~T_STOP;
				goto rescan;
			}
		}

		/*
		 * Save char in raw input buffer.
		 */
		if (tp->t_rawin.SILO_CHAR_COUNT < MAX_SILO_CHARS) {
			tp->t_rawin.si_buf[tp->t_rawin.si_ix] = b;
			if (tp->t_rawin.si_ix < MAX_SILO_INDEX)
				tp->t_rawin.si_ix++;
			else
				tp->t_rawin.si_ix = 0;
			tp->t_rawin.SILO_CHAR_COUNT++;
		}
		if (tp->t_flags & T_MODC)
			if ((tp->t_flags & T_ISTOP)
			|| (tp->t_rawin.SILO_CHAR_COUNT > SILO_HIGH_MARK)) {
				mcr = inb(port+MCR);
				if (mcr & MC_RTS) {
					outb(port+MCR, mcr & ~MC_RTS);
printf("%x RTS OFF\n", port);
				}
			}
		goto rescan;

	case Tx_INTR:
		/*
		 * Do nothing if no raw output data or output is stopped.
		 */
		if (tp->t_rawout.si_ix == tp->t_rawout.si_ox) {
			goto rescan;
		}
		if (tp->t_flags & T_STOP)
			goto rescan;

		/*
		 * Transmit next char in raw output buffer.
		 */
		outb(port+DREG,
			tp->t_rawout.si_buf[ tp->t_rawout.si_ox ]);

		/*
		 * Adjust raw output buffer output index.
		 */
		if (++tp->t_rawout.si_ox >= sizeof(tp->t_rawout.si_buf))
			tp->t_rawout.si_ox = 0;

		/*
		 * Try to fill buffer if now empty.
		 */
		if (tp->t_rawout.si_ox == tp->t_rawout.si_ix) {
			defer(alxcycle, tp);
		}
		goto rescan;

	case MS_INTR:
		AL_MSR_DELTAS |= inb(port+MSR);
		defer(alxcycle, tp);
		goto rescan;
	}
}

/*
 * alxclk will be called every time T0 interrupts - if it returns 0,
 * the usual system timer interrupt stuff is done
 */
static int alxclk()
{
	static int count;
	int i;

	for (i = 0; i < NUM_AL_PORTS;  i++)
		if (com_usage[i] == COM_POLLED)
			alxintr(tp_table[i]);
	count++;
	if (count >= poll_divisor)
		count = 0;
	return count;
}

/*
 * set_poll_rate is called when a port is opened or closed or changes speed
 * it sets the polling rate only as fast as needed, and shuts off polling
 * whenever possible
 */
static set_poll_rate()
{
	int port_num, max_rate, port_rate;

	/*
	 * If another driver has the polling clock, do nothing.
	 */
	if (poll_owner & ~ POLL_AL)
		return;

	/*
	 * find highest valid polling rate in units of HZ/10
	 */
	max_rate = 0;
	for (port_num = 0; port_num < NUM_AL_PORTS; port_num++) {
		if (com_usage[port_num] == COM_POLLED) {
			port_rate = alp_rate[(tp_table[port_num])->t_sgttyb.sg_ispeed];
			if (max_rate < port_rate)
				max_rate = port_rate;
		}
	}
	/*
	 * if max_rate is not current rate, adjust the system clock
	 */
	if (max_rate != poll_rate) {
		poll_rate = max_rate;
		poll_divisor = poll_rate/HZ;  /* used in alxclk() */
		altclk_out();		/* stop previous polling */
		poll_owner &= ~ POLL_AL;
		if (max_rate) {	/* resume polling at new rate if needed */
			poll_owner |= POLL_AL;
			altclk_in(poll_rate, alxclk);
		}
		CDUMP("new rate")
	}
}
