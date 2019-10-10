/*
 * Shellsort algorithm from Donald L Shell cf. Knuth Vol. 3, pp. 84 ff.
 * Divide a set of n elements into n/2 groups. { 0, n/2 } .. {1, n/2+1}
 * etc. Sort the groups. Now divide into n/4 groups of 4 {0, n/4, 2n/4, 3n/4}
 * etc. Sort those and continue.
 */
#include <stdio.h>
#include <limits.h>
#include <string.h>

#define inOrder(h, l) ((*compar)((__VOID__ *)(h), (__VOID__ *)(l)) >= 0)

void
shellsort(base, nmemb, size, compar)
__VOID__ *base;
size_t nmemb, size;
int (*compar)();
{
	register char *bot;
	register j, gap, i, n;

	for (n = gap = size * nmemb;;) {
		if (nmemb & 1)
			gap -= size;
		if (!(nmemb >>= 1))
			break;
		for (i = gap >>= 1; i < n; i += size) {
			for (j = i - gap; j >= 0; j -= gap) {
				bot = base + j;
				if (inOrder(bot + gap, bot))
					break;
				_memxchg(bot, bot + gap, size);
			}
		}
	}
}
