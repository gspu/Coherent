
#line 19 "awk.y"

#include "awk.h"


#include "y.tab.h"
#define YYCLEARIN yychar = -1000
#define YYERROK yyerrflag = 0
extern int yychar;
extern short yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
YYSTYPE yyval, yylval;
#ifdef YYTNAMES
struct yytname yytnames[68] =
{
	"$end", -1, 
	"error", -2, 
	"IF_", 256, 
	"WHILE_", 257, 
	"FOR_", 258, 
	"ELSE_", 259, 
	"BREAK_", 260, 
	"CONTINUE_", 261, 
	"NEXT_", 262, 
	"EXIT_", 263, 
	"IN_", 264, 
	"PRINT_", 265, 
	"PRINTF_", 266, 
	"BEGIN_", 267, 
	"END_", 268, 
	"FAPPEND_", 269, 
	"FOUT_", 270, 
	"ID_", 271, 
	"STRING_", 272, 
	"NUMBER_", 273, 
	"FUNCTION_", 274, 
	"SCON_", 295, 
	"ASADD_", 296, 
	"ASSUB_", 297, 
	"ASMUL_", 298, 
	"ASDIV_", 299, 
	"'='", 61, 
	"ASMOD_", 300, 
	"OROR_", 301, 
	"ANDAND_", 302, 
	"'~'", 126, 
	"NMATCH_", 303, 
	"EQ_", 304, 
	"NE_", 305, 
	"GE_", 306, 
	"'>'", 62, 
	"LE_", 307, 
	"'<'", 60, 
	"'+'", 43, 
	"'-'", 45, 
	"'*'", 42, 
	"'/'", 47, 
	"'%'", 37, 
	"'!'", 33, 
	"INC_", 308, 
	"DEC_", 309, 
	"'$'", 36, 
	"'('", 40, 
	"'['", 91, 
	"REEOL_", 310, 
	"REBOL_", 311, 
	"REANY_", 312, 
	"RECLASS_", 313, 
	"RECHAR_", 314, 
	"REOR_", 315, 
	"RECON_", 316, 
	"RECLOS_", 317, 
	"RENECL_", 318, 
	"REZOCL_", 319, 
	"'\\n'", 10, 
	"','", 44, 
	"')'", 41, 
	"'{'", 123, 
	"'}'", 125, 
	"';'", 59, 
	"']'", 93, 
	"'|'", 124, 
	NULL
} ;
#endif
#include <action.h>
unsigned char yypdnt[102] = {
0, 1, 2, 2, 3, 3, 3, 5,
5, 5, 5, 8, 7, 10, 7, 9,
9, 9, 9, 9, 9, 9, 9, 9,
11, 11, 11, 4, 13, 15, 13, 13,
13, 13, 13, 13, 13, 13, 17, 13,
20, 13, 21, 13, 13, 13, 13, 12,
12, 14, 22, 22, 22, 22, 22, 22,
24, 24, 24, 24, 24, 16, 16, 16,
16, 26, 26, 25, 25, 23, 23, 6,
6, 6, 6, 6, 6, 6, 6, 6,
6, 6, 6, 6, 6, 6, 6, 6,
6, 6, 6, 6, 6, 6, 19, 19,
19, 19, 18, 18, 27, 27 
};
unsigned char yypn[102] = {
2, 1, 2, 1, 2, 2, 3, 1,
1, 1, 3, 0, 4, 0, 4, 2,
3, 3, 2, 1, 1, 1, 1, 1,
1, 1, 1, 3, 5, 0, 8, 5,
7, 9, 2, 2, 1, 2, 0, 4,
0, 3, 0, 4, 2, 2, 1, 1,
2, 1, 3, 3, 2, 2, 2, 2,
1, 1, 1, 1, 1, 4, 2, 4,
1, 1, 1, 1, 1, 1, 2, 3,
3, 3, 2, 2, 1, 1, 3, 3,
3, 3, 3, 3, 3, 3, 3, 3,
3, 3, 3, 1, 4, 1, 3, 3,
3, 1, 3, 1, 1, 3 
};
unsigned char yypgo[28] = {
0, 0, 2, 6, 8, 16, 18, 94,
102, 104, 112, 114, 116, 120, 132, 140,
142, 160, 162, 166, 172, 174, 176, 178,
190, 192, 202, 204 
};
unsigned int yygo[212] = {
-1000, 16, 18, 55, -1000, 17, -1000, 18,
0, 19, 18, 19, 20, 58, -1000, 50,
-1000, 20, 0, 21, 8, 30, 10, 32,
15, 51, 18, 21, 35, 95, 39, 97,
53, 51, 59, 112, 60, 113, 63, 116,
64, 117, 65, 118, 66, 119, 67, 120,
68, 121, 69, 122, 70, 123, 71, 124,
72, 125, 73, 126, 74, 127, 85, 97,
86, 97, 98, 143, 99, 144, 100, 145,
128, 97, 129, 97, 160, 51, 161, 51,
162, 173, 164, 97, 180, 183, 181, 51,
182, 51, 186, 51, -1000, 38, 61, 114,
62, 115, 84, 130, -1000, 22, -1000, 31,
31, 94, 88, 134, 136, 158, -1000, 140,
-1000, 84, -1000, 141, 53, 110, -1000, 52,
160, 171, 161, 172, 181, 184, 182, 185,
186, 187, -1000, 53, 144, 161, 174, 181,
183, 186, -1000, 160, -1000, 182, 11, 33,
12, 34, 13, 36, 100, 146, 150, 36,
151, 36, 153, 36, 163, 174, -1000, 23,
-1000, 105, 107, 155, -1000, 148, 148, 166,
155, 170, -1000, 154, -1000, 106, -1000, 107,
-1000, 24, 14, 39, 28, 85, 79, 128,
83, 129, 147, 164, -1000, 86, -1000, 83,
13, 37, 150, 167, 151, 168, 153, 169,
-1000, 25, -1000, 26, 29, 87, 132, 156,
147, 165, -1000, 149 
};
unsigned short yypa[188] = {
0, 30, 34, 36, 38, 42, 44, 46,
50, 74, 50, 76, 76, 82, 50, 94,
140, 144, 146, 178, 182, 188, 222, 224,
242, 244, 246, 248, 50, 50, 250, 252,
266, 268, 270, 50, 272, 274, 276, 308,
334, 338, 342, 346, 350, 354, 358, 362,
372, 374, 376, 378, 412, 416, 464, 468,
470, 472, 474, 50, 50, 478, 478, 50,
50, 50, 50, 50, 50, 50, 50, 50,
50, 50, 50, 482, 484, 486, 488, 50,
490, 492, 494, 50, 496, 500, 526, 562,
252, 566, 568, 570, 572, 574, 576, 600,
634, 636, 50, 50, 50, 668, 670, 672,
674, 676, 700, 676, 710, 712, 714, 716,
718, 748, 776, 778, 780, 804, 828, 848,
868, 888, 908, 916, 924, 926, 928, 930,
962, 1030, 1098, 1100, 50, 1102, 1104, 1128,
252, 1130, 1132, 1134, 1136, 1154, 1156, 1158,
1158, 1192, 1226, 50, 1246, 1256, 1258, 1258,
1268, 1258, 1270, 1246, 1272, 1274, 1276, 1294,
94, 94, 50, 76, 1296, 1324, 1328, 1330,
1334, 1338, 1342, 1344, 1348, 1350, 1384, 1388,
1390, 1392, 1394, 1396, 50, 94, 94, 1158,
1398, 1400, 94, 1402 
};
unsigned int yyact[1404] = {
2, 267, 3, 268, 4, 271, 5, 272,
6, 273, 7, 274, 8, 45, 9, 47,
10, 33, 11, 308, 12, 309, 13, 36,
14, 40, 15, 123, 1, -1000, 27, 10,
24576, -1000, 8199, -1000, 8200, -1000, 28, 91,
8256, -1000, 8257, -1000, 8258, -1000, 29, 40,
8285, -1000, 4, 271, 5, 272, 6, 273,
7, 274, 8, 45, 9, 47, 10, 33,
11, 308, 12, 309, 13, 36, 14, 40,
1, -1000, 8203, -1000, 4, 271, 13, 36,
24576, -1000, 4, 271, 5, 272, 6, 273,
13, 36, 35, 40, 24576, -1000, 40, 256,
41, 257, 42, 258, 43, 260, 44, 261,
45, 262, 46, 263, 47, 265, 48, 266,
4, 271, 5, 272, 6, 273, 7, 274,
8, 45, 9, 47, 10, 33, 11, 308,
12, 309, 13, 36, 14, 40, 15, 123,
49, 59, 1, -1000, 54, -1, 24576, -1000,
8193, -1000, 2, 267, 3, 268, 4, 271,
5, 272, 6, 273, 7, 274, 8, 45,
9, 47, 10, 33, 11, 308, 12, 309,
13, 36, 14, 40, 15, 123, 8195, -1,
1, -1000, 56, 10, 24576, -1000, 57, 10,
15, 123, 24576, -1000, 59, 301, 60, 302,
61, 126, 62, 303, 63, 304, 64, 305,
65, 306, 66, 62, 67, 307, 68, 60,
69, 43, 70, 45, 71, 42, 72, 47,
73, 37, 74, 44, 8201, -1000, 8283, -1000,
75, 296, 76, 297, 77, 298, 78, 299,
79, 61, 80, 300, 81, 308, 82, 309,
8259, -1000, 8269, -1000, 8268, -1000, 8260, -1000,
8205, -1000, 8267, -1000, 88, 40, 89, 310,
90, 311, 91, 312, 92, 313, 93, 314,
24576, -1000, 8266, -1000, 8244, -1000, 8246, -1000,
8259, -1000, 8254, -1000, 59, 301, 60, 302,
61, 126, 62, 303, 63, 304, 64, 305,
65, 306, 66, 62, 67, 307, 68, 60,
69, 43, 70, 45, 71, 42, 72, 47,
73, 37, 8261, -1000, 4, 271, 5, 272,
6, 273, 7, 274, 8, 45, 9, 47,
10, 33, 11, 308, 12, 309, 13, 36,
14, 40, 96, 41, 1, -1000, 98, 40,
24576, -1000, 99, 40, 24576, -1000, 100, 40,
24576, -1000, 101, 59, 24576, -1000, 102, 59,
24576, -1000, 103, 59, 24576, -1000, 104, 59,
24576, -1000, 8232, 269, 8232, 270, 8232, 59,
8232, 124, 8230, -1000, 8234, -1000, 8238, -1000,
8228, -1000, 59, 301, 60, 302, 61, 126,
62, 303, 63, 304, 64, 305, 65, 306,
66, 62, 67, 307, 68, 60, 69, 43,
70, 45, 71, 42, 72, 47, 73, 37,
108, 59, 24576, -1000, 109, 125, 24576, -1000,
40, 256, 41, 257, 42, 258, 43, 260,
44, 261, 45, 262, 46, 263, 47, 265,
48, 266, 4, 271, 5, 272, 6, 273,
7, 274, 8, 45, 9, 47, 10, 33,
11, 308, 12, 309, 13, 36, 14, 40,
15, 123, 49, 59, 8239, 125, 1, -1000,
16384, -1, 24576, -1000, 8194, -1000, 8196, -1000,
8197, -1000, 111, 10, 24576, -1000, 9, 47,
1, -1000, 8248, -1000, 8249, -1000, 8250, -1000,
8251, -1000, 8252, -1000, 8245, -1000, 8247, -1000,
9, 47, 1, -1000, 4, 271, 5, 272,
6, 273, 7, 274, 8, 45, 9, 47,
10, 33, 11, 308, 12, 309, 13, 36,
14, 40, 131, 93, 1, -1000, 4, 271,
5, 272, 6, 273, 7, 274, 8, 45,
9, 47, 10, 33, 11, 308, 12, 309,
13, 36, 14, 40, 132, 44, 8292, 269,
8292, 270, 8292, 41, 8292, 59, 8292, 124,
1, -1000, 133, 41, 24576, -1000, 8214, -1000,
8213, -1000, 8212, -1000, 8211, -1000, 8215, -1000,
135, 47, 88, 40, 89, 310, 90, 311,
91, 312, 92, 313, 93, 314, 136, 315,
137, 317, 138, 318, 139, 319, 24576, -1000,
59, 301, 60, 302, 61, 126, 62, 303,
63, 304, 64, 305, 65, 306, 66, 62,
67, 307, 68, 60, 69, 43, 70, 45,
71, 42, 72, 47, 73, 37, 142, 41,
24576, -1000, 8263, -1000, 59, 301, 60, 302,
61, 126, 62, 303, 63, 304, 64, 305,
65, 306, 66, 62, 67, 307, 68, 60,
69, 43, 70, 45, 71, 42, 72, 47,
73, 37, 8262, -1000, 8226, -1000, 8227, -1000,
8236, -1000, 8237, -1000, 4, 271, 5, 272,
6, 273, 7, 274, 8, 45, 9, 47,
10, 33, 11, 308, 12, 309, 13, 36,
147, 40, 1, -1000, 150, 269, 151, 270,
152, 59, 153, 124, 24576, -1000, 8229, -1000,
8219, -1000, 8240, -1000, 8198, -1000, 60, 302,
61, 126, 62, 303, 63, 304, 64, 305,
65, 306, 66, 62, 67, 307, 68, 60,
69, 43, 70, 45, 71, 42, 72, 47,
73, 37, 8265, -1000, 61, 126, 62, 303,
63, 304, 64, 305, 65, 306, 66, 62,
67, 307, 68, 60, 69, 43, 70, 45,
71, 42, 72, 47, 73, 37, 8264, -1000,
8281, -1000, 8282, -1000, 63, 304, 64, 305,
65, 306, 66, 62, 67, 307, 68, 60,
69, 43, 70, 45, 71, 42, 72, 47,
73, 37, 8277, -1000, 63, 304, 64, 305,
65, 306, 66, 62, 67, 307, 68, 60,
69, 43, 70, 45, 71, 42, 72, 47,
73, 37, 8278, -1000, 65, 306, 66, 62,
67, 307, 68, 60, 69, 43, 70, 45,
71, 42, 72, 47, 73, 37, 8279, -1000,
65, 306, 66, 62, 67, 307, 68, 60,
69, 43, 70, 45, 71, 42, 72, 47,
73, 37, 8275, -1000, 65, 306, 66, 62,
67, 307, 68, 60, 69, 43, 70, 45,
71, 42, 72, 47, 73, 37, 8280, -1000,
65, 306, 66, 62, 67, 307, 68, 60,
69, 43, 70, 45, 71, 42, 72, 47,
73, 37, 8276, -1000, 71, 42, 72, 47,
73, 37, 8270, -1000, 71, 42, 72, 47,
73, 37, 8271, -1000, 8272, -1000, 8273, -1000,
8274, -1000, 59, 301, 60, 302, 61, 126,
62, 303, 63, 304, 64, 305, 65, 306,
66, 62, 67, 307, 68, 60, 69, 43,
70, 45, 71, 42, 72, 47, 73, 37,
8202, -1000, 4, 271, 5, 272, 6, 273,
7, 274, 8, 45, 9, 47, 10, 33,
11, 308, 12, 309, 13, 36, 14, 40,
8242, 269, 8242, 270, 8242, 301, 8242, 302,
8242, 126, 8242, 303, 8242, 304, 8242, 305,
8242, 306, 8242, 62, 8242, 307, 8242, 60,
8242, 43, 8242, 42, 8242, 37, 8242, 10,
8242, 44, 8242, 41, 8242, 123, 8242, 59,
8242, 93, 8242, 124, 1, -1000, 4, 271,
5, 272, 6, 273, 7, 274, 8, 45,
9, 47, 10, 33, 11, 308, 12, 309,
13, 36, 14, 40, 8243, 269, 8243, 270,
8243, 301, 8243, 302, 8243, 126, 8243, 303,
8243, 304, 8243, 305, 8243, 306, 8243, 62,
8243, 307, 8243, 60, 8243, 43, 8243, 42,
8243, 37, 8243, 10, 8243, 44, 8243, 41,
8243, 123, 8243, 59, 8243, 93, 8243, 124,
1, -1000, 8206, -1000, 8255, -1000, 8284, -1000,
88, 40, 89, 310, 90, 311, 91, 312,
92, 313, 93, 314, 136, 315, 137, 317,
138, 318, 139, 319, 157, 41, 24576, -1000,
8204, -1000, 8216, -1000, 8218, -1000, 8217, -1000,
89, 310, 90, 311, 91, 312, 92, 313,
93, 314, 137, 317, 138, 318, 139, 319,
8207, -1000, 8210, -1000, 8253, -1000, 59, 301,
60, 302, 61, 126, 62, 303, 63, 304,
64, 305, 65, 306, 66, 62, 67, 307,
68, 60, 69, 43, 70, 45, 71, 42,
72, 47, 73, 37, 159, 41, 24576, -1000,
59, 301, 60, 302, 61, 126, 62, 303,
63, 304, 64, 305, 65, 306, 66, 62,
67, 307, 68, 60, 69, 43, 70, 45,
71, 42, 72, 47, 73, 37, 162, 59,
24576, -1000, 163, 264, 75, 296, 76, 297,
77, 298, 78, 299, 79, 61, 80, 300,
81, 308, 82, 309, 8259, -1000, 150, 269,
151, 270, 152, 59, 153, 124, 24576, -1000,
8291, -1000, 4, 271, 5, 272, 6, 273,
13, 36, 24576, -1000, 8289, -1000, 8233, -1000,
8293, -1000, 8208, -1000, 89, 310, 90, 311,
91, 312, 92, 313, 93, 314, 137, 317,
138, 318, 139, 319, 8209, -1000, 8241, -1000,
4, 271, 5, 272, 6, 273, 7, 274,
8, 45, 9, 47, 10, 33, 11, 308,
12, 309, 13, 36, 14, 40, 132, 44,
96, 41, 1, -1000, 175, 41, 24576, -1000,
8231, -1000, 176, 59, 24576, -1000, 177, 59,
24576, -1000, 178, 59, 24576, -1000, 8235, -1000,
179, 259, 8220, -1000, 8223, -1000, 59, 301,
60, 302, 61, 126, 62, 303, 63, 304,
64, 305, 65, 306, 66, 62, 67, 307,
68, 60, 69, 43, 70, 45, 71, 42,
72, 47, 73, 37, 180, 59, 24576, -1000,
159, 41, 24576, -1000, 8290, -1000, 8286, -1000,
8287, -1000, 8288, -1000, 8221, -1000, 8224, -1000,
8222, -1000, 8225, -1000 
};
/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
#include "action.h"
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

