/*
 * Coherent Standard I/O Library
 * Copy one string to another.
 */

#include <stdio.h>

char *
strcpy(as1, s2)
char *as1;
register char *s2;
{
	register char *s1;

	s1 = as1;
	while (*s1++ = *s2++)
		;
	return (as1);
}
