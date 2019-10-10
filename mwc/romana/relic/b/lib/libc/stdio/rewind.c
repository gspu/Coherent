/*
 * libc/stdio/rewind.c
 * ANSI-compliant C standard i/o library.
 * rewind()
 * ANSI 4.9.9.5
 * Rewind stream.
 */

#include <stdio.h>

void
rewind(stream) register FILE *stream;
{
	if (fseek(stream, 0L, SEEK_SET) == 0)
		stream->_ff1 &= ~_FERR;		/* ANSI 4.9.9.5 */
}

/* end of libc/stdio/rewind.c */
