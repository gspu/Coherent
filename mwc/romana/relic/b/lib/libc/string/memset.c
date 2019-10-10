/*
 * memset.c
 * ANSI 4.11.6.1.
 * Set memory.
 */

#include <string.h>

char *memset(s, c, n) char *s; int c; register size_t n;
{
	register unsigned char *cp;
	register unsigned char uc;

	uc = c;
	for (cp = s; n--; )
		*cp++ = uc;
	return (s);
}
