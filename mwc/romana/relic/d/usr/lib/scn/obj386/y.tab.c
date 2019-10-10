
#line 1 "../scn.y"

#include <scnbld.h>

clump *clumps, *lastc, *thisc;
loc *locs, *last, *this;

/*
 * Generate a unique name.
 */
static char *
genname()
{
	char buf[20];
	static int i = 0;

	sprintf(buf, "L%04d", ++i);
	return(newcpy(buf));
}


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
readonly struct yytname yytnames[18] =
{
	"$end", -1, 
	"error", -2, 
	"NAME", 256, 
	"STRING", 257, 
	"NUMBER", 258, 
	"EXT", 259, 
	"RONLY", 260, 
	"LONG", 261, 
	"DEFAULT", 262, 
	"SKIP", 263, 
	"GROUP", 264, 
	"HELP", 265, 
	"VERIFY", 266, 
	"NL", 267, 
	"DO", 268, 
	"DONE", 269, 
	"DESIG", 270, 
	NULL
} ;
#endif
#include <action.h>
readonly unsigned char yypdnt[25] = {
0, 1, 1, 1, 2, 2, 2, 4,
3, 3, 3, 3, 5, 5, 5, 6,
6, 6, 6, 6, 6, 6, 6, 6,
6 
};
readonly unsigned char yypn[25] = {
2, 1, 2, 0, 2, 1, 2, 0,
4, 2, 3, 1, 1, 2, 0, 1,
1, 1, 2, 2, 2, 2, 2, 2,
2 
};
readonly unsigned char yypgo[7] = {
0, 0, 2, 6, 8, 10, 12 
};
readonly unsigned int yygo[16] = {
YYOTHERS, 0x7, 0x7, 0xF, YYOTHERS, 0x8, YYOTHERS, 0x9,
YYOTHERS, 0x11, YYOTHERS, 0x1A, 0x1A, 0x23, YYOTHERS, 0x1B
};
readonly unsigned char yypa[36] = {
0, 14, 18, 22, 24, 28, 30, 34,
48, 50, 54, 56, 58, 62, 64, 68,
70, 72, 88, 90, 92, 94, 96, 102,
108, 114, 118, 134, 136, 138, 140, 142,
144, 146, 148, 150 
};
readonly unsigned int yyact[152] = {
0x2, 0x100, 0x3, 0x10B, 0x4, 0x10C, 0x5, 0x10D,
0x6, 0x10E, 0x2003, YYEOFVAL, 0x1, YYOTHERS, 0xA, 0x10B,
0x6000, YYOTHERS, 0xB, 0x102, 0x6000, YYOTHERS, 0x2005, YYOTHERS,
0xC, 0x102, 0x6000, YYOTHERS, 0x200B, YYOTHERS, 0xD, 0x101,
0x6000, YYOTHERS, 0xE, YYEOFVAL, 0x2, 0x100, 0x3, 0x10B,
0x4, 0x10C, 0x5, 0x10D, 0x6, 0x10E, 0x1, YYOTHERS,
0x2001, YYOTHERS, 0x10, 0x10B, 0x6000, YYOTHERS, 0x2006, YYOTHERS,
0x2007, YYOTHERS, 0x12, 0x100, 0x6000, YYOTHERS, 0x2009, YYOTHERS,
0x4000, YYEOFVAL, 0x6000, YYOTHERS, 0x2002, YYOTHERS, 0x2004, YYOTHERS,
0x13, 0x103, 0x14, 0x104, 0x15, 0x105, 0x16, 0x106,
0x17, 0x107, 0x18, 0x109, 0x19, 0x10A, 0x200E, YYOTHERS,
0x200A, YYOTHERS, 0x2011, YYOTHERS, 0x2010, YYOTHERS, 0x200F, YYOTHERS,
0x1C, 0x100, 0x1D, 0x101, 0x6000, YYOTHERS, 0x1E, 0x102,
0x1F, 0x108, 0x6000, YYOTHERS, 0x20, 0x100, 0x21, 0x101,
0x6000, YYOTHERS, 0x22, 0x100, 0x6000, YYOTHERS, 0x13, 0x103,
0x14, 0x104, 0x15, 0x105, 0x16, 0x106, 0x17, 0x107,
0x18, 0x109, 0x19, 0x10A, 0x2008, YYOTHERS, 0x200C, YYOTHERS,
0x2013, YYOTHERS, 0x2012, YYOTHERS, 0x2016, YYOTHERS, 0x2018, YYOTHERS,
0x2015, YYOTHERS, 0x2014, YYOTHERS, 0x2017, YYOTHERS, 0x200D, YYOTHERS
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

case 6: {

#line 50 "../scn.y"

		yyerrok; 
	}break;

case 7: {

#line 54 "../scn.y"

		if (0 >= yypvt[0].val)
			yyerror("Invalid field size");
		this = alloc(sizeof(*this));
		if (NULL == locs)
			last = locs = this;
		else
			last = last->next = this;
			
		this->field = yypvt[-1].string;
		this->len = yypvt[0].val; 

		if ((NULL != thisc) && (!thisc->count++))
			thisc->from = this;
      	}break;

case 9: {

#line 70 "../scn.y"

		extern char fieldDesig;

		fieldDesig = yypvt[0].string[1];
	}break;

case 10: {

#line 76 "../scn.y"

		if (NULL != thisc)
			yyerror("Nested do statements");
		if (0 >= yypvt[-1].val)
			yyerror("Invalid repeat count");

		thisc = alloc(sizeof(*thisc));
		if (NULL == clumps)
			lastc = clumps = thisc;
		else
			lastc = lastc->next = thisc;

		thisc->times = yypvt[-1].val;

		fprintf(ohp, "#define %s %d\n", yypvt[0].string, yypvt[-1].val);
	}break;

case 11: {

#line 93 "../scn.y"

		if (NULL == thisc)
			yyerror("Unmatched done statement");
		thisc = NULL;
	}break;

case 15: {

#line 101 "../scn.y"
 this->flags |= LONGFIELD; }break;

case 16: {

#line 102 "../scn.y"
 this->flags |= READONLY; }break;

case 17: {

#line 103 "../scn.y"
 this->flags |= EXTERNAL; }break;

case 18: {

#line 104 "../scn.y"

		if (NULL != this->Default)
			yyerror("Default already set");
		this->Default = genname();
		fprintf(ofp, "static char %s[] = %s;\n", this->Default, yypvt[0].string);
		}break;

case 19: {

#line 110 "../scn.y"

		if (NULL != this->Default)
			yyerror("Default already set");
		this->Default = yypvt[0].string;
		}break;

case 20: {

#line 115 "../scn.y"

		if (NULL != this->help)
			yyerror("Help already set");
		this->help = genname();
		fprintf(ofp, "static char %s[] = %s;\n", this->help, yypvt[0].string);
		}break;

case 21: {

#line 121 "../scn.y"

		if (NULL != this->help)
			yyerror("Help already set");
		this->help = yypvt[0].string;
		}break;

case 22: {

#line 126 "../scn.y"

		if (0 >= yypvt[0].val || 254 < yypvt[0].val)
			yyerror("Invalid skip number");
		if (this->skipf)
			yyerror("Skip factor already set");
		this->skipf = yypvt[0].val;
		}break;

case 23: {

#line 133 "../scn.y"

		if (NULL != this->verify)
			yyerror("Verify already set");
		this->verify = yypvt[0].string;
	}break;

case 24: {

#line 138 "../scn.y"

		if (this->skipf)
			yyerror("Skip factor already set");
		if (NULL == thisc)
			yyerror("group must be in do");
		else
			this->skipf = 255;
		}break;

		}
		ip = &yygo[ yypgo[yypdnt[pno]] ];
		while( *ip!=*yys && *ip!=YYNOCHAR )
			ip += 2;
		yystate = ip[1];
		goto stack;
	}
}




