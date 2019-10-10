/*
 * strchr.c
 * ANSI 4.11.5.2.
 * Search for character in string.
 * Previously known as index().
 */

#include <string.h>

char *strchr(s, c) register char *s; int c;
{
	char ch;

	ch = (char)c;
	do {
		if (*s == ch)
			return (s);
	} while (*s++);
	return (NULL);
}
