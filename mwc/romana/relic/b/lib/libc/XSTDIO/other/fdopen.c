/*
 * Standard I/O Library
 * get file structure for given file descriptor
 */

#include <stdio.h>

FILE *
fdopen(fd, type)
int	fd;
char	*type;
{
	register FILE	**fpp;
	extern	FILE	*_fopen();

	if (0<=fd && fd<_NFILE) for (fpp = &_fp[0]; fpp < &_fp[_NFILE]; fpp++)
		if (*fpp==NULL || !((*fpp)->_ff2&_FINUSE))
			return (*fpp = _fopen(NULL, type, *fpp, fd));
	return (NULL);
}
