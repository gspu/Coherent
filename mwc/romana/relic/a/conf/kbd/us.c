/* (-lgl
 * 	COHERENT Version 3.2
 * 	Copyright (c) 1982, 1991 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * U.S. standard AT keyboard code table.
 *
 *	See header files for definitions and constants.
 *
 *	Version: 1.0, 05/31/91
 */
#include "kbscan.h"
#include "kb.h"

char	tbl_name[] = "U.S. AT keyboard table";

KBTBL	kbtbl[] = {
/* AT                                                    Alt
 *Phys                       Ctrl           Alt   Alt    Ctrl     Alt
 *Key#  Base   Shift  Ctrl   Shift  Alt    Shift  Ctrl   Shift  Graphic Flags
 *----  ----   -----  ----   -----  ---    -----  ----   -----  ------- -----*/
{K_1,	'`',   '~',   none,  none,  '`',   '~',   none,  none,  none,	O|T  },
{K_2,	'1',   '!',   none,  none,  '1',   '!',   none,  none,  none,	O|T  },
{K_3,	'2',   '@',   nul,   nul,   '2',   '@',   nul,   nul,   none,	O|T  },
{K_4,	'3',   '#',   none,  none,  '3',   '#',   none,  none,  none,	O|T  },
{K_5,	'4',   '$',   none,  none,  '4',   '$',   none,  none,  none,	O|T  },
{K_6,	'5',   '%',   none,  none,  '5',   '%',   none,  none,  none,	O|T  },
{K_7,	'6',   '^',   rs,    rs,    '6',   '^',   rs,    rs,    none,	O|T  },
{K_8,	'7',   '&',   none,  none,  '7',   '&',   none,  none,  none,	O|T  },
{K_9,	'8',   '*',   none,  none,  '8',   '*',   none,  none,  none,	O|T  },
{K_10,	'9',   '(',   none,  none,  '9',   '(',   none,  none,  none,	O|T  },
{K_11,	'0',   ')',   none,  none,  '0',   ')',   none,  none,  none,	O|T  },
{K_12,	'-',   '_',   us,    us,    '-',   '_',   us,    us,    none,	O|T  },
{K_13,	'=',   '+',   none,  none,  '=',   '+',   none,  none,  none,	O|T  },
/* key 14 undefined */
{K_15,	bs,    bs,    del,   del,   bs,    bs,    del,   del,   none,	O|T  },
{K_16,	f42,   f43,   none,  none,  f42,   f43,   none,  none,  none,   F|T  },
{K_17,	'q',   'Q',   dc1,   dc1,   'q',   'Q',   dc1,   dc1,   none,   C|T  },
{K_18,	'w',   'W',   etb,   etb,   'w',   'W',   etb,   etb,   none,   C|T  },
{K_19,	'e',   'E',   enq,   enq,   'e',   'E',   enq,   enq,   none,   C|T  },
{K_20,	'r',   'R',   dc2,   dc2,   'r',   'R',   dc2,   dc2,   none,   C|T  },
{K_21,	't',   'T',   dc4,   dc4,   't',   'T',   dc4,   dc4,   none,   C|T  },
{K_22,	'y',   'Y',   em,    em,    'y',   'Y',   em,    em,    none,   C|T  },
{K_23,	'u',   'U',   nak,   nak,   'u',   'U',   nak,   nak,   none,   C|T  },
{K_24,	'i',   'I',   ht,    ht,    'i',   'I',   ht,    ht,    none,   C|T  },
{K_25,	'o',   'O',   si,    si,    'o',   'O',   si,    si,    none,   C|T  },
{K_26,	'p',   'P',   dle,   dle,   'p',   'P',   dle,   dle,   none,   C|T  },
{K_27,	'[',   '{',   esc,   esc,   '[',   '{',   esc,   esc,   none,   O|T  },
{K_28,	']',   '}',   gs,    gs,    ']',   '}',   gs,    gs,    none,   O|T  },
{K_29,	'\\',  '|',   fs,    fs,    '\\',  '|',   fs,    fs,    none,   O|T  },
{K_30,	caps,  caps,  caps,  caps,  caps,  caps,  caps,  caps,  caps,   S|M  },
{K_31,	'a',   'A',   soh,   soh,   'a',   'A',   soh,   soh,   none,   C|T  },
{K_32,	's',   'S',   dc3,   dc3,   's',   'S',   dc3,   dc3,   none,   C|T  },
{K_33,	'd',   'D',   eot,   eot,   'd',   'D',   eot,   eot,   none,   C|T  },
{K_34,	'f',   'F',   ack,   ack,   'f',   'F',   ack,   ack,   none,   C|T  },
{K_35,	'g',   'G',   bel,   bel,   'g',   'G',   bel,   bel,   none,   C|T  },
{K_36,	'h',   'H',   bs,    bs,    'h',   'H',   bs,    bs,    none,   C|T  },
{K_37,	'j',   'J',   nl,    nl,    'j',   'J',   nl,    nl,    none,   C|T  },
{K_38,	'k',   'K',   vt,    vt,    'k',   'K',   vt,    vt,    none,   C|T  },
{K_39,	'l',   'L',   ff,    ff,    'l',   'L',   ff,    ff,    none,   C|T  },
{K_40,	';',   ':',   none,  none,  ';',   ':',   none,  none,  none,   O|T  },
{K_41,	'\'',  '"',   none,  none,  '\'',  '"',   none,  none,  none,   O|T  },
{K_42,	'\\',  '|',   fs,    fs,    '\\',  '|',   fs,    fs,    none,   O|T  },
{K_43,	cr,    cr,    nl,    nl,    cr,    cr,    nl,    nl,    none,   O|T  },
{K_44,	lshift,lshift,lshift,lshift,lshift,lshift,lshift,lshift,lshift, S|MB },
{K_45,	none,  none,  none,  none,  none,  none,  none,  none,  none,   O|T  },
{K_46,	'z',   'Z',   sub,   sub,   'z',   'Z',   sub,   sub,   none,   C|T  },
{K_47,	'x',   'X',   can,   can,   'x',   'X',   can,   can,   none,   C|T  },
{K_48,	'c',   'C',   etx,   etx,   'c',   'C',   etx,   etx,   none,   C|T  },
{K_49,	'v',   'V',   syn,   syn,   'v',   'V',   syn,   syn,   none,   C|T  },
{K_50,	'b',   'B',   stx,   stx,   'b',   'B',   stx,   stx,   none,   C|T  },
{K_51,	'n',   'N',   so,    so,    'n',   'N',   so,    so,    none,   C|T  },
{K_52,	'm',   'M',   cr,    cr,    'm',   'M',   cr,    cr,    none,   C|T  },
{K_53,	',',   '<',   none,  none,  ',',   '<',   none,  none,  none,   O|T  },
{K_54,	'.',   '>',   none,  none,  '.',   '>',   none,  none,  none,   O|T  },
{K_55,	'/',   '?',   none,  none,  '/',   '?',   none,  none,  none,   O|T  },
/* key 56 undefined */
{K_57,	rshift,rshift,rshift,rshift,rshift,rshift,rshift,rshift,rshift, S|MB },
{K_58,	lctrl, lctrl, lctrl, lctrl, lctrl, lctrl, lctrl, lctrl, lctrl,  S|MB },
/* key 59 undefined */
{K_60,	lalt,  lalt,  lalt,  lalt,  lalt,  lalt,  lalt,  lalt,  lalt,   S|MB },
{K_61,	' ',   ' ',   ' ',   ' ',   ' ',   ' ',   ' ',   ' ',   none,   O|T  },
{K_62,	ralt,  ralt,  ralt,  ralt,  ralt,  ralt,  ralt,  ralt,  ralt,   S|MB },
/* key 63 undefined */
{K_64,	rctrl, rctrl, rctrl, rctrl, rctrl, rctrl, rctrl, rctrl, rctrl,  S|MB },
/* keys 65 through 74 undefined */
{K_75,	f40,   f40,   f40,   f40,   f40,   f40,   f40,   f40,   f40,    F|M  },
{K_76,	f41,   f41,   f41,   f41,   f41,   f41,   reboot,f41,   f41,    F|M  },
/* keys 77 and 78 undefined */
{K_79,	f34,   f34,   f34,   f34,   f34,   f34,   f34,   f34,   f34,    F|T  },
{K_80,	f37,   f37,   f37,   f37,   f37,   f37,   f37,   f37,   f37,    F|M  },
{K_81,	f31,   f31,   f31,   f31,   f31,   f31,   f31,   f31,   f31,    F|M  },
/* key 82 undefined */
{K_83,	f38,   f38,   f38,   f38,   f38,   f38,   f38,   f38,   f38,    F|T  },
{K_84,	f32,   f32,   f32,   f32,   f32,   f32,   f32,   f32,   f32,    F|T  },
{K_85,	f39,   f39,   f39,   f39,   f39,   f39,   f39,   f39,   f39,    F|M  },
{K_86,	f33,   f33,   f33,   f33,   f33,   f33,   f33,   f33,   f33,    F|M  },
/* keys 87 and 88 undefined */
{K_89,	f36,   f36,   f36,   f36,   f36,   f36,   f36,   f36,   f36,    F|T  },
{K_90,	num,   num,   num,   num,   num,   num,   num,   num,   num,    S|M  },
{K_91,	f37,   f57,   f57,   f57,   f57,   f57,   f57,   f57,   f57,    F|N|M},
{K_92,	f34,   f54,   f54,   f54,   f54,   f54,   f54,   f54,   f54,    F|N|M},
{K_93,	f31,   f51,   f51,   f51,   f51,   f51,   f51,   f51,   f51,    F|N|M},
/* key 94 undefined */
{K_95,	'/',   '/',   none,  none,  none,  none,  none,  none,  none,   O|M  },
{K_96,	f38,   f58,   f58,   f58,   f58,   f58,   f58,   f58,   f58,    F|N|M},
{K_97,	f35,   f55,   f55,   f55,   f55,   f55,   f55,   f55,   f55,    F|N|M},
{K_98,	f32,   f52,   f52,   f52,   f52,   f52,   f52,   f52,   f52,    F|N|M},
{K_99,	f40,   f60,   f60,   f60,   f60,   f60,   f60,   f60,   f60,    F|N|M},
{K_100,	'*',   '*',   none,  none,  none,  none,  none,  none,  none,   O|M  },
{K_101,	f39,   f59,   f59,   f59,   f59,   f59,   f59,   f59,   f59,    F|N|M},
{K_102,	f36,   f56,   f56,   f56,   f56,   f56,   f56,   f56,   f56,    F|N|M},
{K_103,	f33,   f53,   f53,   f53,   f53,   f53,   f53,   f53,   f53,    F|N|M},
{K_104,	f41,   f61,   f61,   f61,   f61,   f61,   reboot,f61,   f61,    F|N|M},
{K_105,	'-',   '-',   none,  none,  none,  none,  none,  none,  none,   O|M  },
{K_106,	'+',   '+',   none,  none,  none,  none,  none,  none,  none,   O|M  },
/* key 107 undefined */
{K_108,	cr,    cr,    nl,    nl,    cr,    cr,    nl,    nl,    none,   O|M  },
/* key 109 undefined */
{K_110,	esc,   esc,   none,  none,  esc,   esc,   none,  none,  none,   O|M  },
/* key 111 undefined */
{K_112,	f1,    f11,   f11,   f11,   f11,   f11,   f11,   f11,   f11,    F|M  },
{K_113,	f2,    f12,   f12,   f12,   f12,   f12,   f12,   f12,   f12,    F|M  },
{K_114,	f3,    f13,   f13,   f13,   f13,   f13,   f13,   f13,   f13,    F|M  },
{K_115,	f4,    f14,   f14,   f14,   f14,   f14,   f14,   f14,   f14,    F|M  },
{K_116,	f5,    f15,   f15,   f15,   f15,   f15,   f15,   f15,   f15,    F|M  },
{K_117,	f6,    f16,   f16,   f16,   f16,   f16,   f16,   f16,   f16,    F|M  },
{K_118,	f7,    f17,   f17,   f17,   f17,   f17,   f17,   f17,   f17,    F|M  },
{K_119,	f8,    f18,   f18,   f18,   f18,   f18,   f18,   f18,   f18,    F|M  },
{K_120,	f9,    f19,   f19,   f19,   f19,   f19,   f19,   f19,   f19,    F|M  },
{K_121,	f10,   f20,   f20,   f20,   f20,   f20,   f20,   f20,   f20,    F|M  },
{K_122,	none,  none,  none,  none,  none,  none,  none,  none,  none,   O|M  },
{K_123,	none,  none,  none,  none,  none,  none,  none,  none,  none,   O|M  },
{K_124,	none,  none,  none,  none,  none,  none,  none,  none,  none,   O|M  },
{K_125,	scroll,scroll,scroll,scroll,scroll,scroll,scroll,scroll,scroll, S|M  },
{K_126,	none,  none,  none,  none,  none,  none,  none,  none,  none,   O|M  }
};

