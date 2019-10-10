/*
 * Coherent Standard I/O Library
 * Return the length of a string.
 */

#include <stdio.h>

strlen(s)
register char *s;
{
	register char *ss;

	for (ss = s; *s++; )
		;
	return (s-ss-1);
}
