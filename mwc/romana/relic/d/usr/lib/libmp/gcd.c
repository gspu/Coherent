#include "mprec.h"


/*
 *	Gcd sets the value of the mint pointed to by "c" to the greatest
 *	common divisor of the mints pointed to by "a" and "b".  Note that
 *	no assumption made on the distinctness of "a", "b" and "c".
 */

void
gcd(a, b, c)
register mint *a, *b, *c;
{
	register mint	*temp;
	mint	al, bl, quot;

	/* make local copyies of arguments */
	minit(&al);
	mcopy(a, &al);
	a = &al;
	minit(&bl);
	mcopy(b, &bl);
	b = &bl;

	/* calculate gcd by Euclidean algorithm */
	minit(&quot);
	while (!zerop(b)) {
		mdiv(a, b, &quot, a);
		temp = a;
		a = b;
		b = temp;
	}

	/* throw away garbage and return result */
	mpfree(quot.val);
	mpfree(b->val);
	mpfree(c->val);
	*c = *a;
}
