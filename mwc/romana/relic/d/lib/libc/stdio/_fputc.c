/*
 * Standard I/O Library Internals
 * Unbuffered output
 */

#include <stdio.h>
#include <errno.h>

int
_fputc(c, fp)
register unsigned int	c;
register FILE	*fp;
{
	char	s[1] = c;

	fp->_cc = 0;
	errno = 0;
	if (fp->_ff&_FERR || _fpseek(fp)) {
		return (EOF);
	} else if (write(fileno(fp), s, 1) == 1) {
		return ((unsigned char)c);
	} else {
		if (errno != EINTR)
			fp->_ff |= _FERR;
		return (EOF);
	}
}
