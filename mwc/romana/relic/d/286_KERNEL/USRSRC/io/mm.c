/* (-lgl
 * 	COHERENT Driver Kit Version 1.1.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Memory Mapped Video
 * High level output routines.
 *
 * $Log$
 */
#include <sys/coherent.h>
#include <sys/sched.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/io.h>
#include <sys/tty.h>
#include <sys/uproc.h>
#include <sys/timeout.h>

/* For beeper */
#define	TIMCTL	0x43			/* Timer control port */
#define	TIMCNT	0x42			/* Counter timer port */
#define	PORTB	0x61			/* Port containing speaker enable */
#define	FREQ	((int)(1193181L/440))	/* Counter for 440 Hz. tone */

int mmtime();

char mmbeeps;		/* number of ticks remaining on bell */
char mmesc;		/* last unserviced escape character */
int  mmcrtsav = 1;	/* crt saver enabled */
int  mmvcnt   = 900;	/* seconds remaining before crt saver is activated */

extern TTY istty;

/*
 * Start the output stream.
 * Called from `ttwrite' and `isrint' routines.
 */
TIM mmtim;

mmstart(tp)
register TTY *tp;
{
	int c;
	IO iob;
	static int mmbegun;

	while ((tp->t_flags&T_STOP) == 0) {
		if ((c = ttout(tp)) < 0)
			break;
		iob.io_seg  = IOSYS;
		iob.io_ioc  = 1;
		iob.io_base = &c;
		iob.io_flag = 0;
		mmwrite( makedev(2,0), &iob );
	}

	if (mmbegun == 0) {
		++mmbegun;
		timeout(&mmtim, HZ/10, mmtime, (char *)tp);
	}
}

mmtime(xp)
char *xp;
{
	register int s;

	s = sphi();
	if (mmbeeps < 0) {
		mmbeeps = 2;
		outb(TIMCTL, 0xB6);	/* Timer 2, lsb, msb, binary */
		outb(TIMCNT, FREQ&0xFF);
	        outb(TIMCNT, FREQ>>8);
		outb(PORTB, inb(PORTB) | 03);	/* Turn speaker on */
	}
	else if ((mmbeeps > 0) && (--mmbeeps == 0))
		outb( PORTB, inb(PORTB) & ~03 );

	if (mmesc) {
		ismmfunc(mmesc);
		mmesc = 0;
	}
	spl(s);

	ttstart( (TTY *) xp );

	timeout(&mmtim, HZ/10, mmtime, xp);
}

/**
 *
 * void
 * mmwatch()	-- turn video display off after 15 minutes inactivity.
 */
void
mmwatch()
{
	if ( (mmcrtsav > 0) && (mmvcnt > 0) && (--mmvcnt == 0) )
		mm_voff();
}

mmwrite( dev, iop )
dev_t dev;
register IO *iop;
{
	int ioc;
	int s;

	ioc = iop->io_ioc;

	/*
	 * Kernel writes.
	 */
	if (iop->io_seg == IOSYS) {
		while (mmgo(iop))
			;
	}

	/*
	 * Blocking user writes.
	 */
	else if ( (iop->io_flag & IONDLY) == 0 ) {
		do {
			while (istty.t_flags & T_STOP) {
				s = sphi();
				istty.t_flags |= T_HILIM;
				sleep((char*) &istty.t_oq,
					CVTTOUT, IVTTOUT, SVTTOUT);
				spl( s );
			}
			/*
			 * Signal received.
			 */
			if ( SELF->p_ssig && nondsig() ) {
				kbunscroll();	/* update kbd LEDS */
				/*
				 * No data transferred yet.
				 */
				if ( ioc == iop->io_ioc )
					u.u_error = EINTR;
				/*
				 * Transfer remaining data
				 * without pausing after scrolling.
				 */
				else while ( mmgo(iop) )
					;

				return;
			}
			mmgo(iop);
		} while ( iop->io_ioc );
	}

	/*
	 * Non-blocking user writes with output stopped.
	 */
	else if ( istty.t_flags & T_STOP ) {
		u.u_error = EAGAIN;
		return;
	}

	/*
	 * Non-blocking user writes do not pause after scrolling.
	 */
	else {
		while ( mmgo(iop) )
			;
	}
}
