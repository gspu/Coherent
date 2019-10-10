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
	register int	count;
	FILE		file;
	_FILE2		file2;

	file._f2p = &file2;
	va_start(args, format);
	_stropen(s, -1, &file);
	count = vfprintf(&file, format, args);
	putc('\0', &file);
	return count;
}

/* end of libc/stdio/sprintf.c */
