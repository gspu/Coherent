/*
 * C general utilities library.
 * calloc()
 * ANSI 4.10.3.1.
 * Allocate and clear.
 */

#include <stdlib.h>
#include <string.h>

/*
 * This does not check that size*nmemb fits in a size_t.
 * The return type should be void *.
 */
char *
calloc(nmemb, size) size_t nmemb; register size_t size;
{
	register char *bp;		/* should be void * */

	size *= nmemb;
	if ((bp = malloc(size)) != NULL)
		memset(bp, 0, size);
	return bp;
}

/* end of calloc.c */
