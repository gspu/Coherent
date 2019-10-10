/* (-lgl
 * 	COHERENT Version 3.0
 *	Copyright 1989 by Inetco Systems, Ltd.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
#ifndef	 V7SGTTY_H
#define	 V7SGTTY_H	V7SGTTY_H

/*
 * Terminal ioctl.
 */

#include <sys/types.h>

/*
 * Ioctl functions.
 */
#define	TIOCSETP	0100		/* Terminal set modes (old stty) */
#define	TIOCGETP	0101		/* Terminal get modes (old gtty) */
#define	TIOCSETC	0102		/* Set characters */
#define	TIOCGETC	0103		/* Get characters */
#define	TIOCSETN	0104		/* No delay or output flushed */
#define	TIOCEXCL	0105		/* Set exclusive use */
#define	TIOCNXCL	0106		/* Set non exclusive use */
#define	TIOCHPCL	0107		/* Hang up on last close */
#define	TIOCFLUSH	0110		/* Flush all characters in queue */
#define	TIOCQUERY	0120		/* Test if any chars on input queue */

#define	TIOCOUTQ	0121		/* Number of chars in output queue */
#define	TIOCSTI		0122		/* Simulate a terminal in character */
#define	TIOCCBRK	0123		/* Clear break condition */
#define	TIOCSBRK	0124		/* Set break condition */

#define	TIOCGETF	0200		/* Get function keys (dev. dep.) */
#define	TIOCSETF	0201		/* Set function keys */

/*
 * Compatibility with gtty and stty.
 */
#define	stty(u,v)	ioctl(u,TIOCSETP,v)
#define	gtty(u,v)	ioctl(u,TIOCGETP,v)

/*
 * Structure for TIOCSETP/TIOCGETP
 */
struct sgttyb {
	char	sg_ispeed;		/* Input speed */
	char	sg_ospeed;		/* Output speed */
	char	sg_erase;		/* Character erase */
	char	sg_kill;		/* Line kill character */
	int	sg_flags;		/* Flags */
};

/*
 * Structure for TIOCSETC/TIOCGETC
 */
struct tchars {
	char	t_intrc;		/* Interrupt */
	char	t_quitc;		/* Quit */
	char	t_startc;		/* Start output */
	char	t_stopc;		/* Stop output */
	char	t_eofc;			/* End of file */
	char	t_brkc;			/* Input delimiter */
};

/*
 * Overlying structure for ioctl.
 */
union ioctl {
	struct	sgttyb io_sgttyb;
	struct	tchars io_tchars;
};

/*
 * Bits from old stty/gtty modes.
 */
#define	V7_EVENP	01		/* Allow even parity */
#define	V7_ODDP		02		/* Allow odd parity */
#define V7_CRMOD	04		/* Map '\r' to '\n' */
#define	V7_ECHO		010		/* Echo input characters */
#define	V7_LCASE	020		/* Lowercase mapping on input */
#define	V7_CBREAK	040		/* Each input character causes wakeup */
#define	V7_RAWIN	0100		/* 8-bit input raw */
#define	V7_RAWOUT	0200		/* 8-bit output raw */
#define	V7_TANDEM	0400		/* flow control protocol */
#define	V7_XTABS	01000		/* Expand tabs to spaces */
#define	V7_CRT		02000		/* CRT character erase */

#define	V7_CSIZE	070000		/* Character size mask */
#define	V7_CSV7		000000		/* V7 (7 bits + parity, 8 - parity) */
#define	V7_CS5		040000		/* 5 data bits */
#define	V7_CS6		050000		/* 6 data bits */
#define	V7_CS7		060000		/* 7 data bits */
#define	V7_CS8		070000		/* 8 data bits */

/*
 * Compatibility.
 */
#define	V7_RAW	(V7_RAWIN|V7_RAWOUT)	/* Raw mode */

/*
 * Names for terminal speeds.
 */
#define	V7_B0		0		/* Undefined */
#define	V7_B50		1		/* 50 bps */
#define	V7_B75		2		/* 75 bps */
#define	V7_B110		3		/* 110 bps */
#define	V7_B134		4		/* 134.5 bps (IBM 2741) */
#define	V7_B150		5		/* 150 bps */
#define	V7_B200		6		/* 200 bps */
#define	V7_B300		7		/* 300 bps */
#define	V7_B600		8		/* 600 bps */
#define	V7_B1200	9		/* 1200 bps */
#define	V7_B1800	10		/* 1800 bps */
#define	V7_B2000	11		/* 2000 bps */
#define	V7_B2400	12		/* 2400 bps */
#define	V7_B3600	13		/* 3600 bps */
#define	V7_B4800	14		/* 4800 bps */
#define	V7_B7200	15		/* 7200 bps */
#define	V7_B9600	16		/* 9600 bps */
#define	V7_B19200	17		/* 19200 bps */
#define	V7_EXTA		18		/* External A (DH-11) */
#define	V7_EXTB		19		/* External B (DH-11) */

#endif
