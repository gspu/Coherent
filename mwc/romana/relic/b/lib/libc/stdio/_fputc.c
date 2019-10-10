/*
 * libc/stdio/_fputc.c
 * ANSI-compliant C standard i/o library internals.
 * _fputc(), _fputca()
 * Write character, unbuffered.
 * Conditional for _FASCII and errno == EINTR code.
 */

#include <stdio.h>
#if	COHERENT || GEMDOS
#include <errno.h>
#endif

int
_fputc(c, fp) register int c; register FILE *fp;
{
	unsigned char s;

	if (fp->_ff1 & _FERR)
		return EOF;
	s = c;
	if (write(fileno(fp), &s, 1) == 1)
		return s;
#if	COHERENT || GEMDOS
	if (errno == EINTR)
		errno = 0;
	else
#endif
	fp->_ff1 |= _FERR;
	return EOF;
}

#if	_ASCII

/* ASCII: prepend '\r' before '\n'. */
int
_fputca(c, fp) register int c; register FILE *fp;
{
 	if ((unsigned char)c == '\n' && _fputc('\r', fp) == EOF)
		return EOF;
	return (_fputc(c, fp));
}

#endif

/* end of libc/stdio/_fputc.c */
