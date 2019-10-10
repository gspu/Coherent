/*
 * Standard I/O Library
 * Setup own buffer (or none)
 */

#include <stdio.h>

void
setbuf(fp, bp)
register FILE	*fp;
register char	*bp;
{
	if (fp->_bp != NULL)
		return;
	fp->_bp = bp;
	fp->_ff |= _FSTBUF;
}
