
#line 1 "../test.y"

/*
 * Set return status based on
 * various specified conditions,
 * mostly related to files.
 * Used mostly in shell files.
 */

#include <stdio.h>
#include <sys/stat.h>
#include <access.h>
#include "testnode.h"

#define	NPRIM	(sizeof(prims)/sizeof(prims[0]))
#define	NFNAME	500		/* size of filename buffer */

#include "y.tab.h"
#define YYCLEARIN yychar = -1000
#define YYERROK yyerrflag = 0
extern int yychar;
extern short yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
YYSTYPE yyval, yylval;

#line 63 "../test.y"

struct	prim	{
	char	*p_name;
	int	p_lval;
	int	p_bin;
}	prims[] = {
	"-r", _R, 0,
	"-w", _W, 0,
	"-f", _F, 0,
	"-d", _D, 0,
	"-s", _S, 0,
	"-t", _T, 0,
	"-z", _Z, 0,
	"-n", _N, 0,
	"-eq", _EQ, 1,
	"-ne", _NE, 1,
	"-gt", _GT, 1,
	"-ge", _GE, 1,
	"-lt", _LT, 1,
	"-le", _LE, 1,
	"-o", OR, 1,
	"-a", AND, 1
};


char	**gav;
int	gac;

struct	stat	sb;

NODE	*code;

char	*next();
NODE	*bnode();
NODE	*lnode();
long	atol();
int	xr();
int	xw();
int	xf();
int	xd();
int	xs();
int	xt();
int	xz();
int	xn();
int	xseq();
int	xsneq();
int	xeq();
int	xne();
int	xgt();
int	xge();
int	xlt();

main(argc, argv)
char *argv[];
{
	gav = argv+1;
	gac = argc-1;
	if (argv[0][0]=='[' && argv[0][1]=='\0') {
		if (strcmp(argv[gac], "]") != 0)
			tsterr("unbalanced [..]");
		gac--;
	}
	if (gac == 0)
		exit(1);
	yyparse();
	exit(!execute(code));
}

/*
 * Lexical analyser
 */
yylex()
{
	static char laststr = 0; /* 1 if last token was string */
	register char *ap;
	register struct prim *pp;

	if ((yylval.fname = ap = next()) == NULL)
		return ('\n');

	if (*ap == '-') {
		for (pp = prims; pp < &prims[NPRIM]; pp++) {
			if (strcmp(pp->p_name, ap) == 0) {
				if (!laststr && pp->p_bin)
					break;
				laststr = 0;
				return (pp->p_lval);
			}
		}
	}
	else {
		laststr = 0;
		if (strcmp("!=", ap) == 0)
			return (SNEQ);

		if (ap[1] == '\0') {
			if (ap[0]==')') {
				laststr = 1;
				return(')');
			}
			if (ap[0]=='(' || ap[0]=='!')
				return (ap[0]);
			if (ap[0]=='=')
				return (SEQ);
		}
	}
	laststr = 1;
	return (STR);
}

yyerror()
{
	fprintf(stderr, "Test expression syntax error\n");
	usage();
}

/*
 * Return the next argument from the arg list.
 */
char *
next()
{
	if (gac < 1)
		return (NULL);
	gac--;
	return (*gav++);
}

/*
 * Build an expression tree node (non-leaf)
 */
NODE *
bnode(op, left, right)
int op;
NODE *left, *right;
{
	register NODE *np;
	char *malloc();

	if ((np = (NODE *)malloc(sizeof (NODE))) == NULL)
		tsterr("Out of space");
	np->n_un.n_op = op;
	np->n_left = left;
	np->n_right = right;
	return (np);
}

/*
 * Build a leaf node in expression tree.
 */
NODE *
lnode(fn, str1, str2)
int (*fn)();
char *str1, *str2;
{
	register NODE *np;
	char *malloc();

	if ((np = (NODE *)malloc(sizeof (NODE))) == NULL)
		tsterr("Out of space");
	np->n_left = np->n_right = NULL;
	np->n_un.n_fun = fn;
	np->n_s1 = str1;
	np->n_s2 = str2;
	return (np);
}

/*
 * Execute compiled code.
 */
execute(np)
register NODE *np;
{
	if (np->n_left != NULL)
		switch (np->n_un.n_op) {
		case AND:
			if (execute(np->n_left) && execute(np->n_right))
				return (1);
			return (0);

		case OR:
			if (execute(np->n_left) || execute(np->n_right))
				return (1);
			return (0);

		case '!':
			return (!execute(np->n_left));

		default:
			tsterr("Panic: bad tree (op %d)", np->n_un.n_op);
		}
	else
		return ((*np->n_un.n_fun)(np));
	/* NOTREACHED */
}

/*
 * Check to see if the file exists
 * and if readable.
 */
xr(np)
NODE *np;
{
	return (access(np->n_s1, AREAD) >= 0);
}

