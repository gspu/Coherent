/*
 * libc/stdio/fopen.c
 * ANSI-compliant C standard i/o library.
 * fopen()
 * ANSI 4.9.5.3.
 * Open file with first available stream.
 */

#include <stdio.h>

FILE *
fopen(filename, mode) const char *filename, *mode;
{
	register FILE **fpp;

	for (fpp = &_fp[0]; fpp < &_fp[_NFILE]; fpp++)
		if (*fpp==NULL || !((*fpp)->_ff2 & _FINUSE))
			return (*fpp = _fopen(filename, mode, *fpp, -1));
	return NULL;
}

/* end of libc/stdio/fopen.c */
