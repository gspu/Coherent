%{
#include "patchl.h"
#include "y.tab.h"
#undef input
#define input()		arg_getc()
%}
%%

"?"		return PEEK;
"="		return ASSIGN;
":"		return COLON;
"in"		return IN;
"out"		return OUT;
"makedev"	return MAKEDEV;
"("		return LPAREN;
")"		return RPAREN;
","		return COMMA;
[csil]		return SIZE;

0			{
			yylval.num = 0;
			return NUMBER;
			}
			
[1-9][0-9]*		{
			sscanf(yytext, "%d", &yylval.num);
			return NUMBER;
			}

"0x"[0-9a-fA-F]+	{
			sscanf(yytext+2, "%x", &yylval.num);
			return NUMBER;
			}

"0"[0-7]+		{
			sscanf(yytext+1, "%o", &yylval.num);
			return NUMBER;
			}

"0o"[0-7]+		{
			sscanf(yytext+2, "%o", &yylval.num);
			return NUMBER;
			}

[_$a-zA-Z][_$a-zA-Z0-9]* 	{
			strncpy(yylval.name, yytext, PATCH_VAR_NAME_LENGTH);
			return VARIABLE;
			}
