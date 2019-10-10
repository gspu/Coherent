
#line 8 "sh.y"

#include "sh.h"

#define YYERROR	{ yyerrflag=1; goto YYerract; }

extern	NODE	*node();

#include "y.tab.h"
#define YYCLEARIN yychar = -1000
#define YYERROK yyerrflag = 0
extern int yychar;
extern short yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
YYSTYPE yyval, yylval;

#line 552 "sh.y"

/*
 * Create a node.
 */
NODE *
node(type, auxp, next)
NODE *auxp, *next;
{
	register NODE *np;

	np = (NODE *) balloc(sizeof (NODE));
	np->n_type = type;
	np->n_auxp = auxp;
	np->n_next = next;
	return np;
}

#define NBPC 8
#define NKEY 8
static char keys[NKEY] = { 0 };
static int  keyi = NKEY * NBPC;

keyflush()
{
	register char *kp;

	for (kp = keys+NKEY; kp > keys; *--kp = 0)
		;
	keyi = NKEY * NBPC;
}

keypop()
{
	register char	*kp;
	register int	km;

	if ((km = keyi++) >= NKEY * NBPC) {
		panic(11);
		NOTREACHED;
	}
	kp = keys + (km / NBPC);
	km = 1 << (km %= NBPC);
	keyflag = (*kp & km) ? 1 : 0;
	*kp &= ~km;
}

keypush()
{
	register char	*kp;
	register int	km;

	if ((km = --keyi) < 0) {
		panic(12);
		NOTREACHED;
	}
	if (keyflag) {
		kp = keys + (km / NBPC);
		km = 1 << (km %= NBPC);
		*kp |= km;
	}
}
/*
 * The following fragments might implement named pipes.
 * The token declaration goes in the header.
 * The nopen production should go with the others of its ilk.
 * The production fragment goes into arg:
%token _NOPEN _NCLOSE
nopen:	_NOPEN optnls ;

|	nopen pipe_cmd ')' {
		$$ = node(NRPIPE, $2, NULL);
	}
|	oparen pipe_cmd _NCLOSE {
		$$ = node(NWPIPE, $2, NULL);
	}
 *
 */
