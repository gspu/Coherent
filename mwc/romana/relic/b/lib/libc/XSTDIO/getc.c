/*
 * libc/stdio/getc.c
 * ANSI-compliant C standard i/o library.
 * getc()
 * ANSI 4.9.7.5.
 * Get character from stream.
 */

#include <stdio.h>
#undef	getc

int
getc(stream) FILE *stream;
{
	return (*stream->_f2p->_gt)(stream);
}

/* end of libc/stdio/getc.c */
