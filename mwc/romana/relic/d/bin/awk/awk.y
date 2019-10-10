/*
 * AWK grammar
 * yacc -prods 120 -terms 80 -nterms 40 -states 200 awk.y
 * to make it fit on the PDP-11 (non separated I/D).
 * Do not be surprised at the 48 S/R
 * conflicts that this grammar gets
 * from yacc.  Worry if you get more
 * though.  This is mostly due to
 * the combination of implicit string
 * concatenation mixed with embedded assignments.
 * In addition, 12 of these come from regular
 * expressions.
 * (NOTE: unary minus cannot be done because
 * it is totally ambiguous with string concatentation.
 * and output redirection is disambiguated by disallowing
 * the relation '>' in `print' and `printf' statements.)
 */

%{
#include "awk.h"

%}

%union {
	int	u_char;
	CHAR	*u_charp;
	NODE	*u_node;
	int	(*u_func)();
}

%token	IF_ WHILE_ FOR_ ELSE_ BREAK_ CONTINUE_ NEXT_ EXIT_
%token	IN_
%token	PRINT_ PRINTF_
%token	BEGIN_ END_
%token	FAPPEND_ FOUT_
%token <u_node>	ID_ STRING_ NUMBER_ FUNCTION_

%type <u_node>	linelist line pattern regexp re
%type <u_node>	stat stlist compound assignment
%type <u_node>	variable terminal constant exp e
%type <u_node>	output elist altlst error
%type <u_char>	reclosure assignop

%left	SCON_
%right	ASADD_ ASSUB_ ASMUL_ ASDIV_ ASMOD_ '='
%left	OROR_
%left	ANDAND_
%left	'~' NMATCH_
%nonassoc EQ_ NE_
%nonassoc GE_ LE_ '>' '<'
%left	'+' '-'
%left	'*' '/' '%'
%nonassoc '!' INC_ DEC_
%nonassoc '$'
%nonassoc '(' '['

/*
 * Special tokens for
 * regular expressions
 * and expression precedences.
 */
%token	REEOL_ REBOL_ REANY_
%token <u_charp> RECLASS_
%token <u_char>	 RECHAR_

%left	REOR_
%left	RECON_
%nonassoc RECLOS_ RENECL_ REZOCL_

%%
code:
	linelist {
		codep = $1;
	}
	;

linelist:
	line linelist {
		$$ = node(ALIST, $1, $2);
	}
      | line
	;

line:
	compound '\n' {
		$$ = node(AROOT, NULL, $1);
	}
      | pattern '\n' {
		$$ = node(AROOT, $1, NULL);
	}
      | pattern compound '\n' {
		$$ = node(AROOT, $1, $2);
	}
	;

pattern:
	BEGIN_ {
		$$ = node(ABEGIN);
	}
      | END_ {
		$$ = node(AEND);
	}
      | e
      | e ',' e {
		$$ = node(ARANGE, $1, $3);
	}
	;

regexp:
	'/' { lexre=1; } re '/' {
		lexre = 0;
		$$ = $3;
	}
      | error '\n' {
		yyerrok;
		yyerror("Badly formed regular expression");
	} regexp {
		$$ = $4;
	}
	;

re:
	re re  %prec RECON_ {
		register NODE *np;

		for (np = $1; np->n_O3!=NULL; np = np->n_O3)
			;
		np->n_O3 = $2;
		$$ = $1;
	}
      | '(' re ')' {
		$$ = $2;
	}
      | re REOR_ re {
		$$ = node(AROR, $1, $3, NULL);
	}
      | re reclosure {
		$$ = node($2, $1, NULL, NULL);
	}
      | RECLASS_ {
		$$ = node(yflag?ARDCLASS:ARCLASS, NULL, NULL, NULL);
		$$->n_o1.n_charp = $1;
	}
      | REANY_ {
		$$ = node(ARANY, NULL, NULL, NULL);
	}
      | REBOL_ {
		$$ = node(ARBOL, NULL, NULL, NULL);
	}
      | REEOL_ {
		$$ = node(AREOL, NULL, NULL, NULL);
	}
      | RECHAR_ {
		$$ = cnode(yflag?ARDCHAR:ARCHAR, $1);
	}
	;

reclosure:
	RECLOS_ {
		$$ = ARCLOS;
	}
      | REZOCL_ {
		$$ = ARZOCL;
	}
      | RENECL_ {
		$$ = ARNECL;
	}
	;

compound:
	'{' stlist '}' {
		$$ = $2;
	}
	;

