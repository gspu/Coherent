/* (-lgl
 * 	COHERENT Version 3.1
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
#include <stdio.h>
extern yyline;
error(s)
{
	if (yyline)
		fprintf(stderr, "%d: ", yyline);
	fprintf(stderr, "%r\n", &s);
	exit (1);
}
