/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__SYS_KBSCAN_H__
#define	__SYS_KBSCAN_H__

/*
 * kbscan.h:	AT physical key number to "scan code set 3" mapping constants
 *
 *		All key values are "make" codes. Equivalent "break" codes
 *		are prefixed by K_BREAK.
 *
 *		See IBM AT keyboard technical reference for further details.
 *
 *		Note: 83-key XT layout keyboards use scan codes
 *		K_14, K_65 through K_74, and K_107. These do not exist on
 *		101- and 102-key AT keyboards.
 *
 *		Version 1.2,  06/26/91
 */

/*    AT kbd   scan code  */
/*   physical    set 3    */
/*     key #    mapping   */
/*     -----    -------   */
#define K_1     0x0E            /* US101: ` ~ */
#define K_2     0x16            /* US101: 1 ! */
#define K_3     0x1E            /* US101: 2 @ */
#define K_4     0x26            /* US101: 3 # */
#define K_5     0x25            /* US101: 4 $ */
#define K_6     0x2E            /* US101: 5 % */
#define K_7     0x36            /* US101: 6 ^ */
#define K_8     0x3D            /* US101: 7 & */
#define K_9     0x3E            /* US101: 8 * */
#define K_10    0x46            /* US101: 9 ( */
#define K_11    0x45            /* US101: 0 ) */
#define K_12    0x4E            /* US101: - _ */
#define K_13    0x55            /* US101: = + */
#define	K_14	0x5D		/* US101: not defined */
#define	K_15	0x66		/* US101: Backspace */
#define	K_16	0x0D		/* US101: TAB/Back-TAB */
#define K_17    0x15            /* US101: q Q */
#define K_18    0x1D            /* US101: w W */
#define K_19    0x24            /* US101: e E */
#define K_20    0x2D            /* US101: r R */
#define K_21    0x2C            /* US101: t T */
#define K_22    0x35            /* US101: y Y */
#define K_23    0x3C            /* US101: u U */
#define K_24    0x43            /* US101: i I */
#define K_25    0x44            /* US101: o O */
#define K_26    0x4D            /* US101: p P */
#define K_27    0x54            /* US101: [ { */
#define K_28    0x5B            /* US101: ] } */
#define K_29    0x5C            /* US101: \ | (US101 only) */
#define	K_30	0x14		/* US101: CAPS LOCK */
#define K_31    0x1C            /* US101: a A */
#define K_32    0x1B            /* US101: s S */
#define K_33    0x23            /* US101: d D */
#define K_34    0x2B            /* US101: f F */
#define K_35    0x34            /* US101: g G */
#define K_36    0x33            /* US101: h H */
#define K_37    0x3B            /* US101: j J */
#define K_38    0x42            /* US101: k K */
#define K_39    0x4B            /* US101: l L */
#define K_40    0x4C            /* US101: ; : */
#define K_41    0x52            /* US101: ' " */
#define	K_42	0x53		/* 102-key keyboard only */
#define	K_43	0x5A		/* US101: ENTER */
#define	K_44	0x12		/* US101: LEFT SHIFT */
#define	K_45	0x13		/* 102-key keyboard only */
#define K_46    0x1A            /* US101: z Z */
#define K_47    0x22            /* US101: x X */
#define K_48    0x21            /* US101: c C */
#define K_49    0x2A            /* US101: v V */
#define K_50    0x32            /* US101: b B */
#define K_51    0x31            /* US101: n N */
#define K_52    0x3A            /* US101: m M */
#define K_53    0x41            /* US101: , < */
#define K_54    0x49            /* US101: . > */
#define K_55    0x4A            /* US101: / ? */
/* key 56 undefined */
#define	K_57	0x59		/* US101: RIGHT SHIFT */
#define	K_58	0x11		/* US101: LEFT CTRL */
/* key 59 undefined */
#define	K_60	0x19		/* US101: LEFT ALT */
#define	K_61	0x29		/* US101: SPACE */
#define	K_62	0x39		/* US101: RIGHT ALT */
/* key 63 undefined */
#define	K_64	0x58		/* US101: RIGHT CTRL */
/* keys 65 through 74 undefined for 101- and 102-key AT layouts */
#define	K_65	0x06
#define	K_66	0x0C
#define	K_67	0x0B
#define	K_68	0x0A
#define	K_69	0x09
#define	K_70	0x05
#define	K_71	0x04
#define	K_72	0x03
#define	K_73	0x83
#define	K_74	0x01
#define	K_75	0x67		/* US101: INSERT */
#define	K_76	0x64		/* US101: DELETE */
/* keys 77 and 78 undefined */
#define	K_79	0x61		/* US101: LEFT ARROW */
#define	K_80	0x6E		/* US101: HOME */
#define	K_81	0x65		/* US101: END */
/* key 82 undefined */
#define	K_83	0x63		/* US101: UP ARROW */
#define	K_84	0x60		/* US101: DOWN ARROW */
#define	K_85	0x6F		/* US101: PAGE UP */
#define	K_86	0x6D		/* US101: PAGE DOWN */
/* keys 87 and 88 undefined */
#define	K_89	0x6A		/* US101: RIGHT ARROW */
#define	K_90	0x76		/* US101: NUM LOCK */
#define	K_91	0x6C		/* US101: KEYPAD 7 */
#define	K_92	0x6B		/* US101: KEYPAD 4 */
#define	K_93	0x69		/* US101: KEYPAD 1 */
/* key 94 undefined */
#define	K_95	0x77		/* US101: KEYPAD / */
#define	K_96	0x75		/* US101: KEYPAD 8 */
#define	K_97	0x73		/* US101: KEYPAD 5 */
#define	K_98	0x72		/* US101: KEYPAD 2 */
#define	K_99	0x70		/* US101: KEYPAD 0 */
#define	K_100	0x7E		/* US101: KEYPAD * */
#define	K_101	0x7D		/* US101: KEYPAD 9 */
#define	K_102	0x74		/* US101: KEYPAD 6 */
#define	K_103	0x7A		/* US101: KEYPAD 3 */
#define	K_104	0x71		/* US101: KEYPAD . */
#define	K_105	0x84		/* US101: KEYPAD - */
#define	K_106	0x7C		/* US101: KEYPAD + */
/* key 107 undefined for 101- and 102-key AT layouts */
#define	K_107	0x7B			/* on right hand side of num. keypad */
#define	K_108	0x79		/* US101: KEYPAD ENTER */
/* key 109 undefined */
#define	K_110	0x08		/* US101: ESC */
/* key 111 undefined */
#define	K_112	0x07		/* US101: F1 */
#define	K_113	0x0F		/* US101: F2 */
#define	K_114	0x17		/* US101: F3 */
#define	K_115	0x1F		/* US101: F4 */
#define	K_116	0x27		/* US101: F5 */
#define	K_117	0x2F		/* US101: F6 */
#define	K_118	0x37		/* US101: F7 */
#define	K_119	0x3F		/* US101: F8 */
#define	K_120	0x47		/* US101: F9 */
#define	K_121	0x4F		/* US101: F10 */
#define	K_122	0x56		/* US101: F11 */
#define	K_123	0x5E		/* US101: F12 */
#define	K_124	0x57		/* US101: PRINT SCREEN */
#define	K_125	0x5F		/* US101: SCROLL LOCK */
#define	K_126	0x62		/* US101: PAUSE */

#endif	/* ! defined (__SYS_KBSCAN_H__) */
