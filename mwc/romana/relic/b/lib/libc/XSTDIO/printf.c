/*
 * libc/stdio/printf.c
 * ANSI-compliant C standard i/o library.
 * printf()
 * ANSI 4.9.6.3.
 * Formatted print to standard output.
 */

#include <stdio.h>
#include <stdarg.h>

int
printf(format) const char *format;
{
	va_list	args;
	int	count;

	va_start(args, format);
	count = vfprintf(stdout, format, args);
	va_end(args);
	return count;
}

/* end of libc/stdio/printf.c */
