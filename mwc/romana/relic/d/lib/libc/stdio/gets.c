/*
 * Standard I/O Library
 * Get string from standard input
 * deleting trailing '\n'
 */

#include <stdio.h>

char *
gets(is)
register char	*is;
{
	register char	*s = is;
	register int	c;

	while ((c=getchar())!=EOF && c!='\n')
		*s++ = c;
	*s = 0;
	return (c==EOF && s==is ? NULL : is);
}
