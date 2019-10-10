/* (-lgl
 * 	COHERENT Version 3.0
 *	Copyright 1989 by Inetco Systems, Ltd.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
#ifndef TERMIO_H
#define TERMIO_H

/*
 *	Terminal Input/Output Parameters
 */

#define	NCC	8
typedef
struct	termio {
	unsigned short	c_iflag;	/* input modes */
	unsigned short	c_oflag;	/* output modes */
	unsigned short	c_cflag;	/* control modes */
	unsigned short	c_lflag;	/* local modes */
	         char	c_line;		/* line discipline */
	unsigned char	c_cc[NCC];	/* control chars */
} termio_t;

/*
 * Basic Terminal Input Control: c_iflag
 */
#define	IGNBRK	0000001		/* Ignore break condition */
#define	BRKINT	0000002		/* Signal interrupt on break */
#define	IGNPAR	0000004		/* Ignore characters with parity errors */
#define	PARMRK	0000010		/* Mark parity errors */
#define	INPCK	0000020		/* Enable input parity check */
#define	ISTRIP	0000040		/* Strip character */
#define	INLCR	0000100		/* Map NL to CR on input */
#define	IGNCR	0000200		/* Ignore CR */
#define	ICRNL	0000400		/* Map CR to NL on input */
#define	IUCLC	0001000		/* Map upper-case to lower-case on input */
#define	IXON	0002000		/* Enable start/stop output control */
#define	IXANY	0004000		/* Enable any character to restart output */
#define	IXOFF	0010000		/* Enable start/stop input control */

/*
 * Basic Terminal Output Control: c_oflag
 */
#define	OPOST	0000001		/* Postprocess output */
#define	OLCUC	0000002		/* Map lower case to upper on output */
#define	ONLCR	0000004		/* Map NL to CR-NL on output */
#define	OCRNL	0000010		/* Map CR to NL on output */
#define	ONOCR	0000020		/* No CR output at column 0 */
#define	ONLRET	0000040		/* NL performs CR function */
#define	OFILL	0000100		/* Use fill characters for delay */
#define	OFDEL	0000200		/* Fill is DEL, else NUL */
#define	NLDLY	0000400		/* Select new-line delays: */
#define	NL0	0
#define	NL1	0000400
#define	CRDLY	0003000		/* Select carriage-return delays: */
#define	CR0	0
#define	CR1	0001000
#define	CR2	0002000
#define	CR3	0003000
#define	TABDLY	0014000		/* Select horizontal-tab delays: */
#define	TAB0	0
#define	TAB1	0004000
#define	TAB2	0010000
#define	TAB3	0014000		
#define	XTABS	TAB3		/* Expand tabs to spaces */
#define	BSDLY	0020000		/* Select backspace delays: */
#define	BS0	0
#define	BS1	0020000
#define	VTDLY	0040000		/* Select vertical-tab delays: */
#define	VT0	0
#define	VT1	0040000
#define	FFDLY	0100000		/* Select form-feed delays: */
#define	FF0	0
#define	FF1	0100000

/*
 * Terminal Hardware Control: c_cflag
 */
#define	CBAUD	0000017		/* Baud rate: */
#define	B0	0		/* Hang up */
#define	B50	0000001		/* 50 baud */
#define	B75	0000002		/* 75 baud */
#define	B110	0000003		/* 110 baud */
#define	B134	0000004		/* 134.5 baud */
#define	B150	0000005		/* 150 baud */
#define	B200	0000006		/* 200 baud */
#define	B300	0000007		/* 300 baud */
#define	B600	0000010		/* 600 baud */
#define	B1200	0000011		/* 1200 baud */
#define	B1800	0000012		/* 1800 baud */
#define	B2400	0000013		/* 2400 baud */
#define	B4800	0000014		/* 4800 baud */
#define	B9600	0000015		/* 9600 baud */
#define	B19200	0000016		/* 19200 baud */
#define	B38400	0000017		/* 38400 baud */
#define	EXTA	0000016		/* External A */
#define	EXTB	0000017		/* External B */
#define	CSIZE	0000060		/* Character size: */
#define	CS5	0		/* 5 bits */
#define	CS6	0000020		/* 6 bits */
#define	CS7	0000040		/* 7 bits */
#define	CS8	0000060		/* 8 bits */
#define	CSTOPB	0000100		/* Send two stop bits, else one */
#define	CREAD	0000200		/* Enable receiver */
#define	PARENB	0000400		/* Parity enable */
#define	PARODD	0001000		/* Odd parity, else even */
#define	HUPCL	0002000		/* Hang up on last close */
#define	CLOCAL	0004000		/* Local line, else dial-up */

/*
 * Terminal Functions - c_lflag
 */
#define	ISIG	0000001		/* Enable signals */
#define	ICANON	0000002		/* Canonical input (erase and kill) */
#define	XCASE	0000004		/* Canonical upper/lower presentation */
#define	ECHO	0000010		/* Enable echo */
#define	ECHOE	0000020		/* Echo erase character as BS-SP-BS */
#define	ECHOK	0000040		/* Echo NL after kill character */
#define	ECHONL	0000100		/* Echo NL */
#define	NOFLSH	0000200		/* Disable flush after interrupt or quit */

/*
 * Offsets into Control Characters
 */
#define	VINTR	0
#define	VQUIT	1
#define	VERASE	2
#define	VKILL	3
#define	VEOF	4
#define	VEOL	5
#define	VEOL2	6
#define	VMIN	4
#define	VTIME	5

/*
 * Ioctl Commands - ioctl(fno, com, &termio)
 */

#define	TIOC	('T'<<8)
#define	TCGETA	(TIOC|1)	/* Get terminal parameters */
#define	TCSETA	(TIOC|2)	/* Set terminal parameters */
#define	TCSETAW	(TIOC|3)	/* Wait for drain, then set parameters */
#define	TCSETAF	(TIOC|4)	/* Wait for drain, flush input, set parms */

/*
 * Ioctl Commands - ioctl(fno, com, arg)
 */
#define	TCSBRK	(TIOC|5)	/* Send 0.25 second break */
#define	TCXONC	(TIOC|6)	/* Start/stop control
				   arg=0 -> suspend output
				   arg=1 -> restart suspended output
				 */
#define	TCFLSH	(TIOC|7)	/* arg=0 -> flush input queue
				   arg=1 -> flush output queue
				   arg=2 -> flush both input and output queues
				 */
#define	TCDTR	(TIOC|8)	/* arg=0 -> drop dtr (carrier)
				   arg=1 -> raise dtr
				 */
#define	TCDSET	(TIOC|32)

#endif