#line 72 "awk.y"

		codep = yypvt[0].u_node;
	}break;

case 2: {

#line 78 "awk.y"

		yyval.u_node = node(ALIST, yypvt[-1].u_node, yypvt[0].u_node);
	}break;

case 4: {

#line 85 "awk.y"

		yyval.u_node = node(AROOT, NULL, yypvt[-1].u_node);
	}break;

case 5: {

#line 88 "awk.y"

		yyval.u_node = node(AROOT, yypvt[-1].u_node, NULL);
	}break;

case 6: {

#line 91 "awk.y"

		yyval.u_node = node(AROOT, yypvt[-2].u_node, yypvt[-1].u_node);
	}break;

case 7: {

#line 97 "awk.y"

		yyval.u_node = node(ABEGIN);
	}break;

case 8: {

#line 100 "awk.y"

		yyval.u_node = node(AEND);
	}break;

case 10: {

#line 104 "awk.y"

		yyval.u_node = node(ARANGE, yypvt[-2].u_node, yypvt[0].u_node);
	}break;

case 11: {

#line 110 "awk.y"
 lexre=1; }break;

case 12: {

#line 110 "awk.y"

		lexre = 0;
		yyval.u_node = yypvt[-1].u_node;
	}break;

case 13: {

#line 114 "awk.y"

		yyerrok;
		yyerror("Badly formed regular expression");
	}break;

