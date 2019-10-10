/*
 * Coherent Standard I/O Library
 * Lexicographic comparison of two strings.
 */

#include <stdio.h>

strcmp(s1, s2)
register unsigned char *s1, *s2;
{
	while (*s1 == *s2++)
		if (*s1++ == '\0')
			return (0);
	return (*s1 - *--s2);
}
