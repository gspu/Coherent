/*
 * libc/stdio/sprintf.c
 * ANSI-compliant C standard i/o library.
 * sprintf()
 * ANSI 4.9.6.5.
 * Formatted print into given string.
 */

#include <stdio.h>
#include <stdarg.h>

int
sprintf(s, format) char *s; const char *format;
{
	va_list		args;

	va_start(args, format);
	return vsprintf(s, format, args);
}

/* end of libc/stdio/sprintf.c */
