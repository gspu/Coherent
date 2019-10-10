/*
 * libc/gen/__assertfail.c
 * Assertion failure.
 * Implementation-defined behavior:
 *	message format "<file>: <line>: assert(<msg>) failed.\n".
 */

#include <stdio.h>
#include <stdlib.h>

void
__assertfail(msg, file, line) char *msg; char *file; int line;
{
	fprintf(stderr, "%s: %d: assert(%s) failed.\n", file, line, msg);
	abort();
}

/* end of libc/gen/__assertfail.c */
