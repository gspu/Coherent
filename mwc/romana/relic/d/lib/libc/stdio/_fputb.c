/*
 * Standard I/O Library Internals
 * Buffered output
 */

#include <stdio.h>

int
_fputb(c, fp)
register int	c;
register FILE	*fp;
{
	if (_fpseek(fp))
		return (EOF);
	fp->_cc = _ep(fp) - fp->_dp - 1;
	return (*fp->_cp++=(unsigned char)c);
}
