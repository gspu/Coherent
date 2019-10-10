/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * /usr/include/sys/kb.h
 *
 * Keyboard definitions and constants for user configurable AT keyboard driver.
 *
 * Revised: Wed Apr  7 17:15:34 1993 CDT
 */

#ifndef	__SYS_KB_H__
#define	__SYS_KB_H__

#include <common/feature.h>

/*
 * Language spec for non-loadable keyboard drivers.
 */
enum {
	kb_lang_us = 0,
	kb_lang_de = 1,
	kb_lang_fr = 2
};

/*
 * ASCII Macro Definitions - stolen from <ascii.h>
 */
#define nul	0x00	/* Null			*/
#define soh	0x01	/* Start Of Header	*/
#define stx	0x02	/* Start Of Text	*/
#define etx	0x03	/* End Of Text		*/
#define eot	0x04	/* End Of Transmission	*/
#define enq	0x05	/* Enquiry		*/
#define ack	0x06	/* Acknowledge		*/
#define bel	0x07	/* Bell			*/
#define bs	0x08	/* Backspace		*/
#define ht	0x09	/* Horizontal Tab	*/
#define nl	0x0A	/* New Line (line feed)	*/
#define lf	0x0A	/* Line Feed (newline)	*/
#define vt	0x0B	/* Vertical Tabulation	*/
#define ff	0x0C	/* Form Feed		*/
#define cr	0x0D	/* Carriage Return	*/
#define so	0x0E	/* Stand Out		*/
#define si	0x0F	/* Stand In		*/
#define dle	0x10	/* Data Link Escape	*/
#define dc1	0x11	/* Data Ctrl 1 - XON	*/
#define dc2	0x12	/* Data Ctrl 2		*/
#define dc3	0x13	/* Data Ctrl 3 - XOFF	*/
#define dc4	0x14	/* Data Ctrl 4		*/
#define nak	0x15	/* Negative Acknowledge	*/
#define syn	0x16	/* Synchronization	*/
#define etb	0x17	/* End of Trans. Block	*/
#define can	0x18	/* Cancel		*/
#define em	0x19	/* End of Medium	*/
#define sub	0x1A	/* Substitute		*/
#define esc	0x1B	/* Escape		*/
#define fs	0x1C	/* Form Sep.		*/
#define gs	0x1D	/* Group Sep.		*/
#define rs	0x1E	/* Record Sep.		*/
#define us	0x1F	/* Unit Sep.		*/
#define del	0x7F	/* Delete		*/
#define rub	0x7F	/* Rubout (Delete)	*/

/*
 * special and shift keys
 */
#define	none	0xFF	/* no key value */
#define	scroll	1	/* SCROLL lock key */
#define	num	2	/* NUM lock key */
#define	caps	3	/* CAPS lock key */
#define	lalt	4	/* left ALT key */
#define	ralt	5	/* right ALT key */
#define	lshift	6	/* left SHIFT key */
#define	rshift	7	/* right SHIFT key */
#define	lctrl	8	/* left CTRL key */
#define	rctrl	9	/* right CTRL key */
#define	altgr	10	/* ALT GR. key */

/*
 * flag/mode field definitions
 */
#define	O	0	/* OFF: no special processing */
#define	N	0x01	/* NUM lock applies */
#define	C	0x02	/* CAP lock applies */
#define	F	0x04	/* programmable function key */
#define	S	0x08	/* shift key (from above list) */
/*		0x10	   available */
/*		0x20	   available */
#define	TMODE	0xC0	/* key mode mask (T, M, MB, TMB) */
#define	M	0x00	/* "Make" only */
#define	T	0x40	/* "Typematic" mode */
#define	MB	0x80	/* "Make/Break" mode */
#define	TMB	0xC0	/* "Typematic/Make/Break" mode */

/*
 * function key definitions
 */
