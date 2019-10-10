/* realloc fits unix version 5 requirments 
 * knows format of malloc list.
 */

#include <stdio.h>
#include <malloc.h>

extern struct mblock	*_a_scanp;	/* search starts here */

static
qmove(cp, ptr, sizem)
register unsigned sizem, *cp, *ptr;
{
	for(; sizem > 0; sizem--)
		*ptr++ = *cp++;
}

/* realloc function */
char *
realloc(cp, size)
register char *cp;
unsigned size;
{
	register unsigned len, siz, nlen;
	register struct mblock *ptr, *nptr;

	siz = roundup(size + sizeof(unsigned), POW2);
	if(siz < size)
		return(NULL);
	len = realsize((ptr = cp - sizeof(unsigned))->blksize);
	ptr->blksize = len;		/* mark used */
	if(len >= siz) {		/* request to shrink */
		if((len -= siz) > LEASTFREE) {
			ptr->blksize = siz;
			ptr = siz + adr(ptr);
			ptr->blksize = (len|FREE);
		}
		return(cp);
	}
	/* request to grow */
	nptr = adr(ptr) + len;
	if((isfree(nlen = nptr->blksize)) &&
	   ((nlen+=len) > siz)) { /*next block free and big enough */
		if((nlen - siz) > LEASTFREE) {
			ptr->blksize = siz;
			ptr = siz + adr(ptr);
			ptr->blksize = nlen - siz;
		}
		else {
			ptr->blksize = realsize(nlen);
			ptr = realsize(nlen) + adr(ptr);
		}
		if(nptr==_a_scanp)
			_a_scanp=ptr;
		return(cp);
	}
	if(NULL == (ptr = malloc(size)))
		return(NULL);
	qmove(cp, ptr, len/sizeof(unsigned) - 1);
	free(cp);
	return(ptr);
}
