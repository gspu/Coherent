/*
 * Coherent Standard I/O Library
 * Catenate 2 fixed length strings.
 */

#include <stdio.h>

char *
strncat(as1, s2, n)
char *as1;
register char *s2;
register unsigned n;
{
	register char *s1;

	s1 = as1;
	while (*s1++)
		;
	for (s1--; n && *s2; n--)
		*s1++ = *s2++;
	*s1 = 0;
	return (as1);
}
