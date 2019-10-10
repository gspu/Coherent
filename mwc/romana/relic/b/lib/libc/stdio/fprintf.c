/*
 * libc/stdio/fprintf.c
 * ANSI-compliant C standard i/o library.
 * fprintf()
 * ANSI 4.9.6.1.
 * Formatted print to a specific file.
 */

#include <stdio.h>
#include <stdarg.h>

int
fprintf(fp, format) FILE *fp; const char *format;
{
	va_list	args;
	int	count;

	va_start(args, format);
	count = vfprintf(fp, format, args);
	va_end(args);
	return count;
}

/* end of libc/stdio/fprintf.c */
