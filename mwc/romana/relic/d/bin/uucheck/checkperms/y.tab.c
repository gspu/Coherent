
#include "y.tab.h"
#define YYCLEARIN yychar = -1000
#define YYERROK yyerrflag = 0
extern int yychar;
extern short yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
YYSTYPE yyval, yylval;

#line 176 "checkperms.y"


#include <stdio.h>
#include "checkperms.h"
#include "monitor.h"

yyerror (s)
	char *s;
{
	fprintf (stderr, "line %d: %s, Token: %d\n", lineno, s, yychar);
}
#ifdef YYTNAMES
struct yytname yytnames[21] =
{
	"$end", -1, 
	"error", -2, 
	"FOREACH", 256, 
	"PATHNAME", 257, 
	"STRING", 258, 
	"INDENT", 259, 
	"UNINDENT", 260, 
	"NL", 261, 
	"ERROR_TOKEN", 262, 
	"WARNING_TOKEN", 263, 
	"EXIST", 264, 
	"OWNER", 265, 
	"GROUP", 266, 
	"CHMOD_PERMISSION", 267, 
	"NUMBER", 268, 
	"FILE_TOKEN", 269, 
	"DIRECTORY", 270, 
	"PIPE", 271, 
	"CHARACTER_SPECIAL", 272, 
	"BLOCK_SPECIAL", 273, 
	NULL
} ;
#endif
unsigned yypdnt[33] = {
00, 01, 01, 02, 02, 02, 02, 02, 
03, 06, 04, 011, 07, 07, 05, 05, 
012, 012, 013, 013, 013, 013, 013, 014, 
015, 015, 015, 015, 015, 016, 017, 010, 
020  
} ;
unsigned yypn[33] = {
02, 01, 02, 01, 01, 02, 01, 02, 
03, 02, 03, 02, 03, 00, 03, 02, 
01, 01, 01, 01, 01, 01, 01, 01, 
01, 01, 01, 01, 01, 02, 02, 01, 
01  
} ;
unsigned yypgo[17] = {
00, 00, 04, 012, 014, 016, 020, 022, 
026, 040, 042, 044, 046, 050, 052, 054, 
056  
} ;
unsigned yygo[48] = {
046, 054, 0176030, 07, 07, 024, 054, 024, 
0176030, 010, 0176030, 011, 0176030, 012, 0176030, 013, 
0176030, 014, 027, 050, 0176030, 047, 03, 022, 
031, 051, 032, 052, 041, 053, 0176030, 021, 
0176030, 015, 0176030, 016, 0176030, 041, 0176030, 042, 
0176030, 043, 0176030, 044, 0176030, 045, 0176030, 00  
} ;
unsigned yypa[46] = {
00, 014, 020, 020, 024, 026, 030, 032, 
050, 052, 054, 056, 062, 066, 072, 0116, 
0120, 0122, 0124, 0126, 0132, 0134, 0136, 0136, 
0142, 020, 020, 0144, 0146, 0150, 0152, 0154, 
0156, 0160, 0164, 0166, 0170, 0172, 00, 0174, 
0176, 0200, 0202, 0204, 0206, 0224  
} ;
unsigned yyact[150] = {
02, 0400, 03, 0401, 04, 0405, 05, 0406, 
06, 0407, 01, 0176030, 017, 0405, 060000, 0176030, 
020, 0402, 060000, 0176030, 020006, 0176030, 020020, 0176030, 
020021, 0176030, 023, 0177777, 02, 0400, 03, 0401, 
04, 0405, 05, 0406, 06, 0407, 01, 0176030, 
020001, 0176030, 020003, 0176030, 020004, 0176030, 025, 0405, 
060000, 0176030, 026, 0405, 060000, 0176030, 027, 0405, 
060000, 0176030, 030, 0410, 031, 0411, 032, 0412, 
033, 0413, 034, 0415, 035, 0416, 036, 0417, 
037, 0420, 040, 0421, 060000, 0176030, 020007, 0176030, 
020037, 0176030, 020011, 0176030, 020013, 0176030, 040000, 0177777, 
060000, 0176030, 020002, 0176030, 020005, 0176030, 046, 0403, 
020015, 0176030, 020027, 0176030, 020026, 0176030, 020030, 0176030, 
020031, 0176030, 020032, 0176030, 020033, 0176030, 020034, 0176030, 
020, 0402, 020017, 0176030, 020022, 0176030, 020023, 0176030, 
020024, 0176030, 020025, 0176030, 020010, 0176030, 020012, 0176030, 
020035, 0176030, 020036, 0176030, 020016, 0176030, 02, 0400, 
03, 0401, 055, 0404, 04, 0405, 05, 0406, 
06, 0407, 01, 0176030, 020014, 0176030  
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

case 7: {

#line 21 "checkperms.y"
yyerrok;}break;

case 8: {

#line 25 "checkperms.y"

			 /* Put lost character back into stdin.  */
			my_ungetchar(delayed_char);
#ifdef DEBUG
			printf("%d:  finished foreach_block.\n", lineno);
#endif /* DEBUG */
		}break;

case 9: {

#line 34 "checkperms.y"

				if(host_looping) {
				  FATAL("%d: foreach can not nest.",
					  lineno);
				} else {
#ifdef DEBUG
/* DEBUG */		  printf("%d: Foreach identified\n", lineno);
#endif /* DEBUG */
				  host_looping = TRUE;
				  current_host = first_host(yypvt[0]);
				  saving_to_monitor_file = TRUE;
				  reading_from_monitor_file = FALSE;
				  open_monitor_file(); /* Start saving input.  */
				} /* if host_looping */
			}break;

case 10: {

#line 52 "checkperms.y"

			/* Once beyond the scope of a path_block, make sure
			 * that there is no current_path set.
			 */
			free(current_path);
			current_path = NULL;
		}break;

case 11: {

#line 61 "checkperms.y"

			if(host_looping) {
				/* We need to build a whole pathname.  */
				current_path = malloc(strlen(current_host)+
						      strlen(yypvt[0]) + 1 );
				/* Insert the current host into the 
				 * partial path provided. */
				sprintf(current_path, yypvt[0], current_host);
			} else {
				if(current_path != NULL) {
				  FATAL("%d: pathnames can not be nested.\n",
					 lineno);
				} /* if there is a current path */
				copy_str(&current_path, yypvt[0]);
			} /* if in a loop of host names */

			/* Now that we have a full pathname, we can build
 			 * a stat structure for it.
			 */
			if (stat(current_path, &current_stat) == -1){
#ifdef DEBUG
/* DEBUG */		printf("Can not stat %s.\n", current_path);
#endif /* DEBUG */
				it_exists = FALSE;
			} /* if stat failed */
			else {
#ifdef DEBUG
/* DEBUG */		printf("Just stat ed %s.\n", current_path);
#endif /* DEBUG */
				it_exists = TRUE;
			} /* else stat succeeded */
			sprintf(bigbuf, "Examining %s.\n", current_path);
			REALLYVERBOSE(bigbuf);
		}break;

case 14: {

#line 101 "checkperms.y"

#ifdef DEBUG
/* DEBUG */		    printf("%d: Message %d, Check %d, String:  %s\n",
				lineno, yypvt[-2], yypvt[-1], yypvt[0]);
#endif /* DEBUG */
			    do_check(yypvt[-2], yypvt[-1], yypvt[0]);
			}break;

case 15: {

#line 108 "checkperms.y"

#ifdef DEBUG
/* DEBUG */		    printf("%d: Message %d, Check %d\n",
				lineno, yypvt[-1], yypvt[0]);
#endif
			    do_check(yypvt[-1], yypvt[0], NULL);
			}break;

case 16: {

#line 119 "checkperms.y"
yyval = ERROR_TOKEN;}break;

case 17: {

#line 121 "checkperms.y"
yyval = WARNING_TOKEN;}break;

case 18: {

#line 126 "checkperms.y"
yyval=yypvt[0];}break;

case 19: {

#line 128 "checkperms.y"
yyval=yypvt[0];}break;

case 20: {

#line 130 "checkperms.y"
yyval=yypvt[0];}break;

case 21: {

#line 132 "checkperms.y"
yyval=yypvt[0];}break;

case 22: {

#line 133 "checkperms.y"

			  yyval=CHMOD_PERMISSION;
			}break;

case 23: {

#line 139 "checkperms.y"
yyval=EXIST;}break;

case 24: {

#line 143 "checkperms.y"
yyval=FILE_TOKEN;}break;

case 25: {

#line 145 "checkperms.y"
yyval=DIRECTORY;}break;

case 26: {

#line 147 "checkperms.y"
yyval=PIPE;}break;

case 27: {

#line 149 "checkperms.y"
yyval=CHARACTER_SPECIAL;}break;

case 28: {

#line 151 "checkperms.y"
yyval=BLOCK_SPECIAL;}break;

case 29: {

#line 154 "checkperms.y"

				yyval=OWNER;
				copy_str(&id_string, yypvt[0]);
			}break;

case 30: {

#line 160 "checkperms.y"

				yyval=GROUP;
				copy_str(&id_string, yypvt[0]);
			}break;

case 31: {

#line 166 "checkperms.y"

			  yyval=ret_string; 
			  ret_string = NULL;
			}break;

case 32: {

#line 172 "checkperms.y"

			  yyval = ret_number;
			}break;

		}
		ip = &yygo[ yypgo[yypdnt[pno]] ];
		while( *ip!=*yys && *ip!=YYNOCHAR )
			ip += 2;
		yystate = ip[1];
		goto stack;
	}
}




