.ds TL "Character Driver"
.NH "Example of a Character Driver"
.PP
This section gives an example driver for a character device:
the \*(CO driver for the 8250-style asynchronous ports.
It is described in the article
.B asy
in the \*(CO Lexicon.
.PP
In this driver, the minor-device number is a bit map that
describes the features of the port, as follows:
.DS
	0x80	1 for NO modem control, `l' (lower-case ``el'')
	0x40	1 for polled operation (no IRQ service), `p'
	0x20	1 for RTS/CTS flow control, `f'
	0x1F	The channel number - 0 through 31
.DE
Character-device line discipline, which includes such operations as processing
backspaces entered and echoing input characters, is performed in the
.B tty
module in \*(CO 4.2.
Source code is provided in the 4.2 Device-Driver Kit.
Future releases of \*(CO will use a \*(ST-based line discipline.
.SH "Preliminaries"
.PP
The following prefaces the body of the driver.
.Sh "Header Files"
.PP
.B asy
begins by including the following header files.
.DM
#include <sys/errno.h>
#include <sys/stat.h>
#include <termio.h>
#include <poll.h>
.DE
.DM
#include <sys/coherent.h>
#include <kernel/trace.h>
#include <sys/uproc.h>
#include <sys/proc.h>
#include <sys/tty.h>
#include <sys/con.h>
#include <sys/devices.h>
#include <sys/sched.h>
#include <sys/asy.h>
#include <sys/ins8250.h>
#include <sys/poll_clk.h>
.DE
.Sh "Manifest Constants"
.PP
The following gives manifest constants used throughout the driver.
.DM
#define	IEN_USE_MSI	(IE_RxI | IE_TxI | IE_LSI | IE_MSI)
#define	IEN_NO_MSI	(IE_RxI | IE_TxI | IE_LSI)
.DE
.DM
#define CTLQ	0021
#define CTLS	0023
.DE
.DM
.ta 2.0i 3.0i
#define NUM_IRQ	16	/* PC allows irq numbers 0..15 */
#define BPB	8	/* 8 bits per byte */
#define DTRTMOUT	3	/* DTR seconds for close */
#define LOOP_LIMIT	100	/* safety valve on irq loops */
.DE
.DM
.ta 2.5i
/*
 * For rawin silo (see poll_clk.h), use last element of si_buf to count
 * the number of characters in the silo.
 */
#define SILO_CHAR_COUNT	si_buf[SI_BUFSIZ-1]
#define SILO_HIGH_MARK	(SI_BUFSIZ-SI_BUFSIZ/4)
#define SILO_LOW_MARK	(SI_BUFSIZ/4)
#define MAX_SILO_INDEX	(SI_BUFSIZ-2)
#define MAX_SILO_CHARS	(SI_BUFSIZ-1)
.DE
.Sh "Macros"
.PP
.B asy
uses the following macros:
.DM
.ta 2.5i
#define RAWIN_FLUSH(in_silo) { \e
  in_silo->si_ox = in_silo->si_ix; \e
  in_silo->SILO_CHAR_COUNT = 0; }
#define RAWOUT_FLUSH(out_silo)	{ out_silo->si_ox = out_silo->si_ix; }
#define channel(dev)	(dev & 0x1F)
#define IEN	((a0->a_nms)?IEN_NO_MSI:IEN_USE_MSI)
.DE
.DM
.ta 2.0i 3.0i
#define NW_OUTSILO	1	/* bits in need_wake[] entries */
.DE
.DM
typedef void (* VPTR)();	/* pointer to function returning void */
typedef int (* FPTR)();	/* pointer to function returning int */
.DE
.Sh "Local Functions"
.PP
The following declares the functions used locally.
.DM
void asy_putchar();
.DE
.DM
/* Configuration functions (local) */
static void cinit();
.DE
.DM
/* Support functions (local) */
static void add_irq();
static void asy_irq();
static int asy_send();
static void asybreak();
static void asyclock();
static void asycycle();
static void asydump();
static int asyintr();
static void asyparam();
static void asysph();
static void asyspr();
static void asystart();
static void endbrk();
static void irqdummy();
static void upd_irq1();
.DE
.DM
static int p1(),p2(),p3(),p4();
.DE
.DM
extern int albaud[], alp_rate[];
.DE
.Sh "Global Variables"
.PP
.B asy
uses the following global variables:
When the command
.B asypatch
patches the
.B asy
driver for your system's configuration of ports,
it checks whether its internal value for
.B ASY_VERSION
matches this driver's value.
This prevents the patch utility and the driver from getting out of synch.
.DM
int ASY_VER = ASY_VERSION;
int ASY_HPCL = 1;
int ASY_NUM = 0;
int ASYGP_NUM = 0;
asy0_t asy0[MAX_ASY] = {
	{ 0 }
};
asy_gp_t asy_gp[MAX_ASYGP] = {
	{ 0 }
};
.DE
.Sh "Static Variables"
.PP
.B asy
uses the following static variables.
.DM
.ta 3.0i
static asy1_t * asy1;	/* unused entries have type US_NONE */
static short dummy_port;	/* used only during driver startup */
static int poll_divisor;	/* set by asyspr(), read by asyclk() */
static char pptbl [MAX_ASY];	/* channel numbers of polled ports */
static int ppnum;	/* number of channels in pptbl */
.DE
Variables \fBirq0[\fIx\^\fB]\fR and \fBirq1[\fIx\^\fB]\fR
are lists for IRQ number
.IR x .
.B irq0[]
has nodes that may possibly cause an IRQ.
.B irq1[]
contains nodes for active devices.
Whenever a device becomes active or inactive,
.B irq1
is rebuilt from
.BR irq0 .
.PP
.B nodespace
is an array of the nodes that are available.
.B nextnode
points to the next free node.
Nodes are taken from node space only when the driver is loaded.
.DM
static FPTR ptbl [PT_MAX] = { asyintr,p1,p2,p3,p4 };
static struct irqnode *irq0[NUM_IRQ], *irq1[NUM_IRQ];
static struct irqnode nodespace[MAX_ASY];
static char need_wake[MAX_ASY];
static char nextnode;
static int initialized;	/* for asy_putchar() */
.DE
.SH "The Load Routine"
.PP
The first function is
.BR asyload() .
The kernel invokes this function when the driver is loaded into memory.
Because \*(CO 4.2 does not support loadable drivers, this function is
executed only when the kernel boots.
.PP
Field
.B c_load
in the
.B CON
structure contains a pointer to this function.
.DM
static void
asyload()
{
	int s, chan;
	asy0_t *a0;
	asy1_t *a1;
	TTY *tp;
	short port;
	char irq;
	char speed;
	char g;
	char sense_ct = 0;
.DE
.DM
	/* Allocate space for asy structs.  Possible error return. */
	asy1 = (asy1_t *)kalloc(ASY_NUM * sizeof(asy1_t));
	if (asy1 == 0) {
		printf("asyload: can't allocate space for %d async devices\en",
		  ASY_NUM);
		return;
	}
	kclear(asy1, ASY_NUM*sizeof(asy1_t));
.DE
.DM
	/*
	 * For each non-null port:
	 *	sense chip type
	 *	write baud rate to sgtty/termio structs
	 *	disable port interrupts
	 *	hang up port
	 *	set default baud rate (also resets UART)
	 *	hook "start" function into line discipline module
	 *	hook "param" function into line discipline module
	 *	hook CS into line discipline module
	 * 	if port uses irq
	 *		if not in a port group
	 *			add to irq list
	 */
.DE
.DM
	for (chan = 0; chan < ASY_NUM; chan++) {
		a0 = asy0 + chan;
		a1 = asy1 + chan;
		tp = &a1->a_tty;
		speed = a0->a_speed;
		tp->t_sgttyb.sg_ispeed = tp->t_sgttyb.sg_ospeed = speed;
		tp->t_dispeed = tp->t_dospeed = speed;
		port = a0->a_port;
.DE
.DM
		/*
		 * A port address of zero means a skipped entry in the table.
		 * In this case a1->a_ut keeps its initial value of US_NONE.
		 */
		if (port) {
			dummy_port = port;
.DE
.DM
			/*
			 * uart_sense() prints port info.
			 * Do this four times per line.
			 */
			a1->a_ut = uart_sense(port);
			sense_ct++;
			if ((sense_ct & 1) == 0)
				putchar('\en');
			else
				putchar('\et');
.DE
.DM
			s = sphi();
			outb(port+MCR, 0);
			outb(port+LCR, LC_DLAB);
			outb(port+DLL, albaud[speed]);
			outb(port+DLH, albaud[speed] >> 8);
			outb(port+LCR, LC_CS8);
			tp->t_start = asystart;
			/* leave tp->t_param at 0 */
			tp->t_ddp = (char *) chan;
			spl(s);
.DE
.DM
			if (a0->a_irqno && a0->a_asy_gp == NO_ASYGP)
				add_irq (a0->a_irqno, asyintr, chan);
		}
	}
.DE
.DM
	if (sense_ct & 1)
		putchar('\en');
.DE
.DM
	/* for each port group,	add group to irq list */
	for (g = 0 ; g < ASYGP_NUM ; g ++)
		add_irq (asy_gp [g].irq, ptbl [asy_gp [g].gp_type], g);
.DE
.DM
	/* Attach irq routines. */
	for (irq = 0 ; irq < NUM_IRQ ; irq ++) {
		if (irq0 [irq])
			setivec (irq, asy_irq);
	}
}
.DE
.SH "The Unload Routine"
.PP
The kernel invokes function
.B asyunload()
when
.B asy
is unloaded from memory.
As \*(CO 4.2 does not support loadable drivers, this function is never
invoked.
.PP
Field
.B c_uload
in the
.B CON
structure contains a pointer to this function.
.DM
static void
asyunload()
{
	char chan, irq;
.DE
.DM
	/*
	 *  for each channel:
	 *	disable UART interrupts
	 *	hangup port
	 *	cancel timer
	 */
	for (chan = 0; chan < ASY_NUM; chan++) {
		asy0_t * a0 = asy0 + chan;
		asy1_t * a1 = asy1 + chan;
		short port = a0->a_port;
		TTY *tp = &a1->a_tty;
.DE
.DM
		outb(port+IER, 0);
		outb(port+MCR, 0);
		timeout (tp->t_rawtim, 0, NULL, 0);
	}
.DE
.DM
	/* for each irq,  detach irq routine if one was attached */
	for (irq = 0 ; irq < NUM_IRQ ; irq ++)
		if (irq0 [irq])
			clrivec(irq);
.DE
.DM
	/* deallocate dynamic storage */
	if (asy1)
		kfree (asy1);
}
.DE
.SH "The Open Routine"
.PP
The kernel invokes function
.B asyopen()
when a user application invokes the system call
.B open()
to open a serial port.
.PP
Field
.B c_open
in the
.B CON
structure contains a pointer to this function.
.DM
static void
asyopen(dev, mode)
dev_t dev;
int mode;
{
	int s;
	char msr, mcr;
	char chan = channel(dev);
	asy0_t *a0 = asy0 + chan;
	asy1_t *a1 = asy1 + chan;
	TTY *tp = &a1->a_tty;
	short port = a0->a_port;
.DE
.DM
	/* chip not found */
	if (a1->a_ut == US_NONE) {
		set_user_error (ENXIO);
		goto bad_open;
	}
.DE
.DM
	if ((tp->t_flags & T_EXCL) != 0 && ! super()) {
		set_user_error (ENODEV);
		goto bad_open;
	}
.DE
.DM
	/* Can't open for hardware flow control if modem status
	 * interrupts are disallowed.
	 */
	if (a0->a_nms && (dev & CFLOW) != 0) {
		set_user_error (ENXIO);
		goto bad_open;
	}
.DE
.DM
	/* Can't open a polled port if another driver is using polling. */
	if (dev & CPOLL && poll_owner & ~ POLL_ASY) {
		set_user_error (EBUSY);
		goto bad_open;
	}
.DE
.DM
	/* Can't have both com[13] or both com[24] IRQ at once. */
	if (!(dev & CPOLL) && a0->a_ixc) {
		struct irqnode *np = irq1[a0->a_irqno];
		while (np) {
			if (np->func != ptbl[0] || np->arg != chan) {
				set_user_error (EBUSY);
				goto bad_open;
			}
			np = np->next_actv;
		}
	}
.DE
.DM
	/* If port already in use, are new and old open modes compatible? */
	if (a1->a_in_use) {
		int oldmode = 0, newmode = 0; /* mctl:1 irq:2 flow:4 */
.DE
.DM
		if (a1->a_modc)
			oldmode += 1;
		if (a1->a_irq)
			oldmode += 2;
		if (a1->a_flc)
			oldmode += 4;
		if ((dev & NMODC) == 0)
			newmode += 1;
		if ((dev & CPOLL) == 0)
			newmode += 2;
		if (dev & CFLOW)
			newmode += 4;
		if (oldmode != newmode) {
			set_user_error (EBUSY);
			goto bad_open;
		}
	}
.DE
At this point, sleep if another process is opening or closing the port.
This can happen if:
.IP \(bu 0.3i
Another process is trying a first open and awaiting CD.
.IP \(bu 0.3i
Another process is closing the port after losing CD.
.IP \(bu 0.3i
A remote process opened the port,
spawned a daemon,
and disconnected, yet the daemon ignored
.B SIGHUP
and is improperly keeping the port open.
.PP
Do not try to set
.B tp->t_flags
before this sleep!
During the sleep,
.B ttclose()
may be called and clear the flags.
.DM
	while (a1->a_in_use &&
		(a1->a_hcls || ((dev & NMODC) == 0 &&
			(inb (port + MSR) & MS_RLSD) == 0))) {
.DE
.DM
		if (x_sleep ((char *) & tp->t_open, pritty, slpriSigCatch,
				"asyblk") == PROCESS_SIGNALLED) {
			set_user_error (EINTR);
			goto bad_open;
		}
	}
.DE
.DM
	/* If channel not in use, mark it as such. */
	if (a1->a_in_use == 0) {
		/* Save modes for this open attempt to avoid future conflicts.
		 * Then start asycycle() for this port.
		 */
		if (dev & NMODC) {
			tp->t_flags &= ~T_MODC;
			a1->a_modc = 0;
		} else {
			tp->t_flags |= T_MODC;
			a1->a_modc = 1;
		}
.DE
.DM
		if (dev & CPOLL)
			a1->a_irq = 0;
		else
			a1->a_irq = 1;
		if (dev & CFLOW) {
			tp->t_flags |= T_CFLOW;
			a1->a_flc = 1;
		} else {
			tp->t_flags &= ~T_CFLOW;
			a1->a_flc = 0;
		}
	}
	a1->a_in_use++;
.DE
.DM
	/* From here, error exit is bad_open_u. */
	if (tp->t_open == 0) {        /* not already open */
		silo_t	* in_silo = &a1->a_in;
.DE
.DM
		if (!(dev & CPOLL)) {
			upd_irq1(a0->a_irqno);
			a1->a_has_irq = 1;
		}
.DE
.DM
		/* Need to start cycling to scan for CD. */
		asycycle(chan);
.DE
.DM
		s = sphi();
		/* Raise basic modem control lines even if modem
		 * control hasn't been specified.
		 * MC_OUT2 turns on NON-open-collector IRQ line from the UART.
		 * since we can't have two UART's on same IRQ with MC_OUT2 on
		 */
		mcr = MC_RTS | MC_DTR;
.DE
.DM
		if (dev & CPOLL) {
			outb(port+MCR, mcr);
		} else {
			outb(port+MCR, mcr | a0->a_outs);
			outb(port+IER, IEN);
		}
.DE
.DM
		if ((dev & NMODC) == 0) {	/* want modem control? */
			tp->t_flags |= T_HOPEN | T_STOP;
			for (;;) {	/* wait for carrier */
				msr = inb(port+MSR);
				/* If carrier detect present
				 *   if port not already open
				 *     break out of loop and finish first open
				 *   else
				 *     do second (or third, etc.) open
				 */
				if (msr & MS_RLSD)
					break;
.DE
.DM
				/* wait for carrier */
				if (x_sleep ((char *) & tp->t_open, pritty,
					     slpriSigCatch, "need CD")
				    == PROCESS_SIGNALLED) {
					outb(port + MCR, 0);
			    		outb(port + IER, 0);
					set_user_error (EINTR);
					tp->t_flags &= ~(T_HOPEN | T_STOP);
					spl(s);
					goto bad_open_u;
				}
			}
.DE
.DM
			/* Mark that we are no longer hanging in open.
			 * Allow output over the port unless hardware flow
			 * control says not to.
			 */
			tp->t_flags &= ~T_HOPEN;
			tp->t_flags &= ~T_STOP;
			if (!(tp->t_flags & T_CFLOW) || (msr & MS_CTS))
				a1->a_ohlt = 0;
			else
				a1->a_ohlt = 1;
.DE
.DM
			/* Awaken any other opens on same device. */
			wakeup((char *)(&tp->t_open));
		}
		ttopen(tp);				/* stty inits */
		tp->t_flags |= T_CARR;
		if (ASY_HPCL)
			tp->t_flags |= T_HPCL;
.DE
.DM
		asyparam(tp);	/* gimmick: do this while t_open is zero */
.DE
.DM
		/* TO DO: flush UART input register(s) */
		spl(s);
.DE
.DM
		/* Turn on polling for the port. */
		if (dev & CPOLL) {
			a1->a_poll = 1;
			asyspr();
		}
	} /* end of first-open case */
.DE
.DM
	tp->t_open++;
	ttsetgrp(tp, dev, mode);
	return;
.DE
.DM
bad_open_u:
	a1->a_in_use--;
	wakeup((char *)(&tp->t_open));
bad_open:
	return;
}
.DE
.SH "The Close Routine"
.PP
The kernel invokes function
.B asyclose()
when a user application invokes the system call
.B close()
to close a serial port.
.PP
Field
.B c_close
in the
.B CON
structure contains a pointer to this function.
.DM
static void
asyclose(dev, mode)
dev_t dev;
int mode;
{
	int chan = channel(dev);
	asy0_t *a0 = asy0 + chan;
	asy1_t *a1 = asy1 + chan;
	TTY *tp = &a1->a_tty;
	silo_t *out_silo = &a1->a_out;
	silo_t *in_silo = &a1->a_in;
	int flags, maj;
	int s;
	short port = a0->a_port;
	char lsr;
.DE
.DM
	if (--tp->t_open)
		goto not_last_close;
	s = sphi();
.DE
.DM
	a1->a_hcls = 1; /* disallow reopen till done closing */
	flags = tp->t_flags; /* save flags - ttclose() zeroes them */
	ttclose(tp);
.DE
.DM
	/* Wait for output silo and UART xmit buffer to empty.
	 * Allow signal to break the sleep.
	 */
	for (;;) {
		int chipEmpty = 0, siloEmpty = 0;
.DE
.DM
		lsr = inb(port + LSR);
		chipEmpty = (lsr & LS_TxIDLE);
		siloEmpty = (out_silo->si_ix == out_silo->si_ox);
.DE
.DM
		if (chipEmpty && siloEmpty)
			break;
		need_wake[chan] |= NW_OUTSILO;
		if (x_sleep ((char *) out_silo, pritty, slpriSigCatch,
			     "asyclose") == PROCESS_SIGNALLED) {
			RAWOUT_FLUSH(out_silo);
			break;
		}
	}
	need_wake[chan] &= ~NW_OUTSILO;
.DE
.DM
	/* If not hanging in open */
	if ((flags & T_HOPEN) == 0) {
		/* Disable interrupts. */
		outb(port+IER, 0);
		outb(port+MCR, inb(port+MCR) & ~MC_OUTS);
	}
.DE
.DM
	/* If hupcls */
	if (flags & T_HPCL) {
		/* Hangup port - drop DTR and RTS. */
		outb(port+MCR, inb(port+MCR) & MC_OUTS);
.DE
.DM
		/* Hold dtr low for timeout */
		maj = major(dev);
		drvl[maj].d_time = 1;
.DE
.DM
		x_sleep ((char *) & drvl [maj].d_time, pritty, slpriNoSig,
			 "drop DTR");
		drvl[maj].d_time = 0;
	}
.DE
.DM
	a1->a_poll = 0;
	asyspr();
	RAWIN_FLUSH(in_silo);
	a1->a_hcls = 0; /* allow reopen - done closing */
	wakeup((char *)(&tp->t_open));
	spl(s);
	a1->a_in_use--;
.DE
.DM
	if (!(dev & CPOLL))
		upd_irq1(a0->a_irqno);
	return;
.DE
.DM
not_last_close:
	a1->a_in_use--;
	wakeup((char *)(&tp->t_open));
	return;
}
.DE
.SH "The Read Routine"
.PP
The kernel invokes function
.B asyread()
when a user application invokes the system call
.B read()
to read data from a serial port.
.PP
Field
.B c_read
in the
.B CON
structure contains a pointer to this function.
.DM
static void
asyread(dev, iop)
dev_t dev;
register IO * iop;
{
	int chan = channel(dev);
	asy1_t *a1 = asy1 + chan;
	TTY *tp = &a1->a_tty;
	ttread(tp, iop);
}
.DE
.SH "The Timeout Routine"
.PP
The kernel invokes function
.B asytimer()
when a timeout occurs.
.PP
Field
.B c_timer
in the
.B CON
structure contains a pointer to this function.
.DM
static void
asytimer(dev)
dev_t dev;
{
	if (++drvl[major(dev)].d_time > DTRTMOUT)
		wakeup((char *)&drvl[major(dev)].d_time);
}
.DE
.SH "The Write Routine"
.PP
The kernel invokes function
.B asywrite()
when a user application invokes the system call
.B write()
to write data to this port.
.PP
Field
.B c_write
in the
.B CON
structure contains a pointer to this function.
.DM
static void
asywrite(dev, iop)
dev_t dev;
register IO * iop;
{
	int chan = channel(dev);
	asy0_t *a0 = asy0 + chan;
	asy1_t *a1 = asy1 + chan;
	TTY *tp = &a1->a_tty;
	short port = a0->a_port;
	register int c;
.DE
.DM
	/* Treat user writes through tty driver. */
	if (iop->io_seg != IOSYS) {
		ttwrite(tp, iop);
		return;
	}
.DE
.DM
	/* Treat kernel writes by blocking on transmit buffer. */
	while ((c = iogetc(iop)) >= 0) {
		/* Wait until transmit buffer is empty.
		 * Check twice to prevent critical race with interrupt handler.
		 */
		for (;;) {
			if (inb(port+LSR) & LS_TxRDY)
				if (inb(port+LSR) & LS_TxRDY)
					break;
		}
.DE
.DM
		/* Output the next character. */
		outb(port+DREG, c);
	}
}
.DE
.SH "The ioctl Routine"
.PP
The kernel invokes function
.B asyioctl()
when a user application invokes the system call
.B ioctl()
to manipulate a serial device.
.PP
Field
.B c_open
in the
.B CON
structure contains a pointer to this function.
.DM
static void
asyioctl(dev, com, vec)
dev_t dev;
int com;
struct sgttyb *vec;
{
	int chan = channel(dev);
	asy0_t *a0 = asy0 + chan;
	asy1_t *a1 = asy1 + chan;
	TTY *tp = &a1->a_tty;
	int s;
	int temp;
	silo_t *out_silo = &a1->a_out;
	silo_t *in_silo = &a1->a_in;
	short port = a0->a_port;
	unsigned char msr, mcr, lcr, ier;
	char do_ttioctl = 0;
	char do_asyparam = 0;
.DE
.DM
	s = sphi();
	ier = inb(port+IER);
	mcr = inb(port+MCR);	/* get current MCR register status */
	lcr = inb(port+LCR);	/* get current LCR register status */
.DE
.DM
	/* If command will drain output, do the drain now
	 * before calling ttioctl().
	 * Don't do this for 286 kernel:  we're running out of code space.
	 */
	switch(com) {
.DE
.DM
	case TCSETAW:
	case TCSETAF:
	case TCSBRK:
	case TIOCSETP:
		/* Wait for output silo and UART xmit buffer to empty.
		 * Allow signal to break the sleep.
		 */
		for (;;) {
			if (! ttoutp (tp) &&
			    out_silo->si_ix == out_silo->si_ox &&
			    (inb (port + LSR) & LS_TxIDLE) != 0)
				break;
			need_wake[chan] |= NW_OUTSILO;
.DE
.DM
			if (x_sleep ((char *) out_silo, pritty, slpriSigCatch,
				     "asydrain") == PROCESS_SIGNALLED)
				break;
		}
		need_wake [chan] &= ~NW_OUTSILO;
	}
.DE
.DM
	switch(com) {
	case TIOCSBRK:	/* set BREAK */
		outb (port + LCR, lcr | LC_SBRK);
		break;
.DE
.DM
	case TIOCCBRK:	/* clear BREAK */
		outb (port + LCR, lcr & ~ LC_SBRK);
		break;
.DE
.DM
	case TIOCSDTR:	/* set DTR */
		outb (port + MCR, mcr | MC_DTR);
		break;
.DE
.DM
	case TIOCCDTR:	/* clear DTR */
		outb (port + MCR, mcr & ~ MC_DTR);
		break;
.DE
.DM
	case TIOCSRTS:	/* set RTS */
		outb (port + MCR, mcr | MC_RTS);
		break;
.DE
.DM
	case TIOCCRTS:	/* clear RTS */
		outb (port + MCR, mcr & ~ MC_RTS);
		break;
.DE
.DM
	case TIOCRSPEED:	/* set "raw" I/O speed divisor */
		outb (port + LCR, lcr | LC_DLAB); /* set speed latch bit */
		outb (port + DLL, (unsigned) vec);
		outb (port + DLH, (unsigned) vec >> 8);
		outb (port + LCR, lcr); /* reset latch bit */
		break;
.DE
.DM
	case TIOCWORDL:		/* set word length and stop bits */
		outb (port + LCR, ((lcr & ~ 0x7) | ((unsigned) vec & 0x7)));
		break;
.DE
.DM
	case TIOCRMSR:		/* get CTS/DSR/RI/RLSD (MSR) */
		msr = inb (port + MSR);
		temp = msr >> 4;
		kucopy (& temp, (unsigned *) vec, sizeof (unsigned));
		break;
.DE
.DM
	case TIOCFLUSH:		/* Flush silos here, queues in tty.c */
		RAWIN_FLUSH (in_silo);
		RAWOUT_FLUSH (out_silo);
		do_ttioctl = 1;
		break;
.DE
.DM
		/* If port parameters change, plan to call asyparam().
		 * Need to check now before structs are updated.
		 */
	case TCSETA:
	case TCSETAW:
	case TCSETAF:
		{
			struct	termio	trm;
.DE
.DM
			ukcopy (vec, & trm, sizeof (struct termio));
			if (trm.c_cflag != tp->t_termio.c_cflag)
				do_asyparam = 1;
		}
		do_ttioctl = 1;
		break;
.DE
.DM
	case TIOCSETP:
	case TIOCSETN:
		{
			struct	sgttyb	sg;
.DE
.DM
			ukcopy(vec, &sg, sizeof(struct sgttyb));
			if (sg.sg_ispeed != tp->t_sgttyb.sg_ispeed ||
			   ((sg.sg_flags ^ tp->t_sgttyb.sg_flags) & ANYP) != 0)
				do_asyparam = 1;
		}
		do_ttioctl = 1;
		break;
.DE
.DM
	default:
		do_ttioctl = 1;
	}
.DE
.DM
	outb (port + IER, ier);
	if (do_ttioctl)
		ttioctl (tp, com, vec);
	spl (s);
	if (do_asyparam)
		asyparam (tp);
.DE
.DM
	/* Things to be done after calling ttioctl(). */
	switch(com) {
	case TCSBRK:
		/* Send 0.25 second break:
		 * 1.  Turn on break level.
		 * 2.  Set timer to turn off break level 0.25 sec later.
		 * 3.  Sleep till timer expires.
		 * 4.  Turn off break level.
		 */
		outb (port + LCR, lcr | LC_SBRK);
		a1->a_brk = 1;
		timeout (& tp->t_sbrk, HZ / 4, endbrk, chan);
.DE
.DM
		while (a1->a_brk)
			x_sleep (a1, pritty, slpriNoSig, "asybreak");
.DE
.DM
		outb (port + LCR, lcr & ~ LC_SBRK);
	}
}
.DE
.SH "Turn Off the Break Level"
.PP
Function
.B endbrk()
turns off the break level.
Called from a timeout after the function
.BR "ioctl(fd, TCSBRK, 0)" .
.DM
void
endbrk(chan)
int chan;
{
	asy1_t	*a1 = asy1 + chan;
	a1->a_brk = 0;
	wakeup (a1);
}
.DE
.SH "Read Parameters"
.PP
Function
.B asyparam()
reads parameters from the port.
.DM
static void
asyparam(tp)
TTY * tp;
{
	int chan = (int)tp->t_ddp;
	asy0_t *a0 = asy0 + chan;
	asy1_t *a1 = asy1 + chan;
	short port = a0->a_port;
	int s;
	int write_baud=1, write_lcr=1;
	unsigned char mcr, newlcr, speed, oldSpeed;
	unsigned short cflag = tp->t_termio.c_cflag;
.DE
.DM
	speed = cflag & CBAUD;
	switch (cflag & CSIZE) {
	case CS5:  newlcr = LC_CS5;  break;
	case CS6:  newlcr = LC_CS6;  break;
	case CS7:  newlcr = LC_CS7;  break;
	case CS8:  newlcr = LC_CS8;  break;
	}
.DE
.DM
	if (cflag & CSTOPB)
		newlcr |= LC_STOPB;
	if (cflag & PARENB) {
		newlcr |= LC_PARENB;
		if ((cflag & PARODD) == 0)
			newlcr |= LC_PAREVEN;
	}
.DE
.DM
	/* Don't bang on the UART needlessly.
	 * Writing baud rate resets the port, which loses characters.
	 * You want this on first open, NOT on later opens.
	 */
	oldSpeed = a0->a_speed;
.DE
.DM
	if (speed == oldSpeed && tp->t_open) {
		write_baud = 0;
		if (newlcr == a1->a_lcr) {
			write_lcr = 0;
		}
	}
	a0->a_speed = speed;
	a1->a_lcr = newlcr;
.DE
.DM
	if (write_lcr) {
		char ier_save;
		s = sphi();
		ier_save = inb(port+IER);
.DE
.DM
		if (write_baud) {
			if (speed) {
				short divisor = albaud [speed];
.DE
.DM
				if (oldSpeed == 0) {
					/* if previous baud rate was zero,
					 * need to go off hook. */
					mcr = inb(port+MCR) | (MC_RTS | MC_DTR);
					outb(port+MCR, mcr);
				}
.DE
.DM
				outb(port+LCR, LC_DLAB);
				outb(port+DLL, divisor);
				outb(port+DLH, divisor >> 8);
			} else {
				/* Baud rate of zero means hang up. */
				mcr = inb(port+MCR) & ~(MC_RTS | MC_DTR);
				outb(port+MCR, mcr);
			}
		}
.DE
.DM
		outb(port+LCR, newlcr);
		if (a1->a_ut == US_16550A)
			outb(port+FCR, FC_ENABLE | FC_Rx_RST | FC_Rx_08);
		outb(port+IER, ier_save);
		spl(s);
	}
.DE
.DM
	if (write_baud)
		asyspr ();
}
.DE
.SH "Start Processing"
.PP
Function
.B asystart()
starts processing of data.
.DM
static void
asystart(tp)
TTY * tp;
{
	int chan = (int)tp->t_ddp;
	asy0_t *a0 = asy0 + chan;
	asy1_t *a1 = asy1 + chan;
	short port = a0->a_port;
	int s;
	int need_xmit = 1; /* True if should start sending data now. */
	silo_t *out_silo = &a1->a_out;
	char lsr;
.DE
.DM
	/* Read line status register AFTER disabling interrupts. */
	s = sphi ();
	lsr = inb (port + LSR);
.DE
.DM
	/* Process break indication.
	 * NOTE: Break indication cleared when line status register was read.
	 */
	if (lsr & LS_BREAK)
		defer (asybreak, chan);
.DE
.DM
	/* If no output data, it may be time to finish closing the port;
	 * but won't need another xmit interrupt.
	 */
	if (out_silo->si_ix == out_silo->si_ox) {
		if (need_wake[chan] & NW_OUTSILO) {
			need_wake[chan] &= ~NW_OUTSILO;
			wakeup((char *)out_silo);
		}
		need_xmit = 0;
	}
.DE
.DM
	/* Do nothing if output is stopped. */
	if (tp->t_flags & T_STOP)
		need_xmit = 0;
	if (a1->a_ohlt)
		need_xmit = 0;
.DE
.DM
	/* Start data transmission by writing to UART xmit reg. */
	if ((lsr & LS_TxRDY) && need_xmit) {
		int xmit_count;
		xmit_count = (a1->a_ut == US_16550A)?16:1;
		asy_send(out_silo, port+DREG, xmit_count);
	}
	spl(s);
}
.DE
.SH "The Poll Routine"
.PP
The kernel invokes function
.B asypoll()
when a user application invokes the system call
.B poll()
to poll a serial port.
.PP
Field
.B c_poll
in the
.B CON
structure contains a pointer to this function.
.DM
static int
asypoll(dev, ev, msec)
dev_t dev;
int ev;
int msec;
{
	int chan = channel(dev);
	asy1_t *a1 = asy1 + chan;
	TTY *tp = & a1->a_tty;
	return ttpoll(tp, ev, msec);
}
.DE
.SH "Wake Up Sleeping Devices"
.PP
Function
.B asycycle()
wakes up of any sleeping ports at regular intervals.
.DM
static void
asycycle(chan)
int chan;
{
	asy0_t *a0 = asy0 + chan;
	asy1_t *a1 = asy1 + chan;
	TTY *tp = &a1->a_tty;
	short port = a0->a_port;
	int s;
	char msr, mcr;
	silo_t *out_silo = &a1->a_out;
	silo_t *in_silo = &a1->a_in;
	int n, ch;
	int do_start = 1;
	unsigned char iir;
.DE
.DM
	/* Check Carrier Detect (RLSD).
	 * Modem status interrupts were not enabled due to 8250 hardware bug.
	 * Enabling modem status and receive interrupts may cause lockup
	 * on older parts.
	 */
	if (tp->t_flags & T_MODC) {
		/* Get status */
		msr = inb(port+MSR);
.DE
.DM
		/* Carrier changed. */
		if ((msr & MS_RLSD) && !(tp->t_flags & T_CARR)) {
			/* Carrier is on - wakeup open. */
			s = sphi();
			tp->t_flags |= T_CARR;
			spl(s);
			wakeup((char *)(&tp->t_open));
		}
.DE
.DM
		if (!(msr & MS_RLSD) && (tp->t_flags & T_CARR)) {
			s = sphi();
			RAWIN_FLUSH(in_silo);
			RAWOUT_FLUSH(out_silo);
			tp->t_flags &= ~T_CARR;
			spl(s);
			tthup(tp);
		}
	}
.DE
.DM
	/* Empty raw input buffer.
	 * The line discipline module (tty.c) will set T_ISTOP true when the
	 * tt input queue is nearly full (tp->t_iq.cq_cc >= IHILIM), and make
	 * T_ISTOP false when it's ready for more input.
	 * When T_ISTOP is true, ttin() simply discards the character passed.
	 */
	if (!(tp->t_flags & T_ISTOP)) {
		while (in_silo->SILO_CHAR_COUNT > 0) {
			s = sphi();
			ttin(tp, in_silo->si_buf[in_silo->si_ox]);
			if (in_silo->si_ox < MAX_SILO_INDEX)
				in_silo->si_ox++;
			else
				in_silo->si_ox = 0;
			in_silo->SILO_CHAR_COUNT--;
			spl(s);
		}
	}
.DE
.DM
	/* Hardware flow control.
	 *	Check CTS to see if we need to halt output.
	 *	(MS_INTR should have done this - repeat code here to be sure)
	 *	Check input silo to see if we need to raise RTS.
	 */
	if (tp->t_flags & T_CFLOW) {
		/* Get status */
		msr = inb(port+MSR);
		s = sphi();
		if (msr & MS_CTS)
			a1->a_ohlt = 0;
		else
			a1->a_ohlt = 1;
		spl(s);
.DE
.DM
		/* If using hardware flow control, see if we need to drop RTS. */
		if ((tp->t_flags & T_CFLOW)
		&& (in_silo->SILO_CHAR_COUNT > SILO_HIGH_MARK)) {
			s = sphi();
			mcr = inb(port+MCR);
			if (mcr & MC_RTS) {
				outb(port+MCR, mcr & ~MC_RTS);
			}
			spl(s);
		}
.DE
.DM
		/* If input silo below low mark, assert RTS */
		if (in_silo->SILO_CHAR_COUNT <= SILO_LOW_MARK) {
			s = sphi();
			mcr = inb(port+MCR);
.DE
.DM
			if ((mcr & MC_RTS) == 0) {
				outb(port+MCR, mcr | MC_RTS);
			}
			spl(s);
		}
	}
.DE
.DM
	/* Calculate free output slot count. */
	n = sizeof(out_silo->si_buf) - 1;
	n += out_silo->si_ox - out_silo->si_ix;
	n %= sizeof(out_silo->si_buf);
.DE
.DM
	/* Fill raw output buffer */
	for (;;) {
		if (--n < 0)
			break;
		s = sphi();
		ch = ttout(tp);
		spl(s);
		if (ch < 0)
			break;
.DE
.DM
		s = sphi();
		out_silo->si_buf[out_silo->si_ix] = ch;
		if (out_silo->si_ix >= sizeof(out_silo->si_buf) - 1)
			out_silo->si_ix = 0;
		else
			out_silo->si_ix++;
		spl(s);
	}
.DE
.DM
	/* if port has an interrupt pending (probably missed an irq)
	 *	the following two loops should not be merged
	 *	- need ALL port irq's inactive at once
	 *	for each port on this irq line (use irq1 for this)
	 *		disable interrupts (clear IER)
	 *	for each port on this irq line
	 *		restore interrupts
	 */
	if (a1->a_has_irq && ((iir = inb (port + IIR)) & 1) == 0) {
		struct irqnode *ip;
		asy_gp_t *gp;
		int s;
		short p;
		char c, slot;
.DE
.DM
		do_start = 0;
		s = sphi ();
		ip = irq1 [a0->a_irqno];
.DE
.DM
		while(ip) {
			if (ip->func == asyintr) {
				p = ip->arg;
				outb (p + IER, 0);
			} else {
				gp = asy_gp + ip->arg;
				for (slot = 0; slot < MAX_SLOTS; slot++) {
					if ((c = gp->chan_list [slot]) <
					    MAX_ASY) {
						p = asy0 [c].a_port;
						outb (p + IER, 0);
					}
				}
			}
			ip = ip->next_actv;
		}
.DE
.DM
		/* Now, all ports on the offending irq line have irq off. */
		ip = irq1 [a0->a_irqno];
		while (ip) {
			if (ip->func == asyintr) {
				p = ip->arg;
				outb (p + IER, IEN);
			} else {
				gp = asy_gp + ip->arg;
				for (slot = 0; slot < MAX_SLOTS; slot++) {
					if ((c = gp->chan_list [slot]) <
					    MAX_ASY){
						p = asy0 [c].a_port;
						outb (p + IER, IEN);
					}
				}
			}
			ip = ip->next_actv;
		}
		spl (s);
	}
.DE
.DM
	if (do_start)
		ttstart (tp);
.DE
.DM
	/* Schedule next cycle. */
	if (a1->a_in_use)
		timeout (& tp->t_rawtim, HZ / 10, asycycle, chan);
}
.DE
.SH "Suppress Interrupts During Chip Sensing"
.PP
Function
.B irqdummy()
suppresses interrupts that may occur during chip sensing.
.DM
static void
irqdummy()
{
	/* Try to clear all pending interrupts. */
	inb(dummy_port+IIR);
	inb(dummy_port+LSR);
	inb(dummy_port+MSR);
	inb(dummy_port+DREG);
}
.DE
.SH "Add a Port Information to IRQ0 List"
.PP
Function
.B add_irq()
adds information about a port to the
.B irq0
list.
.DM
static void
add_irq(irq, func, arg)
int irq;
int (*func)();
int arg;
{
	struct irqnode * np;
.DE
.DM
	/* Sanity check */
	if (irq <= 0 || irq >= NUM_IRQ)
		return;
.DE
.DM
	if (nextnode < MAX_ASY) {
		np = nodespace + nextnode++;
		np->func = func;
		np->arg = arg;
		np->next = irq0[irq];
		irq0[irq] = np;
	} else {
		printf("asy: too many irq nodes (%d)\en", nextnode);
	}
}
.DE
.SH "Service an Interrupt"
.PP
Function
.B asy_irq()
services an async interrupt.
.DM
static void
asy_irq (level)
int level;
{
	struct irqnode *ip = irq1 [level];
	int doit;
.DE
.DM
	do {
		struct irqnode * here = ip;
.DE
.DM
		doit = 0;
.DE
.DM
		while (here != NULL) {
			doit |= (* here->func) (here->arg);
			here = here->next_actv;
		}
	} while (doit);
}
.DE
.SH "Rebuild Links for Active Devices"
.PP
Function
.B upd_irq1()
rebuild the links for active devices.
.DM
static void
upd_irq1(irq)
int irq;
{
	struct irqnode *np;
	asy1_t *a1;
	int chan;
	int s;
.DE
.DM
	/* Sanity check */
	if (irq <= 0 || irq >= NUM_IRQ)
		return;
.DE
.DM
	/* For each node in the irq0 list
	 *	if node is for irq status port
	 *		for each channel using the status port
	 *			if channel in use, in irq mode
	 *				add node to irq1 list
	 *				skip rest of channels for this node
	 *	else - node is for simple UART
	 *		if channel in use, in irq mode
	 *			add node to irq1 list
	 */
.DE
.DM
	s = sphi();
	np = irq0[irq];
	irq1[irq] = 0;
	while (np) {
		if (np->func != asyintr) {
			char ix, loop = 1;
			asy_gp_t *gp = asy_gp + np->arg;
.DE
.DM
			for (ix = 0; ix < MAX_SLOTS && loop; ix++) {
				if ((chan = gp->chan_list[ix]) < MAX_ASY) {
					a1 = asy1 + chan;
					if (a1->a_in_use && a1->a_irq) {
						np->next_actv = irq1[irq];
						irq1[irq] = np;
						loop = 0;
					}
				}
			}
.DE
.DM
		} else {
			a1 = asy1 + np->arg;
			if (a1->a_in_use && a1->a_irq) {
				np->next_actv = irq1[irq];
				irq1[irq] = np;
			}
		}
		np = np->next;
	}
	spl(s);
}
.DE
.SH "The Break Routine"
.PP
Function
.B asybreak()
breaks connection with a port.
.DM
static void
asybreak(chan)
int chan;
{
	int s;
	asy1_t *a1 = asy1 + chan;
	silo_t *out_silo = &a1->a_out;
	silo_t *in_silo = &a1->a_in;
	TTY *tp = &a1->a_tty;
.DE
.DM
	s = sphi();
	RAWIN_FLUSH(in_silo);
	RAWOUT_FLUSH(out_silo);
	spl(s);
	ttsignal(tp, SIGINT);
}
.DE
.SH "Handle an Interrupt"
.PP
Function
.B asyintr()
handles an interrupt for a single channel.
.DM
static int
asyintr(chan)
int chan;
{
	asy0_t *a0 = asy0 + chan;
	asy1_t *a1 = asy1 + chan;
	TTY *tp = &a1->a_tty;
	silo_t *out_silo = &a1->a_out;
	silo_t *in_silo = &a1->a_in;
	int c, xmit_count;
	int ret = 0;
	short port = a0->a_port;
	unsigned char msr, lsr;
.DE
.DM
	if (chan >= MAX_ASY) {
		printf("asy: irq on channel %d\en", chan);
		return 0;
	}
.DE
.DM
rescan:
	switch (inb(port+IIR) & 0x07) {
	case LS_INTR:
		ret = 1;
		lsr = inb(port + LSR);
		if (lsr & LS_BREAK)
			defer(asybreak, chan);
		goto rescan;
.DE
.DM
	case Rx_INTR:
		ret = 1;
		c = inb(port+DREG);
		if (tp->t_open == 0)
			goto rescan;
.DE
.DM
		/* Must recognize XOFF quickly to avoid transmit overrun.
		 * Recognize XON here as well to avoid race conditions.
		 */
		if (_IS_IXON_MODE (tp)) {
			/* XON */
			if (_IS_START_CHAR (tp, c) ||
			    (_IS_IXANY_MODE (tp) &&
			     (tp->t_flags & T_STOP) != 0)) {
				tp->t_flags &= ~(T_STOP | T_XSTOP);
				goto rescan;
			}
.DE
.DM
			/* XOFF */
			if (_IS_STOP_CHAR (tp, c)) {
				tp->t_flags |= T_STOP;
				goto rescan;
			}
		}
.DE
.DM
		/* Save char in raw input buffer. */
		if (in_silo->SILO_CHAR_COUNT < MAX_SILO_CHARS) {
			in_silo->si_buf[in_silo->si_ix] = c;
			if (in_silo->si_ix < MAX_SILO_INDEX)
				in_silo->si_ix ++;
			else
				in_silo->si_ix = 0;
			in_silo->SILO_CHAR_COUNT ++;
		}
.DE
.DM
		/* If using hardware flow control, see if we need to drop RTS. */
		if ((tp->t_flags & T_CFLOW) != 0 &&
		    in_silo->SILO_CHAR_COUNT > SILO_HIGH_MARK) {
			unsigned char mcr = inb (port + MCR);
			if (mcr & MC_RTS) {
				outb(port+MCR, mcr & ~MC_RTS);
			}
		}
		goto rescan;
.DE
.DM
	case Tx_INTR:
		ret = 1;
		/* Do nothing if output is stopped. */
		if (tp->t_flags & T_STOP) {
			goto rescan;
		}
		if (a1->a_ohlt)
			goto rescan;
.DE
.DM
		/* Transmit next char in raw output buffer. */
		xmit_count = (a1->a_ut == US_16550A)?16:1;
		asy_send(out_silo, port+DREG, xmit_count);
		goto rescan;
.DE
.DM
	case MS_INTR:
		ret = 1;
		/* Get status (and clear interrupt). */
		msr = inb(port+MSR);
.DE
.DM
		/* Hardware flow control.
		 *	Check CTS to see if we need to halt output.
		 */
		if (tp->t_flags & T_CFLOW) {
			if (msr & MS_CTS)
				a1->a_ohlt = 0;
			else
				a1->a_ohlt = 1;
		}
		goto rescan;
.DE
.DM
	default:
		return ret;
	} /* endswitch */
}
.DE
.SH "Handle Timer Interrupts"
.PP
Function
.B asyclk()
is called every time
.B T0
interrupts.
If it returns zero,
.B asy
performs the usual system timer routines.
It polls all pollable ports.
.DM
static int
asyclk()
{
	static int count;
	int ix;
.DE
.DM
	for (ix = 0; ix < ppnum; ix++)
		asysph(pptbl[ix]);
	count++;
	if (count >= poll_divisor)
		count = 0;
	return count;
}
.DE
.SH "Set Polling Rate on a Port"
.PP
Function
.B asyspr()
sets the polling rate on a port.
.B asy
calls it
when a port is opened, closed, or changes speed.
It sets the polling rate only as fast as needed, and shuts off polling
whenever possible.
It updates the links in
.BR irq1[0] ,
which lists the polled-mode ports.
.DM
static void
asyspr()
{
	asy0_t *a0;
	asy1_t *a1;
	int chan;
	int s;
	int ix, max_rate, port_rate;
.DE
.DM
	/* Rebuild table of pollable ports. */
	s = sphi();
	ppnum = 0;
	for (chan = 0; chan < ASY_NUM; chan++) {
		a1 = asy1 + chan;
		if (a1->a_poll)
			pptbl[ppnum++] = chan;
	}
	spl(s);
.DE
.DM
	/* If another driver has the polling clock, do nothing. */
	if (poll_owner & ~ POLL_ASY)
		return;
.DE
.DM
	/* Find highest valid polling rate in units of HZ/10.
	 * If using FIFO chip, can poll at 1/16 the usual rate.
	 */
	max_rate = 0;
	for (ix = 0; ix < ppnum; ix++) {
		chan = pptbl[ix];
		a0 = asy0 + chan;
		a1 = asy1 + chan;
		port_rate = alp_rate[a0->a_speed];
		if (a1->a_ut == US_16550A) {
			port_rate /= 16;
			if (port_rate % HZ)
				port_rate += HZ - (port_rate % HZ);
		}
.DE
.DM
		if (max_rate < port_rate)
			max_rate = port_rate;
	}
.DE
.DM
	/* if max_rate is not current rate, adjust the system clock */
	if (max_rate != poll_rate) {
		poll_rate = max_rate;
		poll_divisor = poll_rate/HZ; /* used in asyclk() */
		altclk_out(); /* stop previous polling */
		poll_owner &= ~ POLL_ASY;
.DE
.DM
		if (poll_rate) { /* resume polling at new rate if needed */
			poll_owner |= POLL_ASY;
			altclk_in(poll_rate, asyclk);
		}
	}
}
.DE
.SH "Handle Polling"
.PP
Function
.B asysph()
handles the polling of serial ports.
.DM
static void
asysph(chan)
int chan;
{
	asy0_t *a0 = asy0 + chan;
	asy1_t *a1 = asy1 + chan;
	TTY *tp = &a1->a_tty;
	silo_t *out_silo = &a1->a_out;
	silo_t *in_silo = &a1->a_in;
	int c, xmit_count;
	short port = a0->a_port;
	char lsr;
.DE
.DM
	/* Check for received break first.
	 * This status is wiped out on reading the LSR.
	 */
	lsr = inb(port + LSR);
	if (lsr & LS_BREAK)
		defer(asybreak, chan);
.DE
.DM
	/* Handle all incoming characters. */
	for (;;) {
		lsr = inb(port + LSR);
		if ((lsr & LS_RxRDY) == 0)
			break;
		c = inb(port+DREG);
		if (tp->t_open == 0)
			continue;
.DE
.DM
		/* Must recognize XOFF quickly to avoid transmit overrun.
		 * Recognize XON here as well to avoid race conditions.
		 */
		if (_IS_IXON_MODE (tp)) {
			/* XOFF */
			if (_IS_STOP_CHAR (tp, c)) {
				tp->t_flags |= T_STOP;
				continue;
			}
.DE
.DM
			/* XON */
			if (_IS_START_CHAR (tp, c)) {
				tp->t_flags &= ~T_STOP;
				continue;
			}
		}
.DE
.DM
		/* Save char in raw input buffer. */
		if (in_silo->SILO_CHAR_COUNT < MAX_SILO_CHARS) {
			in_silo->si_buf[in_silo->si_ix] = c;
			if (in_silo->si_ix < MAX_SILO_INDEX)
				in_silo->si_ix++;
			else
				in_silo->si_ix = 0;
			in_silo->SILO_CHAR_COUNT++;
		}
.DE
.DM
		/* If using hardware flow control, see if we need to drop RTS. */
		if ((tp->t_flags & T_CFLOW)
		  && (in_silo->SILO_CHAR_COUNT > SILO_HIGH_MARK)) {
			unsigned char mcr = inb(port+MCR);
			if (mcr & MC_RTS) {
				outb(port+MCR, mcr & ~MC_RTS);
			}
		}
	}
.DE
.DM
	/* Handle outgoing characters. Do nothing if output is stopped. */
	lsr = inb(port + LSR);
	if ((lsr & LS_TxRDY)
	  && !(tp->t_flags & T_STOP)
	  && !(a1->a_ohlt)) {
		/* Transmit next char in raw output buffer. */
		xmit_count = (a1->a_ut == US_16550A)?16:1;
		asy_send(out_silo, port+DREG, xmit_count);
	}
.DE
.DM
	/* Hardware flow control.
	 *	Check CTS to see if we need to halt output.
	 */
	if (tp->t_flags & T_CFLOW) {
		if (inb(port+MSR) & MS_CTS)
			a1->a_ohlt = 0;
		else
			a1->a_ohlt = 1;
	}
}
.DE
.SH "Write to UART"
.PP
Function
.B asy_send()
write to the
.B xmit
data register of the UART.
Assume all checking about whether it's time to send has been done already.
This function is
called by time-critical IRQ and polling routines!
.PP
Argument
.I rawout
is the output silo for the TTY structure that supplies data to the port.
.I dreg
is the I/O address of the UART
.B xmit
data register.
.I xmit_count
is the maximum number of characters we can write (16 for FIFO parts).
.DM
static int
asy_send(rawout, dreg, xmit_count)
register silo_t *rawout;
int dreg, xmit_count;
{
	/* Transmit next chars in raw output buffer. */
	for (;(rawout->si_ix != rawout->si_ox) && xmit_count; xmit_count--) {
		outb(dreg, rawout->si_buf[rawout->si_ox]);
		/* Adjust raw output buffer output index. */
		if (++rawout->si_ox >= sizeof(rawout->si_buf))
			rawout->si_ox = 0;
	}
	return xmit_count;
}
.DE
.SH "Interrupt Handler for Comtrol-Type Port Groups"
.PP
Function
.B p1()
is the interrupt handler for Comtrol-type port groups.
The status register has one in bit positions for interrupting ports.
.DM
static int
p1(g)
int g;
{
	asy_gp_t *gp = asy_gp + g;
	short port = gp->stat_port;
	unsigned char status, index, chan;
	int safety = LOOP_LIMIT;
	int ret = 0;
.DE
.DM
	/* while any port is active
	 *	call simple interrupt handler for active channel
	 */
	while (status = inb(port)) {
		ret = 1;
		index = 0;
		if (status & 0xf0) {
			status &= 0xf0;
			index +=4;
		} else
			status &= 0x0f;
.DE
.DM
		if (status & 0xcc) {
			status &= 0xcc;
			index +=2;
		} else
			status &= 0x33;
.DE
.DM
		if (status & 0xaa)
			index++;
		chan = gp->chan_list[index];
		asyintr(chan);
.DE
.DM
		if (safety-- == 0) {
			printf("asy: p1 runaway - status %x\en", status);
			break;
		}
	}
	return ret;
}
.DE
.SH "Interrupt Handler for Arnet-Type Port Groups"
.PP
Function
.B p2()
is the interrupt handler for Arnet-type port groups.
The status register has zero in bit positions for interrupting ports.
.DM
static int
p2(g)
int g;
{
	asy_gp_t *gp = asy_gp + g;
	short port = gp->stat_port;
	unsigned char status, index, chan;
	int safety = LOOP_LIMIT;
	int ret = 0;
.DE
.DM
	/* while any port is active
	 *	call simple interrupt handler for active channel
	 */
	while (status = ~inb(port)) {
		ret = 1;
		index = 0;
		if (status & 0xf0) {
			status &= 0xf0;
			index +=4;
		} else
			status &= 0x0f;
.DE
.DM
		if (status & 0xcc) {
			status &= 0xcc;
			index +=2;
		} else
			status &= 0x33;
.DE
.DM
		if (status & 0xaa)
			index++;
		chan = gp->chan_list[index];
		asyintr(chan);
		if (safety-- == 0) {
			printf("asy: p2 runaway - status %x\en", status);
			break;
		}
	}
	return ret;
}
.DE
.SH "Interrupt Handler for GTEK-Type Port Groups"
.PP
Function
.B p3()
is the interrupt handler for GTEK-type port groups.
.DM
static int
p3(g)
int g;
{
	asy_gp_t *gp = asy_gp + g;
	short port = gp->stat_port;
	unsigned char index, chan;
.DE
.DM
	/* Call simple interrupt handler for active channel. */
	index = inb(port) & 7;
	chan = gp->chan_list[index];
	return asyintr(chan);
}
.DE
.SH "Interrupt Handler for DigiBoard-Type Port Groups"
.PP
Function
.B p4()
is the interrupt handler for DigiBoard-type port groups.
.DM
static int
p4(g)
int g;
{
	asy_gp_t *gp = asy_gp + g;
	short port = gp->stat_port;
	unsigned char index, chan;
	int ret = 0;
	int safety = LOOP_LIMIT;
.DE
.DM
	/* Status register has slot number for active port,
	 * or 0xFF if no port is active.
	 */
	for (;;) {
		index = inb(port);
		if (index == 0xFF)
			break;
.DE
.DM
		if (safety-- == 0) {
			printf("asy: p4 runaway - status %x\en", index);
			break;
		}
.DE
.DM
		ret = 1;
		chan = gp->chan_list[index&0xF];
		asyintr(chan);
	}
	return ret;
}
.DE
.SH "The CON Structure"
.PP
Finally, the
.B CON
structure
holds pointers to the driver's functions that are invoked by the kernel.
.DM
.ta 0.5i 2.5i
CON asycon = {
	DFCHR|DFPOL,	/* Flags */
	ASY_MAJOR,	/* Major index */
	asyopen,	/* Open */
	asyclose,	/* Close */
	NULL,	/* Block */
	asyread,	/* Read */
	asywrite,	/* Write */
	asyioctl,	/* Ioctl */
	NULL,	/* Powerfail */
	asytimer,	/* Timeout */
	asyload,	/* Load */
	asyunload,	/* Unload */
	asypoll	/* Poll */
};
.DE
.SH "Where To Go From Here"
.PP
The Lexicon describes the functions invoked within this driver.
The previous section gives an example of a driver for a block device.
