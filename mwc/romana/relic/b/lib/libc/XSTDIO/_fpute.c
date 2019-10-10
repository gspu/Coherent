/*
 * libc/stdio/_fpute.c
 * ANSI-compliant C standard i/o library internals.
 * _fpute()
 * Write character to read-only stream; always returns error.
 */

#include <stdio.h>

int
_fpute(c, fp) int c; FILE *fp;
{
	fp->_ff1 |= _FERR;
	return EOF;
}

/* end of libc/stdio/_fpute.c */
