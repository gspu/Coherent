/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * TTY flag defines
 */

#ifndef	TTY_H
#define	TTY_H	TTY_H

#define	T_HILIM	0x0001		/* Hi water mark wait */
#define	T_DRAIN	0x0002		/* Drain wait */
#define	T_INPUT	0x0004		/* Input wait */
#define T_IFULL	0x0008		/* Input buffer full */
#define	T_INL	0x0010		/* Insert newline */
#define	T_STOP	0x0020		/* Stopped */
#define	T_HPCL	0x0040		/* Hang up dataset on last close */
#define	T_EXCL	0x0080		/* Exclusive use */
#define	T_TSTOP	0x0100		/* Tandem input stop */
#define	T_ISTOP	0x0200		/* Input overflow stop */
#define T_MODC	0x0400		/* Modem control */
#define T_CARR	0x0800		/* Carrier detect status */
#define	T_BRD	0x1000		/* Blocking read in CBREAK/RAW mode */
#define	T_HOPEN	0x2000		/* Hanging in open (for modem control) */
#define	T_HCLOS	0x4000		/* Hanging in close (for modem control) */
#define	T_CFLOW	0x8000		/* Use RTS/CTS flow control */

/* don't reset these flags when flushing the input and output queues */
#define T_SAVE	 (T_HPCL|T_EXCL|T_MODC|T_CARR|T_HOPEN|T_BRD|T_HCLOS|T_CFLOW)

#define NMODC	0x80		/* Minor device modem control bit */
				/* Set for NO modem control       */
#define CPOLL	0x40		/* Minor device polled driver bit */
				/* Set for polled operation       */
				/* Clear for IRQ operation        */
#define CFLOW	0x20		/* Minor device flow control bit  */
				/* Set for RTS/CTS flow control   */
				/* Clear for none                 */

#ifdef KERNEL
#include <sys/ktty.h>
#endif

#endif
