/*
 * memcmp.c
 * ANSI 4.11.4.1.
 * Compare up to n chars of strings.
 */

#include <string.h>

int memcmp(s1, s2, n) register char *s1, *s2; register size_t n;
{
	while (n--)
		if (*s1++ != *s2++)
			return ((*--s1 < *--s2) ? -1 : 1);
	return (0);
}
