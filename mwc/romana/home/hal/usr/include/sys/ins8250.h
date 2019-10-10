/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef __SYS_INS8250_H__
#define	__SYS_INS8250_H__

/*
 * INS 8250 ACIA Register Definitions
 */

/*
 * Register Offsets
 */

#define	DREG		0	/* Data Register	       (DLAB=0)	*/
#define	IER		1	/* Interrupt Enable Register   (DLAB=0)	*/
#define	IIR		2	/* Interrupt Identification Register	*/
#define	FCR		2	/* FIFO Control Register		*/
#define	LCR		3	/* Line Control Register		*/
#define	MCR		4	/* Modem Control Register		*/
#define	LSR		5	/* Line Status Register			*/
#define	MSR		6	/* Modem Status Register		*/
#define	SCR		7	/* Scratch Register			*/

#define	DLL		0	/* Divisor Latch Low  Register (DLAB=1)	*/
#define	DLH		1	/* Divisor Latch High Register (DLAB=1)	*/


/*
 * Interrupt Enable Register (IER) - bits 4-7 zeroed
 */

#define	IE_RxI		0x01	/* Enable Received Data Available Intr.	*/
#define	IE_TxI		0x02	/* Enable Transmit Buf Empty Interrupt	*/
#define	IE_LSI		0x04	/* Enable Line Status Interrupt		*/
#define	IE_MSI		0x08	/* Enable Modem Status Interrupt	*/

#define	IENABLE	(IE_RxI | IE_TxI | IE_LSI | IE_MSI)


/*
 * Interrupt Identification Register (IIR)
 */

#define	Rt_INTR		0x0C	/* Received Char Timeout Interrupt	*/
#define	LS_INTR		0x06	/* Receiver Line Status Interrupt	*/
#define	Rx_INTR		0x04	/* Received Data Available Interrupt	*/
#define	Tx_INTR		0x02	/* Transmit Buf Empty Interrupt		*/
#define	MS_INTR		0x00	/* Modem Status Interrupt		*/


/*
 * FIFO Control Register (FCR)
 */

#define	FC_ENABLE	0x01	/* Enable on-chip FIFO (16550A only)	*/
#define	FC_Rx_RST	0x02	/* Reset RCVR FIFO			*/
#define	FC_Tx_RST	0x04	/* Reset XMIT FIFO			*/
#define	FC_Rx_01	0x00	/* Rx Irpt on 1 char in rcvr FIFO	*/
#define	FC_Rx_04	0x40	/* Rx Irpt on 4 char in rcvr FIFO	*/
#define	FC_Rx_08	0x80	/* Rx Irpt on 8 char in rcvr FIFO	*/
#define	FC_Rx_14	0xC0	/* Rx Irpt on 14 char in rcvr FIFO	*/


/*
 * Line Control Register (LCR)
 */

#define	LC_CSIZE	0x03	/* Character Size:			*/
#define	LC_CS5		0	/* 			5 bits		*/
#define	LC_CS6		0x01	/*			6 bits		*/
#define	LC_CS7		0x02	/*			7 bits		*/
#define	LC_CS8		0x03	/*			8 bits		*/

#define	LC_STOPB	0x04	/* 1 1/2 (CS5) or 2 (CS6-CS8) stop bits	*/
#define	LC_PARENB	0x08	/* Parity Enable			*/

#define	LC_PARITY	0x30	/* Parity Selection:			*/
#define	LC_PARODD	0	/*			Odd Parity	*/
#define	LC_PAREVEN	0x10	/*			Even Parity	*/
#define	LC_PARLO	0x20	/*			Low Stick	*/
#define	LC_PARHI	0x30	/*			High Stick	*/
#define	LC_SBRK		0x40	/* Set Break				*/
#define	LC_DLAB		0x80	/* Divisor Latch Access Bit (DLAB)	*/


/*
 * Modem Control Register (MCR) - bits 5..7 zeroed
 */

#define	MC_DTR		0x01	/* Data Terminal Ready			*/
#define	MC_RTS		0x02	/* Request to Send			*/
#define	MC_OUT1		0x04	/* Auxiliary user-designated output 1	*/
#define	MC_OUT2		0x08	/* Auxiliary user-designated output 2	*/
#define	MC_OUTS		0x0C	/* Both OUT signals			*/
#define	MC_LOOP		0x10	/* Loopback enable (diagnostic feature)	*/


/*
 * Line Status Register (LSR) - bit 7 zeroed
 */

#define	LS_RxRDY	0x01	/* Receiver Data Ready			*/
#define	LS_OVER		0x02	/* Overrun Error			*/
#define	LS_PARITY	0x04	/* Parity Error				*/
#define	LS_FRAME	0x08	/* Framing Error			*/
#define	LS_BREAK	0x10	/* Break Interrupt Indicator		*/
#define	LS_TxRDY	0x20	/* Transmitter Ready for Data		*/
#define	LS_TxIDLE	0x40	/* Transmitter Idle			*/


/*
 * Modem Status Register (MSR)
 */

#define	MS_DCTS		0x01	/* Delta Clear to Send			*/
#define	MS_DDSR		0x02	/* Delta Data Set Ready			*/
#define	MS_TERI		0x04	/* Trailing Edge of Ring Indicator	*/
#define	MS_DRLSD 	0x08	/* Delta Rx Line Signal Detect		*/
#define	MS_CTS		0x10	/* Clear To Send			*/
#define	MS_DSR		0x20	/* Data Set Ready			*/
#define	MS_RI		0x40	/* Ring Indicator			*/
#define	MS_RLSD		0x80	/* Receive Line Signal Detect		*/


/*
 * Uart-sense types.
 */

#define US_NONE		0
#define US_8250		1
#define US_16450	2
#define US_16550	3
#define US_16550A	4	/* The only one with usable on-chip FIFO */
#define US_UNKNOWN	-1

#endif	/* ! defined (__SYS_INS8250_H__) */
