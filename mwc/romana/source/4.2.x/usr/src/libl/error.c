/* (-lgl
 * 	COHERENT Version 3.1
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */

#include <stdio.h>
#include <stdarg.h>

extern yyline;

void
error (format)
char * format;
{
	va_list		args;

	if (yyline)
		fprintf(stderr, "%d: ", yyline);

	va_start (args, format);
	vfprintf (stderr, format, args);
	va_end (args);

	fputc ('\n', stderr);
	exit (1);
}
