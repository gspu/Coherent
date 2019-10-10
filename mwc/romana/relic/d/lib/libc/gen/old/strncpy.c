/*
 * Coherent Standard I/O Library
 * Copy one fixed length string to another.
 */

#include <stdio.h>

char *
strncpy(as1, s2, n)
char *as1;
register char *s2;
register unsigned n;
{
	register char *s1;

	s1 = as1;
	while (n > 0) {
		n--;
		if ((*s1++ = *s2++) == '\0')
			break;
	}
	while (n--)
		*s1++ = 0;
	return (as1);
}
