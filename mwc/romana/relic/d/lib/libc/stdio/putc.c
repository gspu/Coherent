/*
 * Standard I/O Library
 * putc function for those too lazy or cheap to use macro
 */

#include <stdio.h>
#undef	putc

int
putc(c, fp)
char	c;
FILE	*fp;
{
	return (fputc(c, fp));
}
