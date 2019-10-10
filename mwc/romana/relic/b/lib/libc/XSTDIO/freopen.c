/*
 * libc/stdio/freopen.c
 * ANSI-compliant C standard i/o library.
 * freopen()
 * ANSI 4.9.5.4.
 * Open file with specified stream.
 */

#include <stdio.h>

FILE *
freopen(filename, mode, stream) const char *filename, *mode; FILE *stream;
{
	fclose(stream);
	return _fopen(filename, mode, stream, -1);
}

/* end of libc/stdio/freopen.c */
