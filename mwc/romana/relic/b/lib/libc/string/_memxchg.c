/*
 * C string library internals.
 * _memxchg()
 * Exchange records of 'n' bytes pointed to by 's1' and 's2'.
 */

#include <string.h>

void
_memxchg(s1, s2, n)
register char *s1, *s2;
register size_t n;
{
	register int i;

	while (n--) {
		i = *s1;
		*s1++ = *s2;
		*s2++ = i;
	}
}
