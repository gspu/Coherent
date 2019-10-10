/*
 * notmem.c
 * Test if pointer is in malloc arena.
 */

#include <stdio.h>
#include <sys/malloc.h>

/*
 * Return 1 if cp is not in the malloc arena,
 * 0 if cp is in the malloc arena, or
 * -1 if trouble is detected in the arena.
 * Note that this returns 0 for freed blocks in the arena.
 */
notmem(cp) char *cp;
{
	register unsigned len, counter;
	register MBLOCK *mp, *ap;

	if (cp == NULL
	   || (mp = __a_scanp) == NULL
	   || ((ap = mblockp(cp))->blksize) == 0)
		return 1;			/* obviously bad */

	for (counter = __a_count; counter--; ) {
		if (mp == ap)
			return 0;		/* obviously good */
		len = mp->blksize;
		mp = (len) ? bumpp(mp, realsize(len)) : mp->uval.next;
	}
	if (mp != __a_scanp)
		return -1;			/* trouble in arena */
	return 1;				/* not found */
}
