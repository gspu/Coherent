/*
 * memmove.c
 * ANSI 4.11.2.2.
 * Copy memory.
 * Works correctly between overlapping blocks,
 * provided s1 < s2 assures object *s1 at lower address than *s2.
 * This may not be the case in some machine models (e.g. LARGE i8086).
 */

#include <string.h>

char *memmove(s1, s2, n) char *s1; register char *s2; register size_t n;
{
	register char *cp;

	if (s1 < s2) {
		for (cp = s1; n--; )
			*cp++ = *s2++;
	}
	else if (s1 > s2) {
		s2 += n;
		for (cp = s1 + n; n--; )
			*--cp = *--s2;
	}
	return (s1);
}
