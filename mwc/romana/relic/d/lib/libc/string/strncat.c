/*
 * strncat.c
 * ANSI 4.11.3.2.
 * Concatenate fixed length strings.
 */

#include <string.h>

char *strncat(s1, s2, n) char *s1; register char *s2; size_t n;
{
	register char *cp;

	for (cp = s1; *cp++; )
		;
	for (cp--; n-- && *s2; )
		*cp++ = *s2++;
	*cp = 0;			/* Always NUL-terminate */
	return (s1);
}
