
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

#line 570 "sh.y"

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
CLOSE {
		$$ = node(NWPIPE, $2, NULL);
	}
 *
 */
#ifdef YYTNAMES
struct yytname yytnames[33] =
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
#define	YYACTSH		13
#define	YYAMASK		017777
#define	YYSHIFTACT	0
#define	YYREDACT	1
#define	YYACCEPTACT	2
#define	YYERRACT	3
#define	YYGOTO		4
#define	YYPACTION	5
#define	YYEOFVAL	(-1)
#define	YYERRVAL	(-2)
#define	YYOTHERS	(-1000)

unsigned char yypdnt[117] = {
0, 1, 1, 1, 1, 3, 5, 6,
7, 8, 8, 9, 9, 10, 10, 12,
13, 14, 15, 16, 17, 18, 2, 2,
2, 2, 2, 19, 19, 19, 20, 20,
21, 22, 23, 23, 23, 23, 23, 23,
25, 27, 26, 26, 24, 24, 24, 24,
24, 32, 32, 32, 32, 35, 33, 34,
34, 34, 34, 34, 34, 38, 39, 39,
39, 39, 39, 39, 39, 39, 39, 39,
39, 39, 39, 39, 39, 31, 40, 37,
28, 36, 41, 29, 29, 29, 29, 29,
29, 29, 29, 42, 42, 47, 47, 44,
44, 44, 48, 49, 49, 43, 43, 46,
46, 46, 45, 45, 30, 30, 11, 11,
11, 4, 4, 50, 50 
};
unsigned char yypn[117] = {
2, 1, 1, 2, 0, 2, 2, 2,
2, 2, 2, 2, 3, 1, 2, 2,
2, 2, 2, 2, 2, 2, 1, 2,
2, 3, 3, 1, 3, 3, 3, 1,
2, 0, 1, 1, 2, 1, 2, 1,
1, 6, 1, 2, 3, 2, 1, 2,
1, 1, 2, 1, 2, 1, 1, 1,
2, 1, 2, 1, 2, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 6, 5, 6, 5, 4,
6, 3, 3, 2, 0, 2, 0, 3,
1, 0, 3, 3, 1, 2, 0, 5,
2, 0, 1, 0, 3, 2, 1, 1,
2, 1, 0, 1, 2 
};
unsigned char yypgo[51] = {
0, 0, 2, 10, 12, 46, 50, 52,
54, 56, 58, 62, 68, 70, 72, 74,
76, 80, 82, 84, 90, 94, 96, 98,
100, 102, 104, 106, 108, 124, 126, 138,
150, 156, 158, 168, 170, 178, 182, 184,
186, 188, 190, 192, 198, 204, 218, 222,
226, 228, 232 
};
unsigned int yygo[244] = {
YYOTHERS, 0x3, 0x0, 0x4, 0xb, 0x2f, 0xc, 0x30,
YYOTHERS, 0x41, YYOTHERS, 0x1e, 0xe, 0x34, 0x11, 0x37,
0x16, 0x3b, 0x19, 0x3c, 0x1b, 0x3e, 0x1c, 0x3f,
0x1d, 0x40, 0x41, 0x6b, 0x49, 0x75, 0x6d, 0x86,
0x6f, 0x89, 0x88, 0x9c, 0x8f, 0x9f, 0x91, 0xa1,
0x97, 0xa5, 0x98, 0xa6, YYOTHERS, 0x32, 0xa7, 0xab,
YYOTHERS, 0x6e, YYOTHERS, 0x99, YYOTHERS, 0x9a, YYOTHERS, 0x1f,
YYOTHERS, 0x70, 0x67, 0x80, YYOTHERS, 0x66, 0x64, 0x7a,
0x6a, 0x83, YYOTHERS, 0x67, YYOTHERS, 0xf, YYOTHERS, 0x10,
YYOTHERS, 0x12, YYOTHERS, 0x20, 0x75, 0x8c, YYOTHERS, 0x21,
YYOTHERS, 0x92, YYOTHERS, 0x90, 0xf, 0x35, 0x10, 0x36,
YYOTHERS, 0x5, 0x12, 0x38, YYOTHERS, 0x6, YYOTHERS, 0x7,
YYOTHERS, 0x8, YYOTHERS, 0x22, YYOTHERS, 0x23, YYOTHERS, 0x24,
YYOTHERS, 0x47, YYOTHERS, 0x25, 0x8, 0x26, 0x14, 0x39,
0x15, 0x3a, 0x1a, 0x3d, 0x29, 0x4b, 0x69, 0x81,
0x81, 0x81, YYOTHERS, 0x7c, YYOTHERS, 0x27, 0x1e, 0x42,
0x1f, 0x43, 0x6c, 0x85, 0x8c, 0x9d, 0x99, 0xa7,
YYOTHERS, 0x44, 0x8, 0x28, 0x24, 0x48, 0x28, 0x28,
0x2b, 0x28, 0x47, 0x74, YYOTHERS, 0x5d, 0x28, 0x4a,
0x2b, 0x62, YYOTHERS, 0x29, YYOTHERS, 0x4c, 0x4c, 0x76,
0x5d, 0x77, 0x5f, 0x78, 0x60, 0x79, YYOTHERS, 0x5e,
YYOTHERS, 0x2a, 0x8, 0x2b, 0x28, 0x2b, 0x2b, 0x2b,
YYOTHERS, 0x5f, 0x29, 0x4d, YYOTHERS, 0x2c, YYOTHERS, 0x60,
YYOTHERS, 0x61, YYOTHERS, 0x2d, YYOTHERS, 0x2e, YYOTHERS, 0x6a,
0x6a, 0x84, 0x83, 0x95, YYOTHERS, 0x71, 0x80, 0x93,
0x90, 0xa0, YYOTHERS, 0x7d, 0x21, 0x46, 0x6e, 0x87,
0x70, 0x8a, 0x92, 0xa2, 0x9a, 0xa8, 0xab, 0xac,
YYOTHERS, 0x45, 0xac, 0xad, YYOTHERS, 0x9b, 0x81, 0x94,
YYOTHERS, 0x82, YYOTHERS, 0x7e, 0x8d, 0x9e, YYOTHERS, 0x7f,
0x39, 0x68, 0x41, 0x6c, 0x64, 0x68, 0x65, 0x7b,
0x6a, 0x68, YYOTHERS, 0x33 
};
unsigned short yypa[174] = {
0, 28, 32, 34, 38, 40, 46, 52,
56, 80, 82, 86, 110, 134, 134, 138,
138, 134, 140, 142, 144, 144, 134, 148,
150, 134, 152, 134, 134, 134, 140, 140,
156, 160, 164, 166, 168, 172, 174, 178,
180, 186, 190, 226, 232, 234, 236, 238,
240, 242, 244, 246, 250, 252, 254, 256,
258, 260, 268, 272, 274, 276, 278, 280,
282, 134, 284, 288, 292, 294, 298, 302,
306, 134, 308, 310, 312, 348, 350, 352,
354, 356, 358, 360, 362, 364, 366, 368,
370, 372, 374, 376, 378, 380, 416, 418,
454, 490, 492, 494, 496, 502, 506, 510,
514, 518, 522, 530, 532, 134, 556, 564,
570, 574, 578, 580, 582, 584, 588, 590,
592, 594, 596, 598, 602, 606, 610, 614,
506, 518, 618, 288, 620, 624, 626, 628,
134, 634, 636, 638, 140, 640, 644, 134,
506, 134, 646, 652, 656, 658, 662, 134,
134, 140, 664, 668, 672, 674, 678, 680,
682, 684, 686, 688, 690, 692, 694, 284,
696, 698, 700, 556, 702, 708 
};
unsigned int yyact[710] = {
0x2, 0xa, 0x2004, YYEOFVAL, 0x2021, 0x101, 0x2021, 0x102,
0x2021, 0x10b, 0x2021, 0x10c, 0x2021, 0x10e, 0x2021, 0x10f,
0x2021, 0x111, 0x2021, 0x114, 0x2021, 0x116, 0x2021, 0x117,
0x2021, 0x28, 0x1, YYOTHERS, 0x9, 0xa, 0x6000, YYOTHERS,
0x2001, YYOTHERS, 0xa, YYEOFVAL, 0x6000, YYOTHERS, 0x2002, YYOTHERS,
0xb, 0x3b, 0xc, 0x26, 0x2016, YYOTHERS, 0xd, 0x100,
0xe, 0x112, 0x201b, YYOTHERS, 0x11, 0x7c, 0x201f, YYOTHERS,
0x13, 0x101, 0x14, 0x102, 0x15, 0x10b, 0x16, 0x10c,
0x17, 0x10e, 0x18, 0x10f, 0x19, 0x111, 0x1a, 0x114,
0x1b, 0x116, 0x1c, 0x117, 0x1d, 0x28, 0x6000, YYOTHERS,
0x2003, YYOTHERS, 0x4000, YYEOFVAL, 0x6000, YYOTHERS, 0x2021, 0x101,
0x2021, 0x102, 0x2021, 0x10b, 0x2021, 0x10c, 0x2021, 0x10e,
0x2021, 0x10f, 0x2021, 0x111, 0x2021, 0x114, 0x2021, 0x116,
0x2021, 0x117, 0x2021, 0x28, 0x2018, YYOTHERS, 0x2021, 0x101,
0x2021, 0x102, 0x2021, 0x10b, 0x2021, 0x10c, 0x2021, 0x10e,
0x2021, 0x10f, 0x2021, 0x111, 0x2021, 0x114, 0x2021, 0x116,
0x2021, 0x117, 0x2021, 0x28, 0x2017, YYOTHERS, 0x31, 0xa,
0x2072, YYOTHERS, 0x2021, YYOTHERS, 0x2021, YYOTHERS, 0x2052, YYOTHERS,
0x18, 0x10f, 0x6000, YYOTHERS, 0x204e, YYOTHERS, 0x2050, YYOTHERS,
0x18, 0x10f, 0x2027, YYOTHERS, 0x206b, 0x29, 0x2021, YYOTHERS,
0x206b, 0x103, 0x2021, YYOTHERS, 0x2020, YYOTHERS, 0x2022, YYOTHERS,
0x17, 0x10e, 0x2023, YYOTHERS, 0x2025, YYOTHERS, 0x49, 0x113,
0x204f, YYOTHERS, 0x2028, YYOTHERS, 0x13, 0x101, 0x17, 0x10e,
0x2031, YYOTHERS, 0x18, 0x10f, 0x202e, YYOTHERS, 0x13, 0x101,
0x4e, 0x102, 0x4f, 0x104, 0x50, 0x105, 0x51, 0x107,
0x52, 0x108, 0x53, 0x109, 0x54, 0x10a, 0x55, 0x10b,
0x56, 0x10c, 0x57, 0x10d, 0x17, 0x10e, 0x58, 0x10f,
0x59, 0x114, 0x5a, 0x115, 0x5b, 0x116, 0x5c, 0x117,
0x2030, YYOTHERS, 0x13, 0x101, 0x17, 0x10e, 0x2033, YYOTHERS,
0x2035, YYOTHERS, 0x204d, YYOTHERS, 0x2051, YYOTHERS, 0x201a, YYOTHERS,
0x2019, YYOTHERS, 0x2073, YYOTHERS, 0x2010, YYOTHERS, 0x63, 0xa,
0x2071, YYOTHERS, 0x200f, YYOTHERS, 0x201c, YYOTHERS, 0x201d, YYOTHERS,
0x2011, YYOTHERS, 0x201e, YYOTHERS, 0x64, 0x10d, 0x31, 0xa,
0x65, 0x3b, 0x6000, YYOTHERS, 0x69, 0x10d, 0x205c, YYOTHERS,
0x2005, YYOTHERS, 0x2013, YYOTHERS, 0x2026, YYOTHERS, 0x200a, YYOTHERS,
0x2009, YYOTHERS, 0x2012, YYOTHERS, 0x6d, 0x115, 0x6000, YYOTHERS,
0x6f, 0x104, 0x2066, YYOTHERS, 0x206a, YYOTHERS, 0x72, 0x29,
0x6000, YYOTHERS, 0x73, 0x103, 0x6000, YYOTHERS, 0x17, 0x10e,
0x2024, YYOTHERS, 0x202a, YYOTHERS, 0x2032, YYOTHERS, 0x204f, YYOTHERS,
0x13, 0x101, 0x4e, 0x102, 0x4f, 0x104, 0x50, 0x105,
0x51, 0x107, 0x52, 0x108, 0x53, 0x109, 0x54, 0x10a,
0x55, 0x10b, 0x56, 0x10c, 0x57, 0x10d, 0x17, 0x10e,
0x58, 0x10f, 0x59, 0x114, 0x5a, 0x115, 0x5b, 0x116,
0x5c, 0x117, 0x202d, YYOTHERS, 0x2036, YYOTHERS, 0x203f, YYOTHERS,
0x2040, YYOTHERS, 0x2041, YYOTHERS, 0x2042, YYOTHERS, 0x2043, YYOTHERS,
0x2044, YYOTHERS, 0x2045, YYOTHERS, 0x2046, YYOTHERS, 0x2047, YYOTHERS,
0x2048, YYOTHERS, 0x203e, YYOTHERS, 0x2049, YYOTHERS, 0x204a, YYOTHERS,
0x204b, YYOTHERS, 0x204c, YYOTHERS, 0x13, 0x101, 0x4e, 0x102,
0x4f, 0x104, 0x50, 0x105, 0x51, 0x107, 0x52, 0x108,
0x53, 0x109, 0x54, 0x10a, 0x55, 0x10b, 0x56, 0x10c,
0x57, 0x10d, 0x17, 0x10e, 0x58, 0x10f, 0x59, 0x114,
0x5a, 0x115, 0x5b, 0x116, 0x5c, 0x117, 0x2037, YYOTHERS,
0x202f, YYOTHERS, 0x13, 0x101, 0x4e, 0x102, 0x4f, 0x104,
0x50, 0x105, 0x51, 0x107, 0x52, 0x108, 0x53, 0x109,
0x54, 0x10a, 0x55, 0x10b, 0x56, 0x10c, 0x57, 0x10d,
0x17, 0x10e, 0x58, 0x10f, 0x59, 0x114, 0x5a, 0x115,
0x5b, 0x116, 0x5c, 0x117, 0x203b, YYOTHERS, 0x13, 0x101,
0x4e, 0x102, 0x4f, 0x104, 0x50, 0x105, 0x51, 0x107,
0x52, 0x108, 0x53, 0x109, 0x54, 0x10a, 0x55, 0x10b,
0x56, 0x10c, 0x57, 0x10d, 0x17, 0x10e, 0x58, 0x10f,
0x59, 0x114, 0x5a, 0x115, 0x5b, 0x116, 0x5c, 0x117,
0x2039, YYOTHERS, 0x203d, YYOTHERS, 0x2034, YYOTHERS, 0x2074, YYOTHERS,
0x31, 0xa, 0x65, 0x3b, 0x200d, YYOTHERS, 0x31, 0xa,
0x206f, YYOTHERS, 0x18, 0x10f, 0x2061, YYOTHERS, 0x64, 0x10d,
0x6000, YYOTHERS, 0x63, 0xa, 0x206e, YYOTHERS, 0x18, 0x10f,
0x205e, YYOTHERS, 0x6f, 0x104, 0x31, 0xa, 0x65, 0x3b,
0x2066, YYOTHERS, 0x206d, YYOTHERS, 0x63, 0xa, 0x2071, 0x103,
0x2071, 0x104, 0x2071, 0x105, 0x2071, 0x106, 0x2071, 0x107,
0x2071, 0x108, 0x2071, 0x109, 0x2071, 0x10a, 0x2071, 0x115,
0x2071, 0x29, 0x2021, YYOTHERS, 0x206b, 0x107, 0x206b, 0x108,
0x206b, 0x10a, 0x2021, YYOTHERS, 0x31, 0xa, 0x88, 0x3b,
0x2072, YYOTHERS, 0x206b, 0x105, 0x2021, YYOTHERS, 0x8b, 0x105,
0x6000, YYOTHERS, 0x2059, YYOTHERS, 0x205a, YYOTHERS, 0x202b, YYOTHERS,
0x19, 0x111, 0x6000, YYOTHERS, 0x202c, YYOTHERS, 0x2038, YYOTHERS,
0x203c, YYOTHERS, 0x203a, YYOTHERS, 0x200e, YYOTHERS, 0x63, 0xa,
0x2070, YYOTHERS, 0x8d, 0x7c, 0x2064, YYOTHERS, 0x8e, 0x109,
0x6000, YYOTHERS, 0x8f, 0x106, 0x2060, YYOTHERS, 0x91, 0x29,
0x6000, YYOTHERS, 0x205b, YYOTHERS, 0x96, 0x105, 0x6000, YYOTHERS,
0x206c, YYOTHERS, 0x2006, YYOTHERS, 0x97, 0x107, 0x98, 0x108,
0x2069, YYOTHERS, 0x200b, YYOTHERS, 0x2065, YYOTHERS, 0x2057, YYOTHERS,
0x18, 0x10f, 0x6000, YYOTHERS, 0x2056, YYOTHERS, 0x206b, 0x106,
0x206b, 0x109, 0x2021, YYOTHERS, 0xa3, 0x109, 0x6000, YYOTHERS,
0x205d, YYOTHERS, 0xa4, 0x105, 0x6000, YYOTHERS, 0x2054, YYOTHERS,
0x206b, 0x10a, 0x2021, YYOTHERS, 0xa9, 0x10a, 0x6000, YYOTHERS,
0x200c, YYOTHERS, 0xaa, 0x103, 0x6000, YYOTHERS, 0x2063, YYOTHERS,
0x2015, YYOTHERS, 0x205f, YYOTHERS, 0x2014, YYOTHERS, 0x2062, YYOTHERS,
0x2055, YYOTHERS, 0x2053, YYOTHERS, 0x2007, YYOTHERS, 0x2008, YYOTHERS,
0x2068, YYOTHERS, 0x2058, YYOTHERS, 0x2029, YYOTHERS, 0x97, 0x107,
0x98, 0x108, 0x2069, YYOTHERS, 0x2067, YYOTHERS 
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

case 1: {

#line 66 "sh.y"

		sesp->s_node = NULL;
		reset (RCMD);
		NOTREACHED;
	}break;

case 2: {

#line 72 "sh.y"

		sesp->s_node = yypvt[0].yu_node;
		reset (errflag ? RERR : RCMD);
		NOTREACHED;
	}break;

case 3: {

#line 77 "sh.y"

		keyflush ();
		keyflag = 1;
		reset (RERR);
		NOTREACHED;
	}break;

case 9: {

#line 95 "sh.y"
	yyval.yu_nval = NWHILE; }break;

case 10: {

#line 96 "sh.y"
	yyval.yu_nval = NUNTIL; }break;

case 22: {

#line 118 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
	}break;

case 23: {

#line 121 "sh.y"

		yyval.yu_node = node(NBACK, yypvt[-1].yu_node, NULL);
	}break;

case 24: {

#line 124 "sh.y"

		yyval.yu_node = yypvt[-1].yu_node;
	}break;

case 25: {

#line 127 "sh.y"

		yyval.yu_node = node(NBACK, yypvt[-2].yu_node, yypvt[0].yu_node);
	}break;

case 26: {

#line 130 "sh.y"

		yyval.yu_node = node(NLIST, yypvt[-2].yu_node, yypvt[0].yu_node);
	}break;

case 27: {

#line 136 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
	}break;

case 28: {

#line 139 "sh.y"

		yyval.yu_node = node(NORF, yypvt[-2].yu_node, yypvt[0].yu_node);
	}break;

case 29: {

#line 142 "sh.y"

		yyval.yu_node = node(NANDF, yypvt[-2].yu_node, yypvt[0].yu_node);
	}break;

case 30: {

#line 148 "sh.y"

		yyval.yu_node = node(NPIPE, yypvt[-2].yu_node, yypvt[0].yu_node);
	}break;

case 31: {

#line 151 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
	}break;

case 32: {

#line 163 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
		keypop ();
	}break;

case 33: {

#line 169 "sh.y"

		keypush ();
		keyflag = 1;
	}break;

case 34: {

#line 176 "sh.y"

		yyval.yu_node = node (NCOMS, yypvt[0].yu_node, NULL);
	}break;

case 35: {

#line 179 "sh.y"

		yyval.yu_node = node (NCOMS, yypvt[0].yu_node, NULL);
	}break;

case 36: {

#line 182 "sh.y"

		yypvt[-1].yu_node->n_next = yypvt[0].yu_node;
		yyval.yu_node = node (NCOMS, yypvt[-1].yu_node, NULL);
	}break;

case 37: {

#line 186 "sh.y"

		yyval.yu_node = node (NCOMS, yypvt[0].yu_node, NULL);
	}break;

case 38: {

#line 189 "sh.y"

		yyval.yu_node = node (NRET, yypvt[0].yu_strp, NULL);
	}break;

case 39: {

#line 192 "sh.y"

		yyval.yu_node = node (NRET, "", NULL);
	}break;

case 40: {

#line 198 "sh.y"

		yyval.yu_node = node (NCTRL, yypvt[0].yu_node, NULL);
	}break;

case 41: {

#line 204 "sh.y"

		yyval.yu_node = node (NCTRL, node (NFUNC, yypvt[-5].yu_strp, yypvt[-1].yu_node), NULL);
	}break;

case 42: {

#line 210 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
	}break;

case 43: {

#line 213 "sh.y"

		(yyval.yu_node = yypvt[-1].yu_node)->n_next = yypvt[0].yu_node;
	}break;

case 44: {

#line 219 "sh.y"

		NODE	      *	tmp = yypvt[-2].yu_node;
		/*
		 * NIGEL: The structure of the nodes that have to be generated
		 * is flat, but now the grammar is structured; deal with this.
		 */

		while (tmp->n_next)
			tmp = tmp->n_next;

		(tmp->n_next = yypvt[-1].yu_node)->n_next = yypvt[0].yu_node;
		yyval.yu_node = yypvt[-2].yu_node;
	}break;

case 45: {

#line 232 "sh.y"

		NODE	      *	tmp = yypvt[-1].yu_node;
		/*
		 * NIGEL: The structure of the nodes that have to be generated
		 * is flat, but now the grammar is structured; deal with this.
		 */

		while (tmp->n_next)
			tmp = tmp->n_next;

		tmp->n_next = yypvt[0].yu_node;
		yyval.yu_node = yypvt[-1].yu_node;
	}break;

case 46: {

#line 245 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
	}break;

case 47: {

#line 248 "sh.y"

		(yyval.yu_node = yypvt[-1].yu_node)->n_next = yypvt[0].yu_node;
	}break;

case 48: {

#line 251 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
	}break;

case 49: {

#line 257 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
	}break;

case 50: {

#line 260 "sh.y"

		(yyval.yu_node = yypvt[-1].yu_node)->n_next = yypvt[0].yu_node;
	}break;

case 51: {

#line 263 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
	}break;

case 52: {

#line 266 "sh.y"

		(yyval.yu_node = yypvt[-1].yu_node)->n_next = yypvt[0].yu_node;
	}break;

case 53: {

#line 272 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
		keyflag = 0;
	}break;

case 54: {

#line 279 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
		keyflag = 0;
	}break;

case 55: {

#line 294 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
	}break;

case 56: {

#line 297 "sh.y"

		(yyval.yu_node = yypvt[-1].yu_node)->n_next = yypvt[0].yu_node;
	}break;

case 57: {

#line 300 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
	}break;

case 58: {

#line 303 "sh.y"

		(yyval.yu_node = yypvt[-1].yu_node)->n_next = yypvt[0].yu_node;
	}break;

case 59: {

#line 306 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
	}break;

case 60: {

#line 309 "sh.y"

		(yyval.yu_node = yypvt[-1].yu_node)->n_next = yypvt[0].yu_node;
	}break;

case 61: {

#line 315 "sh.y"

		yyval.yu_node = node (NARGS, duplstr (strt, 0), NULL);
	}break;

case 77: {

#line 406 "sh.y"

		yyval.yu_node = node (NIORS, yypvt[0].yu_strp, NULL);
	}break;

case 78: {

#line 411 "sh.y"

		yyval.yu_strp = duplstr (strt, 0);
	}break;

case 79: {

#line 416 "sh.y"

		yyval.yu_node = node (NARGS, yypvt[0].yu_strp, NULL);
	}break;

case 80: {

#line 421 "sh.y"

		yyval.yu_strp = duplstr (strt, 0);
	}break;

case 81: {

#line 427 "sh.y"

		yyval.yu_node = node (NASSG, yypvt[0].yu_strp, NULL);
	}break;

case 82: {

#line 432 "sh.y"

		yyval.yu_strp = duplstr (strt, 0);
	}break;

case 83: {

#line 438 "sh.y"

		yyval.yu_node = node(NFOR, yypvt[-4].yu_strp, node(NFOR2, yypvt[-3].yu_node, node(NLIST, yypvt[-1].yu_node, NULL)));
		yyval.yu_node->n_next->n_next->n_next = yyval.yu_node->n_next;
	}break;

case 84: {

#line 442 "sh.y"

		yyval.yu_node = node(NFOR, yypvt[-3].yu_strp, node(NFOR2, yypvt[-2].yu_node, node(NLIST, yypvt[-1].yu_node, NULL)));
		yyval.yu_node->n_next->n_next->n_next = yyval.yu_node->n_next;
	}break;

case 85: {

#line 446 "sh.y"

		yyval.yu_node = node(NCASE, yypvt[-4].yu_strp, yypvt[-1].yu_node);
	}break;

case 86: {

#line 449 "sh.y"

		yyval.yu_node = node(NCASE, yypvt[-3].yu_strp, yypvt[-1].yu_node);
	}break;

case 87: {

#line 452 "sh.y"

		yyval.yu_node = node(yypvt[-3].yu_nval, yypvt[-2].yu_node, node(NLIST, yypvt[-1].yu_node, NULL));
		yyval.yu_node->n_next->n_next = yyval.yu_node;
	}break;

case 88: {

#line 456 "sh.y"

		yyval.yu_node = node(NIF, node(NNULL, yypvt[-4].yu_node, yypvt[-2].yu_node), yypvt[-1].yu_node);
	}break;

case 89: {

#line 459 "sh.y"

		yyval.yu_node = node(NPARN, yypvt[-1].yu_node, NULL);
	}break;

case 90: {

#line 462 "sh.y"

		yyval.yu_node = node(NBRAC, yypvt[-1].yu_node, NULL);
	}break;

case 91: {

#line 468 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
	}break;

case 92: {

#line 471 "sh.y"

		yyval.yu_node = node(NARGS, "\"$@\"", NULL);
	}break;

case 93: {

#line 477 "sh.y"

		yyval.yu_node = node(NARGS, yypvt[-1].yu_strp, yypvt[0].yu_node);
	}break;

case 94: {

#line 480 "sh.y"

		yyval.yu_node = NULL;
	}break;

case 95: {

#line 486 "sh.y"

		register NODE *np;

		for (np=yypvt[-2].yu_node; np->n_next; np=np->n_next)
			;
		np->n_next = yypvt[0].yu_node;
		yyval.yu_node = yypvt[-2].yu_node;
	}break;

case 96: {

#line 494 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
	}break;

case 97: {

#line 497 "sh.y"

		yyval.yu_node = NULL;
	}break;

case 98: {

#line 503 "sh.y"

		yyval.yu_node = node(NCASE2, yypvt[0].yu_node, yypvt[-2].yu_node);
	}break;

case 99: {

#line 509 "sh.y"

		yyval.yu_node = node(NCASE3, yypvt[-2].yu_strp, yypvt[0].yu_node);
	}break;

case 100: {

#line 512 "sh.y"

		yyval.yu_node = node(NCASE3, yypvt[0].yu_strp, NULL);
	}break;

case 101: {

#line 518 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
	}break;

case 102: {

#line 521 "sh.y"

		yyval.yu_node = NULL;
	}break;

case 103: {

#line 527 "sh.y"

		yyval.yu_node = node(NIF, node(NNULL, yypvt[-3].yu_node, yypvt[-1].yu_node), yypvt[0].yu_node);
	}break;

case 104: {

#line 530 "sh.y"

		yyval.yu_node = node(NELSE, yypvt[0].yu_node, NULL);
	}break;

case 105: {

#line 533 "sh.y"

		yyval.yu_node = NULL;
	}break;

case 106: {

#line 539 "sh.y"

		yyval.yu_node = yypvt[0].yu_node;
	}break;

case 107: {

#line 543 "sh.y"

		yyval.yu_node = NULL;
	}break;

case 108: {

#line 549 "sh.y"

		yyval.yu_node = node(NLIST, yypvt[-2].yu_node, yypvt[0].yu_node);
	}break;

case 109: {

#line 552 "sh.y"

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




