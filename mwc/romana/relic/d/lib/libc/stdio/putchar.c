/*
 * Standard I/O Library
 * putchar function for those too lazy to include stdio.h
 */

#include <stdio.h>
#undef	putchar

int
putchar(c)
unsigned char	c;
{
	return (putc(c, stdout));
}