case 14: {

#line 117 "awk.y"

		yyval.u_node = yypvt[0].u_node;
	}break;

case 15: {

#line 123 "awk.y"

		register NODE *np;

		for (np = yypvt[-1].u_node; np->n_O3!=NULL; np = np->n_O3)
			;
		np->n_O3 = yypvt[0].u_node;
		yyval.u_node = yypvt[-1].u_node;
	}break;

case 16: {

#line 131 "awk.y"

		yyval.u_node = yypvt[-1].u_node;
	}break;

case 17: {

#line 134 "awk.y"

		yyval.u_node = node(AROR, yypvt[-2].u_node, yypvt[0].u_node, NULL);
	}break;

case 18: {

#line 137 "awk.y"

		yyval.u_node = node(yypvt[0].u_char, yypvt[-1].u_node, NULL, NULL);
	}break;

case 19: {

#line 140 "awk.y"

		yyval.u_node = node(yflag?ARDCLASS:ARCLASS, NULL, NULL, NULL);
		yyval.u_node->n_o1.n_charp = yypvt[0].u_charp;
	}break;

case 20: {

#line 144 "awk.y"

		yyval.u_node = node(ARANY, NULL, NULL, NULL);
	}break;

case 21: {

#line 147 "awk.y"

		yyval.u_node = node(ARBOL, NULL, NULL, NULL);
	}break;

