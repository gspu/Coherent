/*
 * libc/stdio/fgetpos.c
 * ANSI-compliant C standard i/o library.
 * fgetpos()
 * ANSI 4.9.9.1.
 * Get file position.
 * Assumes files not longer than LONG_MAX,
 * so fgetpos() is just an ftell().
 * ftell() already sets errno if an error occurs.
 */

#include <stdio.h>

int
fgetpos(stream, pos) FILE *stream; fpos_t *pos;
{
	return ((*pos = (fpos_t)ftell(stream)) != -1L);
}

/* end of libc/stdio/fgetpos.c */
