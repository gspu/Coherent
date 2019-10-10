/*
 * Standard stream library for the
 * C compiler and other compilers that use
 * C compiler factilities.
 * Input an ival_t.
 */
#include <stdio.h>
#ifdef   vax
#include "INC$LIB:mch.h"
#else
#include "mch.h"
#endif

ival_t
iget()
{
	register char *p;
	register n;
	ival_t   i;

	p = &i;
	n = sizeof(ival_t);
	do {
		*p++ = bget();
	} while (--n);
	return (i);
}
