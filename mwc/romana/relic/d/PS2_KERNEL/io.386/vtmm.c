/*
 * mm.c
 *
 * Memory Mapped Video
 * High level output routines.
 *
 * $Log:	vtmm.c,v $
 * Revision 1.2  92/07/16  16:35:31  hal
 * Kernel #58
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

#include <sys/kb.h>
#include <sys/vt.h>

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

extern TTY **vttty;

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
		mmwrite( ((VTDATA *)tp->t_ddp)->vt_dev, &iob );
#else
		s = splo();
		mmgo(&iob, tp->t_ddp, ((VTDATA *)(tp->t_ddp))->vt_ind);
		spl(s);
#endif
	}
}

mmtime(xp)
char *xp;
{
	register int s;
	register VTDATA *vp = (VTDATA *)((TTY *)xp)->t_ddp;

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

	if (vp->vmm_esc) {
		ismmfunc(vp->vmm_esc);
		vp->vmm_esc = 0;
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
	if ( (mmcrtsav > 0) && (mmvcnt > 0) && (--mmvcnt == 0) ) {
		mm_voff(vtdata[vtactive]);
	}
}

mmwrite( dev, iop )
dev_t dev;
register IO *iop;
{
	int ioc;
	register TTY *tp = vttty[vtindex(dev)];

	if (!tp) {
		printf( "mmwrite: bad dev %x", dev );
	}
	/*
	 * Kernel writes.
	 */
	if (iop->io_seg == IOSYS) {
		while (mmgo(iop, tp->t_ddp, vtindex(dev)))
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
			while (tp->t_flags & T_STOP) {
				register s = sphi();

				tp->t_flags |= T_HILIM;
				sleep((char*) &tp->t_oq,
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
				else while ( mmgo(iop, tp->t_ddp, vtindex(dev)))
					;
				goto mmwdone;
			}
			mmgo(iop, tp->t_ddp, vtindex(dev));
		} while ( iop->io_ioc );
		goto mmwdone;
	}

	/*
	 * Non-blocking user writes with output stopped.
	 */
	if ( tp->t_flags & T_STOP ) {
		u.u_error = EAGAIN;
		goto mmwdone;
	}

	/*
	 * Non-blocking user writes do not pause after scrolling.
	 */
	{
		while ( mmgo(iop, tp->t_ddp, vtindex(dev)) )
			;
	}
#else
	ttwrite(tp, iop);
#endif
mmwdone:
	return;
}

/******************************************************************************
*
* The following routines are called by deferred isr's, i.e., no sleep() calls 
* allowed 
*
*******************************************************************************/

/*
 * update the screen to match vtactive
 */
updscreen(index)
int index;
{
	register int pos, s;
	VTDATA *vp;

	vp = vtdata[index];
	pos = vp->vmm_voff;
	PRINTV( "update screen@%x {%d @%x|",
		vp->vmm_port, index, pos );

	s = sphi();
	/* update base of video memory */
	outb(vp->vmm_port, 0xC);
	outb(vp->vmm_port+1, pos >> (8 + 1) );
	outb(vp->vmm_port, 0xD);
	outb(vp->vmm_port+1, pos >> (0 + 1) );

	/* update the cursor */
	pos += vp->vmm_pos;

	pos |= vp->vmm_invis;		/* Mask cursor, if set */
	outb(vp->vmm_port, 0xE);
	outb(vp->vmm_port+1, pos >> (8 + 1) );
	outb(vp->vmm_port, 0xF);
	outb(vp->vmm_port+1, pos >> (0 + 1) );

	spl(s);
	PRINTV( "%x}\n", pos );
}
