/*
 * mm.c
 *
 * Memory Mapped Video
 * High level output routines.
 *
 * $Log:	mm.c,v $
 * Revision 1.2  92/08/04  12:53:38  bin
 * upd for ker59
 * 
 * Revision 1.4  92/04/09  10:25:38  hal
 * Call mmgo() from mmstart() at low priority.
 * 
 */
#include <sys/coherent.h>
#include <sys/sched.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/io.h>
#include <sys/tty.h>
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
	int c, s;
	IO iob;
	static int mmbegun;

	if (mmbegun == 0) {
		++mmbegun;
		timeout(&mmtim, HZ/10, mmtime, (char *)tp);
	}

	while ((tp->t_flags&T_STOP) == 0) {
		if ((c = ttout(tp)) < 0)
			break;
		iob.io_seg  = IOSYS;
		iob.io_ioc  = 1;
		iob.io.vbase = &c;
		iob.io_flag = 0;
#if 0
		mmwrite( makedev(2,0), &iob );
#else
		s = splo();
		mmgo(&iob);
		spl(s);
#endif
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

	/*
	 * Kernel writes.
	 */
	if (iop->io_seg == IOSYS) {
		while (mmgo(iop))
			;
		goto mmwdone;
	}

#if 0
	ioc = iop->io_ioc;
	/*
	 * Blocking user writes.
	 */
	if ( (iop->io_flag & IONDLY) == 0 ) {
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
				goto mmwdone;
			}
			mmgo(iop);
		} while ( iop->io_ioc );
		goto mmwdone;
	}

	/*
	 * Non-blocking user writes with output stopped.
	 */
	if ( istty.t_flags & T_STOP ) {
		u.u_error = EAGAIN;
		goto mmwdone;
	}

	/*
	 * Non-blocking user writes do not pause after scrolling.
	 */
	{
		while ( mmgo(iop) )
			;
	}
#else
	ttwrite(&istty, iop);
#endif
mmwdone:
	return;
}
