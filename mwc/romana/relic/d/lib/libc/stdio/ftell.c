/*
 * Standard I/O Library
 * Tell logical (vs physical) file position
 * in units compatible with fseek
 */

#include <stdio.h>

long
ftell(fp)
register FILE	*fp;
{
	extern	long	lseek();
	long	offset;

	if ((offset=lseek(fileno(fp), 0L, SEEK_CUR)) == -1L)
		return (-1L);
	if (fp->_bp!=NULL)
		offset += fp->_cp - fp->_dp;
	if (fp->_ff&_FUNGOT)
		--offset;
	return (offset);
}
