/*
 * libc/stdio/clearerr.c
 * ANSI-compliant C standard i/o library.
 * clearerr()
 * ANSI 4.9.10.1.
 * Clear end-of-file and error indicators for stream.
 */

#include <stdio.h>
#undef	clearerr

void
clearerr(stream) FILE *stream;
{
	stream->_ff1 &= ~(_FERR|_FEOF);
}

/* end of libc/stdio/clearerr.c */
