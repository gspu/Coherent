/*
 * libc/stdio/scanf.c
 * ANSI-compliant C standard i/o library.
 * scanf()
 * ANSI 4.9.6.4.
 * Formatted input from standard input.
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

int
scanf(format) const char *format;
{
	va_list	args;
	int	count;

	va_start(args, format);
	count = _scanf(stdin, format, args);
	va_end(args);
	return count;
}

/* end of libc/stdio/scanf.c */
