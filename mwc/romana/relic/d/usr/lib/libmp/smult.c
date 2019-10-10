#include "mprec.h"
#include <assert.h>


/*
 *	Smult sets the mint pointed to by "c" to the product of that pointed
 *	to by "a" and the char "n".  Note that "n" is assumed to be between
 *	0 and NEFL.  Also note that "a" == "c" is permissable.
 */

void
smult(a, n, c)
mint *a, *c;
char n;
{
	register char *rp, *ap;
	register int carry;
	char *limit;
	char *res;

	assert(0 <= n && n <= NEFL);

	/* handle zero specially */
	if (n == 0) {
		mcopy(mzero, c);
		return;
	}

	/* allocate space for result */
	rp = (char *)mpalc(a->len + 1);
	res = rp;

	/* perform multiplication assuming a is positive */
	ap = a->val;
	limit = ap + a->len;
	carry = 0;
	while (ap < limit) {
		carry += *ap++ * n;
		*rp++ = carry % BASE;
		carry >>= L2BASE;
	}
	--ap;

	/* if a was negative, correct carry */
	if (!ispos(a))
		carry += BASE - n;

	/* set carry in place and replace old c value */
	*rp = carry;
	assert(0 <= carry && carry <= NEFL);
	assert((carry==NEFL && *ap==NEFL) || (carry!=NEFL && *ap!=NEFL));
	mpfree(c->val);
	c->val = res;
	c->len = a->len + 1;
	norm(c);
}
