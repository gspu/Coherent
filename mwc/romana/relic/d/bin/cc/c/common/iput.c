/*
 * Standard stream library for the C compiler and other compilers that use
 * C compiler facilities.
 * Output an ival_t.
 */
#include <stdio.h>
#ifdef   vax
#include "INC$LIB:mch.h"
#else
#include "mch.h"
#endif

iput(i)
ival_t	i;
{
	register unsigned char *p;
	register n;
	ival_t	 tempi;

	tempi = i;
	p = &tempi;
	n = sizeof(ival_t);
	do {
		bput(*p++);
	} while (--n);
}
