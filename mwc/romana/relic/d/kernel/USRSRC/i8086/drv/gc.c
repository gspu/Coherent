/*
 * Interrupt Driver Multi-Port Device Driver.
 * - supports version 7 compatible ioctl
 */

#include "coherent.h"
#include "ins8250.h"
#include <sys/stat.h>
#include <sys/uproc.h>
#include <sys/proc.h>
#include <sys/tty.h>		/* indirectly includes sgtty.h */
#include <sys/con.h>
#include <errno.h>
#include <sys/timeout.h>	/* TIM */
#include <sched.h>		/* CVTTOUT, IVTTOUT, SVTTOUT */
#include <poll_clk.h>

/*
 * Definitions.
 *
 * MAX_GCNUM is the maximum number of devices that can be polled
 *   using this driver and can be revised up or down
 * PORT is a convenience macro for the base address of a port
 * port_config is the structure of the initial configuration for each
 *   polled port;  note that "speed" is NOT the actual baud rate, but
 *   the value of the symbol for that baud rate as defined in
 *   /usr/include/sgtty.h
 * GCINT is the IRQ number for the interrupt belonging to the board.
 *   On the AT, IRQ2 is mapped to the same vector as IRQ9.
 */
#define MAX_GCNUM	8
#define	PORT	((int)(tp->t_ddp))
struct port_config {
	int	addr;	/* base address of the 8250-family UART */
	int	speed;	/* B0..B19200 */
};
#define GCINT		2

/*
 * Export Variables - these can be patched without recompiling and linking
 *
 * GCNUM is the actual number of polled serial ports, and should be
 *   less than or equal to MAX_GCNUM
 * GCIRPT is the I/O address of the multiport interrupt register;
 *   a value of 0 in bit 0..3 means a pending interrupt for port 0..3
 * GC_PORTS is an array of address/speed pairs, one for each port
 */
int	GCNUM = 4;
int	GCIRPT = 0x2BF;
/*
 * Defaults are for PC COM4, Enhanced Mode, High Address.
 */
struct port_config GC_PORTS[MAX_GCNUM] = {
	{ 0x2A0, B9600 },
	{ 0x2A8, B9600 },
	{ 0x2B0, B9600 },
	{ 0x2B8, B9600 }
};

/*
 * Export Functions.
 */
int	gcload();
int	gcopen();
int	gcclose();
int	gcread();
int	gcwrite();
int	gcioctl();
int	gcunload();
int	gcpoll();

int	gccycle();
int	gcintr();
int	gcparam();
int	gcstart();

/*
 * Import Functions
 */
int	nulldev();
int	nonedev();

/*
 * Configuration table.
 */
CON gccon ={
	DFCHR|DFPOL,			/* Flags */
	GCINT,				/* Major index */
	gcopen,				/* Open */
	gcclose,			/* Close */
	nulldev,			/* Block */
	gcread,				/* Read */
	gcwrite,			/* Write */
	gcioctl,			/* Ioctl */
	nulldev,			/* Powerfail */
	nulldev,			/* Timeout */
	gcload,				/* Load */
	gcunload,			/* Unload */
	gcpoll				/* Poll */
};

/*
 * Local variables.
 */
static TTY *hstty;
static TTY *hslimtty;
static TIM hstim;

/*
 * Time constant table.
 * Indexed by ioctl baud rate.
 */
static
int timeconst[] = {
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
	6,				/* EXTA */
	6				/* EXTB */
};

/*
 * Load Routine.
 */
static gcload()
{
	register TTY * tp;
	register int port;
	int i, b, s;

	if ((hstty = (TTY *)kalloc(GCNUM*sizeof(TTY))) == 0) {
		printf("gcload: can't allocate tty's\n");
		return;
	}
	kclear(hstty, GCNUM*sizeof(TTY));

	s = sphi();
	for (i = 0; i < GCNUM; i++) {
		port = GC_PORTS[i].addr;
		tp = hstty + i;

		outb( port+MCR, 0 );
		outb( port+IER, 0 );

		if ( inb( port+IER ) )
			break;

		tp->t_cs_sel  = cs_sel();
		tp->t_start   = gcstart;
		tp->t_param   = gcparam;
		tp->t_sgttyb.sg_ospeed = tp->t_sgttyb.sg_ispeed = 
		tp->t_dispeed = tp->t_dospeed = GC_PORTS[i].speed;
		tp->t_ddp     = port;

		b = timeconst[ tp->t_sgttyb.sg_ospeed ];
		outb( port+LCR, LC_DLAB );
		outb( port+DLL, b );
		outb( port+DLH, b >> 8);
		outb( port+LCR, LC_CS8);

		hslimtty = tp;
	}
	setivec(GCINT, gcintr);
	spl(s);
}