#define	reboot	f0	/* reboot system */
#define	f0	0	/* function key 0 */
#define	f1	1	/* function key 1 */
#define	f2	2	/* function key 2 */
#define	f3	3	/* function key 3 */
#define	f4	4	/* function key 4 */
#define	f5	5	/* function key 5 */
#define	f6	6	/* function key 6 */
#define	f7	7	/* function key 7 */
#define	f8	8	/* function key 8 */
#define	f9	9	/* function key 9 */
#define	f10	10	/* function key 10 */
#define	f11	11	/* function key 11 */
#define	f12	12	/* function key 12 */
#define	f13	13	/* function key 13 */
#define	f14	14	/* function key 14 */
#define	f15	15	/* function key 15 */
#define	f16	16	/* function key 16 */
#define	f17	17	/* function key 17 */
#define	f18	18	/* function key 18 */
#define	f19	19	/* function key 19 */
#define	f20	20	/* function key 20 */
#define	f21	21	/* function key 21 */
#define	f22	22	/* function key 22 */
#define	f23	23	/* function key 23 */
#define	f24	24	/* function key 24 */
#define	f25	25	/* function key 25 */
#define	f26	26	/* function key 26 */
#define	f27	27	/* function key 27 */
#define	f28	28	/* function key 28 */
#define	f29	29	/* function key 29 */
#define	f30	30	/* function key 30 */
#define	f31	31	/* function key 31 */
#define	f32	32	/* function key 32 */
#define	f33	33	/* function key 33 */
#define	f34	34	/* function key 34 */
#define	f35	35	/* function key 35 */
#define	f36	36	/* function key 36 */
#define	f37	37	/* function key 37 */
#define	f38	38	/* function key 38 */
#define	f39	39	/* function key 39 */
#define	f40	40	/* function key 40 */
#define	f41	41	/* function key 41 */
#define	f42	42	/* function key 42 */
#define	f43	43	/* function key 43 */
#define	f44	44	/* function key 44 */
#define	f45	45	/* function key 45 */
#define	f46	46	/* function key 46 */
#define	f47	47	/* function key 47 */
#define	f48	48	/* function key 48 */
#define	f49	49	/* function key 49 */
#define	f50	50	/* function key 50 */
#define	f51	51	/* function key 51 */
#define	f52	52	/* function key 52 */
#define	f53	53	/* function key 53 */
#define	f54	54	/* function key 54 */
#define	f55	55	/* function key 55 */
#define	f56	56	/* function key 56 */
#define	f57	57	/* function key 57 */
#define	f58	58	/* function key 58 */
#define	f59	59	/* function key 59 */
#define	f60	60	/* function key 60 */
#define	f61	61	/* function key 61 */
#define	f62	62	/* function key 62 */
#define	f63	63	/* function key 63 */
#define	f64	64	/* function key 64 */
#define	f65	65	/* function key 65 */
#define	f66	66	/* function key 66 */
#define	f67	67	/* function key 67 */
#define	f68	68	/* function key 68 */
#define	f69	69	/* function key 69 */
#define	f70	70	/* function key 70 */
#define	f71	71	/* function key 71 */
#define	f72	72	/* function key 72 */
#define	f73	73	/* function key 73 */
#define	f74	74	/* function key 74 */
#define	f75	75	/* function key 75 */
#define	f76	76	/* function key 76 */
#define	f77	77	/* function key 77 */
#define	f78	78	/* function key 78 */
#define	f79	79	/* function key 79 */
#define	f80	80	/* function key 80 */
#define	f81	81	/* function key 81 */
#define	f82	82	/* function key 82 */
#define	f83	83	/* function key 83 */
#define	f84	84	/* function key 84 */
#define	f85	85	/* function key 85 */
#define	f86	86	/* function key 86 */
#define	f87	87	/* function key 87 */
#define	f88	88	/* function key 88 */
#define	f89	89	/* function key 89 */
#define	f90	90	/* function key 90 */
#define	f91	91	/* function key 91 */
#define	f92	92	/* function key 92 */
#define	f93	93	/* function key 93 */
#define	f94	94	/* function key 94 */
#define	f95	95	/* function key 95 */
#define	f96	96	/* function key 96 */
#define	f97	97	/* function key 97 */
#define	f98	98	/* function key 98 */
#define	f99	99	/* function key 99 */

