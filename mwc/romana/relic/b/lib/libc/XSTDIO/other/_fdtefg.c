/*
 * Dummy routines for floating point output so programs which
 * don't use floating point can stay small.
 */

#include <stdio.h>

char *
_dtefg()
{
	fprintf(stderr,
"\nYou must compile with the -f flag to include printf() floating point.\n"
		);
	exit(1);
}
