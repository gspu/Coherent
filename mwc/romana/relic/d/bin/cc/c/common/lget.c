/*
 * Standard stream library for the
 * C compiler and other compilers that use
 * C compiler factilities.
 * Input an lval_t.
 */
#include <stdio.h>
#ifdef   vax
#include "INC$LIB:mch.h"
#else
#include "mch.h"
#endif

lval_t
lget()
{
	register char *p;
	register n;
	lval_t   l;

	p = &l;
	n = sizeof(lval_t);
	do {
		*p++ = bget();
	} while (--n);
	return (l);
}
