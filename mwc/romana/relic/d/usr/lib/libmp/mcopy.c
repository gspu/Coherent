#include "mprec.h"
#include <assert.h>


/*
 *	Mcopy sets the mint pointed to by "b" to a copy of the mint
 *	pointed to by "a".  This is used when a scratch copy is needed.
 */

void
mcopy(a, b)
register mint *a, *b;
{
	register char *ap, *bp;
	register unsigned count;

	/* throw away old value and allocate space for new one */
	mpfree(b->val);
	b->val = (char *)mpalc(a->len);
	b->len = a->len;

	/* copy value */
	ap = a->val;
	bp = b->val;
	count = a->len;
	while (count-- != 0)
		*bp++ = *ap++;
	assert(bp == b->val + b->len);
	assert(ap == a->val + a->len);
}
