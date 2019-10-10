/*
 * libc/stdio/_flsbuf.c
 * ANSI-compliant C standard i/o library internals.
 * _flsbuf()
 * This function is defined for compatability with Unix <stdio.h> internals.
 * The Unix version of putc() does a _flsbuf() to flush a buffer of
 * characters when the buffer is full.
 */

#include <stdio.h>

int
_flsbuf(c, fp) register int c; register FILE *fp;
{
	++fp->_cc;
	return (*fp->_f2p->_pt)(c, fp);
}

/* end of libc/stdio/_filbuf.c */
