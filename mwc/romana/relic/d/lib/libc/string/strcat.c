/*
 * strcat.c
 * ANSI 4.11.3.1.
 * Concatenate strings.
 */

#include <string.h>

char *strcat(s1, s2) char *s1; register char *s2;
{
	register char *cp;

	for (cp = s1; *cp++; )
		;
	for (cp--; *cp++ = *s2++; )
		;
	return (s1);
}
