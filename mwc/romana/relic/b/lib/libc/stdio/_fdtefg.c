/*
 * libc/stdio/_fdtefg.c
 *
 * Dummy routine _dtefg() for floating point output conversion,
 * so programs which don't use floating point can stay small.
 */

#include <stdio.h>

/* char *_dtefg() for consistency with <stdio.h>, this one is void _dtefg(). */
char *
_dtefg()
{
	fprintf(stderr,
"\nYou must compile with the -f flag to include printf() floating point.\n"
		);
	exit(1);
}

/* end of libc/stdio/_fdtefg.c */
