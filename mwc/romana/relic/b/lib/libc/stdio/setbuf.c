/*
 * libc/stdio/setbuf.c
 * ANSI-compliant C standard i/o library.
 * setbuf()
 * ANSI 4.9.5.5.
 * Set i/o buffer.
 */

#include <stdio.h>

void
setbuf(stream, buf) FILE *stream; char *buf;
{
	setvbuf(stream, buf, (buf == NULL) ? _IONBF : _IOFBF, BUFSIZ);
}

/* end of libc/stdio/setbuf.c */
