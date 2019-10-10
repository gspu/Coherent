/*
 * C input/output library.
 * fputs()
 * ANSI 4.9.7.4.
 * Write string s to stream.
 */

#include <stdio.h>

/* Should be "const char * s". */
int
fputs(s, stream) register char *s; register FILE *stream;
{
	while (*s)
		if (putc(*s++, stream) == EOF && ferror(stream))
			return EOF;
	return 1;
}

/* end of fputs.c */
