/*
 * This is a driver for PC parallel printers.
 * It has been tested on an EPSON MX-80, Printronix P300, HP LaserJet II.
 * Supports up to three line printers.
 */
#include <sys/coherent.h>
#include <sys/reg.h>
#include <sys/con.h>
#include <sys/devices.h>
#include <errno.h>
#include <sys/io.h>
#include <sys/proc.h>
#include <sys/sched.h>
#include <sys/stat.h>

/*
 * Patchable parameters.
 *
 *	LP0_OK specifies whether LP0 is always THERE.
 *	LPTIME specifies number of ticks between polls.
 *	LPWAIT specifies loop counter to wait in poll.
 *	LPTEST specifies whether or not to test for on-line conditition.
 */
int	LP0_OK = 0;
int	LPTIME = 4;
int	LPWAIT = 400;
int	LPTEST = 1;

/*
 * Driver configuration.
 */
int	lpload();
int	lpunload();
int	lpwrite();
int	lpopen();
int	lpclose();
int	lpintr();
int	nulldev();
int	nonedev();

CON	lpcon =	{
	DFCHR,				/* Flags */
	LP_MAJOR,				/* Major index */
	lpopen,				/* Open */
	lpclose,			/* Close */
	nulldev,			/* Block */
	nonedev,			/* Read */
	lpwrite,			/* Write */
	nonedev,			/* Ioctl */
	nulldev,			/* Powerfail */
	nulldev,			/* Timeout */
	lpload,				/* Load */
	lpunload			/* Unload */
};

/*
 * Line Printer Registers.
 */
#define	LPDAT	(0)			/* Data port, lpbase + 0 */
#define	LPSTR	(1)			/* Status port, lpbase + 1 */
#define	LPCSR	(2)			/* Control port, lpbase + 2 */

/*
 * LP Flag Bits.
 */
#define	LPTHERE	0x01			/* Interface actually there */
#define	LPOPEN	0x02			/* Printer is open */
#define	LPSLEEP	0x04			/* Sleeping on buffer event */
#define	LPRAW	0x80			/* Raw mode */

/*
 * Printer database.
 * Terminated by lpbase = 0.
 * NLP = # entries - 1.
 */
static struct	lpinfo	{
	int	lpbase;			/* I/O Base address */
	int	lpflag;			/* Flags */
	int	lpcol;			/* Current horizontal position */
}	lpinfo[] = {
	{	0x3BC	},
	{	0x378	},
	{	0x278	},
	{	0x000	}
};
#define	NLP	(sizeof(lpinfo) / sizeof(lpinfo[0]) - 1)

/*
 * LP Status Register Bits.
 */
#define	ACK	0x80			/* Ack (active high) */
#define	BUSY	0x40			/* Busy (active high) */
#define	NOPAPER	0x20			/* No paper */
#define	ONLINE	0x10			/* On line */
#define	NERROR	0x08			/* Error (active low) */

/* IBM cable */
#define	IBMNBSY	0x80			/* Busy (active low) */
#define	IBMNACK	0x40			/* Ack (active low) */

/*
 * LP Control Register Bits.
 */
#define	IENABLE	0x10			/* Interrupt enable */
#define	SEL	0x08			/* Select input */
#define	NINIT	0x04			/* Initialise printer (active low) */
#define	AFEED	0x02			/* Auto line feed */
#define	STROBE	0x01			/* Strobe */

/*
 * On load
 * compute the port addresses,
 * reset the printer, and select it.
 */
static
lpload()
{
	register struct lpinfo * p;
	register int delay;
	static int notfirst;

	/*
	 * Only initialize hardware on first invocation.
	 * Necessary if used as console device [condev].
	 */
	if (notfirst)
		return;
	notfirst = 1;

	/*
	 * Note: since some PC clones lp ports can't be read,
	 * their lpflag field has to be patched to 'LPTHERE'.
	 */
	if (LP0_OK & 1)
		lpinfo[0].lpflag |= LPTHERE;
	if (LP0_OK & 2)
		lpinfo[1].lpflag |= LPTHERE;
	if (LP0_OK & 4)
		lpinfo[2].lpflag |= LPTHERE;

	for (p = lpinfo; p->lpbase ; ++p) {

		/*
		 * Check printer port existence.
		 */
		if ((p->lpflag & LPTHERE) == 0) {
			outb(p->lpbase+LPDAT, 0xA5);
			delay = LPWAIT; do {
			} while (--delay);
			if (inb(p->lpbase+LPDAT) == 0xA5)
				p->lpflag |= LPTHERE;
		}

		/*
		 * Initialize and select printer.
		 */
		outb(p->lpbase+LPCSR, SEL);
		delay = LPWAIT; do {
		} while (--delay);
		outb(p->lpbase+LPCSR, SEL|NINIT);
	}
}

/*
 * On unload
 * cancel any timed functions.
 */
static
lpunload()
{
	lptimer();
}

