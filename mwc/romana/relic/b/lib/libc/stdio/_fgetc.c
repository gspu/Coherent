/*
 * libc/stdio/_fgetc.c
 * ANSI-compliant C standard i/o library internals.
 * _fgetc(), _fgetca()
 * Read character, unbuffered.
 */

#include <stdio.h>
#if	COHERENT || GEMDOS
#include <errno.h>
#endif

int
_fgetc(fp) register FILE *fp;
{
	unsigned char c;
	register int n;

	/* Special kludge: fflush stdout if line buffered. */
	if ((stdout->_ff1 & _IOLBF) != 0)
		fflush(stdout);

	if ((n = read(fileno(fp), &c, 1)) == 1)
		return c;
	else if (n == 0)
		fp->_ff1 |= _FEOF;
	else {
#if	COHERENT || GEMDOS
		if (errno == EINTR)
			errno = 0;
		else
#endif
		fp->_ff1 |= _FERR;
	}
	return EOF;
}

#if	_ASCII

/* ASCII: ignore '\r', map _EOFCHAR to EOF. */
int
_fgetca(fp) register FILE *fp;
{
 	register int c;

	while ((c = _fgetc(fp)) == '\r')
		;
	if (c == _EOFCHAR) {
		fp->_ff |= _FEOF;
		return EOF;
	}
	return c;
}

#endif

/* end of libc/stdio/_fgetc.c */
