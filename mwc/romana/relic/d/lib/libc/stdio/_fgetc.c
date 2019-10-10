/*
 * libc/stdio/_fgetc.c
 * C Standard I/O Library internals.
 * Unbuffered input.
 */

#include <stdio.h>
#include <errno.h>

extern	int	_fputt();

int
_fgetc(fp) register FILE *fp;
{
	register unsigned char s[1];
	register int n, oerrno;

	if (stdout->_pt==&_fputt)	/* special kludge */
		fflush(stdout);
	fp->_cc = 0;
	oerrno = errno;			/* save old errno */
	errno = 0;
	n = EOF;			/* return value in case error */
	switch (read(fileno(fp), s, 1)) {
	case -1:			/* error */
		if (errno != EINTR)
			fp->_ff |= _FERR;
		break;
	case 0:				/* EOF */
		fp->_ff |= _FEOF;
		break;
	default:			/* success */
		n = s[0];
		break;
	}
	if (errno == 0)
		errno = oerrno;		/* preserve errno if no error */
	return n;
}

/* end of libc/stdio/_fgetc.c */
