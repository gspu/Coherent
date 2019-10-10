/*
 * strncpy.c
 * ANSI 4.11.2.4.
 * Copy one fixed length string to another.
 */

#include <string.h>

char *strncpy(s1, s2, n) char *s1; char *s2; size_t n;
{
	register char *cp;

	for (cp = s1; n; ) {
		n--;
		if ((*cp++ = *s2++) == '\0')
			break;
	}
	/* Pad remainder of s1 with NUL if required. */
	while (n-- > 0)
		*cp++ = '\0';
	return (s1);
}
