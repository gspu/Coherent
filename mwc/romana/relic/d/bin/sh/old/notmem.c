
/*
 * Tell if this block of memory is in the malloc area
 */

#include <stdio.h>
#include <malloc.h>

extern  struct mblock	*_a_scanp;	/* first arena */

notmem(cp)
char *cp;
{
	register unsigned len;
	register struct mblock *ap, *ptr;
	
	if(cp == NULL || (ptr = _a_scanp) == NULL ||
	  !((ap = cp - sizeof(unsigned))->blksize))
		return(1);	/* bullshit address */

	do {
		if(ptr == ap)
			return(0);	/* obviously good */

		if(len = ptr->blksize)
			ptr = realsize(len) + adr(ptr);
		else
			ptr = ptr->uval.next;
	} while (ptr != _a_scanp);

	return(1);	/* not found is bad address */
}