/*
 * Check if the file exists and is
 * writeable.
 */
xw(np)
NODE *np;
{
	return (access(np->n_s1, AWRITE) >= 0);
}

/*
 * Check if the file exists and is not
 * a directory.
 */
xf(np)
NODE *np;
{
	return (stat(np->n_s1, &sb)>=0 && (sb.st_mode&S_IFMT)!=S_IFDIR);
}

/*
 * Check to see if the file exists
 * and is a directory.
 */
xd(np)
NODE *np;
{
	return (stat(np->n_s1, &sb)>=0 && (sb.st_mode&S_IFMT)==S_IFDIR);
}

/*
 * Check to see if the file exists
 * and has a non-zero size.
 */
xs(np)
NODE *np;
{
	return (stat(np->n_s1, &sb)>=0 && sb.st_size>0);
}

/*
 * Check to see if the file
 * descriptor is associated
 * with a terminal.
 */
xt(np)
NODE *np;
{
	return (isatty(atoi(np->n_s1)));
}

/*
 * True if the length of the given
 * string is zero.
 */
xz(np)
NODE *np;
{
	return (np->n_s1[0] == '\0');
}

/*
 * True if the length of the given
 * string is non-zero.
 */
xn(np)
NODE *np;
{
	return (np->n_s1[0] != '\0');
}

/*
 * True if the two strings are
 * lexicographically equal.
 */
xseq(np)
register NODE *np;
{
	return (strcmp(np->n_s1, np->n_s2) == 0);
}

/*
 * True if the two strings are
 * lexicographically unequal.
 */
xsneq(np)
register NODE *np;
{
	return (strcmp(np->n_s1, np->n_s2) != 0);
}

/*
 * True if the two numbers are
 * equal.
 */
xeq(np)
register NODE *np;
{
	return (atol(np->n_s1) == atol(np->n_s2));
}

/*
 * True if the two numbers are
 * not equal.
 */
xne(np)
register NODE *np;
{
	return (atol(np->n_s1) != atol(np->n_s2));
}

/*
 * True if the first number is
 * greater than the second.
 */
xgt(np)
register NODE *np;
{
	return (atol(np->n_s1) > atol(np->n_s2));
}

/*
 * True if the first number is
 * greater than or equal to the second.
 */
xge(np)
register NODE *np;
{
	return (atol(np->n_s1) >= atol(np->n_s2));
}

/*
 * True if the first number is
 * less than the second.
 */
xlt(np)
register NODE *np;
{
	return (atol(np->n_s1) < atol(np->n_s2));
}

/*
 * True if the first number is
 * less than or equal to the second.
 */
xle(np)
register NODE *np;
{
	return (atol(np->n_s1) <= atol(np->n_s2));
}

/*
 * Error messages.
 */
/* VARARGS */
tsterr(x)
{
	fprintf(stderr, "test: %r\n", &x);
	exit(1);
}

usage()
{
	fprintf(stderr, "Usage: test expression\n");
	exit(1);
}
#ifdef YYTNAMES
readonly struct yytname yytnames[26] =
{
	"$end", -1, 
	"error", -2, 
	"OR", 256, 
	"AND", 257, 
	"'!'", 33, 
	"_R", 258, 
	"_W", 259, 
	"_F", 260, 
	"_D", 261, 
	"_S", 262, 
	"_T", 263, 
	"_Z", 264, 
	"_N", 265, 
	"SEQ", 266, 
	"SNEQ", 267, 
	"_EQ", 268, 
	"_NE", 269, 
	"_GT", 270, 
	"_GE", 271, 
	"_LT", 272, 
	"_LE", 273, 
	"STR", 275, 
	"'\\n'", 10, 
	"'('", 40, 
	"')'", 41, 
	NULL
} ;
#endif
#include <action.h>
unsigned char yypdnt[24] = {
0, 1, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2
};
unsigned char yypn[24] = {
2, 2, 3, 2, 3, 3, 2, 2,
2, 2, 2, 2, 1, 2, 2, 3,
3, 3, 3, 3, 3, 3, 3, 1
};
unsigned char yypgo[3] = {
0, 0, 2 
};
unsigned int yygo[12] = {
-1000, 12, 1, 14, 11, 31, 33, 45,
34, 46, -1000, 13 
};
unsigned char yypa[47] = {
0, 0, 24, 28, 32, 36, 40, 44,
48, 52, 56, 0, 74, 78, 86, 88,
90, 92, 94, 96, 98, 100, 102, 104,
108, 112, 116, 120, 124, 128, 132, 136,
144, 0, 0, 148, 150, 152, 154, 156,
158, 160, 162, 164, 166, 168, 172 
};
unsigned int yyact[174] = {
1, 33, 2, 258, 3, 259, 4, 260,
5, 261, 6, 262, 7, 263, 8, 264,
9, 265, 10, 275, 11, 40, 24576, -1000,
15, 275, 24576, -1000, 16, 275, 24576, -1000,
17, 275, 24576, -1000, 18, 275, 24576, -1000,
19, 275, 24576, -1000, 20, 275, 8204, -1000,
21, 275, 24576, -1000, 22, 275, 24576, -1000,
23, 266, 24, 267, 25, 268, 26, 269,
27, 270, 28, 271, 29, 272, 30, 273,
8215, -1000, 32, -1, 24576, -1000, 33, 256,
34, 257, 35, 10, 24576, -1000, 8195, -1000,
8198, -1000, 8199, -1000, 8200, -1000, 8201, -1000,
8202, -1000, 8203, -1000, 8205, -1000, 8206, -1000,
36, 275, 24576, -1000, 37, 275, 24576, -1000,
38, 275, 24576, -1000, 39, 275, 24576, -1000,
40, 275, 24576, -1000, 41, 275, 24576, -1000,
42, 275, 24576, -1000, 43, 275, 24576, -1000,
33, 256, 34, 257, 44, 41, 24576, -1000,
16384, -1, 24576, -1000, 8193, -1000, 8207, -1000,
8208, -1000, 8209, -1000, 8210, -1000, 8211, -1000,
8212, -1000, 8213, -1000, 8214, -1000, 8194, -1000,
34, 257, 8196, -1000, 8197, -1000 
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

#line 35 "../test.y"
 code = yypvt[-1].nodeptr; return; }break;

