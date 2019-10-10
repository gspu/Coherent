/*
 * strlen.c
 * ANSI 4.11.6.3.
 * Return the length of a string.
 */

#include <string.h>

size_t strlen(s) char *s;
{
	register char *cp;

	for (cp = s; *cp++; )
		;
	return ((size_t)(cp-s-1));
}
