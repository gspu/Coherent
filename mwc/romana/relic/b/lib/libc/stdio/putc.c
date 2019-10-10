/*
 * libc/stdio/putc.c
 * ANSI-compliant C standard i/o library.
 * putc()
 * ANSI 4.9.7.8.
 * Write character c to stream.
 */

#include <stdio.h>
#undef	putc

int
putc(c, stream) int c; FILE *stream;
{
	return (*stream->_f2p->_pt)(c,stream);
}

/* end of libc/stdio/putc.c */
