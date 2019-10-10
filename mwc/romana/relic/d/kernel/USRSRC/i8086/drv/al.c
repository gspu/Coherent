/* (-lgl
 * 	COHERENT Driver Kit Version 1.1.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 *
 * $Log:	al.c,v $
 * Revision 1.8  91/08/01  13:47:28  bin
 * updated by hal to include rts/cts handshaking.
 * 
 * Revision 1.4  91/07/31  16:06:33  hal
 * Change include usage.  Use AL[01]_MAJOR.
 * 
 * Revision 1.2	91/02/21  11:21:28	hal
 * Used in COH Release 3.1.0 - add COM3/COM4 and polling
 * 
 * Revision 1.1	91/02/21  11:07:36	hal
 * Used in COH Release 3.0.0 - no COM3/COM4
 * 
 -lgl) */
/*
 * Driver for an IBM PC asyncronous
 * line, using interrupts. The interface
 * uses a Natty/WD 8250 chip.
 */
#include <sys/coherent.h>
#include <sys/i8086.h>
#include <sys/con.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/tty.h>
#include <sys/uproc.h>
#include <sys/clist.h>
#include <sys/ins8250.h>
#include <sys/sched.h>
#include <sys/al.h>
#include <sys/devices.h>

#define	minor_st(dev)	(dev & 0x3f)
#define	DEV_TTY		(alttab[minor_st(dev)])
#define ALPORT		(((COM_DDP *)(DEV_TTY.t_ddp))->port)

/*
 * This driver can be compiled to drive any possible
 * async port by appropriate definitions of:
 *	ALPORT[ab]	the io port address(es)
 *	ALNUM[ab]	com index number (0..3 for com[1..4])
 *	ALINT	the interrupt level
 *	ALNAME	the xxcon name
 *	ALMAJ	the major device number
 *      ALCNT	number of ports sharing the interrupt
 *
 *	NOTE:	if ALCNT is changed, alttab and alintr will need hacking
 * Common code for the different ports is handled by alx.c
 */

#ifdef ALCOM1			/* COM1_3 definitions */
#define ALPORTa	0x3F8		/* Base of com1 port */
#define ALPORTb	0x3E8		/* Base of com3 port */
#define ALNUMa	0		/* com1 has com number of 0 */
#define ALNUMb	2		/* com3 has com number of 2 */
#define ALINT	4		/* Interrupt level of com1_3 ports */
#define	ALNAME	a0con		/* CON name of com1_3 ports */
#define ALMAJ	AL0_MAJOR	/* Major number of com1_3 port */
#define ALCNT	A0CNT		/* Number of ports for this IRQ */
#define ALSPEEDa C1BAUD		/* Name of patchable variable for com1 speed */
#define ALSPEEDb C3BAUD		/* Name of patchable variable for com3 speed */
#endif

#ifdef ALCOM2			/* COM2_4 definitions */
#define ALPORTa	0x2F8		/* Base of com2 port */
#define ALPORTb	0x2E8		/* Base of com4 port */
#define ALNUMa	1		/* com2 has com number of 1 */
#define ALNUMb	3		/* com4 has com number of 3 */
#define ALINT	3		/* Interrupt level of com2_4 ports */
#define ALNAME	a1con		/* CON name of com2_4 ports */
#define ALMAJ	AL1_MAJOR	/* Major number of com2_4 ports */
#define ALCNT	A1CNT		/* Number of ports for this IRQ */
#define ALSPEEDa C2BAUD		/* Name of patchable variable for com2 speed */
#define ALSPEEDb C4BAUD		/* Name of patchable variable for com4 speed */
#endif

/*
 * Functions.
 */
int	alxopen();
int	alxclose();
int	alxioctl();
int	alxtimer();
int	alxparam();
int	alxcycle();
int	alxstart();
int	alxbreak();

int	alintr();
int	alopen();
int	alclose();
int	alread();
int	alwrite();
int	alioctl();
int	alload();
int	alunload();
int	alpoll();
int	nulldev();
int	nonedev();

/*
 * Configuration table.
 */
CON ALNAME ={
	DFCHR|DFPOL,			/* Flags */
	ALMAJ,				/* Major index */
	alopen,				/* Open */
	alclose,			/* Close */
	nulldev,			/* Block */
	alread,				/* Read */
	alwrite,			/* Write */
	alioctl,			/* Ioctl */
	nulldev,			/* Powerfail */
	alxtimer,			/* Timeout */
	alload,				/* Load */
	alunload,			/* Unload */
	alpoll				/* Poll */
};

/*
 * Terminal structures.
 */
static COM_DDP	* ddp;
static TTY	* alttab;
static TTY	* irqtty;  /* point to alttab entry which is IRQ-enabled */

