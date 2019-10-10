%{
#include "patchl.h"
struct PatchInfo argInfo;
static void assign_fn();
%}
%union {
	int num;
	Vname name;
	struct PatchInfo pinfo;
	}
%token PEEK ASSIGN COLON IN OUT MAKEDEV LPAREN RPAREN COMMA SIZE
%token <num> NUMBER
%token <name> VARIABLE
%type <num> number
%start arg
%%
arg		: peek_arg
		| assign_arg
		| con_in_arg
		| con_out_arg
		;

peek_arg	: VARIABLE PEEK {
			argInfo.piType = peek_arg;
			strncpy(argInfo.piData.peekData.vname, $1,
			  PATCH_VAR_NAME_LENGTH);
		};

assign_arg	: VARIABLE ASSIGN number {
			assign_fn($1, $3);
		}
		| VARIABLE ASSIGN NUMBER COLON SIZE {
			assign_fn($1, $3);
		};

con_in_arg	: VARIABLE COLON NUMBER COLON IN {
			argInfo.piType = con_in_arg;
			strncpy(argInfo.piData.conData.vname, $1,
			  PATCH_VAR_NAME_LENGTH);
			argInfo.piData.conData.maj = $3;
		};

con_out_arg	: VARIABLE COLON NUMBER COLON OUT {
			argInfo.piType = con_out_arg;
			strncpy(argInfo.piData.conData.vname, $1,
			  PATCH_VAR_NAME_LENGTH);
			argInfo.piData.conData.maj = $3;
		};

number		: NUMBER {
			$$ = $1;
		}
		| MAKEDEV LPAREN NUMBER COMMA NUMBER RPAREN {
			$$ = ($3 << 8) | $5;
		}
		;

%%
static void assign_fn(var, val)
char * var;
int val;
{
	argInfo.piType = assign_arg;
	strncpy(argInfo.piData.assignData.vname, var,
	  PATCH_VAR_NAME_LENGTH);
	argInfo.piData.assignData.newValue = val;
}
