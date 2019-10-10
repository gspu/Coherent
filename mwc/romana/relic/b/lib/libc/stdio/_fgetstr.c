/*
 * libc/stdio/_fgetstr.c
 * ANSI-compliant C standard i/o library internals.
 * _fgetstr()
 * Get character from string stream.
 */

#include <stdio.h>

int
_fgetstr(fp) register FILE *fp;
{
	if (--fp->_cc < 0) {
		fp->_cc = 0;
		fp->_ff1 |= _FEOF;
		return EOF;
	}
	return (*fp->_cp++);
}

/* end of libc/stdio/_fgetstr.c */
