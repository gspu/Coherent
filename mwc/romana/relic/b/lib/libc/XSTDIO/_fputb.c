/*
 * libc/stdio/_fputb.c
 * ANSI-compliant C standard i/o library internals.
 * _fputb(), _fputba()
 * Write character, buffered.
 */

#include <stdio.h>

int
_fputb(c, fp) register int c; register FILE *fp;
{
	register _FILE2 *f2p;

	f2p = fp->_f2p;
	if (--fp->_cc < 0) {
		if (_fpseek(fp))
			return EOF;
		fp->_cc = f2p->_ep - f2p->_dp - 1;
	}
	return (*fp->_cp++ = (unsigned char)c);
}

#if	_ASCII

/* ASCII: prepend '\r' before '\n'. */
int
_fputba(c, fp) register int c; register FILE *fp;
{
 	if ((unsigned char)c == '\n' && _binputb('\r', fp) == EOF)
		return EOF;
	return _binputb(c, fp);
}

#endif

/* end of libc/stdio/_fputb.c */
