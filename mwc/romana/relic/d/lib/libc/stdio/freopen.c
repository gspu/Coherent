/*
 * Standard I/O Library
 * Reopen file given structure ptr
 */

#include <stdio.h>

FILE *
freopen(name, type, fp)
char	*name, *type;
FILE	*fp;
{
	extern	FILE	*_fopen();

	fclose(fp);
	return (_fopen(name, type, fp, -1));
}
