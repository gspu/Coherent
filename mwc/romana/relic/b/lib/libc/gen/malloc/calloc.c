/*
 * C general utilities library.
 * calloc()
 * ANSI 4.10.3.1.
 * Allocate and clear.
 */

#include <stdlib.h>
#include <string.h>

__VOID__ *
calloc(nmemb, size) size_t nmemb; register size_t size;
{
	register __VOID__ *bp;
	long lsize;

	lsize = (long)size * nmemb;
	size *= nmemb;
	if (lsize != (long)size)
		return (__VOID__ *)NULL;
	if ((bp = malloc(size)) != NULL)
		memset(bp, 0, size);
	return bp;
}

/* end of calloc.c */
