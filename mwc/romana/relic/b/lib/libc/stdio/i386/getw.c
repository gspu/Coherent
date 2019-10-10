/*
 * Standard I/O Library
 * Get word (int)
 * Routine rather than macro, for the aesthetically inclined
 */

#include <stdio.h>
#undef	getw

int
getw(fp)
register FILE	*fp;
{
	register int	c0, c1;

	if ((c0=getc(fp))==EOF)
		return (EOF);
	else if ((c1=getc(fp))==EOF) {
		fp->_ff1 |= _FERR;
		return (EOF);
	} else
		return (c1<<8|c0);
}
