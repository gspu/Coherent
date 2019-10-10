/*
 * Standard I/O Library
 * Get character function (rather than macro)
 */

#include <stdio.h>

int
fgetc(fp)
register FILE	*fp;
{
	return (getc(fp));
}
