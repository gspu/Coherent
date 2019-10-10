/*	Bc - an infix desk calculator */

%{
#include <stdio.h>
#include "bc.h"
#define	YYTNAMES

/* This will generate yyerror() and error recovery */
#define YYSYNTAX	goto YYerract
/* This will enter error recovery */
#define YYERROR		++yyerrflag; goto YYerract

static code	*breakloc	= NULL;	/* where to go on break statement */
static code	*contloc	= NULL;	/* where to go on continue statement */
static dicent	*retfrom	= NULL;	/* what function to return from */
static dicent	**dvec;			/* list of locals */
static dicent	**pardvec;		/* list of formal parameters */
static dicent	**autdvec;		/* list of automatic variables */
static int	ldvec;			/* length of dvec */
static int	lpardvec;		/* length of pardvec */
static int	lautdvec;		/* length of autdvec */
%}


%union	{
	opcode	opcode;
	rvalue	*lvalue;
	char	*svalue;
	dicent	*dvalue;
	int	ivalue;
	code	*location;
}


%token	<lvalue>	NUMBER
%token	<svalue>	STRING
%token	<dvalue>	IDENTIFIER
%token			ADDAB	AUTO	BREAK	CONTINUE DECR	DEFINE
%token			DIVAB	DO	DOT	ELSE	EQP	 ERROR
%token			EXPAB	FOR	GEP	GTP	IBASE	 IF
%token			INCR	LENGTH_	LEP	LTP	MULAB	 NEP
%token			OBASE	QUIT	REMAB	RETURN_	SCALE_	 SQRT_
%token			SUBAB	WHILE

%type	<opcode>	assignment_op	special_load	special_store
%type	<opcode>	function_like_primitive		relational
%type	<dvalue>	local
%type	<ivalue>	argument_list	non_empty_argument_list
%type	<location>	jump_true	jump_false	else_part
%type	<location>	goto		save_loc	save_break
%type	<location>	save_continue

%right	'='
%left	'+'	'-'
%left	'*'	'/'	'%'
%right	'^'
%left	INCR	DECR	UMINUS

%%
/* Error recovery: allok is FALSE if any syntax or semantic error
 * occurred, but the error production will be reduced in all cases
 * except one, the case where cstream overflows.
 */

session:
	/* empty */
|	session statement {
		emitop(STOP);
immfix:		if (allok)
			interp();
		litfree(cstream, loc);
		loc = cstream;
		allok = TRUE;
		breakloc = contloc = retfrom = NULL;
	}
|	session definition {
		if (loc[-1].opcode != RETURN) {
			emitop(PGLSC);
			emitnum(&zero);
			emitop(LOAD);
			emitop(RETURN);
			emitid(retfrom);
		}
deffix:		if (allok)
			install(&retfrom->globalv.fvalue, pardvec, lpardvec,
				autdvec, lautdvec);
		else {
			remloc(pardvec, lpardvec);
			remloc(autdvec, lautdvec);
			retfrom->globalt = UNDEFINED;
			litfree(cstream, loc);
		}
		mpfree(pardvec);
		lpardvec = 0;
		pardvec = NULL;
		mpfree(autdvec);
		lautdvec = 0;
		autdvec = NULL;
		mpfree(dvec);
		ldvec = 0;
		dvec = NULL;
		loc = cstream;
		allok = TRUE;
		breakloc = contloc = retfrom = NULL;
	}
|	session error '\n' {
		YYERROK;
		if (retfrom != NULL)
			goto deffix;
		else
			goto immfix;
	}
;

/*
 *	Statements.
 */

statement:
	/*
	 * The code generated for an IF statement  with an else part is as
	 * follows:
	 *			if not condition, goto L1
	 *			true part of IF statement
	 *			goto L2
	 *	L1:		false part of IF statement
	 *	L2:
	 *
	 * If there is no ELSE part, then the code generated is as
	 * follows:
	 *			if not condition, goto L1
	 *			true part of IF statement
	 *	L1:
	 */
	IF '(' jump_false ')'					/* $1  */
		statement					/* $5  */
		else_part {					/* $6  */
			patch($3, $6);
		}
