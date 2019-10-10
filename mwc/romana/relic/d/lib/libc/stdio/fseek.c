/*
 * Standard I/O Library
 * Seek; first ensure buffer is clean; afterwards put ptrs at right place.
 */

#include <stdio.h>

int
fseek(fp, offset, origin)
register FILE	*fp;
long	offset;
int	origin;
{
	long	lseek();

	if ((origin == SEEK_CUR) && (fp->_ff & _FUNGOT))
		offset--;
	if (_fpseek(fp)==EOF)
		return (EOF);
	if ((offset=lseek(fileno(fp), offset, origin)) == -1L)
		return (EOF);
	if (fp->_bp!=NULL)
		fp->_dp = fp->_cp = fp->_bp + (unsigned)offset%BUFSIZ;
	fp->_ff &= ~_FEOF;
	return (0);
}