case 22: {

#line 150 "awk.y"

		yyval.u_node = node(AREOL, NULL, NULL, NULL);
	}break;

case 23: {

#line 153 "awk.y"

		yyval.u_node = cnode(yflag?ARDCHAR:ARCHAR, yypvt[0].u_char);
	}break;

case 24: {

#line 159 "awk.y"

		yyval.u_char = ARCLOS;
	}break;

case 25: {

#line 162 "awk.y"

		yyval.u_char = ARZOCL;
	}break;

case 26: {

#line 165 "awk.y"

		yyval.u_char = ARNECL;
	}break;

case 27: {

#line 171 "awk.y"

		yyval.u_node = yypvt[-1].u_node;
	}break;

case 28: {

#line 177 "awk.y"

		yyval.u_node = node(AIF, yypvt[-2].u_node, yypvt[0].u_node, NULL);
	}break;

case 29: {

#line 180 "awk.y"
nlskip = 1;}break;

case 30: {

#line 180 "awk.y"

		yyval.u_node = node(AIF, yypvt[-5].u_node, yypvt[-3].u_node, yypvt[0].u_node);
	}break;

case 31: {

#line 183 "awk.y"

		yyval.u_node = node(AWHILE, yypvt[-2].u_node, yypvt[0].u_node);
	}break;

