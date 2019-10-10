/*
 * libc/gen/malloc/memok.c
 * Test for corruption of malloc arena.
 * Assumes contiguous allocation from __a_first to __a_top.
 */

#include <stdio.h>
#include <sys/malloc.h>

/*
 * Return 1 if malloc arena is ok,
 * 0 if it is corrupted.
 */
int
memok()
{
	register MBLOCK *mp;
	unsigned counter, flag, len;

	if (NULL == (mp = __a_first))
		return ((NULL == __a_scanp) && !__a_count);
	flag = 0;
	for (counter = __a_count; counter--; ) {
		if (mp == __a_scanp)
			flag++;
		if ((mp >= __a_top) || (mp < __a_first))
			return 0;	/* out of bounds */
		mp = (len = realsize(mp->blksize)) ? bumpp(mp, len) : mp->uval.next;
	}
	return ((1 == flag) && (mp == __a_first));
}

/* end of memok.c */
