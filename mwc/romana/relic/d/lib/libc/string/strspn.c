/*
 * strspn.c
 * ANSI 4.11.5.6.
 * Search string for string characters.
 */

#include <string.h>

size_t *strspn(s1, s2) char *s1; char *s2;
{
	register char *cp1, *cp2;
	register char c1, c2;

	for (cp1 = s1; c1 = *cp1; cp1++) {
		for (cp2 = s2; c2 = *cp2; cp2++)
			if (c1 == c2)
				break;
		if (c2 == '\0')
			break;
	}
	return((size_t)(cp1 - s1));
}