case 2: {

#line 39 "../test.y"
 yyval.nodeptr = yypvt[-1].nodeptr; }break;

case 3: {

#line 40 "../test.y"
 yyval.nodeptr = bnode('!', yypvt[0].nodeptr, NULL); }break;

case 4: {

#line 41 "../test.y"
 yyval.nodeptr = bnode(OR, yypvt[-2].nodeptr, yypvt[0].nodeptr); }break;

case 5: {

#line 42 "../test.y"
 yyval.nodeptr = bnode(AND, yypvt[-2].nodeptr, yypvt[0].nodeptr); }break;

case 6: {

#line 43 "../test.y"
 yyval.nodeptr = lnode(xr, yypvt[0].fname, NULL); }break;

case 7: {

#line 44 "../test.y"
 yyval.nodeptr = lnode(xw, yypvt[0].fname, NULL); }break;

case 8: {

#line 45 "../test.y"
 yyval.nodeptr = lnode(xf, yypvt[0].fname, NULL); }break;

case 9: {

#line 46 "../test.y"
 yyval.nodeptr = lnode(xd, yypvt[0].fname, NULL); }break;

case 10: {

#line 47 "../test.y"
 yyval.nodeptr = lnode(xs, yypvt[0].fname, NULL); }break;

case 11: {

#line 48 "../test.y"
 yyval.nodeptr = lnode(xt, yypvt[0].fname, NULL); }break;

case 12: {

#line 49 "../test.y"
 yyval.nodeptr = lnode(xt, "1", NULL); }break;

case 13: {

#line 50 "../test.y"
 yyval.nodeptr = lnode(xz, yypvt[0].fname, NULL); }break;

case 14: {

#line 51 "../test.y"
 yyval.nodeptr = lnode(xn, yypvt[0].fname, NULL); }break;

case 15: {

#line 52 "../test.y"
 yyval.nodeptr = lnode(xseq, yypvt[-2].fname, yypvt[0].fname); }break;

case 16: {

#line 53 "../test.y"
 yyval.nodeptr = lnode(xsneq, yypvt[-2].fname, yypvt[0].fname); }break;

case 17: {

#line 54 "../test.y"
 yyval.nodeptr = lnode(xeq, yypvt[-2].fname, yypvt[0].fname); }break;

case 18: {

#line 55 "../test.y"
 yyval.nodeptr = lnode(xne, yypvt[-2].fname, yypvt[0].fname); }break;

case 19: {

#line 56 "../test.y"
 yyval.nodeptr = lnode(xgt, yypvt[-2].fname, yypvt[0].fname); }break;

case 20: {

#line 57 "../test.y"
 yyval.nodeptr = lnode(xge, yypvt[-2].fname, yypvt[0].fname); }break;

case 21: {

#line 58 "../test.y"
 yyval.nodeptr = lnode(xlt, yypvt[-2].fname, yypvt[0].fname); }break;

case 22: {

#line 59 "../test.y"
 yyval.nodeptr = lnode(xle, yypvt[-2].fname, yypvt[0].fname); }break;

case 23: {

#line 60 "../test.y"
 yyval.nodeptr = lnode(xn, yypvt[0].fname, NULL); }break;

		}
		ip = &yygo[ yypgo[yypdnt[pno]] ];
		while( *ip!=*yys && *ip!=YYNOCHAR )
			ip += 2;
		yystate = ip[1];
		goto stack;
	}
}




