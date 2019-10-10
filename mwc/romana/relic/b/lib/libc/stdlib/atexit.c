/*
 * libc/stdlib/atexit.c
 * C general utilities library.
 * atexit()
 * ANSI 4.10.4.2.
 * Register function for execution on exit.
 */

#include <stdlib.h>

#define	ATEXITN	32

/*
 * The Standard requires the implementation to support registration
 * of at least 32 functions.
 * This minimal implementation statically allocates a buffer for 32.
 * More could be stored in a malloc'ed buffer, but this code does not do so.
 * Globals _atexitn and _atexitfp are defined in exit.c.
 */

static void (*atexitfp[ATEXITN])();

int
atexit(func) void (*func)();
{
	if (_atexitn == 0)		/* first time */
		_atexitfp = &atexitfp[0];
	else if (_atexitn == _ATEXITN)
		return 1;		/* too many, failure */
	*_atexitfp++ = func;		/* store the function */
	++_atexitn;			/* and bump the count */
	return 0;			/* success */
}

/* end of libc/stdlib/atexit.c */
