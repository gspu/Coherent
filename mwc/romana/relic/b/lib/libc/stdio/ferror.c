/*
 * libc/stdio/ferror.c
 * ANSI-compliant C standard i/o library.
 * ferror()
 * ANSI 4.9.10.3.
 * Test the error indicator for stream.
 */

#include <stdio.h>
#undef	ferror

int
ferror(stream) FILE *stream;
{
	return stream->_ff1 & _FERR;
}

/* end of libc/stdio/ferror.c */