/*
 * table entry definition
 */
typedef	struct	{
	unsigned char	k_key;		/* Scan code set 3 key value */
	unsigned char	k_val[9];	/* key value for each mode */
	unsigned char	k_flags;	/* flags and mode bits */
} KBTBL;

/*
 * Index values for k_val[] array.
 * These are computed in the driver based upon the following weighting:
 *
 *	Shift Keys	bit 0
 *	Control Keys	bit 1
 *	Alt Keys	bit 2
 *	Alt Graphic Key	bit 3
 *
 * Use of the Alt Graphic key ignores other shift modes.
 */
#define	BASE		0
#define	SHIFT		1
#define	CTRL		2
#define	CTRL_SHIFT	3
#define	ALT		4
#define	ALT_SHIFT	5
#define	ALT_CTRL	6
#define	ALT_CTRL_SHIFT	7
#define	ALT_GR		8

/*
 * Keyboard responses to system
 */
#define	K_BREAK		0xF0		/* break code prefix byte */
#define	K_ACK		0xFA		/* Acknowledgement */
#define	K_BAT_OK	0xAA		/* BAT Completion code */
#define	K_BAT_BAD	0xFC		/* BAT Failure code */
#define	K_ECHO_R	0xEE		/* Echo response */
#define	K_DETECT_1	0xFF		/* Key detect error, code set 1 */
#define	K_DETECT_23	0x00		/* Key detect error, code sets 2 & 3 */
#define	K_OVERRUN_1	0xFF		/* Buffer overrun, code set 1 */
#define	K_OVERRUN_23	0x00		/* Buffer overrun, code sets 2 & 3 */
#define	K_RESEND	0xFE		/* Resend prior command */

/*
 * Keyboard commands
 */
#define	K_LED_CMD	0xED		/* Set/reset LEDs */
#define	K_ECHO_CMD	0xEE		/* Echo */
#define	K_SCANCODE_CMD	0xF0		/* Select scan code set */
#define	K_RATE_CMD	0xF3		/* Set typematic rate/delay */
#define	K_ENABLE_CMD	0xF4		/* Enable keyboard */
#define	K_DEFAULT_CMD	0xF6		/* Set defaults */
#define	K_ALL_T_CMD	0xF7		/* Set all keys typematic */
#define	K_ALL_MB_CMD	0xF8		/* Set all keys make/break */
#define	K_ALL_M_CMD	0xF9		/* Set all keys make */
#define	K_ALL_TMB_CMD	0xFA		/* Set all keys typematic/make/break */
#define	K_KEY_T_CMD	0xFB		/* Set key as typematic */
#define	K_KEY_MB_CMD	0xFC		/* Set key as make/break */
#define	K_KEY_M_CMD	0xFD		/* Set key as make */
#define	K_RESEND_CMD	0xFE		/* Resend */
#define	K_RESET_CMD	0xFF		/* Reset keyboard */

/*
 * function key definitions
 *
 * After this structure are the actual function key text strings.
 * Entries are delimited by DELIM (0xFF or '\377') to allow NUL characters to
 * be embedded in function strings.
 */
typedef	struct	{
	unsigned char	k_nfkeys;	/* number of function keys */
} FNKEY;

#define	MAX_KEYS	256		/* all that can fit in a char */
#define	MAX_TABLE_SIZE	(MAX_KEYS * sizeof(KBTBL)) /* worst case table size */
#define	MAX_FKEYS	75
#define	MAX_FCHAR	2048		/* default Fn key arena size */
#define	DELIM		0xFF		/* function key delimiter */