|
	/*
	 * The code generated for the WHILE statement is as
	 * follows:
	 *	CONTINUE:	if condition, goto L1
	 *	BREAK:		goto L2
	 *	L1:		body of WHILE statement
	 *			goto CONTINUE
	 *	L2:
	 */
	WHILE save_continue '(' jump_true ')'			/* $1  */
		save_break goto					/* $6  */
		save_loc statement				/* $8  */
		goto {						/* $10 */
			patch($4, $8);
			patch($7, loc);
			patch($10, contloc);
			breakloc = $6;			/* restore break */
			contloc = $2;			/* restore continue */
		}
|
	/*
	 * The code produced for the FOR statement is as
	 * follows:
	 *			initialization expression
	 *	L1:		if condition, goto L3
	 *	BREAK:		goto L4
	 *	CONTINUE:	increment expression
	 *			goto L1
	 *	L3:		body of FOR statement
	 *			goto CONTINUE
	 *	L4:
	 */
	FOR '(' optional_expression_list';'			/* $1  */
		save_loc jump_true ';'				/* $5  */
		save_break goto					/* $8  */
		save_continue optional_expression_list ')'	/* $10 */
		goto						/* $13 */
		save_loc statement				/* $14 */
		goto {						/* $16 */
			patch($6, $14);
			patch($9, loc);
			patch($13, $5);
			patch($16, contloc);
			breakloc = $8;
			contloc = $10;
		}
|
	/*
	 * The code produced for the DO-WHILE statement is as
	 * follows:
	 *			goto L1
	 *	BREAK:		goto L3
	 *	CONTINUE:	goto L2
	 *	L1:		body of DO-WHILE statement
	 *	L2:		if condition, goto L1
	 *	L3:
	 */
	DO goto							/* $1  */
		save_break goto					/* $3  */
		save_continue goto				/* $5  */
		save_loc statement WHILE '('			/* $7  */
		save_loc jump_true ')' end_of_statement {	/* $11 */
			patch($2, $7);
			patch($4, loc);
			patch($6, $11);
			patch($12, $7);
			breakloc = $3;
			contloc = $5;
		}
|	BREAK end_of_statement {
		if (breakloc == NULL) {
			gerror("Break not in loop");
			YYERROR;
		}
		emitop(BRALW);
		emitaddr(breakloc);
	}
|	CONTINUE end_of_statement {
		if (contloc == NULL) {
			gerror("Continue not in loop");
			YYERROR;
		}
		emitop(BRALW);
		emitaddr(contloc);
	}
|	RETURN_ end_of_statement {
		if (retfrom == NULL) {
			gerror("Return not in function");
			YYERROR;
		}
		emitop(PGLSC);
		emitnum(&zero);
		emitop(LOAD);
		emitop(RETURN);
		emitid(retfrom);
	}
|	RETURN_ expression end_of_statement {
		if (retfrom == NULL) {
			gerror("Return not in function");
			YYERROR;
		}
		emitop(RETURN);
		emitid(retfrom);
	}
|	assignment_expression end_of_statement {
		emitop(POP);
	}
|	non_assignment_expression end_of_statement {
		emitop(PRNUM);
		emitop(PRNL);
	}
|	non_assignment_expression '$' end_of_statement {
		emitop(PRNUM);
	}
|	STRING end_of_statement {
		emitop(PRSTR);
		emitstr($1);
		emitop(PRNL);
	}
|	STRING '$' end_of_statement {
		emitop(PRSTR);
		emitstr($1);
	}
|	'{' statement_list '}'
|	QUIT end_of_statement {
		emitop(EXIT);
	}
|	end_of_statement
;

end_of_statement:
	';'
|	'\n'
;

statement_list:
	/* empty */
|	statement_list statement
;

else_part:
	/* empty */ {
		$$ = loc;
	}
|	ELSE goto save_loc statement {
		$$ = $3;
		patch($2, loc);
	}
