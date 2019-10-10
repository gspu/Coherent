/* (-lgl
 * 	COHERENT Driver Kit Version 1.1.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Polled Serial Port Device Driver.
 * - supports version 7 compatible ioctl
 */

#include <sys/coherent.h>
#include <sys/ins8250.h>
#include <sys/stat.h>
#include <sys/uproc.h>
#include <sys/proc.h>
#include <sys/tty.h>		/* indirectly includes sgtty.h */
#include <sys/con.h>
#include <sys/devices.h>
#include <errno.h>
#include <sys/sched.h>		/* CVTTOUT, IVTTOUT, SVTTOUT */
#include <sys/poll_clk.h>

/*
 * Definitions.
 *
 * HSBAUD is the highest baud rate supported by this driver
 * HS_HZ is the polling rate, i.e. the number of times per second
 *   at which all open ports are checked for input, output, and
 *   line status changes
 * MAX_HSNUM is the maximum number of devices that can be polled
 *   using this driver and can be revised up or down
 * PORT is a convenience macro for the base address of a port
 * port_config is the structure of the initial configuration for each
 *   polled port;  note that "speed" is NOT the actual baud rate, but
 *   the value of the symbol for that baud rate as defined in
 *   /usr/include/sgtty.h
 */
#define	HSBAUD	9600
#define	HS_HZ	(HSBAUD/6)
#define MAX_HSNUM	8
#define	PORT	((int)(tp->t_ddp))
struct port_config {
	int	addr;	/* base address of the 8250-family UART */
	int	speed;	/* B0..B19200 */
};

/*
 * Export Variables - these can be patched without recompiling and linking
 *
 * HSNUM is the actual number of polled serial ports, and should be
 *   less than or equal to MAX_HSNUM
 * HS_PORTS is an array of address/speed pairs, one for each port
 */
int	HSNUM = 4;
struct port_config HS_PORTS[MAX_HSNUM] = {
	{ 0x3F8, B9600 },
	{ 0x2F8, B9600 },
	{ 0x3E8, B9600 },
	{ 0x2E8, B9600 }
};

/*
 * Export Functions.
 */
int	hsload();
int	hsopen();
int	hsclose();
int	hsread();
int	hswrite();
int	hsioctl();
int	hsunload();
int	hspoll();

int	hscycle();
int	hsintr();
int	hsparam();
int	hsstart();
int	hsclk();
int	set_poll_rate();

/*
 * Import Functions
 */
int	nulldev();
int	nonedev();

/*
 * Configuration table.
 */
CON hscon ={
	DFCHR|DFPOL,			/* Flags */
	HS_MAJOR,			/* Major index */
	hsopen,				/* Open */
	hsclose,			/* Close */
	nulldev,			/* Block */
	hsread,				/* Read */
	hswrite,			/* Write */
	hsioctl,			/* Ioctl */
	nulldev,			/* Powerfail */
	nulldev,			/* Timeout */
	hsload,				/* Load */
	hsunload,			/* Unload */
	hspoll				/* Poll */
};

/*
 * Local variables.
 */
static TTY *hstty;
static TTY *hslimtty;
static TIM hstim;
static int poll_divisor;	/* used in hsclk() and set_poll_rate() */
static int iocbaud[MAX_HSNUM];
static char ioclcr[MAX_HSNUM];

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
 * poll_hz[] is tied to timeconst[] - it gives the minimum polling
 *	rate for the corresponding port speed; it must be a multiple
 *	of 100 (system clock Hz) and >= baud/6
 */
