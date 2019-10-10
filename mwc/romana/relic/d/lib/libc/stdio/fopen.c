/*
 * Standard I/O Library
 * open file with first available structure
 */

#include <stdio.h>

FILE *
fopen(name, type)
char	*name, *type;
{
	register FILE	**fpp;
	extern	FILE	*_fopen();

	for (fpp = &_fp[0]; fpp < &_fp[_NFILE]; fpp++)
		if (*fpp==NULL || !((*fpp)->_ff&_FINUSE))
			return (*fpp = _fopen(name, type, *fpp, -1));
	return (NULL);
}
