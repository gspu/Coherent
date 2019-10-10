/*
 * libc/stdio/fscanf.c
 * ANSI-compliant C standard i/o library.
 * fscanf()
 * ANSI 4.9.6.2.
 * Formatted input from FILE.
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

int
fscanf(stream, format) FILE *stream; const char *format;
{
	va_list	args;
	int	count;

	va_start(args, format);
	count = _scanf(stream, format, args);
	va_end(args);
	return count;
}

/* end of libc/stdio/fscanf.c */
