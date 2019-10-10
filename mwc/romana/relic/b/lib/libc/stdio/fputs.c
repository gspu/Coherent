/*
 * libc/stdio/fputs.c
 * ANSI-compliant C standard i/o library.
 * fputs()
 * ANSI 4.9.7.4.
 * Write string s to stream.
 */

#include <stdio.h>

int
fputs(s, stream) register const char *s; register FILE *stream;
{
	while (*s)
		if (putc(*s++, stream) == EOF && ferror(stream))
			return EOF;
	return 1;
}

/* end of libc/stdio/fputs.c */
