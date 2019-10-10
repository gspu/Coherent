/*
 * libc/stdio/fgetc.c
 * ANSI-compliant C standard i/o library.
 * fgetc()
 * ANSI 4.9.7.1.
 * Get character from stream.
 */

#include <stdio.h>

int
fgetc(stream) FILE *stream;
{
	return getc(stream);
}

/* end of libc/stdio/fgetc.c */
