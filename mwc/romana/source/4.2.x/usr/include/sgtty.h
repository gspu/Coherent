/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */
/*
 * Terminal ioctl.
 * This is an IBM-PC special-case file.  Many of these ioctls apply
 * only to the PC console and PC asynchronous ports.
 */

#ifndef	 __SGTTY_H__
#define	 __SGTTY_H__

/*
 * Ioctl functions.
 */

#define	__TIOC		('t' << 8)

#define	TIOCHPCL	(__TIOC | 2)	/* Hang up on last close */
#define	TIOCGETP	(__TIOC | 8)	/* Terminal get modes (old gtty) */
#define	TIOCSETP	(__TIOC | 9)	/* Terminal set modes (old stty) */
#define	TIOCSETN	(__TIOC | 10)	/* Set modes w/o delay or out flush */
#define	TIOCEXCL	(__TIOC | 13)	/* Set exclusive use */
#define	TIOCNXCL	(__TIOC | 14)	/* Set non-exclusive use */
#define TIOCFLUSH	(__TIOC | 16)	/* Flush characters in I/O queues */
#define	TIOCSETC	(__TIOC | 17)	/* Set characters */
#define	TIOCGETC	(__TIOC | 18)	/* Get characters */


/*
 * Structure for TIOVGETB/TIOVPUTB
 */

struct vidctl {
	short	v_position;		/* Position in video memory */
	short	v_count;		/* Number of characters to transfer */
	short	v_buffer;		/* Character buffer to read/write */
};


/*
 * stty/gtty modes.
 */

#define	XTABS	0x0002		/* Expand tabs to spaces */
#define	LCASE	0x0004		/* Lowercase mapping on input */
#define	ECHO	0x0008		/* Echo input characters */
#define CRMOD	0x0010		/* Map '\r' to '\n' */
#define	RAW	0x0020
#define	ODDP	0x0040		/* Allow odd parity */
#define	EVENP	0x0080		/* Allow even parity */
#define	ANYP	0x00C0		/* Allow any parity */


/*
 * The following are not part of S5 sgtty.
 * They are here to allow compilation of old V7 sources.
 */

#define	CBREAK	0x010000	/* Each input character causes wakeup */
#define	TANDEM	0x020000	/* flow control protocol */
#define	CRT	0x040000	/* CRT character erase */
#define RAWIN	0x080000
#define RAWOUT	0x100000


/*
 * Names for terminal speeds.
 */

#define	B0	0		/* Hangup if modem control enabled */
#define	B50	1		/* 50 bps */
#define	B75	2		/* 75 bps */
#define	B110	3		/* 110 bps */
#define	B134	4		/* 134.5 bps (IBM 2741) */
#define	B150	5		/* 150 bps */
#define	B200	6		/* 200 bps */
#define	B300	7		/* 300 bps */
#define	B600	8		/* 600 bps */
#define	B1200	9		/* 1200 bps */
#define	B1800	10		/* 1800 bps */
#define	B2400	11		/* 2400 bps */
#define	B4800	12		/* 4800 bps */
#define	B9600	13		/* 9600 bps */
#define	B19200	14		/* 19200 bps */
#define	B38400	15
#define	EXTA	14
#define	EXTB	15


/*
 * Common definitions for 286/386 versions
 */

#define TIOCCHPCL	0111		/* Don't hang up on last close */
#define TIOCGETTF	0112		/* Get tty flag word */
#define TIOCBREAD	0113		/* Blocking read in CBREAK/RAW mode */
#define TIOCCBREAD	0114		/* Turn off TIOCBREAD */
#define TIOCSHIFT	0115		/* Switch console left-SHIFT & "\" */
#define TIOCCSHIFT	0116		/* Normal console left-SHIFT & "\" */

#define	TIOCQUERY	0120		/* No. of chars waiting for read */

#define	TIOCSETG	0130		/* Set tty process group */

#define	TIOCGETF	0200		/* Get function keys  (dev. dep.) */
#define	TIOCSETF	0201		/* Set function keys  (dev. dep.) */
#define	TIOCGETKBT	0202		/* Get keyboard table (dev. dep.) */
#define	TIOCSETKBT	0203		/* Set keyboard table (dev. dep.) */

/* misc. functions */

#define TIOCSDTR	0301		/* Set DTR */
#define TIOCCDTR	0302		/* Clear DTR */
#define TIOCSRTS	0303		/* Set RTS */
#define TIOCCRTS	0304		/* Clear RTS */
#define TIOCSBRK	0305		/* Set BREAK */
#define TIOCCBRK	0306		/* Clear BREAK */

#define TIOCRSPEED	0320		/* Set "raw" line I/O speed divisor */
#define TIOCWORDL	0321		/* Set line word length and stop bits */

#define TIOCRMSR	0400		/* Get CTS/DSR/RI/RLSD (MSR) */

#define TIOVGETB	0500		/* Read video memory */
#define TIOVPUTB	0501		/* Write video memory */


/*
 * Bits for TIOCRMSR.
 */

#define MSRCTS		1		/* Clear to Send */
#define MSRDSR		2		/* Data Set Ready */
#define MSRRI		4		/* Ring Indicator */
#define MSRRLSD		8		/* Received Line Signal Detect */


/*
 * Attribute masks for TIOVPUTB - attributes occupy odd addresses
 * in video memory.
 */

#define	VNORM	0x07			/* Ordinary Video */
#define	VINTE	0x08			/* Intense video */
#define	VBLIN	0x80			/* Blinking video */
#define	VREVE	0x70			/* Reverse video */
#define	VUNDE	0x01			/* Underline video (mono board) */


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

#endif	/* ! defined (__SGTTY_H__) */
