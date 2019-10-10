/*
 * strcmp.c
 * ANSI 4.11.4.2.
 * Lexicographic comparison of two strings.
 */

#include <string.h>

int strcmp(s1, s2) register char *s1, *s2;
{
	while (*s1 == *s2++)
		if (*s1++ == '\0')
			return (0);
	if (*s1 < *--s2)
		return (-1);
	return (1);
}