case 32: {

#line 186 "awk.y"

		yyval.u_node = node(AFORIN, yypvt[-4].u_node, yypvt[-2].u_node, yypvt[0].u_node);
	}break;

case 33: {

#line 189 "awk.y"

		yyval.u_node = node(AFOR, yypvt[-6].u_node, yypvt[-4].u_node, yypvt[-2].u_node, yypvt[0].u_node);
	}break;

case 34: {

#line 192 "awk.y"

		yyval.u_node = node(ABREAK);
	}break;

case 35: {

#line 195 "awk.y"

		yyval.u_node = node(ACONTIN);
	}break;

case 38: {

#line 200 "awk.y"
outflag++;}break;

case 39: {

#line 200 "awk.y"

		yyval.u_node = node(APRINT, yypvt[-1].u_node, yypvt[0].u_node);
	}break;

case 40: {

#line 203 "awk.y"
outflag++;}break;

case 41: {

#line 203 "awk.y"

		yyval.u_node = node(APRINT, &xfield0, yypvt[0].u_node);
	}break;

case 42: {

#line 206 "awk.y"
outflag++;}break;

case 43: {

#line 206 "awk.y"

		yyval.u_node = node(APRINTF, yypvt[-1].u_node, yypvt[0].u_node);
	}break;

case 44: {

#line 209 "awk.y"

		yyval.u_node = node(ANEXT);
	}break;

