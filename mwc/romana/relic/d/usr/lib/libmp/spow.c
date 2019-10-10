#include "mprec.h"


/*
 *	Spow sets "b" to "a" raised to the "n" power.  Note that
 *	a = b is acceptable.
 */

void
spow(a, n, b)
register mint	*a, *b;
register unsigned n;
{
	mint	ac;

	minit(&ac);
	mcopy(a, &ac);
	mcopy(mone, b);
	if (n != 0) {
		for (; n != 1; n >>= 1, mult(&ac, &ac, &ac))
			if (n % 2 != 0)
				mult(&ac, b, b);
		mult(&ac, b, b);
	}
	mpfree(ac.val);
}
