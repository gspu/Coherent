/*
 * libc/stdio/_fgetb.c
 * ANSI-compliant C standard i/o library internals.
 * _fgetb(), _fgetba()
 * Read character, buffered.
 */

#include <stdio.h>
#if	COHERENT || GEMDOS
#include <errno.h>
#endif

int
_fgetb(fp) register FILE *fp;
{
	register _FILE2 *f2p;

	if (--fp->_cc < 0) {
		if (fflush(fp))
			return EOF;

		/* Special kludge: fflush stdout if line buffered. */
		if ((stdout->_ff1 & _IOLBF) != 0)
			fflush(stdout);

		f2p = fp->_f2p;
		if ((fp->_cc = read(fileno(fp), f2p->_dp, (int)(f2p->_ep - f2p->_dp))) == 1) {
#if	COHERENT || GEMDOS
			if (errno == EINTR)
				errno = 0;
			else
#endif
			fp->_ff1 |= _FERR;
			fp->_cc = 0;
			return EOF;
		} else if (fp->_cc == 0) {
			fp->_ff1 |= _FEOF;
			return EOF;
		}
		f2p->_dp += fp->_cc--;
	}
	return (*fp->_cp++);
}

#if	_ASCII

/* ASCII: ignore '\r', map _EOFCHAR to EOF. */
int
_fgetba(fp) register FILE *fp;
{
 	register int c;

	while ((c = _bingetb(fp)) == '\r')
		;
	if (c == _EOFCHAR) {
		fp->_ff |= _FEOF;
		return EOF;
	}
	return c;
}

#endif

/* end of libc/stdio/_fgetb.c */
