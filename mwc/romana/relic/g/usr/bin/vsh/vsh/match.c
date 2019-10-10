/*
 *	This functions are taken from COHERENT's "misc" library.
 *	The file doesn't have had any Copyrights or hints who
 *	wrote them, so I don't know. However, I modified the
 *	functions to fits the needs of vsh.
 *
 *	Udo Munk (udo@umunk.GUN.de)
 */

/* 
 * match(string, pattern, fin)
 * returns start of pattern in string or NULL if no match.
 * pattern:
 *	[c1c2...cn-cm]	class of characters.
 *	?		any character.
 *	*		any # of any character.
 * if pattern is found fin is aimed past the end of the pattern.
 */

#include <stdio.h>

/*
 * amatch is used internally by match.
 */
static char *
amatch(s, p, pfc, start, fin)
register char *s, *p;
int pfc;	/* past first char */
char **start, **fin;
{
	char *save;
	register c1;
	int c2;

	if(NULL == *start)
		*start = s;
	for(;c2 = *p++; pfc = 1) {
		c1 = *s++;
		*fin = s;

		switch(c2) {
		case '[':
			for (;;) {
				c2 = *p++;
				if (c2=='\0' || c2==']')
					return(*start = NULL);
				if (c2 == '\\' && *p == '-')
					c2 = *p++;
				if (c2 == c1)
					break;
				if (*p == '-')
					if (c1<=*++p && c1>=c2)
						break;
			}
			while (*p && *p++!=']')
				;

		case '?':
			if (c1)
				continue;
			return(*start = NULL);

		case '*':
			if (!*p)
				return(*start);
			s--;
			save = *start;
			do {
				if(NULL != amatch(s, p, 1, start, fin))
					return(*start);
				*start = save;
			} while(*s++ != '\0');
			return(*start = NULL);

		case '\\':
			if ((c2 = *p++) == '\0')
				return(*start = NULL);
		}
		if (c1 != c2)
			return(*start = NULL);
	}
	return(*start);
}

char *
match(s, p, fin)
register char *s, *p;
char **fin;
{
	char *start;
	int pfc;

	start = *fin = NULL;
	for(pfc = 0; *s; pfc = 1)
		if(NULL != amatch(s++, p, pfc, &start, fin))
			return(start);
	return(*fin = start);
}
