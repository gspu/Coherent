/* 
 * match(string, pattern, fin)
 * returns start of pattern in string or NULL if no match.
 * pattern:
 *	[c1c2...cn-cm]	class of characters.
 *	?		any character.
 *	*		any # of any character.
 *	^		beginning of string
 *	$		end of string
 * if patttern is found fin is aimed past the end of the pattern.
 */
#include "misc.h"

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
		case '^':
			if(pfc)
				return(*start = NULL);
			s--;
			continue;

		case '$':
			if(c1)
				*start = NULL;
			return(*start);

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

/*
 * This pnmatch takes advantage of amatch.
 */
pnmatch(s1, pat, flag)
char *s1, *pat;
{
	char *start, *fin;

	if(flag)
		return(NULL != match(s1, pat, &fin));
	return((NULL != amatch(s1, pat, 0, &start, &fin)) && !*fin);
}

#ifdef TEST
main()
{
	char s1[80], pat[80], *start, *fin;

	for(;;) {
		ask(s1, "String  ");
		if(!strcmp(s1, "quit"))
			exit(0);
		ask(pat, "Pattern");
		if(NULL == (start = match(s1, pat, &fin)))
			printf("Not Found\n");
		else {
			*fin = '\0';
			printf("%s\n", start);
		}
	}
}
#endif
