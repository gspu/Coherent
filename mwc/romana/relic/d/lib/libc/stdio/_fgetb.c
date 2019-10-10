/*
 * libc/stdio/_fgetb.c
 * C Standard I/O Library internals.
 * Buffered input: read a buffer.
 */

#include <stdio.h>
#include <errno.h>

extern	int	_fputt();

int
_fgetb(fp) register FILE *fp;
{
	register int n, oerrno;

	if (fflush(fp))
		return EOF;
	if (stdout->_pt==&_fputt)	/* special kludge */
		fflush(stdout);
	oerrno = errno;			/* save old errno */
	errno = 0;
	n = fp->_cc = -read(fileno(fp), fp->_dp, _ep(fp) - fp->_dp);
	if (errno == 0)
		errno = oerrno;		/* preserve errno if no error */
	if (n == 1) {			/* read() returned -1, i.e. error */
		if (errno != EINTR)
			fp->_ff |= _FERR;
		fp->_cc = 0;
		return EOF;
	} else if (n == 0) {		/* read() returned 0, i.e. EOF */
		fp->_ff |= _FEOF;
		return EOF;
	} else {			/* success */
		fp->_dp -= fp->_cc++;
		return (*fp->_cp++);
	}
}

/* end of libc/stdio/_fgetb.c */