/*
 * Special and programable function key definitions:
 *
 * Notes:
 *   1) If a key outputs a multi-byte sequence in any mode, the key
 *	must be defined as a function key (flags field == F) and all entries
 *	for the key must be function keys (i.e., f1 through f50).
 *
 *   2)	All key definition strings must be terminated by a \377 sequence.
 *	This allows the NUL ('\0') character to be embedded in function strings.
 */

unsigned char	*funkey[] = {
/* 0/reboot */	"reboot\377",		/* jump to reboot code */
/* 1 */		"\033[1x\377",		/* F1 */
/* 2 */		"\033[2x\377",		/* F2 */
/* 3 */		"\033[3x\377",		/* F3 */
/* 4 */		"\033[4x\377", 		/* F4 */
/* 5 */		"\033[5x\377",		/* F5 */
/* 6 */		"\033[6x\377",		/* F6 */
/* 7 */		"\033[7x\377",		/* F7 */
/* 8 */		"\033[8x\377",		/* F8 */
/* 9 */		"\033[9x\377",		/* F9 */
/* 10 */	"\033[0x\377",		/* F10 - historical value */
/* 11 */	"\033[1y\377",		/* F11 */
/* 12 */	"\033[2y\377",		/* F12 */
/* 13 */	"\033[3y\377",		/* F13 */
/* 14 */	"\033[4y\377", 		/* F14 */
/* 15 */	"\033[5y\377",		/* F15 */
/* 16 */	"\033[6y\377",		/* F16 */
/* 17 */	"\033[7y\377",		/* F17 */
/* 18 */	"\033[8y\377",		/* F18 */
/* 19 */	"\033[9y\377",		/* F19 */
/* 20 */	"\033[0y\377",		/* F20 */
/* 21 */	"F21\377",		/* unused */
/* 22 */	"F22\377",		/* unused */
/* 23 */	"F23\377",		/* unused */
/* 24 */	"F24\377",		/* unused */
/* 25 */	"F25\377",		/* unused */
/* 26 */	"F26\377",		/* unused */
/* 27 */	"F27\377",		/* unused */
/* 28 */	"F28\377",		/* unused */
/* 29 */	"F29\377",		/* unused */
/* 30 */	"F30\377",		/* unused */
/* 31 */	"\033[24H\377", 	/* End */
/* 32 */	"\033[B\377",		/* Down Arrow */
/* 33 */	"\033[U\377", 		/* Page Down */
/* 34 */	"\033[D\377",	 	/* Left Arrow */
/* 35 */	"\377",			/* Unshifted keypad 5 */
/* 36 */	"\033[C\377",	 	/* Right Arrow */
/* 37 */	"\033[H\377",	 	/* Home */
/* 38 */	"\033[A\377",	 	/* Up Arrow */
/* 39 */	"\033[V\377",	 	/* Page Up */
/* 40 */	"\033[@\377",	 	/* Insert */
/* 41 */	"\033[P\377",	 	/* Delete */
/* 42 */	"\t\377",		/* Tab */
/* 43 */	"\033[Z\377",	 	/* Back Tab */
/* 44 */	"F44\377",		/* unused */
/* 45 */	"F45\377",		/* unused */
/* 46 */	"F46\377",		/* unused */
/* 47 */	"F47\377",		/* unused */
/* 48 */	"F48\377",		/* unused */
/* 49 */	"F49\377",		/* unused */
/* 50 */	"F50\377",		/* unused */
/* 51 */	"1\377",	 	/* Keypad 1 */
/* 52 */	"2\377",	 	/* Keypad 2 */
/* 53 */	"3\377",	 	/* Keypad 3 */
/* 54 */	"4\377",	 	/* Keypad 4 */
/* 55 */	"5\377",	 	/* Keypad 5 */
/* 56 */	"6\377",	 	/* Keypad 6 */
/* 57 */	"7\377",	 	/* Keypad 7 */
/* 58 */	"8\377",	 	/* Keypad 8 */
/* 59 */	"9\377",	 	/* Keypad 9 */
/* 60 */	"0\377",	 	/* Keypad 0 */
/* 61 */	".\377"		 	/* Keypad . */
};

int	numfun	= sizeof(funkey) / sizeof(funkey[0]);	/* # of Fn keys */
int	numkey	= sizeof(kbtbl) / sizeof(kbtbl[0]);	/* # of actual keys */
/* end of us.c */