;

optional_expression_list:
	/* empty */
|	non_empty_expression_list
;

non_empty_expression_list:
	expression {
		emitop(POP);
	}
|	non_empty_expression_list ',' expression {
		emitop(POP);
	}
;

goto:
	/* empty */ {
		emitop(BRALW);
		$$ = emitzap;
	}
;

save_loc:
	/* empty */ {
		$$ = loc;
	}
;

save_break:
	/* empty */ {
		$$ = breakloc;
		breakloc = loc;
	}
;

save_continue:
	/* empty */ {
		$$ = contloc;
		contloc = loc;
	}
;

/*
 *	Function definition.
 */

definition:
	definition_header '(' parameter_list ')'		/* $1 */
		optional_nl '{' '\n'				/* $5 */
		optional_auto statement_list '}'		/* $8 */
;

definition_header:
	DEFINE IDENTIFIER {
		if (chkfunc($2)) {
			retfrom = $2;
		} else {
			YYERROR;
		}
	}
;

parameter_list:
	/* empty */
|	non_empty_local_list {
		pardvec = dvec;
		lpardvec = ldvec;
		locaddr(pardvec, lpardvec, 0);
		dvec = NULL;
		ldvec = 0;
	}
;

optional_auto:
	/* empty */
|	AUTO non_empty_local_list end_of_statement {
		autdvec = dvec;
		lautdvec = ldvec;
		locaddr(autdvec, lautdvec, lpardvec);
		dvec = NULL;
		ldvec = 0;
	}
;

non_empty_local_list:
	local {
		dvec = (dicent **)mpalc(ldvec * sizeof (*dvec));
		dvec += ldvec;
		*--dvec = $1;
	}
|	local ',' non_empty_local_list {
		*--dvec = $1;
	}
;

local:
	IDENTIFIER {
		if ($1->localt != UNDEFINED) {
			gerror("Attempt to redeclare %s", $1->word);
			YYERROR;
		}
		$1->localt = SCALAR;
		++ldvec;
		/* $$ = $1 */
	}
|	IDENTIFIER '[' ']' {
		if ($1->localt != UNDEFINED) {
			gerror("Attempt to redeclare %s", $1->word);
			YYERROR;
		}
		$1->localt = ARRAY;
		++ldvec;
		/* $$ = $1 */
	}
;

optional_nl:
	/* empty */
|	'\n'
;

/*
 *	Expressions.
 */

expression:
	assignment_expression
|	non_assignment_expression
;

assignment_expression:
	l_value '=' expression {
		emitop(STORE);
	}
|	l_value add_r_value assignment_op expression {
		emitop($3);
		emitop(STORE);
	}
|	special_store '=' expression {
		emitop($1);
	}
|	special_load assignment_op expression {
		emitop($2);
		emitop($1);
	}
;

non_assignment_expression:
	NUMBER {
		emitop(PLISC);
		emitnum($1);
		emitop(LOAD);
	}
|	l_value {
		emitop(LOAD);
	}
|	special_load
|	IDENTIFIER '(' argument_list ')' {
		if (chkfunc($1)) {
			emitop(CALL);
			emitid($1);
			emitcnt($3);
		} else {
			YYERROR;
		}
	}
|	INCR l_value {
		emitop(PRVAL);
		emitop(INC);
		emitop(STORE);
	}
|	INCR special_load {
		emitop(INC);
		emitop($2);
	}
|	DECR l_value {
		emitop(PRVAL);
		emitop(DEC);
		emitop(STORE);
	}
|	DECR special_load {
		emitop(DEC);
		emitop($2);
	}
|	l_value INCR {
		emitop(PRVAL);
		emitop(INC);
		emitop(STORE);
		emitop(DEC);
	}
|	special_load INCR {
		emitop(INC);
		emitop($1);
		emitop(DEC);
	}
|	l_value DECR {
		emitop(PRVAL);
		emitop(DEC);
		emitop(STORE);
		emitop(INC);
	}
