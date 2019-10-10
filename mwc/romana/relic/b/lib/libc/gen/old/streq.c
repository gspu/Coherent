/*
 * Coherent Standard I/O Library
 * Comparison of two strings for equality
 * (not in portable stdio set).
 */

#include <stdio.h>

streq(s1, s2)
register char *s1, *s2;
{
	while (*s1 == *s2++)
		if (*s1++ == '\0')
			return (1);
	return (0);
}
