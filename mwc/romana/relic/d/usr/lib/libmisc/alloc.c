/*
 * Get space or die.
 */
#include <stdio.h>
char *memset();

char *
alloc(n)
unsigned n;
{
	char	*tmp;

	if (NULL == (tmp = malloc(n)))
		fatal("Out of space");
	return(memset(tmp, '\0', n));
}
