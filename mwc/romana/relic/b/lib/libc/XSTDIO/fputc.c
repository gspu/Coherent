/*
 * libc/stdio/fputc.c
 * ANSI-compliant C standard i/o library.
 * fputc()
 * ANSI 4.9.7.3.
 * Write character c to stream.
 */

#include <stdio.h>

int
fputc(c, stream) int c; FILE *stream;
{
	return putc(c, stream);
}

/* end of libc/stdio/fputc.c */
