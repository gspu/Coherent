/*
 * streq.c
 * Not in ANSI draft standard.
 * Compare two strings for equality.
 */

#include <stdio.h>

int streq(s1, s2) register char *s1, *s2;
{
	while (*s1 == *s2++)
		if (*s1++ == '\0')
			return (1);
	return (0);
}