case 45: {

#line 212 "awk.y"

		yyval.u_node = node(AEXIT);
	}break;

case 46: {

#line 215 "awk.y"

		yyval.u_node = NULL;
	}break;

case 48: {

#line 222 "awk.y"

		if (yypvt[-1].u_node!=NULL && yypvt[0].u_node!=NULL)
			yyval.u_node = node(ALIST, yypvt[-1].u_node, yypvt[0].u_node);
		else if (yypvt[-1].u_node != NULL)
			yyval.u_node = yypvt[-1].u_node;
		else
			yyval.u_node = yypvt[0].u_node;
	}break;

case 49: {

#line 233 "awk.y"

		nlskip = 1;
	}break;

case 50: {

#line 239 "awk.y"

		yyval.u_node = node(AASGN, yypvt[-2].u_node, yypvt[0].u_node);
	}break;

case 51: {

#line 242 "awk.y"

		yyval.u_node = node(AASGN, yypvt[-2].u_node, node(yypvt[-1].u_char, yypvt[-2].u_node, yypvt[0].u_node));
	}break;

case 52: {

#line 245 "awk.y"

		yyval.u_node = node(AASGN, yypvt[0].u_node, node(AADD, yypvt[0].u_node, &xone));
	}break;

case 53: {

#line 248 "awk.y"

		yyval.u_node = node(AINCA, yypvt[-1].u_node);
	}break;

