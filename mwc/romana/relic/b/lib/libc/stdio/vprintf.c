/*
 * libc/stdio/vprintf.c
 * ANSI-compliant C standard i/o library.
 * vprintf()
 * ANSI 4.9.6.8.
 * Formatted print to standard output.
 */

#include <stdio.h>
#include <stdarg.h>

int
vprintf(format, args) const char *format; va_list args;
{
	return vfprintf(stdout, format, args);
}

/* end of libc/stdio/vprintf.c */