int poll_hz[] ={
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
 * Load Routine.
 */
static hsload()
{
	register TTY * tp;
	register int port;
	int i, b;

	if ((hstty = (TTY *)kalloc(HSNUM*sizeof(TTY))) == 0) {
		printf("hsload: can't allocate tty's\n");
		return;
	}
	kclear(hstty, HSNUM*sizeof(TTY));

	for (i = 0; i < HSNUM; i++) {
		port = HS_PORTS[i].addr;
		tp = hstty + i;

		outb(port+MCR, 0);
		outb(port+IER, 0);

		if (inb(port+IER))
			break;

		tp->t_cs_sel  = cs_sel();
		tp->t_start   = hsstart;
		tp->t_param   = hsparam;
		tp->t_sgttyb.sg_ospeed = tp->t_sgttyb.sg_ispeed = 
		tp->t_dispeed = tp->t_dospeed = HS_PORTS[i].speed;
		tp->t_ddp     = port;

		b = timeconst[ tp->t_sgttyb.sg_ospeed ];
		outb(port+LCR, LC_DLAB);
		outb(port+DLL, b);
		outb(port+DLH, b >> 8);
		outb(port+LCR, LC_CS8);

		hslimtty = tp;
	}
}

static hsunload()
{
	if (hstty != (TTY *)0)
		kfree(hstty);
}

/*
 * Open Routine.
 */
hsopen(dev, mode)
dev_t dev;
{
	register TTY * tp = &hstty[ dev & 15 ];
	register int b;
	int s;

	/*
	 * Verify hardware exists.
	 */
	if ((PORT == 0) || (inb(PORT+IER) & ~IE_TxI)) {
		u.u_error = ENXIO;
		return;
	}

	/*
	 * Can't open if another driver is using polling
	 */
	if (poll_owner & ~ POLL_HS) {
		u.u_error = EDBUSY;
		return;
	}

	/*
	 * Initialize if not already open.
	 */
	if (++tp->t_open == 1) {
		ttopen(tp);

		if (dev & 0x80) {
			s = sphi();
			b = inb(PORT+MSR);
			tp->t_flags |= T_MODC + T_STOP;
			if (b & MS_CTS)
				tp->t_flags &= ~T_STOP;
			if (b & MS_DSR)
				tp->t_flags |=  T_CARR;
			spl(s);
		} else  {
			tp->t_flags &= ~T_MODC;
			tp->t_flags |=  T_CARR;
		}
		hscycle(tp);
	}
	ttsetgrp(tp, dev);
	set_poll_rate();
}

/*
 * Close Routine.
 */
hsclose(dev)
dev_t dev;
{
	register TTY * tp = &hstty[ dev & 15 ];

	/*
	 * Reset if last close.
	 */
	if (tp->t_open == 1) {
		int state;

		ttclose(tp);
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
	set_poll_rate();
}

/*
 * Read Routine.
 */
hsread(dev, iop)
dev_t dev;
register IO * iop;
{
	ttread(&hstty[ dev & 15 ], iop);
}

/*
 * Write Routine.
 */
hswrite(dev, iop)
dev_t dev;
register IO * iop;
{
	ttwrite(&hstty[ dev & 15 ], iop);
}

/*
 * Ioctl Routine.
 */
hsioctl(dev, com, vec)
dev_t dev;
int com;
struct sgttyb * vec;
{
	ttioctl(&hstty[ dev & 15 ], com, vec);
}

/*
 * Polling Routine.
 */
hspoll(dev, ev, msec)
dev_t dev;
int ev;
int msec;
{
	return ttpoll(&hstty[ dev & 15 ], ev, msec);
}

/*
 * Cyclic routine - invoked every clock tick to perform raw input/output.
 *
 *	Notes:	Invoked 10 times per second.
 */
hscycle(tp)
register TTY * tp;
{
	register int resid;
	register int c;

	/*
	 * Process rawin buf.
	 */
	while (tp->t_rawin.si_ix != tp->t_rawin.si_ox) {

		ttin(tp, tp->t_rawin.si_buf[ tp->t_rawin.si_ox ]);

		if (tp->t_rawin.si_ox >= sizeof(tp->t_rawin.si_buf) - 1)
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
	while ((--resid >= 0) && ((c = ttout(tp)) >= 0)) {

		tp->t_rawout.si_buf[ tp->t_rawout.si_ix ] = c;

		if (tp->t_rawout.si_ix >= sizeof(tp->t_rawout.si_buf) - 1)
			tp->t_rawout.si_ix = 0;
		else
			tp->t_rawout.si_ix++;
	}

	/*
	 * (Re)start output, waking processes waiting to output, etc.
	 */
	ttstart(tp);

	/*
	 * Schedule next cycle.
	 */
	if (tp->t_open != 0)
		timeout(&tp->t_rawtim, HZ/10, hscycle, tp);
}

/*
 * Clock Interrupt driven Polling routine.
 */
hsintr()
{
	register TTY * tp = &hstty[0];
	register int b;

	do {
		if (tp->t_open == 0)
			continue;

		/*
		 * Check modem status if modem control is enabled.
		 */
		if (tp->t_flags & T_MODC) {

			b = inb(PORT+MSR);

			if (b & (MS_DCTS|MS_DDSR)) {

				if (b & MS_DCTS) {
					if (b & MS_CTS)
						tp->t_flags &= ~T_STOP;
					else
						tp->t_flags |=  T_STOP;
				}
				if (b & MS_DDSR) {
					if (b & MS_DSR)
						tp->t_flags |=  T_CARR;
					else {
						tp->t_flags &= ~T_CARR;
						tthup(tp);
					}
				}
			}
		}

		b = inb(PORT+LSR);

		if ((b & LS_BREAK) && (tp->t_flags & T_CARR))
			ttsignal(tp, SIGINT);

		/*
		 * Receive ready.
		 */
		if (b & LS_RxRDY) {

			tp->t_rawin.si_buf[tp->t_rawin.si_ix] = inb(PORT+DREG);

			if (tp->t_flags & T_CARR) {

				if (++(tp->t_rawin.si_ix) >=
						sizeof(tp->t_rawin.si_buf))
					tp->t_rawin.si_ix = 0;
			}
		}

		/*
		 * Transmit ready and raw output data exists.
		 */
		if ((b & LS_TxRDY) && ((tp->t_flags & T_STOP) == 0)
		  && (tp->t_rawout.si_ix != tp->t_rawout.si_ox)) {

			outb(	PORT+DREG,
				tp->t_rawout.si_buf[ tp->t_rawout.si_ox ]);

			if (++(tp->t_rawout.si_ox) >=
					sizeof(tp->t_rawout.si_buf))
				tp->t_rawout.si_ox = 0;
		}

	} while (++tp <= hslimtty);
}

/*
 * Set hardware parameters.
 */
hsparam(tp)
register TTY * tp;
{
	register int b;
	int s;
	int hnum;
	int newbaud;
	char newlcr;
	int write_baud = 1, write_lcr = 1;

	newbaud = timeconst[tp->t_sgttyb.sg_ospeed];

	switch (tp->t_sgttyb.sg_flags & (EVENP|ODDP|RAW)) {
	case ODDP:
		newlcr = LC_CS7|LC_PARENB;
		break;
	case EVENP:
		newlcr = LC_CS7|LC_PARENB|LC_PAREVEN;
		break;
	default:
		newlcr = LC_CS8;
		break;
	}
	
	hnum = tp - hstty;
	if (hnum >= 0 && hnum < HSNUM) {
		if (newbaud == iocbaud[hnum]) {
			write_baud = 0;
			if (newlcr == ioclcr[hnum]) {
				write_lcr = 0;
			}
		}
		iocbaud[hnum] = newbaud;
		ioclcr[hnum] = newlcr;
	}

	s = sphi();
	/*
	 * Assert required modem control lines (DTR, RTS).
	 */
	if (tp->t_sgttyb.sg_ospeed == B0) {
		outb(PORT+MCR, 0);
	} else {
		outb(PORT+MCR, MC_DTR | MC_RTS);
	}

	/*
	 * Program baud rate.
	 */
	if (write_baud) {
		outb(PORT+LCR, LC_DLAB);
		outb(PORT+DLL, newbaud);
		outb(PORT+DLH, newbaud >> 8);
	}

	/*
	 * Program character size, parity.
	 */
	if (write_lcr)
		outb(PORT+LCR, newlcr);

	/*
	 * Enable Transmit Buffer Empty Interrupts.
	 */
	outb(PORT+IER, IE_TxI);

	spl(s);
	set_poll_rate();
}

/*
 * Start Routine.
 */
hsstart(tp)
register TTY * tp;
{
	register int s;

	/*
	 * Transmit buffer is empty, and raw output buffer is not.
	 */
	s = sphi();
	if ((inb(PORT+LSR) & LS_TxRDY)
	  && (tp->t_rawout.si_ix != tp->t_rawout.si_ox)) {

		/*
		 * Send next char from raw output buffer.
		 */
		outb(PORT+DREG, tp->t_rawout.si_buf[ tp->t_rawout.si_ox ]);

		if (++tp->t_rawout.si_ox >= sizeof(tp->t_rawout.si_buf))
			tp->t_rawout.si_ox = 0;
	}
	spl(s);
}

/*
 * hsclk will be called every time T0 interrupts - if it returns 0,
 * the usual system timer interrupt stuff is done
 */
static int hsclk()
{
  static int count;

  hsintr();
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
	if (poll_owner & ~ POLL_HS)
		return;

	/*
	 * find highest valid polling rate in units of HZ/10
	 */
	max_rate = 0;
	for (port_num = 0; port_num < HSNUM; port_num++) {
		if (hstty[port_num].t_open) {
		  port_rate = poll_hz[hstty[port_num].t_sgttyb.sg_ispeed];
		  if (max_rate < port_rate)
			max_rate = port_rate;
		}
	}
	/*
	 * if max_rate is not current rate, adjust the system clock
	 */
	if (max_rate != poll_rate) {
		poll_rate = max_rate;
		poll_divisor = poll_rate/HZ;  /* used in hsclk() */
		altclk_out();		/* stop previous polling */
		poll_owner &= ~POLL_HS;
		if (max_rate) {	/* resume polling at new rate if needed */
			altclk_in(poll_rate, hsclk);
			poll_owner |= POLL_HS;
		}
	}
}
