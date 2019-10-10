/*
 * Standard I/O Library
 * getchar function for those too lazy to include stdio.h
 */

#include <stdio.h>
#undef	getchar

int
getchar()
{
	return (getc(stdin));
}
