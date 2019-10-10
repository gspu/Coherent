/*
 * libc/stdio/_filbuf.c
 * ANSI-compliant C standard i/o library internals.
 * _filbuf()
 * This function is defined for compatability with Unix <stdio.h> internals.
 * The Unix version of getc() does a _filbuf() to get a buffer of
 * characters when the buffer is empty.
 */

#include <stdio.h>

int
_filbuf(fp) register FILE *fp;
{
	++fp->_cc;
	return (*fp->_f2p->_gt)(fp);
}

/* end of libc/stdio/_filbuf.c */