/*
 * to change default speeds - patch kernel variables C1BAUD..C4BAUD
 *   new value should be one of B0..B9600 in /usr/include/sgtty.h
 */
int ALSPEEDa = B9600;
int ALSPEEDb = B9600;

/*
 * to enable com[34], patch here
 *	A0CNT should be 2 if you want com3, 1 otherwise
 *	A1CNT should be 2 if you want com4, 1 otherwise
 */
int ALCNT = 2;

static
alload()
{
	register int s;
	static int init;
	extern int albaud[];
	int port, i;

	if (init == 0
	  && (alttab = (TTY *)kalloc(ALCNT * sizeof(TTY)))
	  && (ddp = (COM_DDP *)kalloc(ALCNT * sizeof(COM_DDP)))) {
		kclear(alttab, ALCNT*sizeof(TTY));
		kclear(ddp, ALCNT*sizeof(COM_DDP));
		s = sphi();
		++init;

		alttab[0].t_dispeed = alttab[0].t_dospeed = ALSPEEDa;
		alttab[0].t_ddp = (char *)&ddp[0];
		tp_table[ALNUMa] = alttab; /* set TTY pointers for polling */
		ddp[0].port = ALPORTa;
		ddp[0].com_num = ALNUMa;

		if (ALCNT > 1) {
			alttab[1].t_dispeed = alttab[1].t_dospeed = ALSPEEDb;
			alttab[1].t_ddp = (char *)&ddp[1];
			tp_table[ALNUMb] = alttab+1;
			ddp[1].port = ALPORTb;
			ddp[1].com_num = ALNUMb;
		}

		for (i = 0;  i < ALCNT; i++) {
			int speed = alttab[i].t_dospeed;

			/* port = base I/O address */
			port = ((COM_DDP *)(alttab[i].t_ddp))->port;
			outb(port+IER, 0);	/* disable port interrupts */
			if (inb(port+IER) == 0) {
				outb(port+MCR, 0);  /* hangup port */
				outb(port+LCR, LC_DLAB);
				outb(port+DLL, albaud[speed]);
				outb(port+DLH, albaud[speed] >> 8);
				outb(port+LCR, LC_CS8);
			}
			alttab[i].t_start = alxstart;
			alttab[i].t_param = alxparam;
			alttab[i].t_cs_sel= cs_sel();
		}

		setivec(ALINT, alintr);     /* set interrupt vector */
		spl(s);
	}
}

static
alunload()
{
	int port, i;

	for (i = 0;  i < ALCNT; i++) {
		port = ((COM_DDP *)(alttab[i].t_ddp))->port;
		outb(port+IER, 0);	/* disable port interrupts */
		outb(port+MCR, 0);	/* hangup port */
		timeout(alttab[i].t_rawtim, 0, NULL, 0);/* cancel timer */
	}
	clrivec(ALINT);			/* release interrupt vector */
	kfree(alttab);
	kfree(ddp);
}

static
alopen(dev, mode)
dev_t	dev;
int	mode;
{
	if (minor_st(dev) < ALCNT) {
		alload();
		alxcycle(&DEV_TTY);
		alxopen(dev, mode, &DEV_TTY, &irqtty);
	} else
		u.u_error = ENXIO;
}

static
alclose(dev, mode)
dev_t	dev;
int	mode;
{
	register int s;

	if (--DEV_TTY.t_open == 0) {	/* Last open */
		s = sphi();
		alxclose(dev, mode, &DEV_TTY);
		spl(s);
	}
}

static
alread(dev, iop)
dev_t	dev;
IO	*iop;
{
	ttread(&DEV_TTY, iop);
}

static
alwrite(dev, iop)
dev_t	dev;
register IO	*iop;
{
	register int c;

	/*
	 * Treat user writes through tty driver.
	 */
	if (iop->io_seg != IOSYS) {
		ttwrite(&DEV_TTY, iop);
		return;
	}

	/*
	 * Treat kernel writes by blocking on transmit buffer.
	 */
	while ((c = iogetc(iop)) >= 0) {
		/*
		 * Wait until transmit buffer is empty.
		 * Check twice to prevent critical race with interrupt handler.
		 */
		for (;;) {
			if (inb(ALPORT+LSR) & LS_TxRDY)
				if (inb(ALPORT+LSR) & LS_TxRDY)
					break;
		}

		/*
		 * Output the next character.
		 */
		outb(ALPORT+DREG, c);
	}
}

static
alioctl(dev, com, vec)
dev_t	dev;
struct sgttyb *vec;
{
	alxioctl(dev, com, vec, &DEV_TTY);
}

static
alpoll(dev, ev, msec)
dev_t dev;
int ev;
int msec;
{
	return ttpoll(&DEV_TTY, ev, msec);
}

static
alintr()
{
	alxintr(irqtty);
}
