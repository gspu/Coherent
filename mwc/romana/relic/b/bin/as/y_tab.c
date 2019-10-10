
#line 1 "asm.y"

#include <asm.h>
/*
 * count macro parms.
 */
static 
parmCt()
{
	if(NULL == trueMac) {
		yyerror(".parmct not in macro");
 		/* \fB.parmct\fR returns the number of
		 * parameters in the current macro. */
		return(0);
	}
	else
		return(countList(trueMac->parms));
}

/*
 * Verify that 2 symbols have the same segment.
 */
static void
ckseg(s1, s2)
sym *s1, *s2;
{
	if(s1->sg != s2->sg)
		yyerror("Arithmetic between addresses on different segments");
		/* You may only add or subtract addresses if they
		 * are in the same segment. */
}

/*
 * Create Immediate exps.
 */
static expr *
setImm(val, symRef)
long val;
sym  *symRef;
{
	register expr *oper;
	register short w;

	oper = xalloc();
	oper->mode = T_IMM;
	if (NULL != (oper->ref = symRef))
		oper->ref = symRef->ref;
	w = oper->exp = val;
	if(w == val)
		oper->size = 2;
	else
		oper->size = 4;
	return(oper);
}

/*
 * Set up for most addressing modes.
 */
static expr *
qbild(val, mode, r1, r2, scale, symRef)
long val;
int mode;
psym *r1, *r2;
long scale;
sym *symRef;
{
	register expr *oper;
	char i;

	if (NULL != r1 && T_R != mode) {
		switch((int)r1->size) {
		case 2:
			lflags |= A_SHORT;
			if (!scale)
				break;
		case 1:
			regerror(r1);
			break;
		case 4:
			lflags |= A_LONG;
			break;
		}

		if (ORD_REG != r1->flag)	/* cant ind via ctl regs */
			regerror(r1);

		if (NULL != r2) {
			if (ORD_REG != r2->flag)
				regerror(r2);
			if (r1->size != r2->size)
				yyerror("Mixed length addressing registers");
		/* Addressing registers must both be the same length. */
		}
	}
	else if ((T_R == mode) && (r1->flag == ORD_REG)) {
		switch ((int)r1->size) {
		case 4:
			lflags |= O_LONG;
			break;
		case 2:
			lflags |= O_SHORT;
		}
	}

	oper = xalloc();
	oper->exp  = val;
	oper->r1 = r1;
	oper->r2 = r2;
	oper->mode = mode;

	i = scale;
	if (i != scale)
		i = 3;	/* set a bad scale */

	switch (i) {
	case 0:
	case 1:	/* for generated code */
		oper->scale = 0; break;
	case 2:
		oper->scale = 1; break;
	case 4:
		oper->scale = 2; break;
	case 8:
		oper->scale = 3; break;
	default:
		yyerror("Bad scale");
		/* Address scale must be 0, 1, 2, 4, or 8. */
	}

	if (NULL != (oper->ref = symRef))
		oper->ref = symRef->ref;
	return(oper);
}

/*
 * Floating point register expr.
 */
static expr *
fbild(regno)
long regno;
{
	register expr *oper;

	if (regno < 0 || regno > 7) {
		regno &= 7;
		yyerror("Invalid floating point register number"); /**/
	}

	oper = xalloc();
	oper->exp = regno;
	oper->mode = T_FP;
	return (oper);
}
	
/*
 * Report register error.
 */
regerror(rg)
psym *rg;
{
	yyerror("%s is an improper register in this context", rg->name); /**/
}

/*
 * Concatinate strings.
 */
static char *
concat(s1, s2)
char *s1, *s2;
{
	long l;
	unsigned short u;
	char *res;

	u = l = (long)strlen(s1) + (long)strlen(s2) + 1;
	if(u != l) {
		yyerror("Length %ld string range exceeded", l);
		/* Strings may not exceed 32 kilobytes. */
		return(galloc(1));
	}
	res = galloc(u);
	sprintf(res, "%s%s", s1, s2);
	return(res);
}

/*
 * Substring.
 * Assumes that all strings are created from unsigned lengths.
 */
static char *
substr(s, from, len)
char *s;
long from, len;
{
	register char *p, *res;
	unsigned short l;

	l = strlen(s);
	s += from;
	from = l - from;	/* from now len to end */

	if(len > from)
		len = from;	/* since strlen fit unsigned len must */
	if(len < 0)
		len = 0;

	for(p = res = galloc((unsigned)(len + 1)); len--; )
		*p++ = *s++;
	return(res);
}

/*
 * String search.
 */
static
stringSearch(s1, s2)
char *s1, *s2;
{
	char *p;

	if(NULL == (p = strstr(s1, s2)))
		return(-1);
	return(p - s1);
}

/*
 * Do long comparisons.
 * < > <= >= != ==  compare operator
 * 1 2  5  6  3  4  t
 */
static
compare(t, v)
int t;
long v;
{
	return(((v < 0) ? t : (v > 0) ? (t >> 1) : (t >> 2)) & 1);
}

/*
 * Do double comparisons.
 * < > <= >= != ==  compare operator
 * 1 2  5  6  3  4  t
 */
static
fcompare(t, v)
int t;
double v;
{
	return(((v < 0.0) ? t : (v > 0.0) ? (t >> 1) : (t >> 2)) & 1);
}

static void
unmatched(c)
{
	yyerror("Unmatched '%c'", c);
	/* A delimeter, [, (, ), or ] is unmatched in this command. */
}


