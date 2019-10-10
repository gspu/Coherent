/*
 * Standard I/O Library
 * Read nitems of size from file fp
 */

#include <stdio.h>

int
fread(bp, size, nitems, fp)
register char	*bp;
unsigned int	size;
unsigned int	nitems;
register FILE	*fp;
{
	unsigned int	nb = size*nitems;
	register int	c;

	if (fp->_ff&_FUNGOT) {
		*bp++ = (*fp->_gt)(fp);
		nb--;
	}
	if (fp->_bp!=NULL || !(fp->_ff&_FSTBUF))
		for (; nb && (c=getc(fp))!=EOF; nb--)
			*bp++ = c;
	else if ((c=read(fileno(fp), bp, nb)) > 0)
		nb -= c;
	else if (c == 0)
		fp->_ff |= _FEOF;
	else
		fp->_ff |= _FERR;
	return ((size*nitems-nb)/size);
}
