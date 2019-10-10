/*
 * Standard I/O Library Internals
 * Terminal output (buffered by line)
 */

#include <stdio.h>

int
_fputt(c, fp)
register unsigned int	c;
register FILE	*fp;
{
	fp->_cc = 0;
	if (fp->_cp==_ep(fp) && fflush(fp)
	 || (*fp->_cp++ = c) == '\n' && fflush(fp))
		return (EOF);
	return ((unsigned char)c);
}
