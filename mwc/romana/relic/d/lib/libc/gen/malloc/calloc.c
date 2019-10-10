/*
 * C general utilities library.
 * calloc()
 * ANSI 4.10.3.1.
 * Allocate and clear.
 */

#include <stdlib.h>
#include <string.h>

Void *
calloc(nmemb, size) size_t nmemb; register size_t size;
{
	register Void *bp;
	long lsize;

	lsize = (long)size * nmemb;
	size *= nmemb;
	if (lsize != (long)size)
		return (Void *)NULL;
	if ((bp = malloc(size)) != NULL)
		memset(bp, 0, size);
	return bp;
}

/* end of calloc.c */
