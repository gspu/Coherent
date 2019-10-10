/*
 * libc/stdio/_fgete.c
 * ANSI-compliant C standard i/o library internals.
 * _fgete()
 * Read character from write-only stream; always returns error.
 */

#include <stdio.h>

int
_fgete(fp) FILE *fp;
{
	fp->_ff1 |= _FERR;
	return EOF;
}

/* end of libc/stdio/_fgete.c */