static gcunload()
{
	clrivec(GCINT);
	kfree(hstty);
}

/*
 * Open Routine.
 */
gcopen( dev, mode )
dev_t dev;
{
	register TTY * tp = &hstty[ dev & 15 ];
	register int b;
	int s;

	/*
	 * Verify hardware exists.
	 */
	if ( (PORT == 0) || (inb(PORT+IER) & ~IE_TxI) ) {
		u.u_error = ENXIO;
		return;
	}

	/*
	 * Initialize if not already open.
	 */
	if ( ++tp->t_open == 1 ) {
		ttopen( tp );

		if ( dev & 0x80 ) {
			s = sphi();
			b = inb(PORT+MSR);
			tp->t_flags |= T_MODC + T_STOP;
			if ( b & MS_CTS )
				tp->t_flags &= ~T_STOP;
			if ( b & MS_DSR )
				tp->t_flags |=  T_CARR;
			spl( s );
		} else  {
			tp->t_flags &= ~T_MODC;
			tp->t_flags |=  T_CARR;
		}
		gccycle( tp );
	}
	ttsetgrp( tp, dev );
}

/*
 * Close Routine.
 */
gcclose( dev )
dev_t dev;
{
	register TTY * tp = &hstty[ dev & 15 ];

	/*
	 * Reset if last close.
	 */
	if ( tp->t_open == 1 ) {
		int state;

		ttclose( tp );
		/*
		 * ttclose() only emptied the output queue tp->t_oq;
		 * now wait 0.1 sec for the silo tp->rawout to empty
		 * and allow a delay for the UART on-chip xmit buffer to empty
		 *
		 * state 2: waiting for silo to empty
		 * state 1: stalling so UART can empty xmit buffer
		 * state 0: done!
		 */
		state = 2;
		while (state) {
			timeout(&hstim, 10, wakeup, (int)&hstim);
			sleep((char *)&hstim, CVTTOUT, IVTTOUT, SVTTOUT);
			if (tp->t_rawout.si_ix == tp->t_rawout.si_ox  && state)
				state--;
		}
	}

	--tp->t_open;
}

/*
 * Read Routine.
 */
gcread( dev, iop )
dev_t dev;
register IO * iop;
{
	ttread( &hstty[ dev & 15 ], iop, 0 );
}

/*
 * Write Routine.
 */
gcwrite( dev, iop )
dev_t dev;
register IO * iop;
{
	ttwrite( &hstty[ dev & 15 ], iop, 0 );
}

/*
 * Ioctl Routine.
 */
gcioctl( dev, com, vec )
dev_t dev;
int com;
struct sgttyb * vec;
{
	ttioctl( &hstty[ dev & 15 ], com, vec );
}

/*
 * Polling Routine.
 */
gcpoll( dev, ev, msec )
dev_t dev;
int ev;
int msec;
{
	return ttpoll( &hstty[ dev & 15 ], ev, msec );
}

/*
 * Cyclic routine - invoked every clock tick to perform raw input/output.
 *
 *	Notes:	Invoked 10 times per second.
 */
gccycle( tp )
register TTY * tp;
{
	register int resid;
	register int c;

	/*
	 * Process rawin buf.
	 */
	while ( tp->t_rawin.si_ix != tp->t_rawin.si_ox ) {

		ttin( tp, tp->t_rawin.si_buf[ tp->t_rawin.si_ox ] );

		if ( tp->t_rawin.si_ox >= sizeof(tp->t_rawin.si_buf) - 1 )
			tp->t_rawin.si_ox = 0;
		else
			tp->t_rawin.si_ox++;
	}

	/*
	 * Calculate free output slot count.
	 */
	resid  = sizeof(tp->t_rawout.si_buf) - 1;
	resid += tp->t_rawout.si_ox - tp->t_rawout.si_ix;
	resid %= sizeof(tp->t_rawout.si_buf);

	/*
	 * Fill raw output buffer.
	 */
	while ( (--resid >= 0) && ((c = ttout(tp)) >= 0) ) {

		tp->t_rawout.si_buf[ tp->t_rawout.si_ix ] = c;

		if ( tp->t_rawout.si_ix >= sizeof(tp->t_rawout.si_buf) - 1 )
			tp->t_rawout.si_ix = 0;
		else
			tp->t_rawout.si_ix++;
	}

	/*
	 * (Re)start output, waking processes waiting to output, etc.
	 */
	ttstart( tp );

	/*
	 * Schedule next cycle.
	 */
	if ( tp->t_open != 0 )
		timeout( &tp->t_rawtim, HZ/10, gccycle, tp );
}

