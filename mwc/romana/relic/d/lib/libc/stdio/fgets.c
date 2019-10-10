/*
 * Coherent Standard I/O Library
 * fgets -- read a string from input
 * file pointer leaving the trailing
 * newline character.
 */

#include <stdio.h>

char *
fgets(as, lim, iop)
char *as;
register lim;
FILE *iop;
{
	register c;
	register char *s;

	s = as;
	while (--lim > 0 && (c = getc(iop)) != EOF)
		if ((*s++ = c) == '\n')
			break;
	*s = 0;
	return (c==EOF && s==as ? NULL : as);
}
