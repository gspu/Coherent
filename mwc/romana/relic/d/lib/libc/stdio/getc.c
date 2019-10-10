/*
 * Standard I/O Library
 * getc function for those too lazy or cheap to use macro
 */

#include <stdio.h>
#undef	getc

int
getc(fp)
FILE	*fp;
{
	return (fgetc(fp));
}
