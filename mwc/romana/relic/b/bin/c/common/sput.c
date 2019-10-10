/*
 * Standard stream library for the
 * C compiler and other compilers that use
 * C compiler factilities.
 * Output a string.
 */
#include <stdio.h>
#ifdef   vax
#include "INC$LIB:mch.h"
#else
#include "mch.h"
#endif

sput(p)
register unsigned char	*p;
{
	register c;

	do {
		bput(c = *p++);
	} while (c != 0);
}
