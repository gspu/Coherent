/*
 * strrchr.c
 * ANSI 4.11.5.5.
 * Search string in reverse.
 * Previously known as rindex().
 */

#include <string.h>

char *strrchr(s, c) char *s; int c;
{
	register char *cp;
	char ch;

	ch = (char)c;
	for (cp = s; *cp++; )
		;
	while (cp > s)
		if (*--cp == ch)
			return (cp);
	return (NULL);
}
