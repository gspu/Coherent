/*
 * Standard stream library for the C compiler and other compilers that use
 * C compiler facilities.
 * Output an lval_t.
 */
#include <stdio.h>
#ifdef   vax
#include "INC$LIB:mch.h"
#else
#include "mch.h"
#endif

lput(l)
lval_t	l;
{
	register unsigned char *p;
	register n;
	lval_t   templ;

	templ = l;
	p = &templ;
	n = sizeof(lval_t);
	do {
		bput(*p++);
	} while (--n);
}
