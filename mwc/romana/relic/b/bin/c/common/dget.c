/*
 * Standard stream library for the
 * C compiler and other compilers that use
 * C compiler factilities.
 * Input an dval_t.
 */
#include <stdio.h>
#ifdef   vax
#include "INC$LIB:mch.h"
#else
#include "mch.h"
#endif

dget(dp)
dval_t *dp;
{
	register char *p;
	register n;

	p = (char *) dp;
	n = sizeof(dval_t);
	do {
		*p++ = bget();
	} while (--n);
}
