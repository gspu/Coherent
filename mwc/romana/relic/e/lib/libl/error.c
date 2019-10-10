#include <stdio.h>
extern yyline;
error(s)
{
	if (yyline)
		fprintf(stderr, "%d: ", yyline);
	fprintf(stderr, "%r\n", &s);
	exit (1);
}
