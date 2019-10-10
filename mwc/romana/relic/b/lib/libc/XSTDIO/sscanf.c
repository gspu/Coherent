/*
 * libc/stdio/sscanf.c
 * ANSI-compliant C standard i/o library.
 * sscanf()
 * ANSI 4.9.6.6.
 * Formatted input from string.
 */

#include <stdio.h>
#include <stdarg.h>

int
sscanf(s, format) const char *s; const char *format;
{
	va_list		args;
	register int	count;
	FILE		file;
	_FILE2		file2;

	file._f2p = &file2;
	va_start(args, format);
	_stropen(s, (int)strlen(s), &file);
	count = _scanf(&file, format, args);
	va_end(args);
	return count;
}

/* end of libc/stdio/sscanf.c */