#ifdef YYTNAMES
readonly struct yytname yytnames[33] =
{
	"$end", -1, 
	"error", -2, 
	"_ANDF", 256, 
	"_ASGN", 257, 
	"_CASE", 258, 
	"_CBRAC", 259, 
	"_DO", 260, 
	"_DONE", 261, 
	"_DSEMI", 262, 
	"_ELIF", 263, 
	"_ELSE", 264, 
	"_ESAC", 265, 
	"_FI", 266, 
	"_FOR", 267, 
	"_IF", 268, 
	"_IN", 269, 
	"_IORS", 270, 
	"_NAME", 271, 
	"_NULL", 272, 
	"_OBRAC", 273, 
	"_ORF", 274, 
	"_PARENS", 275, 
	"_RET", 276, 
	"_THEN", 277, 
	"_UNTIL", 278, 
	"_WHILE", 279, 
	"'\\n'", 10, 
	"';'", 59, 
	"'|'", 124, 
	"'('", 40, 
	"')'", 41, 
	"'&'", 38, 
	NULL
} ;
#endif
#include <action.h>
readonly unsigned char yypdnt[118] = {
0, 1, 1, 2, 2, 2, 4, 6,
7, 8, 9, 9, 10, 10, 11, 11,
13, 14, 15, 16, 17, 18, 19, 3,
3, 3, 3, 3, 20, 20, 20, 21,
21, 22, 23, 24, 24, 24, 24, 24,
24, 26, 28, 27, 27, 25, 25, 25,
25, 25, 33, 33, 33, 33, 36, 34,
35, 35, 35, 35, 35, 35, 39, 40,
40, 40, 40, 40, 40, 40, 40, 40,
40, 40, 40, 40, 40, 40, 32, 41,
38, 29, 37, 42, 30, 30, 30, 30,
30, 30, 30, 30, 43, 43, 48, 48,
45, 45, 45, 49, 50, 50, 44, 44,
47, 47, 47, 46, 46, 31, 31, 12,
12, 12, 5, 5, 51, 51 
};
readonly unsigned char yypn[118] = {
2, 2, 0, 1, 2, 2, 2, 2,
2, 2, 2, 2, 2, 3, 1, 2,
2, 2, 2, 2, 2, 2, 2, 1,
2, 2, 3, 3, 1, 3, 3, 3,
1, 2, 0, 1, 1, 2, 1, 2,
1, 1, 6, 1, 2, 3, 2, 1,
2, 1, 1, 2, 1, 2, 1, 1,
1, 2, 1, 2, 1, 2, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 6, 5, 6, 5,
4, 6, 3, 3, 2, 0, 2, 0,
3, 1, 0, 3, 3, 1, 2, 0,
5, 2, 0, 1, 0, 3, 2, 1,
1, 2, 1, 0, 1, 2 
};
readonly unsigned char yypgo[52] = {
0, 0, 2, 4, 12, 14, 48, 52,
54, 56, 58, 60, 64, 70, 72, 74,
76, 78, 82, 84, 86, 92, 96, 98,
100, 102, 104, 106, 108, 110, 126, 128,
140, 152, 158, 160, 170, 172, 180, 184,
186, 188, 190, 192, 194, 200, 206, 220,
224, 228, 230, 234 
};
readonly unsigned int yygo[246] = {
YYOTHERS, 0x1, YYOTHERS, 0x5, 0x1, 0x6, 0xd, 0x31,
0xe, 0x32, YYOTHERS, 0x43, YYOTHERS, 0x20, 0x10, 0x36,
0x13, 0x39, 0x18, 0x3d, 0x1b, 0x3e, 0x1d, 0x40,
0x1e, 0x41, 0x1f, 0x42, 0x43, 0x6d, 0x4b, 0x77,
0x6f, 0x88, 0x71, 0x8b, 0x8a, 0x9e, 0x91, 0xa1,
0x93, 0xa3, 0x99, 0xa7, 0x9a, 0xa8, YYOTHERS, 0x34,
0xa9, 0xad, YYOTHERS, 0x70, YYOTHERS, 0x9b, YYOTHERS, 0x9c,
YYOTHERS, 0x21, YYOTHERS, 0x72, 0x69, 0x82, YYOTHERS, 0x68,
0x66, 0x7c, 0x6c, 0x85, YYOTHERS, 0x69, YYOTHERS, 0x11,
YYOTHERS, 0x12, YYOTHERS, 0x14, YYOTHERS, 0x22, 0x77, 0x8e,
YYOTHERS, 0x23, YYOTHERS, 0x94, YYOTHERS, 0x92, 0x11, 0x37,
0x12, 0x38, YYOTHERS, 0x7, 0x14, 0x3a, YYOTHERS, 0x8,
YYOTHERS, 0x9, YYOTHERS, 0xa, YYOTHERS, 0x24, YYOTHERS, 0x25,
YYOTHERS, 0x26, YYOTHERS, 0x49, YYOTHERS, 0x27, 0xa, 0x28,
0x16, 0x3b, 0x17, 0x3c, 0x1c, 0x3f, 0x2b, 0x4d,
0x6b, 0x83, 0x83, 0x83, YYOTHERS, 0x7e, YYOTHERS, 0x29,
0x20, 0x44, 0x21, 0x45, 0x6e, 0x87, 0x8e, 0x9f,
0x9b, 0xa9, YYOTHERS, 0x46, 0xa, 0x2a, 0x26, 0x4a,
0x2a, 0x2a, 0x2d, 0x2a, 0x49, 0x76, YYOTHERS, 0x5f,
0x2a, 0x4c, 0x2d, 0x64, YYOTHERS, 0x2b, YYOTHERS, 0x4e,
0x4e, 0x78, 0x5f, 0x79, 0x61, 0x7a, 0x62, 0x7b,
YYOTHERS, 0x60, YYOTHERS, 0x2c, 0xa, 0x2d, 0x2a, 0x2d,
0x2d, 0x2d, YYOTHERS, 0x61, 0x2b, 0x4f, YYOTHERS, 0x2e,
YYOTHERS, 0x62, YYOTHERS, 0x63, YYOTHERS, 0x2f, YYOTHERS, 0x30,
YYOTHERS, 0x6c, 0x6c, 0x86, 0x85, 0x97, YYOTHERS, 0x73,
0x82, 0x95, 0x92, 0xa2, YYOTHERS, 0x7f, 0x23, 0x48,
0x70, 0x89, 0x72, 0x8c, 0x94, 0xa4, 0x9c, 0xaa,
0xad, 0xae, YYOTHERS, 0x47, 0xae, 0xaf, YYOTHERS, 0x9d,
0x83, 0x96, YYOTHERS, 0x84, YYOTHERS, 0x80, 0x8f, 0xa0,
YYOTHERS, 0x81, 0x3b, 0x6a, 0x43, 0x6e, 0x66, 0x6a,
0x67, 0x7d, 0x6c, 0x6a, YYOTHERS, 0x35 
};
readonly unsigned short yypa[176] = {
0, 2, 30, 34, 38, 40, 42, 46,
52, 58, 62, 86, 88, 90, 114, 138,
138, 142, 142, 138, 144, 146, 148, 148,
138, 152, 154, 138, 156, 138, 138, 138,
144, 144, 160, 164, 168, 170, 172, 176,
178, 182, 184, 190, 194, 230, 236, 238,
240, 242, 244, 246, 248, 250, 254, 256,
258, 260, 262, 264, 272, 276, 278, 280,
282, 284, 286, 138, 288, 292, 296, 298,
302, 306, 310, 138, 312, 314, 316, 352,
354, 356, 358, 360, 362, 364, 366, 368,
370, 372, 374, 376, 378, 380, 382, 384,
420, 422, 458, 494, 496, 498, 500, 506,
510, 514, 518, 522, 526, 534, 536, 138,
560, 568, 574, 578, 582, 584, 586, 588,
592, 594, 596, 598, 600, 602, 606, 610,
614, 618, 510, 522, 622, 292, 624, 628,
630, 632, 138, 638, 640, 642, 144, 644,
648, 138, 510, 138, 650, 656, 660, 662,
666, 138, 138, 144, 668, 672, 676, 678,
682, 684, 686, 688, 690, 692, 694, 696,
698, 288, 700, 702, 704, 560, 706, 712
};
readonly unsigned int yyact[714] = {
0x2002, YYOTHERS, 0x2, YYEOFVAL, 0x4, 0xa, 0x2022, 0x101,
0x2022, 0x102, 0x2022, 0x10b, 0x2022, 0x10c, 0x2022, 0x10e,
0x2022, 0x10f, 0x2022, 0x111, 0x2022, 0x114, 0x2022, 0x116,
0x2022, 0x117, 0x2022, 0x28, 0x3, YYOTHERS, 0x4000, YYEOFVAL,
0x6000, YYOTHERS, 0xb, 0xa, 0x6000, YYOTHERS, 0x2003, YYOTHERS,
0x2001, YYOTHERS, 0xc, 0xa, 0x6000, YYOTHERS, 0xd, 0x3b,
0xe, 0x26, 0x2017, YYOTHERS, 0xf, 0x100, 0x10, 0x112,
0x201c, YYOTHERS, 0x13, 0x7c, 0x2020, YYOTHERS, 0x15, 0x101,
0x16, 0x102, 0x17, 0x10b, 0x18, 0x10c, 0x19, 0x10e,
0x1a, 0x10f, 0x1b, 0x111, 0x1c, 0x114, 0x1d, 0x116,
0x1e, 0x117, 0x1f, 0x28, 0x6000, YYOTHERS, 0x2005, YYOTHERS,
0x2004, YYOTHERS, 0x2022, 0x101, 0x2022, 0x102, 0x2022, 0x10b,
0x2022, 0x10c, 0x2022, 0x10e, 0x2022, 0x10f, 0x2022, 0x111,
0x2022, 0x114, 0x2022, 0x116, 0x2022, 0x117, 0x2022, 0x28,
0x2019, YYOTHERS, 0x2022, 0x101, 0x2022, 0x102, 0x2022, 0x10b,
0x2022, 0x10c, 0x2022, 0x10e, 0x2022, 0x10f, 0x2022, 0x111,
0x2022, 0x114, 0x2022, 0x116, 0x2022, 0x117, 0x2022, 0x28,
0x2018, YYOTHERS, 0x33, 0xa, 0x2073, YYOTHERS, 0x2022, YYOTHERS,
0x2022, YYOTHERS, 0x2053, YYOTHERS, 0x1a, 0x10f, 0x6000, YYOTHERS,
0x204f, YYOTHERS, 0x2051, YYOTHERS, 0x1a, 0x10f, 0x2028, YYOTHERS,
0x206c, 0x29, 0x2022, YYOTHERS, 0x206c, 0x103, 0x2022, YYOTHERS,
0x2021, YYOTHERS, 0x2023, YYOTHERS, 0x19, 0x10e, 0x2024, YYOTHERS,
0x2026, YYOTHERS, 0x4b, 0x113, 0x2050, YYOTHERS, 0x2029, YYOTHERS,
0x15, 0x101, 0x19, 0x10e, 0x2032, YYOTHERS, 0x1a, 0x10f,
0x202f, YYOTHERS, 0x15, 0x101, 0x50, 0x102, 0x51, 0x104,
0x52, 0x105, 0x53, 0x107, 0x54, 0x108, 0x55, 0x109,
0x56, 0x10a, 0x57, 0x10b, 0x58, 0x10c, 0x59, 0x10d,
0x19, 0x10e, 0x5a, 0x10f, 0x5b, 0x114, 0x5c, 0x115,
0x5d, 0x116, 0x5e, 0x117, 0x2031, YYOTHERS, 0x15, 0x101,
0x19, 0x10e, 0x2034, YYOTHERS, 0x2036, YYOTHERS, 0x204e, YYOTHERS,
0x2052, YYOTHERS, 0x201b, YYOTHERS, 0x201a, YYOTHERS, 0x2074, YYOTHERS,
0x2011, YYOTHERS, 0x65, 0xa, 0x2072, YYOTHERS, 0x2010, YYOTHERS,
0x201d, YYOTHERS, 0x201e, YYOTHERS, 0x2012, YYOTHERS, 0x201f, YYOTHERS,
0x66, 0x10d, 0x33, 0xa, 0x67, 0x3b, 0x6000, YYOTHERS,
0x6b, 0x10d, 0x205d, YYOTHERS, 0x2006, YYOTHERS, 0x2014, YYOTHERS,
0x2027, YYOTHERS, 0x200b, YYOTHERS, 0x200a, YYOTHERS, 0x2013, YYOTHERS,
0x6f, 0x115, 0x6000, YYOTHERS, 0x71, 0x104, 0x2067, YYOTHERS,
0x206b, YYOTHERS, 0x74, 0x29, 0x6000, YYOTHERS, 0x75, 0x103,
0x6000, YYOTHERS, 0x19, 0x10e, 0x2025, YYOTHERS, 0x202b, YYOTHERS,
0x2033, YYOTHERS, 0x2050, YYOTHERS, 0x15, 0x101, 0x50, 0x102,
0x51, 0x104, 0x52, 0x105, 0x53, 0x107, 0x54, 0x108,
0x55, 0x109, 0x56, 0x10a, 0x57, 0x10b, 0x58, 0x10c,
0x59, 0x10d, 0x19, 0x10e, 0x5a, 0x10f, 0x5b, 0x114,
0x5c, 0x115, 0x5d, 0x116, 0x5e, 0x117, 0x202e, YYOTHERS,
0x2037, YYOTHERS, 0x2040, YYOTHERS, 0x2041, YYOTHERS, 0x2042, YYOTHERS,
0x2043, YYOTHERS, 0x2044, YYOTHERS, 0x2045, YYOTHERS, 0x2046, YYOTHERS,
0x2047, YYOTHERS, 0x2048, YYOTHERS, 0x2049, YYOTHERS, 0x203f, YYOTHERS,
0x204a, YYOTHERS, 0x204b, YYOTHERS, 0x204c, YYOTHERS, 0x204d, YYOTHERS,
0x15, 0x101, 0x50, 0x102, 0x51, 0x104, 0x52, 0x105,
0x53, 0x107, 0x54, 0x108, 0x55, 0x109, 0x56, 0x10a,
0x57, 0x10b, 0x58, 0x10c, 0x59, 0x10d, 0x19, 0x10e,
0x5a, 0x10f, 0x5b, 0x114, 0x5c, 0x115, 0x5d, 0x116,
0x5e, 0x117, 0x2038, YYOTHERS, 0x2030, YYOTHERS, 0x15, 0x101,
0x50, 0x102, 0x51, 0x104, 0x52, 0x105, 0x53, 0x107,
0x54, 0x108, 0x55, 0x109, 0x56, 0x10a, 0x57, 0x10b,
0x58, 0x10c, 0x59, 0x10d, 0x19, 0x10e, 0x5a, 0x10f,
0x5b, 0x114, 0x5c, 0x115, 0x5d, 0x116, 0x5e, 0x117,
0x203c, YYOTHERS, 0x15, 0x101, 0x50, 0x102, 0x51, 0x104,
0x52, 0x105, 0x53, 0x107, 0x54, 0x108, 0x55, 0x109,
0x56, 0x10a, 0x57, 0x10b, 0x58, 0x10c, 0x59, 0x10d,
0x19, 0x10e, 0x5a, 0x10f, 0x5b, 0x114, 0x5c, 0x115,
0x5d, 0x116, 0x5e, 0x117, 0x203a, YYOTHERS, 0x203e, YYOTHERS,
0x2035, YYOTHERS, 0x2075, YYOTHERS, 0x33, 0xa, 0x67, 0x3b,
0x200e, YYOTHERS, 0x33, 0xa, 0x2070, YYOTHERS, 0x1a, 0x10f,
0x2062, YYOTHERS, 0x66, 0x10d, 0x6000, YYOTHERS, 0x65, 0xa,
0x206f, YYOTHERS, 0x1a, 0x10f, 0x205f, YYOTHERS, 0x71, 0x104,
0x33, 0xa, 0x67, 0x3b, 0x2067, YYOTHERS, 0x206e, YYOTHERS,
0x65, 0xa, 0x2072, 0x103, 0x2072, 0x104, 0x2072, 0x105,
0x2072, 0x106, 0x2072, 0x107, 0x2072, 0x108, 0x2072, 0x109,
0x2072, 0x10a, 0x2072, 0x115, 0x2072, 0x29, 0x2022, YYOTHERS,
0x206c, 0x107, 0x206c, 0x108, 0x206c, 0x10a, 0x2022, YYOTHERS,
0x33, 0xa, 0x8a, 0x3b, 0x2073, YYOTHERS, 0x206c, 0x105,
0x2022, YYOTHERS, 0x8d, 0x105, 0x6000, YYOTHERS, 0x205a, YYOTHERS,
0x205b, YYOTHERS, 0x202c, YYOTHERS, 0x1b, 0x111, 0x6000, YYOTHERS,
0x202d, YYOTHERS, 0x2039, YYOTHERS, 0x203d, YYOTHERS, 0x203b, YYOTHERS,
0x200f, YYOTHERS, 0x65, 0xa, 0x2071, YYOTHERS, 0x8f, 0x7c,
0x2065, YYOTHERS, 0x90, 0x109, 0x6000, YYOTHERS, 0x91, 0x106,
0x2061, YYOTHERS, 0x93, 0x29, 0x6000, YYOTHERS, 0x205c, YYOTHERS,
0x98, 0x105, 0x6000, YYOTHERS, 0x206d, YYOTHERS, 0x2007, YYOTHERS,
0x99, 0x107, 0x9a, 0x108, 0x206a, YYOTHERS, 0x200c, YYOTHERS,
0x2066, YYOTHERS, 0x2058, YYOTHERS, 0x1a, 0x10f, 0x6000, YYOTHERS,
0x2057, YYOTHERS, 0x206c, 0x106, 0x206c, 0x109, 0x2022, YYOTHERS,
0xa5, 0x109, 0x6000, YYOTHERS, 0x205e, YYOTHERS, 0xa6, 0x105,
0x6000, YYOTHERS, 0x2055, YYOTHERS, 0x206c, 0x10a, 0x2022, YYOTHERS,
0xab, 0x10a, 0x6000, YYOTHERS, 0x200d, YYOTHERS, 0xac, 0x103,
0x6000, YYOTHERS, 0x2064, YYOTHERS, 0x2016, YYOTHERS, 0x2060, YYOTHERS,
0x2015, YYOTHERS, 0x2063, YYOTHERS, 0x2056, YYOTHERS, 0x2054, YYOTHERS,
0x2008, YYOTHERS, 0x2009, YYOTHERS, 0x2069, YYOTHERS, 0x2059, YYOTHERS,
0x202a, YYOTHERS, 0x99, 0x107, 0x9a, 0x108, 0x206a, YYOTHERS,
0x2068, YYOTHERS 
};
#define YYNOCHAR (-1000)
#define	yyerrok	yyerrflag=0
#define	yyclearin	yylval=YYNOCHAR
int yystack[YYMAXDEPTH];
YYSTYPE yyvstack[YYMAXDEPTH], *yyv;
int yychar;

