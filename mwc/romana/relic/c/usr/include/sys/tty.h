/* (-lgl
 * 	COHERENT Driver Kit Version 1.1.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * TTY flag defines
 */

#ifndef	TTY_H
#define	TTY_H	TTY_H

#define	T_HILIM	01		/* Hi water mark wait */
#define	T_DRAIN	02		/* Drain wait */
#define	T_INPUT	04		/* Input wait */
#define T_IFULL 010		/* Input buffer full */
#define	T_INL	020		/* Insert newline */
#define	T_STOP	040		/* Stopped */
#define	T_HPCL	0100		/* Hang up dataset on last close */
#define	T_EXCL	0200		/* Exclusive use */
#define	T_TSTOP	0400		/* Tandem input stop */
#define	T_ISTOP	01000		/* Input overflow stop */
#define T_MODC  02000		/* Modem control */
#define T_CARR	04000		/* Carrier detect status */
#define	T_BRD	010000		/* Blocking read in CBREAK/RAW mode */
#define	T_HOPEN	020000		/* Hanging in open (for modem control) */
#define	T_UN1	040000		/* Unused bit 1 */
#define	T_UN0	0100000		/* Unused bit 0 */

/* don't reset these flags when flushing the input and output queues */
#define T_SAVE	 (T_HPCL|T_EXCL|T_MODC|T_CARR|T_HOPEN|T_BRD)

#define NMODC	0x80		/* Minor device modem control bit */
				/* Set for NO modem control       */
#define CPOLL	0x40		/* Minor device polled driver bit */
				/* Set for polled operation       */
				/* Clear for IRQ operation        */

#ifdef KERNEL
#include <sys/ktty.h>
#endif

#endif
