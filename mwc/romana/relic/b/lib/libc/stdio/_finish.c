/*
 * libc/stdio/_finish.c
 * ANSI-compliant C standard i/o library internals.
 * _finish()
 * Close all files.
 * Called from exit().
 */

#include <stdio.h>

void
_finish()
{
	register FILE **fpp;

	for (fpp = _fp+_NFILE; --fpp >= _fp; )
		if (*fpp != NULL)
			fclose(*fpp);
}

/* end of libc/stdio/_finish.c */
