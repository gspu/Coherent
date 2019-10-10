/*
 * libc/stdio/feof.c
 * ANSI-compliant C standard i/o library.
 * feof()
 * ANSI 4.9.10.2.
 * Test the end-of-file indicator for stream.
 */

#include <stdio.h>
#undef	feof

int
feof(stream) FILE *stream;
{
	return stream->_ff1 & _FEOF;
}

/* end of libc/stdio/feof.c */
