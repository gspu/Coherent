/*
 * Standard stream library for the
 * C compiler and other compilers that use
 * C compiler factilities.
 * Output an dval_t.
 */
#include <stdio.h>
#ifdef   vax
#include "INC$LIB:mch.h"
#else
#include "mch.h"
#endif

dput(dp)
dval_t *dp;
{
	register unsigned char *p;
	register n;

	p = (char *) dp;
	n = sizeof(dval_t);
	do {
		bput(*p++);
	} while (--n);
}
