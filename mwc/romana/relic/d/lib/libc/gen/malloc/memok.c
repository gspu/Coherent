/*
 * memok.c
 * Test for corruption of malloc arena.
 */

#include <stdio.h>
#include <sys/malloc.h>

/*
 * memok() returns 1 if memory is ok,
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
		mp = (len = realsize(mp->blksize)) ? bumpp(mp, len) : mp->uval.next;
	}
	return ((1 == flag) && (mp == __a_first));
}
