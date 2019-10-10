/*
 * C general utilities library.
 * bsearch()
 * ANSI 4.10.5.1.
 * Binary search.
 */

#include <stdlib.h>

__VOID__ *
bsearch(key, base, nmemb, size, compar)
const __VOID__ *key, *base;
register size_t nmemb;
size_t size;
int (*compar)();
{
	register size_t i;
	register int n;
	register __VOID__ *p;

	while (nmemb > 0) {
		i = nmemb / 2;
		p = (__VOID__ *)(((char *)base) + i * size);
		if ((n = (*compar)(key, p)) == 0)
			return p;
		else if (n < 0)
			nmemb = i;
		else {
			nmemb -= i + 1;
			base = (__VOID__ *)(((char *)p) + size);
		}
	}
	return NULL;
}

/* end of bsearch.c */
