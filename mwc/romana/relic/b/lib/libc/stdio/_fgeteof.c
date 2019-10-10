/*
 * Standard I/O Library Internals
 * routine to always return EOF at end of string, for sscanf
 */

#include <stdio.h>

int
_fgeteof(fp)
register FILE	*fp;
{
	fp->_cc = 0;
	return (EOF);
}
