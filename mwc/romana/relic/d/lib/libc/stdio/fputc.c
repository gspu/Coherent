/*
 * Standard I/O Library
 * Put char function (rather than macro)
 */

#include <stdio.h>

int
fputc(c, fp)
char	c;
register FILE	*fp;
{
	return (putc(c, fp));
}
