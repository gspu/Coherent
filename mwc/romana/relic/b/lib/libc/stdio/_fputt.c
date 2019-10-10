/*
 * libc/stdio/_fputt.c
 * ANSI-compliant C standard i/o library internals.
 * _fputt(), _fputta()
 * Write character, line buffered.
 */

#include <stdio.h>

int
_fputt(c, fp) int c; register FILE *fp;
{
	register int uc;
	register _FILE2 *f2p;

	uc = (unsigned char)c;
	f2p = fp->_f2p;
#if	1
	if (fp->_cp==f2p->_ep && fflush(fp)
	 || (*fp->_cp++ = c) == '\n' && fflush(fp))
		return EOF;
#else
	if (putb(uc, fp) == EOF || (uc == '\n' && fflush(fp)))
		return EOF;
#endif
	return uc;
}

#if	_ASCII

/* ASCII: prepend '\r' before '\n'. */
int
_fputta(c, fp) int c; register FILE *fp;
{
	register int uc;

	if ((uc = (unsigned char)c) == '\n') {
		if (_binputb('\r', fp) == EOF
		 || _binputb(uc, fp) == EOF
		 || fflush(fp))
			return EOF;
		return (uc);
	}
	return _binputb(uc, fp);
}

#endif

/* end of libc/stdio/_fputt.c */
