/*
 * Coherent Standard I/O Library.
 * Return a pointer to where a character
 * appears in a string or NULL.
 * Amended to return the location of (c == '\0') as well, rec 1983.12.12.
 */

#include <stdio.h>

char *
index(s, c)
register unsigned char *s;
register c;
{
	do
		if (*s == c)
			return (s);
	while (*s++ != '\0');
	return (NULL);
}
