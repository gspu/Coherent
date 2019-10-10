/*
 * strcpy.c
 * ANSI 4.11.2.3.
 * Copy one string to another.
 */

#include <string.h>

char *strcpy(s1, s2) char *s1; register char *s2;
{
	register char *cp;

	for (cp = s1; *cp++ = *s2++; )
		;
	return (s1);
}