#ifdef YYDEBUG
int yydebug = 1;	/* No sir, not in the BSS */
#include <stdio.h>
#endif

short yyerrflag;
int *yys;

yyparse()
{
	register YYSTYPE *yypvt;
	int act;
	register unsigned *ip, yystate;
	int pno;
	yystate = 0;
	yychar = YYNOCHAR;
	yyv = &yyvstack[-1];
	yys = &yystack[-1];

stack:
	if( ++yys >= &yystack[YYMAXDEPTH] ) {
		write(2, "Stack overflow\n", 15);
		exit(1);
	}
	*yys = yystate;
	*++yyv = yyval;
#ifdef YYDEBUG
	if( yydebug )
		fprintf(stdout, "Stack state %d, char %d\n", yystate, yychar);
#endif

read:
	ip = &yyact[yypa[yystate]];
	if( ip[1] != YYNOCHAR ) {
		if( yychar == YYNOCHAR ) {
			yychar = yylex();
#ifdef YYDEBUG
			if( yydebug )
				fprintf(stdout, "lex read char %d, val %d\n", yychar, yylval);
#endif
		}
		while (ip[1]!=YYNOCHAR) {
			if (ip[1]==yychar)
				break;
			ip += 2;
		}
	}
	act = ip[0];
	switch( act>>YYACTSH ) {
	case YYSHIFTACT:
		if( ip[1]==YYNOCHAR )
			goto YYerract;
		if( yychar != -1 )
			yychar = YYNOCHAR; /* dont throw away EOF */
		yystate = act&YYAMASK;
		yyval = yylval;
#ifdef YYDEBUG
		if( yydebug )
			fprintf(stdout, "shift %d\n", yystate);
#endif
		if( yyerrflag )
			--yyerrflag;
		goto stack;

	case YYACCEPTACT:
#ifdef YYDEBUG
		if( yydebug )
			fprintf(stdout, "accept\n");
#endif
		return(0);

	case YYERRACT:
	YYerract:
		switch (yyerrflag) {
		case 0:
			yyerror("Syntax error");

		case 1:
		case 2:

			yyerrflag = 3;
			while( yys >= & yystack[0] ) {
				ip = &yyact[yypa[*yys]];
				while( ip[1]!=YYNOCHAR )
					ip += 2;
				if( (*ip&~YYAMASK) == (YYSHIFTACT<<YYACTSH) ) {
					yystate = *ip&YYAMASK;
					goto stack;
				}
#ifdef YYDEBUG
				if( yydebug )
					fprintf(stderr, "error recovery leaves state %d, uncovers %d\n", *yys, yys[-1]);
#endif
				yys--;
				yyv--;
			}
#ifdef YYDEBUG
			if( yydebug )
				fprintf(stderr, "no shift on error; abort\n");
#endif
			return(1);

		case 3:
#ifdef YYDEBUG
			if( yydebug )
				fprintf(stderr, "Error recovery clobbers char %o\n", yychar);
#endif
			if( yychar==YYEOFVAL )
				return(1);
			yychar = YYNOCHAR;
			goto read;
		}

	case YYREDACT:
		pno = act&YYAMASK;
#ifdef YYDEBUG
		if( yydebug )
			fprintf(stdout, "reduce %d\n", pno);
#endif
		yypvt = yyv;
		yyv -= yypn[pno];
		yys -= yypn[pno];
		yyval = yyv[1];
		switch(pno) {

case 3: {

#line 71 "sh.y"

		sesp->s_node = NULL;
		reset(RCMD);
		NOTREACHED;
	}break;

case 4: {

#line 77 "sh.y"

		sesp->s_node = yypvt[-1].yu_node;
		reset(errflag ? RERR : RCMD);
		NOTREACHED;
	}break;

case 5: {

#line 82 "sh.y"

		keyflush();
		keyflag = 1;
		reset(RERR);
		NOTREACHED;
	}break;

case 10: {

#line 98 "sh.y"
	yyval.yu_nval = NWHILE;	}break;

case 11: {

#line 99 "sh.y"
	yyval.yu_nval = NUNTIL;	}break;

case 23: {

#line 121 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
	}break;

case 24: {

#line 124 "sh.y"

		yyval.yu_node = node(NBACK, yypvt[-1].yu_node, NULL);
	}break;

case 25: {

#line 127 "sh.y"

		yyval.yu_node = yypvt[-1].yu_node;
	}break;

case 26: {

#line 130 "sh.y"

		yyval.yu_node = node(NBACK, yypvt[-2].yu_node, yypvt[0].yu_node);
	}break;

case 27: {

#line 133 "sh.y"

		yyval.yu_node = node(NLIST, yypvt[-2].yu_node, yypvt[0].yu_node);
	}break;

case 28: {

#line 139 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
	}break;

case 29: {

#line 142 "sh.y"

		yyval.yu_node = node(NORF, yypvt[-2].yu_node, yypvt[0].yu_node);
	}break;

case 30: {

#line 145 "sh.y"

		yyval.yu_node = node(NANDF, yypvt[-2].yu_node, yypvt[0].yu_node);
	}break;

case 31: {

#line 151 "sh.y"

		yyval.yu_node = node(NPIPE, yypvt[-2].yu_node, yypvt[0].yu_node);
	}break;

case 32: {

#line 154 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
	}break;

case 33: {

#line 166 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
		keypop ();
	}break;

case 34: {

#line 172 "sh.y"

		keypush ();
		keyflag = 1;
	}break;

case 35: {

#line 179 "sh.y"

		yyval.yu_node = node (NCOMS, yypvt[0].yu_node, NULL);
	}break;

case 36: {

#line 182 "sh.y"

		yyval.yu_node = node (NCOMS, yypvt[0].yu_node, NULL);
	}break;

case 37: {

#line 185 "sh.y"

		yyval.yu_node = node (NCOMS, yypvt[-1].yu_node->n_next = yypvt[0].yu_node, NULL);
	}break;

case 38: {

#line 188 "sh.y"

		yyval.yu_node = node (NCOMS, yypvt[0].yu_node, NULL);
	}break;

case 39: {

#line 191 "sh.y"

		yyval.yu_node = node (NRET, yypvt[0].yu_strp, NULL);
	}break;

case 40: {

#line 194 "sh.y"

		yyval.yu_node = node (NRET, "", NULL);
	}break;

case 41: {

#line 200 "sh.y"

		yyval.yu_node = node (NCTRL, yypvt[0].yu_node, NULL);
	}break;

case 42: {

#line 206 "sh.y"

		yyval.yu_node = node (NCTRL, node (NFUNC, yypvt[-5].yu_strp, yypvt[-1].yu_node), NULL);
	}break;

case 43: {

#line 212 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
	}break;

case 44: {

#line 215 "sh.y"

		(yyval.yu_node = yypvt[-1].yu_node)->n_next = yypvt[0].yu_node;
	}break;

case 45: {

#line 221 "sh.y"

		((yyval.yu_node = yypvt[-2].yu_node)->n_next = yypvt[-1].yu_node)->n_next = yypvt[0].yu_node;
	}break;

case 46: {

#line 224 "sh.y"

		(yyval.yu_node = yypvt[-1].yu_node)->n_next = yypvt[0].yu_node;
	}break;

case 47: {

#line 227 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
	}break;

case 48: {

#line 230 "sh.y"

		(yyval.yu_node = yypvt[-1].yu_node)->n_next = yypvt[0].yu_node;
	}break;

case 49: {

#line 233 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
	}break;

case 50: {

#line 239 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
	}break;

case 51: {

#line 242 "sh.y"

		(yyval.yu_node = yypvt[-1].yu_node)->n_next = yypvt[0].yu_node;
	}break;

case 52: {

#line 245 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
	}break;

case 53: {

#line 248 "sh.y"

		(yyval.yu_node = yypvt[-1].yu_node)->n_next = yypvt[0].yu_node;
	}break;

case 54: {

#line 254 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
		keyflag = 0;
	}break;

case 55: {

#line 261 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
		keyflag = 0;
	}break;

case 56: {

#line 276 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
	}break;

case 57: {

#line 279 "sh.y"

		(yyval.yu_node = yypvt[-1].yu_node)->n_next = yypvt[0].yu_node;
	}break;

case 58: {

#line 282 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
	}break;

case 59: {

#line 285 "sh.y"

		(yyval.yu_node = yypvt[-1].yu_node)->n_next = yypvt[0].yu_node;
	}break;

case 60: {

#line 288 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
	}break;

case 61: {

#line 291 "sh.y"

		(yyval.yu_node = yypvt[-1].yu_node)->n_next = yypvt[0].yu_node;
	}break;

case 62: {

#line 297 "sh.y"

		yyval.yu_node = node (NARGS, duplstr (strt, 0), NULL);
	}break;

case 78: {

#line 388 "sh.y"

		yyval.yu_node = node (NIORS, yypvt[0].yu_strp, NULL);
	}break;

case 79: {

#line 393 "sh.y"

		yyval.yu_strp = duplstr (strt, 0);
	}break;

case 80: {

#line 398 "sh.y"

		yyval.yu_node = node (NARGS, yypvt[0].yu_strp, NULL);
	}break;

case 81: {

#line 403 "sh.y"

		yyval.yu_strp = duplstr (strt, 0);
	}break;

case 82: {

#line 409 "sh.y"

		yyval.yu_node = node (NASSG, yypvt[0].yu_strp, NULL);
	}break;

case 83: {

#line 414 "sh.y"

		yyval.yu_strp = duplstr (strt, 0);
	}break;

case 84: {

#line 420 "sh.y"

		yyval.yu_node = node(NFOR, yypvt[-4].yu_strp, node(NFOR2, yypvt[-3].yu_node, node(NLIST, yypvt[-1].yu_node, NULL)));
		yyval.yu_node->n_next->n_next->n_next = yyval.yu_node->n_next;
	}break;

case 85: {

#line 424 "sh.y"

		yyval.yu_node = node(NFOR, yypvt[-3].yu_strp, node(NFOR2, yypvt[-2].yu_node, node(NLIST, yypvt[-1].yu_node, NULL)));
		yyval.yu_node->n_next->n_next->n_next = yyval.yu_node->n_next;
	}break;

case 86: {

#line 428 "sh.y"

		yyval.yu_node = node(NCASE, yypvt[-4].yu_strp, yypvt[-1].yu_node);
	}break;

case 87: {

#line 431 "sh.y"

		yyval.yu_node = node(NCASE, yypvt[-3].yu_strp, yypvt[-1].yu_node);
	}break;

case 88: {

#line 434 "sh.y"

		yyval.yu_node = node(yypvt[-3].yu_nval, yypvt[-2].yu_node, node(NLIST, yypvt[-1].yu_node, NULL));
		yyval.yu_node->n_next->n_next = yyval.yu_node;
	}break;

case 89: {

#line 438 "sh.y"

		yyval.yu_node = node(NIF, node(NNULL, yypvt[-4].yu_node, yypvt[-2].yu_node), yypvt[-1].yu_node);
	}break;

case 90: {

#line 441 "sh.y"

		yyval.yu_node = node(NPARN, yypvt[-1].yu_node, NULL);
	}break;

case 91: {

#line 444 "sh.y"

		yyval.yu_node = node(NBRAC, yypvt[-1].yu_node, NULL);
	}break;

case 92: {

#line 450 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
	}break;

case 93: {

#line 453 "sh.y"

		yyval.yu_node = node(NARGS, "\"$@\"", NULL);
	}break;

case 94: {

#line 459 "sh.y"

		yyval.yu_node = node(NARGS, yypvt[-1].yu_strp, yypvt[0].yu_node);
	}break;

case 95: {

#line 462 "sh.y"

		yyval.yu_node = NULL;
	}break;

case 96: {

#line 468 "sh.y"

		register NODE *np;

		for (np=yypvt[-2].yu_node; np->n_next; np=np->n_next)
			;
		np->n_next = yypvt[0].yu_node;
		yyval.yu_node = yypvt[-2].yu_node;
	}break;

case 97: {

#line 476 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
	}break;

case 98: {

#line 479 "sh.y"

		yyval.yu_node = NULL;
	}break;

case 99: {

#line 485 "sh.y"

		yyval.yu_node = node(NCASE2, yypvt[0].yu_node, yypvt[-2].yu_node);
	}break;

case 100: {

#line 491 "sh.y"

		yyval.yu_node = node(NCASE3, yypvt[-2].yu_strp, yypvt[0].yu_node);
	}break;

case 101: {

#line 494 "sh.y"

		yyval.yu_node = node(NCASE3, yypvt[0].yu_strp, NULL);
	}break;

case 102: {

#line 500 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
	}break;

case 103: {

#line 503 "sh.y"

		yyval.yu_node = NULL;
	}break;

case 104: {

#line 509 "sh.y"

		yyval.yu_node = node(NIF, node(NNULL, yypvt[-3].yu_node, yypvt[-1].yu_node), yypvt[0].yu_node);
	}break;

case 105: {

#line 512 "sh.y"

		yyval.yu_node = node(NELSE, yypvt[0].yu_node, NULL);
	}break;

case 106: {

#line 515 "sh.y"

		yyval.yu_node = NULL;
	}break;

case 107: {

#line 521 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
	}break;

case 108: {

#line 525 "sh.y"

		yyval.yu_node = NULL;
	}break;

case 109: {

#line 531 "sh.y"

		yyval.yu_node = node(NLIST, yypvt[-2].yu_node, yypvt[0].yu_node);
	}break;

case 110: {

#line 534 "sh.y"

		yyval.yu_node = yypvt[-1].yu_node;
	}break;

		}
		ip = &yygo[ yypgo[yypdnt[pno]] ];
		while( *ip!=*yys && *ip!=YYNOCHAR )
			ip += 2;
		yystate = ip[1];
		goto stack;
	}
}




