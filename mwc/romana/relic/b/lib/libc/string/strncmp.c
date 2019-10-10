/*
 * strncmp.c
 * ANSI 4.11.4.4.
 * Compare up to n chars of strings.
 */

#include <string.h>

int strncmp(s1, s2, n) register char *s1, *s2; register size_t n;
{
	while (n--) {
		if (*s1 > *s2)
			return (1);
		else if (*s1 < *s2)
			return (-1);
		else if (*s1 == '\0')
			return (0);
		s1++; s2++;
	}
	return (0);
}
