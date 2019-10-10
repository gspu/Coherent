/*
 * Open a file or die in the attempt.
 */
#include <stdio.h>
FILE *
xopen(fn, acs)
char *fn, *acs;
{
	FILE *tmp;

	if(NULL == (tmp = fopen(fn, acs)))
		fatal("Cannot fopen(%s, %s)", fn, acs);
	return(tmp);
}
