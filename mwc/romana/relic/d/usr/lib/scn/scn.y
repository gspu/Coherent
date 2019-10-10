%{
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

%}

%union {
	char *string;
	int  val;
}

%token	<string> NAME
%token  <string> STRING
%token  <val>	 NUMBER

%token EXT	/* external field do not define */
%token RONLY	/* readonly field */
%token LONG	/* long field */
%token DEFAULT  /* default for field */
%token SKIP	/* skip factor */
%token GROUP	/* group skip */
%token HELP	/* help message */
%token VERIFY	/* verify function */
%token NL	/* new line */
%token DO	/* repeat group */
%token DONE	/* end of repeat group */
%token DESIG	/* change field designator */

%%

file : line | file line | /* nothing */ ;

line	: command NL
	| NL 
	| error NL {
		yyerrok; 
	};

command	: NAME NUMBER {
		if (0 >= $2)
			yyerror("Invalid field size");
		this = alloc(sizeof(*this));
		if (NULL == locs)
			last = locs = this;
		else
			last = last->next = this;
			
		this->field = $1;
		this->len = $2; 

		if ((NULL != thisc) && (!thisc->count++))
			thisc->from = this;
      	} options

	| DESIG STRING {
		extern char fieldDesig;

		fieldDesig = $2[1];
	}

	| DO NUMBER NAME {
		if (NULL != thisc)
			yyerror("Nested do statements");
		if (0 >= $2)
			yyerror("Invalid repeat count");

		thisc = alloc(sizeof(*thisc));
		if (NULL == clumps)
			lastc = clumps = thisc;
		else
			lastc = lastc->next = thisc;

		thisc->times = $2;

		fprintf(ohp, "#define %s %d\n", $3, $2);
	}

	| DONE {
		if (NULL == thisc)
			yyerror("Unmatched done statement");
		thisc = NULL;
	};

options : option | options option | /* nothing */ ;

option	: LONG	{ this->flags |= LONGFIELD; }
	| RONLY	{ this->flags |= READONLY; }
	| EXT	{ this->flags |= EXTERNAL; }
	| DEFAULT STRING {
		if (NULL != this->Default)
			yyerror("Default already set");
		this->Default = genname();
		fprintf(ofp, "static char %s[] = %s;\n", this->Default, $2);
		}
	| DEFAULT NAME {
		if (NULL != this->Default)
			yyerror("Default already set");
		this->Default = $2;
		}
	| HELP STRING {
		if (NULL != this->help)
			yyerror("Help already set");
		this->help = genname();
		fprintf(ofp, "static char %s[] = %s;\n", this->help, $2);
		}
	| HELP NAME {
		if (NULL != this->help)
			yyerror("Help already set");
		this->help = $2;
		}
	| SKIP NUMBER {
		if (0 >= $2 || 254 < $2)
			yyerror("Invalid skip number");
		if (this->skipf)
			yyerror("Skip factor already set");
		this->skipf = $2;
		}
	| VERIFY NAME {
		if (NULL != this->verify)
			yyerror("Verify already set");
		this->verify = $2;
	}
	| SKIP GROUP {
		if (this->skipf)
			yyerror("Skip factor already set");
		if (NULL == thisc)
			yyerror("group must be in do");
		else
			this->skipf = 255;
		};