/*
 * The open routine makes sure that
 * only one process has the printer open
 * at one time, and not too much else.
 */
static
lpopen(dev, mode)
dev_t	dev;
{
	register struct lpinfo * p;

	/*
	 * Illegal printer port.
	 */
	if ((minor(dev) & ~LPRAW) >= NLP) {
		u.u_error = ENXIO;
		return;
	}

	/*
	 * Access attributes.
	 */
	p = &lpinfo[ minor(dev) & ~LPRAW ];

	/*
	 * Attempt initialization if printer port not found.
	 */
	if ((p->lpflag&LPTHERE) == 0)
		lpload();

	/*
	 * Printer port not found.
	 */
	if ((p->lpflag&LPTHERE) == 0) {
		u.u_error = ENXIO;
		return;
	}

	/*
	 * Printer port already open.
	 */
	if ((p->lpflag&LPOPEN) != 0) {
		u.u_error = EBUSY;
		return;
	}

	/*
	 * Printer powered off or off-line
	 */
	if (LPTEST && !(inb(p->lpbase+LPSTR) & ONLINE)) {
		u.u_error = EIO;
		return;
	}

	/*
	 * Flag port as being open.
	 */
	p->lpflag &= ~LPRAW;
	p->lpflag |= LPOPEN | minor(dev) & LPRAW;

	/*
	 * Initiate periodic printer scan if user open.
	 */
	if ((SELF != NULL) && (SELF->p_pid != 0))
		lptimer();
}

/*
 * The close routine marks the device as no longer open.
 */
static
lpclose(dev)
dev_t	dev;
{
	lpinfo[ minor(dev) & ~LPRAW ].lpflag &= ~LPOPEN;
}

/*
 * The write routine copies the
 * characters from the user buffer to
 * the printer buffer, expanding tabs and
 * keeping track of the current horizontal
 * position of the print head.
 */
static
lpwrite(dev, iop)
dev_t	dev;
IO	*iop;
{
	register struct lpinfo * p;
	register int	c;

	p = &lpinfo[ minor(dev) & ~LPRAW ];

	/*
	 * Writes from kernel are handled via busy-waits instead of timeouts.
	 */
	if (iop->io_seg == IOSYS) {

		while ((c=iogetc(iop)) >= 0) {

			while ((inb(p->lpbase+LPSTR) & IBMNBSY) == 0)
				;

			outb(p->lpbase+LPDAT, c);
			outb(p->lpbase+LPCSR, SEL|NINIT|STROBE);
			outb(p->lpbase+LPCSR, SEL|NINIT);
		}
		return;
	}

	/*
	 * Writes from user are handled via lpchar() which uses timeouts.
	 */
	while ((c=iogetc(iop)) >= 0) {

		if ((p->lpflag&LPRAW) == 0) {

			switch (c) {

			case '\t':
				do {
					lpchar(p, ' ');
				} while ((++p->lpcol&07) != 0);
				continue;
	
			case '\n':
				lpchar(p, '\r');
				/* no break */

			case '\r':
			case '\f':
				p->lpcol = 0;
				break;
	
			case '\b':
				--p->lpcol;
				break;
	
			default:
				++p->lpcol;
			}
		}
		lpchar(p, c);
	}
}

/*
 * Put a character into the printer buffer.
 * If the printer doesn't respond ready in a reasonable time
 * sleep for a while.
 */
static
lpchar(p, c)
register struct lpinfo *p;
int c;
{
	register int	waitCount;
	register int	s;

	waitCount = LPWAIT;
	while ((inb(p->lpbase+LPSTR) & IBMNBSY) == 0) {
		if (--waitCount == 0) {
			s = sphi();
			p->lpflag |= LPSLEEP;
			x_sleep((char *)p, pritty, slpriSigLjmp, "lpchar");
			spl(s);
			waitCount = LPWAIT;
		}
	}

	outb(p->lpbase+LPDAT, c);
	outb(p->lpbase+LPCSR, SEL|NINIT|STROBE);
	outb(p->lpbase+LPCSR, SEL|NINIT);
}

/*
 * Poll the line printer interface from the clock.
 * Turn it off when there is nothing left to do.
 */
static
lptimer()
{
	register struct lpinfo *p;
	int isopen = 0;
	static TIM tim;

	/*
	 * Scan all printers.
	 */
	for (p = lpinfo; p->lpbase; ++p) {

		/*
		 * Ignore unopened printers.
		 */
		if ((p->lpflag & LPOPEN) == 0)
			continue;

		++isopen;

		/*
		 * Check for sleeping process on ready printer.
		 */
		if((p->lpflag & LPSLEEP) && (inb(p->lpbase+LPSTR) & IBMNBSY)){
			p->lpflag &= ~LPSLEEP;
			wakeup((char *)p);
		}
	}

	/*
	 * Reschedule timer function if at least 1 printer is still open.
	 */
	if (isopen)
		timeout(&tim, LPTIME, lptimer, &tim);
}
