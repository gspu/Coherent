/*
 * memcpy.c
 * ANSI 4.11.2.1.
 * Copy memory.
 */

#include <string.h>

char *memcpy(s1, s2, n) char *s1; register char *s2; register size_t n;
{
	register char *cp;

	for (cp = s1; n--; )
		*cp++ = *s2++;
	return (s1);
}
