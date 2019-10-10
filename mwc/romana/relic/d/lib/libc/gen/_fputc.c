/*
 * libc/stdio/_fputc.c
 * C Standard I/O Library internals.
 * Unbuffered output.
 */

#include <stdio.h>
#include <errno.h>

int
_fputc(c, fp) register unsigned int c; register FILE *fp;
{
	register int n, oerrno;
	char s[1] = c;

	fp->_cc = 0;
	oerrno = errno;
	errno = 0;
	n = (unsigned char)c;
	if (fp->_ff&_FERR || _fpseek(fp))
		n = EOF;
	else if (write(fileno(fp), s, 1) != 1) {
		if (errno != EINTR)
			fp->_ff |= _FERR;
		n = EOF;
	}
	if (errno == 0)
		errno = oerrno;
	return n;
}

/* end of libc/stdio/_fputc.c */
