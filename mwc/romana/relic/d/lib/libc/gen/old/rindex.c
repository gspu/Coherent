/*
 * Find rightmost match or return NULL.
 */
#include <stdio.h>

char *
rindex(s, c)
register unsigned char *s;
unsigned int c;
{
	register unsigned char c1, c2, *match;

	for(c2 = c, match = NULL; c1 = *s; s++)
		if(c1 == c2)
			match = s;
	return(match);
}
