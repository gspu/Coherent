/*
 * Standard stream library for the
 * C compiler and other compilers that use
 * C compiler factilities.
 * Input a string of max. length 'n'.
 */
#include <stdio.h>
#ifdef   vax
#include "INC$LIB:mch.h"
#else
#include "mch.h"
#endif

sget(p, n)
register char	*p;
register	n;
{
	register c;

	while (c = bget()) {
		if (c == EOF)
			cfatal("unexpected EOF");
		if (n>1) {
			*p++ = c;
			--n;
		}
	}
	*p = 0;
}
