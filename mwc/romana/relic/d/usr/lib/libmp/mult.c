#include "mprec.h"


/*
 *	Mult multiplies the mint pointed to by "a" by that pointed to
 *	by "b" and places the result in "c".  Note that no assumption
 *	is made as to "a", "b" and "c" all being different.
 */

void
mult(a, b, c)
register mint *a, *b;
mint *c;
{
	mint res, al;
	int apos;	/* a-is-positive flag */
	char *ap;
	register unsigned count;

	/* make sure that b is at least as long as a */
	if (a->len > b->len) {
		register mint *temp;

		temp = a;
		a = b;
		b = temp;
	}

	/* replace a by abs. value */
	apos = ispos(a);
	if (!apos) {
		minit(&al);
		mneg(a, &al);
		a = &al;
	}

	/* if a is one byte, use smult */
	if (a->len == 1) {
		smult(b, *a->val, c);
		if (!apos) {
			mneg(c, c);
			mpfree(al.val);
		}
		return;
	}

	/* allocate result and zero it out */
	res.len = a->len + b->len;
	res.val = (char *)mpalc(res.len);
	count = res.len;
	{
		register char *ap;

		ap = res.val;
		while (count-- != 0)
			*ap++ = 0;
	}

	/* form partial products and total in res */
	count = a->len;
	ap = a->val + a->len;
	while (count-- != 0) {
		msma(b, *--ap, count, &res);
	}
	norm(&res);

	/* if needed, negate result and throw away negated a */
	if (!apos) {
		mpfree(al.val);
		mneg(&res, &res);
	}

	/* replace old c value by new one */
	mpfree(c->val);
	*c = res;
}
