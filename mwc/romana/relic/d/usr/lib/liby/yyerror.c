

#include	<stdio.h>


yyerror( mesg)
char	*mesg;
{
	fprintf( stderr, "%s\n", mesg);
}