/*
 * Virtual terminal key definitions.
 * These are byte values, and MUST NOT overlap with f0..fn values above.
 *
 * For VTKEY test to work, virtual terminal selector values must be
 * in a contiguous range from VTKEY_HOME to VTKEY_MAX.
 */

#define	vt0		130
#define	vt1		vt0+1
#define	vt2		vt0+2
#define	vt3		vt0+3
#define	vt4		vt0+4
#define	vt5		vt0+5
#define	vt6		vt0+6
#define	vt7		vt0+7
#define	vt8		vt0+8
#define	vt9		vt0+9
#define	vt10		vt0+10
#define	vt11		vt0+11
#define	vt12		vt0+12
#define	vt13		vt0+13
#define	vt14		vt0+14
#define	vt15		vt0+15

#define color0		vt0+16
#define color1		color0+1
#define color2		color0+2
#define color3		color0+3
#define color4		color0+4
#define color5		color0+5
#define color6		color0+6
#define color7		color0+7
#define color8		color0+8
#define color9		color0+9
#define color10		color0+10
#define color11		color0+11
#define color12		color0+12
#define color13		color0+13
#define color14		color0+14
#define color15		color0+15

#define mono0		vt0+32
#define mono1		mono0+1
#define mono2		mono0+2
#define mono3		mono0+3
#define mono4		mono0+4
#define mono5		mono0+5
#define mono6		mono0+6
#define mono7		mono0+7
#define mono8		mono0+8
#define mono9		mono0+9
#define mono10		mono0+10
#define mono11		mono0+11
#define mono12		mono0+12
#define mono13		mono0+13
#define mono14		mono0+14
#define mono15		mono0+15

/*
 * vtn - next logical session (increasing minor numbers)
 * vtp - previous logical session (decreasing minor numbers)
 * vtt - toggle current with latest session
 */
#define	vtn		vt0+48
#define	vtp		vt0+49
#define	vtt		vt0+50

#define	VTKEY_HOME	vt0
#define	VTKEY_NEXT	vtn
#define	VTKEY_PREV	vtp
#define	VTKEY_TOGL	vtt
#define	VTKEY_MAX	vtt

#define	VTKEY(x)	((x)>=VTKEY_HOME && (x)<=VTKEY_MAX)

/*
 * The following is provisional, pending STREAMS enhancements to the console.
 *
 * "fgk" is normally bound to <alt>+<enter>.
 */
#define fgk		(VTKEY_MAX+1)

/*
 * function key definitions
 *
 * Struct member k_fnval contains the actual function key values.
 * Entries are delimited by DELIM (0xFF or '\377') to allow NUL characters to
 * be embedded in function strings.
 */

#define	VNKB_FALSE	0
#define	VNKB_TRUE	1

#if	__KERNEL__

/*
 * patchable params for non-standard keyboards
 */
extern	int	KBDATA;			/* Keyboard data */
extern	int	KBCTRL;			/* Keyboard control */
extern	int	KBSTS_CMD;		/* Keyboard status/command */
extern	int	KBFLAG;			/* Keyboard reset flag */
extern	int	KBBOOT;			/* 0: disallow reboot from keyboard */
extern	int	KBTIMEOUT;		/* shouldn't need this much */
extern	int	KBCMDBYTE;		/* no translation */

#ifndef	_I386
extern	SEG	*kbsegp;		/* keyboard table segment */
#endif
extern	int	isbusy;			/* Raw input conversion busy */
extern	int	vt_kb_state, vt_kb_cmd2, vt_kb_prev_cmd;

#if	VT_MAJOR != KB_MAJOR
extern	Ksetivec(), Kisrint();
extern	Kclrivec();
#endif

#endif	/* __KERNEL__ */

#endif	/* ! defined (__SYS_KB_H__) */
