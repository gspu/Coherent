/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
#ifndef	ASCII_H
#define	ASCII_H

/*
 * Ascii Macro Definitions.
 */

#define A_NUL	0x00	/* Ctrl-@ Null			*/
#define A_SOH	0x01	/* Ctrl-a Start Of Header	*/
#define A_STX	0x02	/* Ctrl-b Start Of Text	*/
#define A_ETX	0x03	/* Ctrl-c End Of Text		*/
#define A_EOT	0x04	/* Ctrl-d End Of Transmission	*/
#define A_ENQ	0x05	/* Ctrl-e Enquiry		*/
#define A_ACK	0x06	/* Ctrl-f Acknowledge		*/
#define A_BEL	0x07	/* Ctrl-g Bell			*/
#define A_BS	0x08	/* Ctrl-h Backspace		*/
#define A_HT	0x09	/* Ctrl-i Horizontal Tab	*/
#define A_NL	0x0A	/* Ctrl-j New Line		*/
#define A_LF	0x0A	/* Ctrl-j Line Feed		*/
#define A_VT	0x0B	/* Ctrl-k Vertical Tabulation	*/
#define A_FF	0x0C	/* Ctrl-l Form Feed		*/
#define A_CR	0x0D	/* Ctrl-m Carriage Return	*/
#define A_SO	0x0E	/* Ctrl-n Stand Out		*/
#define A_SI	0x0F	/* Ctrl-o Stand In		*/
#define A_DLE	0x10	/* Ctrl-p Data Link Escape	*/
#define A_DC1	0x11	/* Ctrl-q Data Ctrl 1 - XON	*/
#define A_DC2	0x12	/* Ctrl-r Data Ctrl 2		*/
#define A_DC3	0x13	/* Ctrl-s Data Ctrl 3 - XOFF	*/
#define A_DC4	0x14	/* Ctrl-t Data Ctrl 4		*/
#define A_NAK	0x15	/* Ctrl-u Negative Acknowledge	*/
#define A_SYN	0x16	/* Ctrl-v Synchronization	*/
#define A_ETB	0x17	/* Ctrl-w 			*/
#define A_CAN	0x18	/* Ctrl-x Cancel		*/
#define A_EM	0x19	/* Ctrl-y 			*/
#define A_SUB	0x1A	/* Ctrl-z			*/
#define A_ESC	0x1B	/* Ctrl-[ Escape		*/
#define A_FS	0x1C	/* Ctrl-|			*/
#define A_GS	0x1D	/* Ctrl-]			*/
#define A_RS	0x1E	/* Ctrl-^			*/
#define A_US	0x1F	/* Ctrl-_			*/
#define A_DEL	0x7F	/*        Delete		*/

#endif
