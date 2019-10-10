/*
 * One or more characters not matching some criterion.
 * skip(s1, matcher, fin)
 */
#include <stdio.h>
char *
skip(s1, matcher, fin)
register char *s1;
int (*matcher)();
register char **fin;
{
	char c;
	char *start = s1;

	for (*fin = NULL; (c = *s1++) && !(*matcher)(c); *fin = s1)
		;
	if (NULL == *fin)
		return (NULL);
	return (start);
}
#ifdef TEST
#include <ctype.h>
#include "misc.h"

/* eliminatye under ansi */
digit(s)
char s;
{
	return (isdigit(s));
}

main()
{
	char s1[80], *fin;

	for (;;) {
		ask(s1, "s1");
		if (!strcmp(s1, "quit"))
			exit(0);
		if (NULL == skip(s1, digit, &fin))
			printf("No non digits\n");
		else {
			*fin = '\0';
			printf("%s\n", s1);
		}
	}
}
#endif