case 54: {

#line 251 "awk.y"

		yyval.u_node = node(AASGN, yypvt[0].u_node, node(ASUB, yypvt[0].u_node, &xone));
	}break;

case 55: {

#line 254 "awk.y"

		yyval.u_node = node(ADECA, yypvt[-1].u_node);
	}break;

case 56: {

#line 260 "awk.y"

		yyval.u_char = AADD;
	}break;

case 57: {

#line 263 "awk.y"

		yyval.u_char = ASUB;
	}break;

case 58: {

#line 266 "awk.y"

		yyval.u_char = AMUL;
	}break;

case 59: {

#line 269 "awk.y"

		yyval.u_char = ADIV;
	}break;

case 60: {

#line 272 "awk.y"

		yyval.u_char = AMOD;
	}break;

case 61: {

#line 278 "awk.y"

		yyval.u_node = node(AFIELD, yypvt[-1].u_node);
	}break;

case 62: {

#line 281 "awk.y"

		yyval.u_node = node(AFIELD, yypvt[0].u_node);
	}break;

case 63: {

#line 284 "awk.y"

		yyval.u_node = node(AARRAY, yypvt[-3].u_node, yypvt[-1].u_node);
	}break;

case 70: {

#line 302 "awk.y"

		yyval.u_node = node(ACONC, yypvt[-1].u_node, yypvt[0].u_node);
	}break;

case 71: {

#line 308 "awk.y"

		yyval.u_node = yypvt[-1].u_node;
	}break;

case 72: {

#line 311 "awk.y"

		yyval.u_node = node(AANDAND, yypvt[-2].u_node, yypvt[0].u_node);
	}break;

case 73: {

#line 314 "awk.y"

		yyval.u_node = node(AOROR, yypvt[-2].u_node, yypvt[0].u_node);
	}break;

