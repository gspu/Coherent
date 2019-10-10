/*
 * memchr.c
 * ANSI 4.11.5.1.
 * Search memory.
 */

#include <string.h>

char *memchr(s, c, n) register char *s; int c; size_t n;
{
	register unsigned char uc;

	uc = c;
	while (n--)
		if (*s++ == uc)
			return((char *)--s);
	return (NULL);
}