/*
 * Interrupt driven Polling routine.
 */
gcintr()
{
	register TTY * tp = &hstty[0];
	register int b;

	do {
		if ( tp->t_open == 0 )
			continue;

		/*
		 * Check modem status if modem control is enabled.
		 */
		if ( tp->t_flags & T_MODC ) {

			b = inb( PORT+MSR );

			if ( b & (MS_DCTS|MS_DDSR) ) {

				if ( b & MS_DCTS ) {
					if ( b & MS_CTS )
						tp->t_flags &= ~T_STOP;
					else
						tp->t_flags |=  T_STOP;
				}
				if ( b & MS_DDSR ) {
					if ( b & MS_DSR )
						tp->t_flags |=  T_CARR;
					else {
						tp->t_flags &= ~T_CARR;
						tthup( tp );
					}
				}
			}
		}

		b = inb( PORT+LSR );

		if ( (b & LS_BREAK) && (tp->t_flags & T_CARR) )
			ttsignal( tp, SIGINT );

		/*
		 * Receive ready.
		 */
		if ( b & LS_RxRDY ) {

			tp->t_rawin.si_buf[tp->t_rawin.si_ix] = inb(PORT+DREG);

			if ( tp->t_flags & T_CARR ) {

				if ( ++(tp->t_rawin.si_ix) >=
						sizeof(tp->t_rawin.si_buf) )
					tp->t_rawin.si_ix = 0;
			}
		}

		/*
		 * Transmit ready and raw output data exists.
		 */
		if ( (b & LS_TxRDY) && ((tp->t_flags & T_STOP) == 0)
		  && (tp->t_rawout.si_ix != tp->t_rawout.si_ox) ) {

			outb(	PORT+DREG,
				tp->t_rawout.si_buf[ tp->t_rawout.si_ox ] );

			if ( ++(tp->t_rawout.si_ox) >=
					sizeof(tp->t_rawout.si_buf) )
				tp->t_rawout.si_ox = 0;
		}

	} while ( ++tp <= hslimtty );
}

/*
 * Set hardware parameters.
 */
gcparam( tp )
register TTY * tp;
{
	register int b;
	int s;

	s = sphi();
	/*
	 * Assert required modem control lines (DTR, RTS).
	 */
	b = 0;
	if ( tp->t_sgttyb.sg_ospeed != B0 )
		b |=  MC_DTR | MC_RTS;
	outb( PORT+MCR, b );

	/*
	 * Program baud rate.
	 */
	if (b = timeconst[ tp->t_sgttyb.sg_ospeed ]) {
		outb( PORT+LCR, LC_DLAB );
		outb( PORT+DLL, b );
		outb( PORT+DLH, b >> 8 );
	}

	/*
	 * Program character size, parity.
	 */
	switch ( tp->t_sgttyb.sg_flags & (EVENP|ODDP|RAW) ) {
	case ODDP:		b = LC_CS7|LC_PARENB;		 break;
	case EVENP:		b = LC_CS7|LC_PARENB|LC_PAREVEN; break;
	default:		b = LC_CS8;			 break;
	}
	outb( PORT+LCR, b );

	/*
	 * Enable Transmit Buffer Empty Interrupts.
	 */
	outb( PORT+IER, IE_TxI );

	spl(s);
}

/*
 * Start Routine.
 */
gcstart( tp )
register TTY * tp;
{
	register int s;

	/*
	 * Transmit buffer is empty, and raw output buffer is not.
	 */
	s = sphi();
	if ( (inb( PORT+LSR ) & LS_TxRDY)
	  && (tp->t_rawout.si_ix != tp->t_rawout.si_ox) ) {

		/*
		 * Send next char from raw output buffer.
		 */
		outb( PORT+DREG, tp->t_rawout.si_buf[ tp->t_rawout.si_ox ] );

		if ( ++tp->t_rawout.si_ox >= sizeof(tp->t_rawout.si_buf) )
			tp->t_rawout.si_ox = 0;
	}
	spl( s );
}