case 74: {

#line 317 "awk.y"

		yyval.u_node = node(ANOT, yypvt[0].u_node);
	}break;

case 75: {

#line 320 "awk.y"

		yyval.u_node = node(ANEG, yypvt[0].u_node);
	}break;

case 78: {

#line 325 "awk.y"

		yyval.u_node = node(AADD, yypvt[-2].u_node, yypvt[0].u_node);
	}break;

case 79: {

#line 328 "awk.y"

		yyval.u_node = node(ASUB, yypvt[-2].u_node, yypvt[0].u_node);
	}break;

case 80: {

#line 331 "awk.y"

		yyval.u_node = node(AMUL, yypvt[-2].u_node, yypvt[0].u_node);
	}break;

case 81: {

#line 334 "awk.y"

		yyval.u_node = node(ADIV, yypvt[-2].u_node, yypvt[0].u_node);
	}break;

case 82: {

#line 337 "awk.y"

		yyval.u_node = node(AMOD, yypvt[-2].u_node, yypvt[0].u_node);
	}break;

case 83: {

#line 340 "awk.y"

		yyval.u_node = node(AGT, yypvt[-2].u_node, yypvt[0].u_node);
	}break;

case 84: {

#line 343 "awk.y"

		yyval.u_node = node(ALT, yypvt[-2].u_node, yypvt[0].u_node);
	}break;

case 85: {

#line 346 "awk.y"

		yyval.u_node = node(AEQ, yypvt[-2].u_node, yypvt[0].u_node);
	}break;

case 86: {

#line 349 "awk.y"

		yyval.u_node = node(ANE, yypvt[-2].u_node, yypvt[0].u_node);
	}break;

case 87: {

#line 352 "awk.y"

		yyval.u_node = node(AGE, yypvt[-2].u_node,yypvt[0].u_node);
	}break;

case 88: {

#line 355 "awk.y"

		yyval.u_node = node(ALE, yypvt[-2].u_node, yypvt[0].u_node);
	}break;

case 89: {

#line 358 "awk.y"

		yyval.u_node = node(AREMAT, yypvt[-2].u_node, yypvt[0].u_node);
	}break;

case 90: {

#line 361 "awk.y"

		yyval.u_node = node(ARENMAT, yypvt[-2].u_node, yypvt[0].u_node);
	}break;

case 91: {

#line 364 "awk.y"

		if (brlevel)
			awkerr("Regular expression illegal in action");
		yyval.u_node = node(ARE, yypvt[0].u_node);
	}break;

case 92: {

#line 369 "awk.y"

		yyval.u_node = node(AFUNC, yypvt[-3].u_node, yypvt[-1].u_node);
	}break;

case 93: {

#line 372 "awk.y"

		yyval.u_node = node(AFUNC, yypvt[0].u_node, NULL);
	}break;

case 94: {

#line 378 "awk.y"

		yyval.u_node = node(AFAPP, yypvt[-1].u_node);
	}break;

case 95: {

#line 381 "awk.y"

		yyval.u_node = node(AFOUT, yypvt[-1].u_node);
	}break;

case 96: {

#line 384 "awk.y"

		yyval.u_node = node(AFPIPE, yypvt[-1].u_node);
	}break;

case 97: {

#line 387 "awk.y"

		yyval.u_node = NULL;
	}break;

case 98: {

#line 392 "awk.y"

		yyval.u_node = yypvt[-1].u_node;
	 }break;

case 99: {

#line 395 "awk.y"

		yyval.u_node = yypvt[0].u_node;
	 }break;

case 100: {

#line 401 "awk.y"

		yyval.u_node = yypvt[0].u_node;
	}break;

case 101: {

#line 404 "awk.y"

		yyval.u_node = node(ALIST, yypvt[-2].u_node, yypvt[0].u_node);
	}break;

		}
		ip = &yygo[ yypgo[yypdnt[pno]] ];
		while( *ip!=*yys && *ip!=YYNOCHAR )
			ip += 2;
		yystate = ip[1];
		goto stack;
	}
}




