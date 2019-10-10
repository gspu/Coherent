/*
 * libc/stdio/fsetpos.c
 * ANSI-compliant C standard i/o library.
 * fsetpos()
 * ANSI 4.9.9.3.
 * Set file position.
 * Assumes files not longer than LONG_MAX,
 * so fsetpos() is just an fseek().
 * fseek() does not set errno if an error occurs.
 */

#include <stdio.h>
#include <errno.h>

#define	EFSETPOS	EINVAL

int
fsetpos(stream, pos) FILE *stream; const fpos_t *pos;
{
	register int status;

	if ((status = fseek(stream, (long)(*pos), SEEK_SET)) != 0)
		errno = EFSETPOS;
	return status;
}

/* end of libc/stdio/fsetpos.c */
