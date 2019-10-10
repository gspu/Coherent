
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
struct yytname yytnames[18] =
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
unsigned yypdnt[25] = {
00, 01, 01, 01, 02, 02, 02, 04, 
03, 03, 03, 03, 05, 05, 05, 06, 
06, 06, 06, 06, 06, 06, 06, 06, 
06  
} ;
unsigned yypn[25] = {
02, 01, 02, 00, 02, 01, 02, 00, 
04, 02, 03, 01, 01, 02, 00, 01, 
01, 01, 02, 02, 02, 02, 02, 02, 
02  
} ;
unsigned yypgo[7] = {
00, 00, 02, 06, 010, 012, 014  
} ;
unsigned yygo[16] = {
0176030, 07, 07, 017, 0176030, 010, 0176030, 011, 
0176030, 021, 0176030, 032, 032, 043, 0176030, 033  
} ;
unsigned yypa[36] = {
00, 016, 022, 026, 030, 034, 036, 042, 
060, 062, 066, 070, 072, 076, 0100, 0104, 
0106, 0110, 0130, 0132, 0134, 0136, 0140, 0146, 
0154, 0162, 0166, 0206, 0210, 0212, 0214, 0216, 
0220, 0222, 0224, 0226  
} ;
unsigned yyact[152] = {
02, 0400, 03, 0413, 04, 0414, 05, 0415, 
06, 0416, 020003, 0177777, 01, 0176030, 012, 0413, 
060000, 0176030, 013, 0402, 060000, 0176030, 020005, 0176030, 
014, 0402, 060000, 0176030, 020013, 0176030, 015, 0401, 
060000, 0176030, 016, 0177777, 02, 0400, 03, 0413, 
04, 0414, 05, 0415, 06, 0416, 01, 0176030, 
020001, 0176030, 020, 0413, 060000, 0176030, 020006, 0176030, 
020007, 0176030, 022, 0400, 060000, 0176030, 020011, 0176030, 
040000, 0177777, 060000, 0176030, 020002, 0176030, 020004, 0176030, 
023, 0403, 024, 0404, 025, 0405, 026, 0406, 
027, 0407, 030, 0411, 031, 0412, 020016, 0176030, 
020012, 0176030, 020021, 0176030, 020020, 0176030, 020017, 0176030, 
034, 0400, 035, 0401, 060000, 0176030, 036, 0402, 
037, 0410, 060000, 0176030, 040, 0400, 041, 0401, 
060000, 0176030, 042, 0400, 060000, 0176030, 023, 0403, 
024, 0404, 025, 0405, 026, 0406, 027, 0407, 
030, 0411, 031, 0412, 020010, 0176030, 020014, 0176030, 
020023, 0176030, 020022, 0176030, 020026, 0176030, 020030, 0176030, 
020025, 0176030, 020024, 0176030, 020027, 0176030, 020015, 0176030  
} ;
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




