/* (-lgl
 * 	COHERENT Driver Kit Version 2.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Machine dependent stuff.
 * 8086/8088 & 286 Coherent PC/XT/AT.
 */
#ifndef	I8086_H
#define	I8086_H

/*
 * Useful definitions.
 */
#define	PIC	0x20			/* 8259 command port */
#define	PICM	0x21			/* 8259 mask port */
#define	SPIC	0xA0			/* Slave 8259 command port */
#define	SPICM	0xA1			/* Slave 8259 mask port */
#define	MFTTB	0x0100			/* Trace trap bit */
#define	MFINT	0x0200			/* Interupt enable */
#define	MUERR	0x0002			/* Location of errno */

/*
 * Trap codes.
 * Passed in the upper 8 bits of
 * the "id" passed to "trap".
 */
#define	SIDIV	0			/* Divide overflow */
#define SISST	1			/* Single step */
#define	SINMI	2			/* NMI (parity) */
#define	SIBPT	3			/* Breakpoint */
#define	SIOVF	4			/* Overflow */
#define	SIBND	5			/* Bound */
#define	SIOP	6			/* Invalid opcode */
#define	SIXNP	7			/* Processor extension not available */
#define	SIDBL	8			/* Double exception */
#define	SIXS	9			/* Processor extension segment overrun */
#define	SITS	10			/* Invalid task state segment */
#define	SINP	11			/* Segment not present */
#define	SISS	12			/* Stack segment overrun/not present */
#define	SIGP	13			/* General protection */
#define	SISYS	32			/* System call */
#define	SIRAN	33			/* Random interrupt */
#define	SIDEV	64			/* Device interrupt */

/*
 * The saved registers are accessed
 * via constant offsets from the top of the
 * user area stack. The "O" symbols, defined below,
 * are the offsets, in words, from the initial system
 * stack. The offsets depend on the actual save order
 * defined by "tsave" in the assist.
 */
#define	regl	((int *)((char *)&u+UPASIZE))

#define	OFW	-1
#define	OCS	-2
#define	OIP	-3
#define	OID	-4
#define	OSS	-5
#define	OSP	-6
#define	ODS	-7
#define	OBX	-8
#define	OAX	-9
#define	ODX	-10
#define	OCX	-11
#define	OES	-12
#define	ORA	-13
#define	OSI	-14
#define	ODI	-15
#define	OBP	-16

/*
 * For accessing high and low words of a long.
 */
struct l {
	int	l_lo;
	int	l_hi;
};

#ifdef KERNEL
/*
 * Global variables.
 */
extern	unsigned ucs;			/* User code segment */
extern	unsigned ucl;			/* Length of code segment */
extern	unsigned uds;			/* User data segment */
extern	unsigned udl;			/* Length of data segment */
extern	char	 depth;			/* Stack depth */

#endif

#endif
