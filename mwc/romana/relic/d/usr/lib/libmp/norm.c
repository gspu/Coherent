#include "mprec.h"


/*
 *	Norm normalizes the mint pointed to by "a".  This simply entails
 *	propogating any carry and truncating any leading 00 or NEFL bytes.
 *	Note that norm assumes that there is enough room for the result.
 */

void
norm(a)
mint *a;
{
	register unsigned char *ap, *limit;
	register int carry;

	ap = (unsigned char *)a->val;
	limit = ap + a->len;
	carry = 0;
	do {
		carry += *ap;
		*ap++ = carry % BASE;
		carry >>= L2BASE;
	} while (ap < limit);
	--ap;
	limit = a->val;
	if (*ap == NEFL) {
		while (*ap == NEFL && ap >= limit)
			--ap;
		++ap;
	} else {
		while (*ap == 0 && ap > limit)
			--ap;
		if (*ap == NEFL)
			++ap;
	}
	a->len = 1 + ap - a->val;
}