stat:
	IF_ '(' e specparen stat {
		$$ = node(AIF, $3, $5, NULL);
	}
      | IF_ '(' e specparen stat ELSE_ {nlskip = 1;} stat {
		$$ = node(AIF, $3, $5, $8);
	}
      | WHILE_ '(' e specparen stat {
		$$ = node(AWHILE, $3, $5);
	}
      | FOR_ '(' variable IN_ variable specparen stat {
		$$ = node(AFORIN, $3, $5, $7);
	}
      | FOR_ '(' e ';' e ';' e specparen stat {
		$$ = node(AFOR, $3, $5, $7, $9);
	}
      | FOR_ '(' ';' e ';' e specparen stat {
		$$ = node(AFOR, NULL, $4, $6, $8);
	}
      | BREAK_ ';' {
		$$ = node(ABREAK);
	}
      | CONTINUE_ ';' {
		$$ = node(ACONTIN);
	}
      | compound
      | e ';'
      | PRINT_ {outflag++;} altlst output {
		$$ = node(APRINT, $3, $4);
	}
      | PRINT_ {outflag++;} output {
		$$ = node(APRINT, &xfield0, $3);
	}
      | PRINTF_ {outflag++;} altlst output {
		$$ = node(APRINTF, $3, $4);
	}
      | NEXT_ ';' {
		$$ = node(ANEXT);
	}
      | EXIT_ ';' {
		$$ = node(AEXIT);
	}
      | ';' {
		$$ = NULL;
	}
	;

stlist:
	stat
      | stat stlist {
		if ($1!=NULL && $2!=NULL)
			$$ = node(ALIST, $1, $2);
		else if ($1 != NULL)
			$$ = $1;
		else
			$$ = $2;
	}
	;

specparen:
	')' {
		nlskip = 1;
	}
	;

assignment:
	variable '=' exp {
		$$ = node(AASGN, $1, $3);
	}
      | variable assignop exp {
		$$ = node(AASGN, $1, node($2, $1, $3));
	}
      | INC_ variable {
		$$ = node(AASGN, $2, node(AADD, $2, &xone));
	}
      | variable INC_ {
		$$ = node(AINCA, $1);
	}
      | DEC_ variable {
		$$ = node(AASGN, $2, node(ASUB, $2, &xone));
	}
      | variable DEC_ {
		$$ = node(ADECA, $1);
	}
	;

assignop:
	ASADD_ {
		$$ = AADD;
	}
      | ASSUB_ {
		$$ = ASUB;
	}
      | ASMUL_ {
		$$ = AMUL;
	}
      | ASDIV_ {
		$$ = ADIV;
	}
      | ASMOD_ {
		$$ = AMOD;
	}
	;

variable:
	'$' '(' e ')' {
		$$ = node(AFIELD, $3);
	}
      | '$' terminal {
		$$ = node(AFIELD, $2);
	}
      | ID_ '[' exp ']' {
		$$ = node(AARRAY, $1, $3);
	}
      | ID_
	;

constant:
	STRING_
      | NUMBER_
	;

terminal:
	variable
      | constant
	;

exp:
	e
      | exp e  %prec SCON_ {
		$$ = node(ACONC, $1, $2);
	}
	;

e:
	'(' exp ')' {
		$$ = $2;
	}
      | e ANDAND_ e {
		$$ = node(AANDAND, $1, $3);
	}
      | e OROR_ e {
		$$ = node(AOROR, $1, $3);
	}
      | '!' e {
		$$ = node(ANOT, $2);
	}
      | '-' e %prec '!' {
		$$ = node(ANEG, $2);
	}
      | terminal
      | assignment
      | e '+' e {
		$$ = node(AADD, $1, $3);
	}
      | e '-' e {
		$$ = node(ASUB, $1, $3);
	}
      | e '*' e {
		$$ = node(AMUL, $1, $3);
	}
      | e '/' e {
		$$ = node(ADIV, $1, $3);
	}
      | e '%' e {
		$$ = node(AMOD, $1, $3);
	}
      | e '>' e {
		$$ = node(AGT, $1, $3);
	}
      | e '<' e {
		$$ = node(ALT, $1, $3);
	}
      | e EQ_ e {
		$$ = node(AEQ, $1, $3);
	}
      | e NE_ e {
		$$ = node(ANE, $1, $3);
	}
      | e GE_ e {
		$$ = node(AGE, $1,$3);
	}
      | e LE_ e {
		$$ = node(ALE, $1, $3);
	}
      | e '~' regexp {
		$$ = node(AREMAT, $1, $3);
	}
      | e NMATCH_ regexp {
		$$ = node(ARENMAT, $1, $3);
	}
      | regexp {
		if (brlevel)
			awkerr("Regular expression illegal in action");
		$$ = node(ARE, $1);
	}
      | FUNCTION_ '(' elist ')' {
		$$ = node(AFUNC, $1, $3);
	}
      | FUNCTION_ {
		$$ = node(AFUNC, $1, NULL);
	}
	;

output:
	FAPPEND_ terminal ';' {
		$$ = node(AFAPP, $2);
	}
      | FOUT_ terminal  %prec '$' ';' {
		$$ = node(AFOUT, $2);
	}
      | '|' terminal ';' {
		$$ = node(AFPIPE, $2);
	}
      | ';' {
		$$ = NULL;
	}
	;

altlst : '(' elist ')' {
		$$ = $2;
	 }
	| elist {
		$$ = $1;
	 }
	 ;

elist:
	exp {
		$$ = $1;
	}
      | exp ',' elist {
		$$ = node(ALIST, $1, $3);
	}
	;
