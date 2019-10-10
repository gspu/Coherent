/*
 * Coherent Standard I/O Library
 * Catenate strings
 */

#include <stdio.h>

char *
strcat(as1, s2)
char *as1;
register char *s2;
{
	register char *s1;

	s1 = as1;
	while (*s1++)
		;
	for (s1--; *s1++ = *s2++; )
		;
	return (as1);
}