#include "y_tab.h"
#define YYCLEARIN yychar = -1000
#define YYERROK yyerrflag = 0
extern int yychar;
extern short yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
YYSTYPE yyval, yylval;
#ifdef YYTNAMES
struct yytname yytnames[50] =
{
	"$end", -1, 
	"error", -2, 
	"PC", 256, 
	"NUMBER", 257, 
	"FNUM", 258, 
	"COMPARISON", 312, 
	"OP", 260, 
	"DATA", 261, 
	"CMD", 262, 
	"DCMD", 263, 
	"ICMD", 264, 
	"NCMD", 265, 
	"ECMD", 266, 
	"ECMDX", 267, 
	"IDENTIFIER", 268, 
	"REG", 269, 
	"DEFINED", 320, 
	"SIZEOF", 321, 
	"SEGMENT", 322, 
	"LOCATION", 323, 
	"PLUS", 315, 
	"MINUS", 316, 
	"TIMES", 317, 
	"DIVIDE", 318, 
	"REM", 319, 
	"LSHIFT", 313, 
	"RSHIFT", 314, 
	"AND", 311, 
	"OR", 309, 
	"XOR", 310, 
	"COMMA", 305, 
	"LPAREN", 328, 
	"RPAREN", 286, 
	"LBRACK", 329, 
	"RBRACK", 288, 
	"AT", 307, 
	"D_SIGN", 290, 
	"NOT", 324, 
	"BANG", 326, 
	"COLON", 306, 
	"PARMCT", 294, 
	"TOSTRING", 330, 
	"TONUMBER", 331, 
	"TOFLOAT", 332, 
	"FSTACK", 298, 
	"NL", 333, 
	"TOKEN", 327, 
	"P_SIGN", 308, 
	"LEN", 325, 
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

unsigned char yypdnt[108] = {
0, 1, 1, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 7,
7, 7, 6, 6, 6, 6, 6, 4,
4, 4, 5, 3, 3, 8, 8, 8,
8, 13, 13, 14, 14, 14, 14, 14,
14, 14, 14, 14, 14, 14, 14, 14,
14, 14, 14, 14, 14, 14, 14, 14,
14, 9, 9, 9, 9, 9, 11, 11,
11, 11, 11, 11, 11, 11, 11, 11,
11, 11, 11, 11, 11, 11, 11, 11,
11, 11, 11, 11, 11, 11, 11, 11,
11, 11, 11, 11, 12, 12, 12, 12,
12, 12, 12, 10, 10, 10, 10, 10,
10, 10, 10, 10 
};
unsigned char yypn[108] = {
2, 1, 2, 4, 4, 6, 6, 3,
3, 4, 4, 4, 4, 2, 2, 3,
1, 0, 1, 1, 1, 1, 3, 1,
3, 0, 1, 0, 1, 3, 1, 2,
0, 1, 3, 1, 1, 4, 3, 5,
6, 5, 7, 4, 6, 7, 6, 8,
4, 6, 7, 6, 8, 1, 1, 2,
2, 1, 3, 3, 3, 3, 1, 3,
3, 3, 3, 3, 3, 2, 2, 2,
2, 2, 1, 3, 3, 3, 3, 3,
3, 3, 3, 3, 3, 2, 2, 2,
4, 3, 2, 2, 1, 3, 3, 6,
4, 2, 2, 1, 3, 3, 3, 3,
3, 2, 2, 2 
};
unsigned char yypgo[15] = {
0, 0, 2, 6, 8, 14, 20, 28,
34, 38, 76, 114, 198, 228, 232 
};
unsigned int yygo[236] = {
YYOTHERS, 0x3, 0x3, 0x8, YYOTHERS, 0x4, YYOTHERS, 0x5,
0xc, 0x38, 0x7d, 0xb2, YYOTHERS, 0x36, 0xf, 0x3c,
0x10, 0x3d, YYOTHERS, 0x37, 0xd, 0x39, 0x7b, 0xb1,
0x80, 0xb3, YYOTHERS, 0x2f, 0x79, 0xb0, 0x81, 0xb4,
YYOTHERS, 0x30, 0x78, 0xaf, YYOTHERS, 0x28, 0x9, 0x29,
0xa, 0x31, 0xd, 0x31, 0x18, 0x42, 0x19, 0x44,
0x1a, 0x45, 0x1c, 0x29, 0x1d, 0x4a, 0x1f, 0x50,
0x3f, 0x29, 0x5e, 0x90, 0x60, 0x92, 0x68, 0x9c,
0x78, 0x29, 0x79, 0x31, 0x7b, 0x31, 0x80, 0x31,
0x81, 0x31, YYOTHERS, 0x46, 0xa, 0x32, 0xd, 0x32,
0x1b, 0x47, 0x1d, 0x4b, 0x23, 0x54, 0x24, 0x58,
0x56, 0x8c, 0x57, 0x8d, 0x62, 0x96, 0x63, 0x97,
0x64, 0x98, 0x65, 0x99, 0x66, 0x9a, 0x79, 0x32,
0x7b, 0x32, 0x80, 0x32, 0x81, 0x32, 0x8b, 0x8d,
YYOTHERS, 0x2a, 0x9, 0x2b, 0x18, 0x43, 0x19, 0x43,
0x1a, 0x43, 0x1b, 0x48, 0x1c, 0x2b, 0x1d, 0x4c,
0x1f, 0x51, 0x20, 0x52, 0x21, 0x53, 0x23, 0x55,
0x25, 0x5a, 0x3f, 0x2b, 0x5c, 0x8f, 0x5e, 0x43,
0x5f, 0x91, 0x60, 0x93, 0x67, 0x9b, 0x68, 0x9d,
0x69, 0x9e, 0x6a, 0x9f, 0x6b, 0xa0, 0x6c, 0xa1,
0x6d, 0xa2, 0x6e, 0xa3, 0x6f, 0xa4, 0x70, 0xa5,
0x71, 0xa6, 0x75, 0xac, 0x76, 0xad, 0x78, 0x2b,
0x88, 0xb6, 0xb7, 0xc7, 0xb9, 0xc9, 0xbc, 0xcc,
0xbf, 0xce, 0xc4, 0xcf, 0xca, 0xd4, 0xcd, 0xd8,
0xd1, 0xdb, 0xd5, 0xdd, YYOTHERS, 0x33, 0xa, 0x34,
0xd, 0x34, 0x1d, 0x4d, 0x24, 0x59, 0x25, 0x5b,
0x57, 0x8e, 0x73, 0xaa, 0x74, 0xab, 0x77, 0xae,
0x79, 0x34, 0x7b, 0x34, 0x80, 0x34, 0x81, 0x34,
0xa9, 0x8e, YYOTHERS, 0x2c, 0x1c, 0x49, YYOTHERS, 0x2d,
0x3f, 0x83, YYOTHERS, 0x2e 
};
unsigned short yypa[225] = {
0, 22, 26, 28, 52, 54, 74, 76,
80, 82, 128, 162, 162, 166, 200, 204,
210, 214, 216, 218, 220, 224, 226, 230,
166, 166, 166, 166, 236, 166, 278, 166,
166, 166, 284, 166, 286, 166, 300, 304,
306, 310, 320, 332, 358, 370, 374, 376,
380, 384, 392, 404, 430, 442, 444, 448,
452, 456, 460, 462, 464, 468, 472, 474,
516, 518, 520, 522, 546, 548, 550, 558,
560, 562, 564, 574, 588, 614, 628, 634,
638, 646, 670, 672, 674, 676, 678, 688,
702, 704, 706, 708, 166, 710, 166, 166,
166, 712, 678, 678, 678, 678, 678, 166,
166, 166, 166, 166, 166, 166, 166, 166,
166, 166, 718, 724, 724, 166, 166, 732,
740, 128, 784, 166, 786, 162, 788, 790,
166, 128, 792, 794, 796, 798, 800, 802,
804, 840, 842, 678, 558, 846, 858, 866,
892, 898, 906, 908, 916, 922, 926, 936,
942, 948, 950, 952, 968, 970, 978, 986,
988, 990, 992, 1004, 1016, 1034, 1056, 1076,
1082, 732, 1086, 1092, 1096, 1124, 1150, 1156,
1158, 1160, 1162, 1164, 1168, 1172, 1178, 166,
1204, 1206, 1242, 1244, 1248, 1284, 1286, 166,
1290, 1292, 1294, 1296, 166, 1298, 1300, 1302,
1328, 1334, 166, 1360, 1366, 166, 1392, 1418,
1444, 166, 1446, 1448, 1450, 166, 1476, 1478,
1480, 1506, 1508, 1510, 1536, 1538, 1564, 1566,
1568 
};
unsigned int yyact[1570] = {
0x2, 0x147, 0x201b, 0x104, 0x201b, 0x105, 0x201b, 0x106,
0x201b, 0x107, 0x201b, 0x108, 0x201b, 0x109, 0x201b, 0x10a,
0x201b, 0x10b, 0x201b, 0x14d, 0x1, YYOTHERS, 0x6, 0x14d,
0x6000, YYOTHERS, 0x201c, YYOTHERS, 0x7, YYEOFVAL, 0x2, 0x147,
0x201b, 0x104, 0x201b, 0x105, 0x201b, 0x106, 0x201b, 0x107,
0x201b, 0x108, 0x201b, 0x109, 0x201b, 0x10a, 0x201b, 0x10b,
0x201b, 0x14d, 0x1, YYOTHERS, 0x2001, YYOTHERS, 0x9, 0x104,
0xa, 0x105, 0xb, 0x106, 0xc, 0x107, 0xd, 0x108,
0xe, 0x109, 0xf, 0x10a, 0x10, 0x10b, 0x11, 0x14d,
0x6000, YYOTHERS, 0x200e, YYOTHERS, 0x4000, YYEOFVAL, 0x6000, YYOTHERS,
0x2002, YYOTHERS, 0x12, 0x101, 0x13, 0x102, 0x14, 0x104,
0x15, 0x10c, 0x16, 0x10d, 0x17, 0x140, 0x18, 0x141,
0x19, 0x142, 0x1a, 0x143, 0x1b, 0x13c, 0x1c, 0x13d,
0x1d, 0x148, 0x1e, 0x149, 0x1f, 0x122, 0x20, 0x144,
0x21, 0x146, 0x22, 0x126, 0x23, 0x14a, 0x24, 0x14b,
0x25, 0x14c, 0x26, 0x12a, 0x27, 0x147, 0x2020, YYOTHERS,
0x12, 0x101, 0x13, 0x102, 0x15, 0x10c, 0x17, 0x140,
0x18, 0x141, 0x19, 0x142, 0x1a, 0x143, 0x1b, 0x13c,
0x1d, 0x148, 0x20, 0x144, 0x21, 0x146, 0x22, 0x126,
0x23, 0x14a, 0x24, 0x14b, 0x25, 0x14c, 0x27, 0x147,
0x2011, YYOTHERS, 0x35, 0x147, 0x2019, YYOTHERS, 0x12, 0x101,
0x13, 0x102, 0x15, 0x10c, 0x17, 0x140, 0x18, 0x141,
0x19, 0x142, 0x1a, 0x143, 0x1b, 0x13c, 0x1d, 0x148,
0x20, 0x144, 0x21, 0x146, 0x22, 0x126, 0x23, 0x14a,
0x24, 0x14b, 0x25, 0x14c, 0x27, 0x147, 0x6000, YYOTHERS,
0x3a, 0x14d, 0x6000, YYOTHERS, 0x3b, 0x14d, 0x35, 0x147,
0x6000, YYOTHERS, 0x35, 0x147, 0x6000, YYOTHERS, 0x200d, YYOTHERS,
0x203e, YYOTHERS, 0x2063, YYOTHERS, 0x3e, 0x14d, 0x6000, YYOTHERS,
0x2039, YYOTHERS, 0x3f, 0x132, 0x2023, YYOTHERS, 0x40, 0x101,
0x41, 0x10c, 0x6000, YYOTHERS, 0x12, 0x101, 0x13, 0x102,
0x15, 0x10c, 0x16, 0x10d, 0x17, 0x140, 0x18, 0x141,
0x19, 0x142, 0x1a, 0x143, 0x1b, 0x13c, 0x1d, 0x148,
0x1e, 0x149, 0x1f, 0x122, 0x20, 0x144, 0x21, 0x146,
0x22, 0x126, 0x23, 0x14a, 0x24, 0x14b, 0x25, 0x14c,
0x26, 0x12a, 0x27, 0x147, 0x6000, YYOTHERS, 0x4e, 0x10d,
0x4f, 0x131, 0x6000, YYOTHERS, 0x204a, YYOTHERS, 0x13, 0x102,
0x56, 0x13c, 0x57, 0x148, 0x23, 0x14a, 0x25, 0x14c,
0x27, 0x147, 0x6000, YYOTHERS, 0x5c, 0x149, 0x2024, YYOTHERS,
0x205c, YYOTHERS, 0x5d, 0x14d, 0x6000, YYOTHERS, 0x5e, 0x138,
0x5f, 0x13b, 0x60, 0x13c, 0x61, 0x149, 0x2035, YYOTHERS,
0x62, 0x138, 0x63, 0x13b, 0x64, 0x13c, 0x65, 0x13d,
0x66, 0x13e, 0x6000, YYOTHERS, 0x67, 0x138, 0x68, 0x13b,
0x69, 0x13c, 0x6a, 0x13d, 0x6b, 0x13e, 0x6c, 0x13f,
0x6d, 0x139, 0x6e, 0x13a, 0x6f, 0x137, 0x70, 0x135,
0x71, 0x136, 0x72, 0x149, 0x2036, YYOTHERS, 0x73, 0x138,
0x74, 0x13b, 0x75, 0x148, 0x76, 0x149, 0x77, 0x133,
0x6000, YYOTHERS, 0x78, 0x131, 0x201e, YYOTHERS, 0x2021, YYOTHERS,
0x79, 0x131, 0x2010, YYOTHERS, 0x7a, 0x14d, 0x6000, YYOTHERS,
0x5e, 0x138, 0x5f, 0x13b, 0x60, 0x13c, 0x2012, YYOTHERS,
0x62, 0x138, 0x63, 0x13b, 0x64, 0x13c, 0x65, 0x13d,
0x66, 0x13e, 0x2013, YYOTHERS, 0x67, 0x138, 0x68, 0x13b,
0x69, 0x13c, 0x6a, 0x13d, 0x6b, 0x13e, 0x6c, 0x13f,
0x6d, 0x139, 0x6e, 0x13a, 0x6f, 0x137, 0x70, 0x135,
0x71, 0x136, 0x7b, 0x134, 0x2014, YYOTHERS, 0x73, 0x138,
0x74, 0x13b, 0x75, 0x148, 0x76, 0x149, 0x77, 0x133,
0x2015, YYOTHERS, 0x201a, YYOTHERS, 0x7c, 0x14d, 0x6000, YYOTHERS,
0x7d, 0x131, 0x2017, YYOTHERS, 0x7e, 0x14d, 0x6000, YYOTHERS,
0x7f, 0x14d, 0x6000, YYOTHERS, 0x2008, YYOTHERS, 0x2007, YYOTHERS,
0x80, 0x131, 0x6000, YYOTHERS, 0x81, 0x131, 0x6000, YYOTHERS,
0x200c, YYOTHERS, 0x12, 0x101, 0x13, 0x102, 0x15, 0x10c,
0x82, 0x10d, 0x17, 0x140, 0x18, 0x141, 0x19, 0x142,
0x1a, 0x143, 0x1b, 0x13c, 0x1d, 0x148, 0x1e, 0x149,
0x1f, 0x122, 0x20, 0x144, 0x21, 0x146, 0x22, 0x126,
0x23, 0x14a, 0x24, 0x14b, 0x25, 0x14c, 0x26, 0x12a,
0x27, 0x147, 0x6000, YYOTHERS, 0x2049, YYOTHERS, 0x2048, YYOTHERS,
0x2045, YYOTHERS, 0x67, 0x138, 0x68, 0x13b, 0x69, 0x13c,
0x6a, 0x13d, 0x6b, 0x13e, 0x6c, 0x13f, 0x6d, 0x139,
0x6e, 0x13a, 0x6f, 0x137, 0x70, 0x135, 0x71, 0x136,
0x6000, YYOTHERS, 0x2047, YYOTHERS, 0x2046, YYOTHERS, 0x5e, 0x138,
0x5f, 0x13b, 0x60, 0x13c, 0x6000, YYOTHERS, 0x2069, YYOTHERS,
0x2055, YYOTHERS, 0x201f, YYOTHERS, 0x5e, 0x138, 0x5f, 0x13b,
0x60, 0x13c, 0x84, 0x11e, 0x6000, YYOTHERS, 0x62, 0x138,
0x63, 0x13b, 0x64, 0x13c, 0x65, 0x13d, 0x66, 0x13e,
0x85, 0x11e, 0x6000, YYOTHERS, 0x67, 0x138, 0x68, 0x13b,
0x69, 0x13c, 0x6a, 0x13d, 0x6b, 0x13e, 0x6c, 0x13f,
0x6d, 0x139, 0x6e, 0x13a, 0x6f, 0x137, 0x70, 0x135,
0x71, 0x136, 0x86, 0x11e, 0x6000, YYOTHERS, 0x73, 0x138,
0x74, 0x13b, 0x75, 0x148, 0x87, 0x11e, 0x76, 0x149,
0x77, 0x133, 0x6000, YYOTHERS, 0x88, 0x131, 0x89, 0x120,
0x6000, YYOTHERS, 0x8a, 0x10d, 0x6000, YYOTHERS, 0x5e, 0x138,
0x5f, 0x13b, 0x60, 0x13c, 0x2038, YYOTHERS, 0x67, 0x138,
0x68, 0x13b, 0x69, 0x13c, 0x6a, 0x13d, 0x6b, 0x13e,
0x6c, 0x13f, 0x6d, 0x139, 0x6e, 0x13a, 0x6f, 0x137,
0x70, 0x135, 0x71, 0x136, 0x2037, YYOTHERS, 0x2057, YYOTHERS,
0x2056, YYOTHERS, 0x2062, YYOTHERS, 0x2061, YYOTHERS, 0x13, 0x102,
0x56, 0x13c, 0x8b, 0x148, 0x25, 0x14c, 0x6000, YYOTHERS,
0x13, 0x102, 0x56, 0x13c, 0x57, 0x148, 0x23, 0x14a,
0x25, 0x14c, 0x27, 0x147, 0x6000, YYOTHERS, 0x205b, YYOTHERS,
0x205a, YYOTHERS, 0x206b, YYOTHERS, 0x206a, YYOTHERS, 0x200b, YYOTHERS,
0x94, 0x10d, 0x95, 0x131, 0x6000, YYOTHERS, 0xa7, 0x10d,
0xa8, 0x131, 0x6000, YYOTHERS, 0xa9, 0x148, 0x23, 0x14a,
0x27, 0x147, 0x6000, YYOTHERS, 0xa9, 0x148, 0x23, 0x14a,
0x27, 0x147, 0x6000, YYOTHERS, 0x12, 0x101, 0x13, 0x102,
0x15, 0x10c, 0x16, 0x10d, 0x17, 0x140, 0x18, 0x141,
0x19, 0x142, 0x1a, 0x143, 0x1b, 0x13c, 0x1c, 0x13d,
0x1d, 0x148, 0x1e, 0x149, 0x1f, 0x122, 0x20, 0x144,
0x21, 0x146, 0x22, 0x126, 0x23, 0x14a, 0x24, 0x14b,
0x25, 0x14c, 0x26, 0x12a, 0x27, 0x147, 0x2020, YYOTHERS,
0x200a, YYOTHERS, 0x2003, YYOTHERS, 0x2004, YYOTHERS, 0x2009, YYOTHERS,
0x2023, YYOTHERS, 0x2022, YYOTHERS, 0x203a, YYOTHERS, 0x2064, YYOTHERS,
0x203f, YYOTHERS, 0x205d, YYOTHERS, 0x12, 0x101, 0x13, 0x102,
0x15, 0x10c, 0xb5, 0x10d, 0x17, 0x140, 0x18, 0x141,
0x19, 0x142, 0x1a, 0x143, 0x1b, 0x13c, 0x1d, 0x148,
0x20, 0x144, 0x21, 0x146, 0x22, 0x126, 0x23, 0x14a,
0x24, 0x14b, 0x25, 0x14c, 0x27, 0x147, 0x6000, YYOTHERS,
0x2026, YYOTHERS, 0xb7, 0x131, 0x6000, YYOTHERS, 0x63, 0x13b,
0x64, 0x13c, 0x65, 0x13d, 0x66, 0x13e, 0x85, 0x11e,
0x6000, YYOTHERS, 0x74, 0x13b, 0x75, 0x148, 0x87, 0x11e,
0x6000, YYOTHERS, 0x67, 0x138, 0x68, 0x13b, 0x69, 0x13c,
0x6a, 0x13d, 0x6b, 0x13e, 0x6c, 0x13f, 0x6d, 0x139,
0x6e, 0x13a, 0x6f, 0x137, 0x70, 0x135, 0x71, 0x136,
0xb8, 0x120, 0x6000, YYOTHERS, 0x5f, 0x13b, 0x60, 0x13c,
0x2041, YYOTHERS, 0x6a, 0x13d, 0x6b, 0x13e, 0x6c, 0x13f,
0x203b, YYOTHERS, 0x2040, YYOTHERS, 0x6a, 0x13d, 0x6b, 0x13e,
0x6c, 0x13f, 0x203c, YYOTHERS, 0xb9, 0x131, 0xba, 0x120,
0x6000, YYOTHERS, 0xbb, 0x10d, 0x6000, YYOTHERS, 0x63, 0x13b,
0x64, 0x13c, 0x65, 0x13d, 0x66, 0x13e, 0x2044, YYOTHERS,
0x65, 0x13d, 0x66, 0x13e, 0x2065, YYOTHERS, 0x65, 0x13d,
0x66, 0x13e, 0x2066, YYOTHERS, 0x2067, YYOTHERS, 0x2068, YYOTHERS,
0x68, 0x13b, 0x69, 0x13c, 0x6a, 0x13d, 0x6b, 0x13e,
0x6c, 0x13f, 0x6d, 0x139, 0x6e, 0x13a, 0x2042, YYOTHERS,
0x203d, YYOTHERS, 0x6a, 0x13d, 0x6b, 0x13e, 0x6c, 0x13f,
0x204b, YYOTHERS, 0x6a, 0x13d, 0x6b, 0x13e, 0x6c, 0x13f,
0x204c, YYOTHERS, 0x204d, YYOTHERS, 0x204e, YYOTHERS, 0x204f, YYOTHERS,
0x68, 0x13b, 0x69, 0x13c, 0x6a, 0x13d, 0x6b, 0x13e,
0x6c, 0x13f, 0x2050, YYOTHERS, 0x68, 0x13b, 0x69, 0x13c,
0x6a, 0x13d, 0x6b, 0x13e, 0x6c, 0x13f, 0x2051, YYOTHERS,
0x67, 0x138, 0x68, 0x13b, 0x69, 0x13c, 0x6a, 0x13d,
0x6b, 0x13e, 0x6c, 0x13f, 0x6d, 0x139, 0x6e, 0x13a,
0x2052, YYOTHERS, 0x67, 0x138, 0x68, 0x13b, 0x69, 0x13c,
0x6a, 0x13d, 0x6b, 0x13e, 0x6c, 0x13f, 0x6d, 0x139,
0x6e, 0x13a, 0x6f, 0x137, 0x71, 0x136, 0x2053, YYOTHERS,
0x67, 0x138, 0x68, 0x13b, 0x69, 0x13c, 0x6a, 0x13d,
0x6b, 0x13e, 0x6c, 0x13f, 0x6d, 0x139, 0x6e, 0x13a,
0x6f, 0x137, 0x2054, YYOTHERS, 0xbc, 0x131, 0xbd, 0x120,
0x6000, YYOTHERS, 0xbe, 0x10d, 0x6000, YYOTHERS, 0x74, 0x13b,
0x75, 0x148, 0x2043, YYOTHERS, 0x75, 0x148, 0x205e, YYOTHERS,
0x67, 0x138, 0x68, 0x13b, 0x69, 0x13c, 0x6a, 0x13d,
0x6b, 0x13e, 0x6c, 0x13f, 0x6d, 0x139, 0x6e, 0x13a,
0x6f, 0x137, 0x70, 0x135, 0x71, 0x136, 0xbf, 0x131,
0xc0, 0x11e, 0x6000, YYOTHERS, 0x67, 0x138, 0x68, 0x13b,
0x69, 0x13c, 0x6a, 0x13d, 0x6b, 0x13e, 0x6c, 0x13f,
0x6d, 0x139, 0x6e, 0x13a, 0x6f, 0x137, 0x70, 0x135,
0x71, 0x136, 0xc1, 0x120, 0x6000, YYOTHERS, 0x74, 0x13b,
0x75, 0x148, 0x2059, YYOTHERS, 0x201d, YYOTHERS, 0x200f, YYOTHERS,
0x2016, YYOTHERS, 0x2018, YYOTHERS, 0xc2, 0x14d, 0x6000, YYOTHERS,
0xc3, 0x14d, 0x6000, YYOTHERS, 0xc4, 0x131, 0xc5, 0x120,
0x6000, YYOTHERS, 0x67, 0x138, 0x68, 0x13b, 0x69, 0x13c,
0x6a, 0x13d, 0x6b, 0x13e, 0x6c, 0x13f, 0x6d, 0x139,
0x6e, 0x13a, 0x6f, 0x137, 0x70, 0x135, 0x71, 0x136,
0xc6, 0x120, 0x6000, YYOTHERS, 0x2025, YYOTHERS, 0x12, 0x101,
0x13, 0x102, 0x15, 0x10c, 0xc8, 0x10d, 0x17, 0x140,
0x18, 0x141, 0x19, 0x142, 0x1a, 0x143, 0x1b, 0x13c,
0x1d, 0x148, 0x20, 0x144, 0x21, 0x146, 0x22, 0x126,
0x23, 0x14a, 0x24, 0x14b, 0x25, 0x14c, 0x27, 0x147,
0x6000, YYOTHERS, 0x2030, YYOTHERS, 0xca, 0x131, 0x6000, YYOTHERS,
0x12, 0x101, 0x13, 0x102, 0x15, 0x10c, 0xcb, 0x10d,
0x17, 0x140, 0x18, 0x141, 0x19, 0x142, 0x1a, 0x143,
0x1b, 0x13c, 0x1d, 0x148, 0x20, 0x144, 0x21, 0x146,
0x22, 0x126, 0x23, 0x14a, 0x24, 0x14b, 0x25, 0x14c,
0x27, 0x147, 0x6000, YYOTHERS, 0x202b, YYOTHERS, 0xcd, 0x131,
0x6000, YYOTHERS, 0x2060, YYOTHERS, 0x2058, YYOTHERS, 0x2005, YYOTHERS,
0x2006, YYOTHERS, 0x2029, YYOTHERS, 0x2027, YYOTHERS, 0x67, 0x138,
0x68, 0x13b, 0x69, 0x13c, 0x6a, 0x13d, 0x6b, 0x13e,
0x6c, 0x13f, 0x6d, 0x139, 0x6e, 0x13a, 0x6f, 0x137,
0x70, 0x135, 0x71, 0x136, 0xd0, 0x120, 0x6000, YYOTHERS,
0xd1, 0x131, 0xd2, 0x120, 0x6000, YYOTHERS, 0x67, 0x138,
0x68, 0x13b, 0x69, 0x13c, 0x6a, 0x13d, 0x6b, 0x13e,
0x6c, 0x13f, 0x6d, 0x139, 0x6e, 0x13a, 0x6f, 0x137,
0x70, 0x135, 0x71, 0x136, 0xd3, 0x120, 0x6000, YYOTHERS,
0xd5, 0x131, 0xd6, 0x120, 0x6000, YYOTHERS, 0x67, 0x138,
0x68, 0x13b, 0x69, 0x13c, 0x6a, 0x13d, 0x6b, 0x13e,
0x6c, 0x13f, 0x6d, 0x139, 0x6e, 0x13a, 0x6f, 0x137,
0x70, 0x135, 0x71, 0x136, 0xd7, 0x120, 0x6000, YYOTHERS,
0x67, 0x138, 0x68, 0x13b, 0x69, 0x13c, 0x6a, 0x13d,
0x6b, 0x13e, 0x6c, 0x13f, 0x6d, 0x139, 0x6e, 0x13a,
0x6f, 0x137, 0x70, 0x135, 0x71, 0x136, 0xd9, 0x11e,
0x6000, YYOTHERS, 0x67, 0x138, 0x68, 0x13b, 0x69, 0x13c,
0x6a, 0x13d, 0x6b, 0x13e, 0x6c, 0x13f, 0x6d, 0x139,
0x6e, 0x13a, 0x6f, 0x137, 0x70, 0x135, 0x71, 0x136,
0xda, 0x120, 0x6000, YYOTHERS, 0x2028, YYOTHERS, 0x2033, YYOTHERS,
0x2031, YYOTHERS, 0x67, 0x138, 0x68, 0x13b, 0x69, 0x13c,
0x6a, 0x13d, 0x6b, 0x13e, 0x6c, 0x13f, 0x6d, 0x139,
0x6e, 0x13a, 0x6f, 0x137, 0x70, 0x135, 0x71, 0x136,
0xdc, 0x120, 0x6000, YYOTHERS, 0x202e, YYOTHERS, 0x202c, YYOTHERS,
0x67, 0x138, 0x68, 0x13b, 0x69, 0x13c, 0x6a, 0x13d,
0x6b, 0x13e, 0x6c, 0x13f, 0x6d, 0x139, 0x6e, 0x13a,
0x6f, 0x137, 0x70, 0x135, 0x71, 0x136, 0xde, 0x120,
0x6000, YYOTHERS, 0x205f, YYOTHERS, 0x202a, YYOTHERS, 0x67, 0x138,
0x68, 0x13b, 0x69, 0x13c, 0x6a, 0x13d, 0x6b, 0x13e,
0x6c, 0x13f, 0x6d, 0x139, 0x6e, 0x13a, 0x6f, 0x137,
0x70, 0x135, 0x71, 0x136, 0xdf, 0x120, 0x6000, YYOTHERS,
0x2032, YYOTHERS, 0x67, 0x138, 0x68, 0x13b, 0x69, 0x13c,
0x6a, 0x13d, 0x6b, 0x13e, 0x6c, 0x13f, 0x6d, 0x139,
0x6e, 0x13a, 0x6f, 0x137, 0x70, 0x135, 0x71, 0x136,
0xe0, 0x120, 0x6000, YYOTHERS, 0x202d, YYOTHERS, 0x2034, YYOTHERS,
0x202f, YYOTHERS 
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

#line 352 "asm.y"
 /* assembler command with parms */
		docmd(yypvt[-3].p, yypvt[-2].o, yypvt[-1].p); }break;

case 4: {

#line 354 "asm.y"
 /* a command with string parms */
		docmd(yypvt[-3].p, yypvt[-2].o, yypvt[-1].p); }break;

case 5: {

#line 356 "asm.y"
 /* command with a name & an expr */
		ecmd(yypvt[-5].p, yypvt[-4].o, yypvt[-3].p, yypvt[-1].d); }break;

case 6: {

#line 359 "asm.y"

		ecmd(yypvt[-5].p, yypvt[-4].o, yypvt[-3].p, yypvt[-1].d); }break;

case 7: {

#line 361 "asm.y"

		ecmd(yypvt[-2].p, yypvt[-1].o, NULL, NULL); }break;

case 8: {

#line 363 "asm.y"
	/* assembler command takes no parms */
		docmd(yypvt[-2].p, yypvt[-1].o, (parm *)NULL); }break;

case 9: {

#line 365 "asm.y"
 /* assembler command with data parm */
		ncmd(yypvt[-3].p, yypvt[-2].o, yypvt[-1].d); }break;

case 10: {

#line 367 "asm.y"
 /* data list */
		dcmd(yypvt[-3].p, yypvt[-2].o, yypvt[-1].d); }break;

case 11: {

#line 369 "asm.y"
	/* opcode operands */
		buildind(yypvt[-3].p, yypvt[-2].o, yypvt[-1].e); }break;

case 12: {

#line 371 "asm.y"
	/* built by rep instr */
		buildind(yypvt[-3].p, yypvt[-2].o, NULL);
		buildind(NULL, yypvt[-1].o, NULL ); }break;

case 13: {

#line 374 "asm.y"
	/* label alone on line */
		buildlab(yypvt[-1].p); }break;

case 14: {

#line 376 "asm.y"
	/* syntax error */
		if (bcnt > 0)
			unmatched('[');
		if (bcnt < 0)
			unmatched(']');
		if (pcnt > 0)
			unmatched('(');
		if (pcnt < 0)
			unmatched(')');
		yyerrok; }break;

case 15: {

#line 387 "asm.y"

		yyval.d = yypvt[-2].d;
		yyval.d->next = yypvt[0].d; }break;

case 16: {

#line 390 "asm.y"

		yyval.d = yypvt[0].d; }break;

case 17: {

#line 392 "asm.y"

		yyval.d = NULL; }break;

case 18: {

#line 395 "asm.y"

		yyval.d = gitem('y');
		yyval.d->d.y  = yypvt[0].s; }break;

case 19: {

#line 398 "asm.y"

		yyval.d = gitem('d');
		yyval.d->d.d = yypvt[0].dbl; }break;

case 20: {

#line 401 "asm.y"

		yyval.d = gitem('l');
		yyval.d->d.l = yypvt[0].val; }break;

case 21: {

#line 404 "asm.y"

		yyval.d = gitem('s');
		yyval.d->d.s = yypvt[0].t; }break;

case 22: {

#line 407 "asm.y"

		yyval.d = yypvt[0].d;
		yyval.d->count = yypvt[-2].val; }break;

case 23: {

#line 411 "asm.y"
		/* start parm list */
		yyval.p = yypvt[0].p; }break;

case 24: {

#line 413 "asm.y"
	/* chain parm list */
		yyval.p = yypvt[-2].p;
		yyval.p->next = yypvt[0].p; }break;

case 25: {

#line 416 "asm.y"

		yyval.p = NULL; }break;

case 26: {

#line 419 "asm.y"
		/* start a parm */
		yyval.p = (parm *)gcpy(yypvt[0].t, offset(parm, str)); }break;

case 27: {

#line 422 "asm.y"
 
		yyval.p = NULL; }break;

case 28: {

#line 424 "asm.y"

		yyval.p = (parm *)gcpy(yypvt[0].t, offset(parm, str)); }break;

case 29: {

#line 428 "asm.y"

		yyval.e = yypvt[-2].e;
		yyval.e->next = yypvt[0].e; }break;

case 30: {

#line 431 "asm.y"

		yyval.e = yypvt[0].e; }break;

case 31: {

#line 433 "asm.y"

		lflags |= A_INDIR;
		yyval.e = yypvt[0].e; }break;

case 32: {

#line 436 "asm.y"

		yyval.e = NULL; }break;

case 33: {

#line 439 "asm.y"

	yyval.e->sg = -1;
	yyval.e = yypvt[0].e; }break;

case 34: {

#line 442 "asm.y"

		yyval.e = yypvt[0].e;
		if (yypvt[-2].s->flag != SEG_REG)
			regerror(yypvt[-2].s);
		yyval.e->sg = yypvt[-2].s->loc; }break;

case 35: {

#line 449 "asm.y"

		yyval.e = qbild(0L, T_R, yypvt[0].s, NULL, 0L, NULL); }break;

case 36: {

#line 452 "asm.y"

     		yyval.e = fbild(0L);	}break;

case 37: {

#line 455 "asm.y"

     		yyval.e = fbild(yypvt[-1].val); }break;

case 38: {

#line 458 "asm.y"

		yyval.e = qbild(0L, T_RI, yypvt[-1].s, NULL, 0L, NULL); }break;

case 39: {

#line 461 "asm.y"

		yyval.e = qbild(0L, T_RIS, yypvt[-3].s, NULL, yypvt[-1].val, NULL); }break;

case 40: {

#line 464 "asm.y"

		yyval.e = qbild(0L, T_RIS, yypvt[-3].s, NULL, yypvt[-1].val, NULL); }break;

case 41: {

#line 467 "asm.y"

     		yyval.e = qbild(0L, T_RIX, yypvt[-3].s, yypvt[-1].s, 0L, NULL); }break;

case 42: {

#line 470 "asm.y"

     		yyval.e = qbild(0L, T_RIXS, yypvt[-5].s, yypvt[-3].s, yypvt[-1].val, NULL); }break;

case 43: {

#line 473 "asm.y"

		yyval.e = qbild(yypvt[-3].val, T_RID, yypvt[-1].s, NULL, 0L, NULL); }break;

case 44: {

#line 476 "asm.y"

		yyval.e = qbild(yypvt[-5].val, T_RIDS, yypvt[-3].s, NULL, yypvt[-1].val, NULL); }break;

case 45: {

#line 479 "asm.y"

		yyval.e = qbild(yypvt[-6].val, T_RIDS, yypvt[-3].s, NULL, yypvt[-1].val, NULL); }break;

case 46: {

#line 482 "asm.y"

     		yyval.e = qbild(yypvt[-5].val, T_RIXD, yypvt[-3].s, yypvt[-1].s, 0L, NULL); }break;

case 47: {

#line 485 "asm.y"

     		yyval.e = qbild(yypvt[-7].val, T_RIXDS, yypvt[-5].s, yypvt[-3].s, yypvt[-1].val, NULL); }break;

case 48: {

#line 488 "asm.y"

		yyval.e = qbild(yypvt[-3].s->loc, T_RID, yypvt[-1].s, NULL, 0L, yypvt[-3].s); }break;

case 49: {

#line 491 "asm.y"

		yyval.e = qbild(yypvt[-5].s->loc, T_RIDS, yypvt[-3].s, NULL, yypvt[-1].val, yypvt[-5].s); }break;

case 50: {

#line 494 "asm.y"

		yyval.e = qbild(yypvt[-6].s->loc, T_RIDS, yypvt[-3].s, NULL, yypvt[-1].val, yypvt[-6].s); }break;

case 51: {

#line 497 "asm.y"

     		yyval.e = qbild(yypvt[-5].s->loc, T_RIXD, yypvt[-3].s, yypvt[-1].s, 0L, yypvt[-5].s); }break;

case 52: {

#line 500 "asm.y"

     		yyval.e = qbild(yypvt[-7].s->loc, T_RIXDS, yypvt[-5].s, yypvt[-3].s, yypvt[-1].val, yypvt[-7].s); }break;

case 53: {

#line 503 "asm.y"

     		yyval.e = qbild(yypvt[0].s->loc, T_D, NULL, NULL, 0L, yypvt[0].s); }break;

case 54: {

#line 506 "asm.y"

     		yyval.e = qbild(yypvt[0].val, T_D, NULL, NULL, 0L, NULL); }break;

case 55: {

#line 509 "asm.y"

		yyval.e = setImm(yypvt[0].val, (sym *)NULL); }break;

case 56: {

#line 512 "asm.y"

		yyval.e = setImm(yypvt[0].s->loc, yypvt[0].s); }break;

case 57: {

#line 518 "asm.y"
 yyval.s = yypvt[0].s; }break;

case 58: {

#line 519 "asm.y"
 yyval.s=yypvt[-1].s; }break;

case 59: {

#line 520 "asm.y"
 yyval.s = copySym(yypvt[-2].s); yyval.s->loc += yypvt[0].val; }break;

case 60: {

#line 521 "asm.y"
 yyval.s = copySym(yypvt[-2].s); yyval.s->loc -= yypvt[0].val; }break;

case 61: {

#line 522 "asm.y"
 yyval.s = copySym(yypvt[0].s); yyval.s->loc += yypvt[-2].val; }break;

case 62: {

#line 525 "asm.y"

		yyval.val = yypvt[0].val; }break;

case 63: {

#line 527 "asm.y"

		yyval.val = yypvt[-1].val; }break;

case 64: {

#line 529 "asm.y"

		ckseg(yypvt[-2].s, yypvt[0].s); yyval.val = yypvt[-2].s->loc - yypvt[0].s->loc; }break;

case 65: {

#line 531 "asm.y"

		ckseg(yypvt[-2].s, yypvt[0].s); yyval.val = compare((int)yypvt[-1].val, yypvt[-2].s->loc - yypvt[0].s->loc); }break;

case 66: {

#line 533 "asm.y"

		yyval.val = compare((int)yypvt[-1].val, yypvt[-2].val - yypvt[0].val); }break;

case 67: {

#line 535 "asm.y"

		yyval.val = compare((int)yypvt[-1].val, (long)strcmp(yypvt[-2].t, yypvt[0].t)); }break;

case 68: {

#line 537 "asm.y"

		yyval.val = fcompare((int)yypvt[-1].val, yypvt[-2].dbl - yypvt[0].dbl); }break;

case 69: {

#line 540 "asm.y"

		yyval.val = yypvt[0].s->size; }break;

case 70: {

#line 543 "asm.y"

		yyval.val = yypvt[0].s->loc; }break;

case 71: {

#line 546 "asm.y"

		yyval.val = yypvt[0].s->sg + 1; }break;

case 72: {

#line 549 "asm.y"

		yyval.val = yypvt[0].s->statement && (statement >= yypvt[0].s->statement); }break;

case 73: {

#line 551 "asm.y"

		yyval.val = 1; }break;

case 74: {

#line 553 "asm.y"

		yyval.val = parmCt(); }break;

case 75: {

#line 556 "asm.y"

		yyval.val = yypvt[-2].val + yypvt[0].val; }break;

case 76: {

#line 558 "asm.y"

		yyval.val = yypvt[-2].val - yypvt[0].val; }break;

case 77: {

#line 560 "asm.y"

		yyval.val = yypvt[-2].val * yypvt[0].val; }break;

case 78: {

#line 562 "asm.y"

		yyval.val = yypvt[-2].val / yypvt[0].val; }break;

case 79: {

#line 564 "asm.y"

		yyval.val = yypvt[-2].val % yypvt[0].val; }break;

case 80: {

#line 567 "asm.y"

		yyval.val = yypvt[-2].val << yypvt[0].val; }break;

case 81: {

#line 569 "asm.y"

		yyval.val = yypvt[-2].val >> yypvt[0].val; }break;

case 82: {

#line 573 "asm.y"

		yyval.val = yypvt[-2].val & yypvt[0].val; }break;

case 83: {

#line 575 "asm.y"

		yyval.val = yypvt[-2].val | yypvt[0].val; }break;

case 84: {

#line 577 "asm.y"

		yyval.val = yypvt[-2].val ^ yypvt[0].val; }break;

case 85: {

#line 579 "asm.y"

		yyval.val = - yypvt[0].val; }break;

case 86: {

#line 581 "asm.y"

		yyval.val = !yypvt[0].val; }break;

case 87: {

#line 583 "asm.y"

		yyval.val = ~yypvt[0].val; }break;

case 88: {

#line 586 "asm.y"

		yyval.val = (yypvt[-1].val > strlen(yypvt[-3].t)) ? 0 : yypvt[-3].t[(short)yypvt[-1].val]; }break;

case 89: {

#line 588 "asm.y"

		yyval.val = stringSearch(yypvt[-2].t, yypvt[0].t); }break;

case 90: {

#line 590 "asm.y"

		yyval.val = atol(yypvt[0].t); }break;

case 91: {

#line 592 "asm.y"

		yyval.val = yypvt[0].dbl; }break;

case 92: {

#line 595 "asm.y"

		yyval.t = gcpy(yypvt[0].t, 0); }break;

case 93: {

#line 597 "asm.y"

		yyval.t = yypvt[-1].t; }break;

case 94: {

#line 599 "asm.y"

		yyval.t = concat(yypvt[-2].t, yypvt[0].t); }break;

case 95: {

#line 601 "asm.y"

		yyval.t = substr(yypvt[-5].t, yypvt[-3].val, yypvt[-1].val); }break;

case 96: {

#line 603 "asm.y"

		yyval.t = substr(yypvt[-3].t, yypvt[-1].val, strlen(yypvt[-3].t) - yypvt[-1].val); }break;

case 97: {

#line 605 "asm.y"

		yyval.t = galloc(12);
		sprintf(yyval.t, "%ld", yypvt[0].val); }break;

case 98: {

#line 608 "asm.y"

		yyval.t = galloc(20);
		sprintf(yyval.t, "%g", yypvt[0].dbl); }break;

case 99: {

#line 613 "asm.y"

		yyval.dbl = yypvt[0].dbl; }break;

case 100: {

#line 615 "asm.y"

		yyval.dbl = yypvt[-1].dbl; }break;

case 101: {

#line 617 "asm.y"

		yyval.dbl = yypvt[-2].dbl + yypvt[0].dbl; }break;

case 102: {

#line 619 "asm.y"

		yyval.dbl = yypvt[-2].dbl - yypvt[0].dbl; }break;

case 103: {

#line 621 "asm.y"

		yyval.dbl = yypvt[-2].dbl * yypvt[0].dbl; }break;

case 104: {

#line 623 "asm.y"

		yyval.dbl = yypvt[-2].dbl / yypvt[0].dbl; }break;

case 105: {

#line 625 "asm.y"

		yyval.dbl = - yypvt[0].dbl; }break;

case 106: {

#line 627 "asm.y"

		yyval.dbl = strtod(yypvt[0].t, (char **)NULL); }break;

case 107: {

#line 629 "asm.y"

		yyval.dbl = yypvt[0].val; }break;

		}
		ip = &yygo[ yypgo[yypdnt[pno]] ];
		while( *ip!=*yys && *ip!=YYNOCHAR )
			ip += 2;
		yystate = ip[1];
		goto stack;
	}
}