|	special_load DECR {
		emitop(DEC);
		emitop($1);
		emitop(INC);
	}
|	'-' non_assignment_expression	%prec UMINUS {
		emitop(NEG);
	}
|	non_assignment_expression '^' non_assignment_expression {
		emitop(EXP);
	}
|	non_assignment_expression '*' non_assignment_expression {
		emitop(MUL);
	}
|	non_assignment_expression '/' non_assignment_expression {
		emitop(DIV);
	}
|	non_assignment_expression '%' non_assignment_expression {
		emitop(REM);
	}
|	non_assignment_expression '+' non_assignment_expression {
		emitop(ADD);
	}
|	non_assignment_expression '-' non_assignment_expression {
		emitop(SUB);
	}
|	'(' expression ')'
|	function_like_primitive '(' expression ')' {
		emitop($1);
	}
;

l_value:
	IDENTIFIER {
		if (chktype($1, SCALAR))
			sload($1);
		else {
			YYERROR;
		}
	}
|	IDENTIFIER '[' expression ']' {
		if (chktype($1, ARRAY))
			aeload($1);
		else {
			YYERROR;
		}
	}
|	DOT {
		emitop(PGLSC);
		emitnum(&dot);
	}
;

argument:
	expression
|	IDENTIFIER '[' ']' {
		if (chktype($1, ARRAY))
			arload($1);
		else {
			YYERROR;
		}
	}
;

argument_list:
	/* empty */ {
		$$ = 0;
	}
|	non_empty_argument_list
		/* $$ = $1 */
;

non_empty_argument_list:
	argument {
		$$ = 1;
	}
|	non_empty_argument_list ',' argument {
		$$ = $1 + 1;
	}
;

add_r_value:
	/* empty */ {
		emitop(PRVAL);
	}
;

special_store:
	IBASE {
		$$ = SIBASE;
	}
|	OBASE {
		$$ = SOBASE;
	}
|	SCALE_ {
		$$ = SSCALE;
	}
;

special_load:
	IBASE {
		emitop(LIBASE);
		$$ = SIBASE;
	}
|	OBASE {
		emitop(LOBASE);
		$$ = SOBASE;
	}
|	SCALE_ {
		emitop(LSCALE);
		$$ = SSCALE;
	}
;

assignment_op:
	ADDAB {
		$$ = ADD;
	}
|	SUBAB {
		$$ = SUB;
	}
|	MULAB {
		$$ = MUL;
	}
|	DIVAB {
		$$ = DIV;
	}
|	REMAB {
		$$ = REM;
	}
|	EXPAB {
		$$ = EXP;
	}
;

function_like_primitive:
	SQRT_ {
		$$ = SQRT;
	}
|	LENGTH_ {
		$$ = LENGTH;
	}
|	SCALE_ {
		$$ = SCALE;
	}
;

/*
 *	Conditionals.
 */

jump_true:
	/* empty */ {
		emitop(BRALW);
		$$ = emitzap;
	}
|	non_assignment_expression relational non_assignment_expression {
		emitop($2);
		$$ = emitzap;
	}
;

jump_false:
	/* empty */ {
		emitop(BRNEV);
		$$ = emitzap;
	}
|	non_assignment_expression relational non_assignment_expression {
		emitop(negate($2));
		$$ = emitzap;
	}
;

relational:
	LTP {
		$$ = BRLT;
	}
|	LEP {
		$$ = BRLE;
	}
|	EQP {
		$$ = BREQ;
	}
|	GEP {
		$$ = BRGE;
	}
|	GTP {
		$$ = BRGT;
	}
|	NEP {
		$$ = BRNE;
	}
;

%%

/*
 *	Yyerror is the error routine called on a syntax error by
 *	yyparse.
 */

yyerror(m)
char *m;
{
	register struct yytname *ptr;

	for (ptr = yytnames; ptr->tn_name != NULL; ++ptr)
		if (ptr->tn_val == yychar)
			return gerror("%s at %s", m, ptr->tn_name);
	return gerror("%s", m);
}
