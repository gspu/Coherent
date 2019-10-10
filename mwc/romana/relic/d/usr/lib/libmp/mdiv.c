#include "mprec.h"
#include <assert.h>
#include <sys/mdata.h>


/*
 *	Mdiv sets the mints pointed to by "q" and "r" to the quotient
 *	and remainder (respectively) of dividing "a" by "b".  If "b"
 *	is zero, then it calls mperr with the appropriate message.
 *	The division is performed such that the following two properties
 *	hold :
 *		1. "r" + "q" * "b" = "a".
 *		2. The sign of "r" = the sign of "q".
 *		3. The abs. value of "r" < abs. value of "b".
 *	Note that no assumption is made as to the distinctness of "a",
 *	"b", "q" and "r".
 */

void
mdiv(a, b, q, r)
register mint *a, *b, *q, *r;
{
	int	apos, bpos;
	mint	al, bl;
	int	ispos();
	void	shortd(), longd();

	/* take care of small denominators */
	if (b->len <= 2 && (
		   b->len == 1				/* 0 to BASE - 2 */
		|| b->val[1] == NEFL			/* -BASE to -2 */
		|| b->val[1] == 0			/* BASE - 1 */
		|| (b->val[1] == 1 && b->val[0] == 0)	/* BASE */
		)
	) {
		shortd(a, b, q, r);
		return;
	}

	/* make denominator negative */
	if (bpos = ispos(b)) {
		minit(&bl);
		mneg(b, &bl);
		b = &bl;
	}
	assert(b->len > 2);

	/* copy numerator, makeing it positive */
	minit(&al);
	if (apos = ispos(a))
		mcopy(a, &al);
	else
		mneg(a, &al);
	a = &al;

	/*
	 *	BASE ^ (a->len - 1) <= a < BASE ^ a->len
	 *	BASE ^ (b->len - 2) < -b <= BASE ^ (b->len - 1)
	 *
	 *	do divide.
	 */

	if (a->len <= b->len - 2)
		mcopy(mzero, q);
	else
		longd(a, b, q);

	/* adjust signs */
	if (apos ? !bpos : bpos) {
		mneg(q, q);
		mneg(a, r);
	} else
		mcopy(a, r);

	/* throw away garbage */
	mpfree(al.val);
	if (bpos)
		mpfree(bl.val);
}


/*
 *	Shortd divides the mints pointed to by "a" and "b".
 *	It sets the mints pointed to by "q" and "r" to the quotient
 *	and remainder respectively.  It assumes that "b" is between
 *	-BASE and BASE.
 */

static void
shortd(a, b, q, r)
register mint *a, *b, *q, *r;
{
	int rem;
	int denom;

	assert(b->len <= 2);
	if (ispos(b)) {
		denom = b->val[0];
		if (b->len != 1 && denom == 0)
			denom = BASE;
	} else
		denom = b->val[0] - BASE;
	assert(-BASE <= denom && denom <= BASE);
	sdiv(a, denom, q, &rem);
	mitom(rem, r);
}


/*
 *	Longd does the actual long divide.  It sets "q" and "a" to
 *	the quotient and remainder (respectively) of "a" / (- "b").
 *	It assumes the following:
 *	  1. "b" is negative
 *	  2. "a" is positive
 *	  3. "a" is a destroyable copy
 *	  4. ("a"->len + 2) > ("b"->len) >= 3
 */

static void
longd(a, b, q)
register mint *a, *b;
mint *q;
{
	register char *rp;
	unsigned shift;
	int tden;
	unsigned tbias;
	mint res;


	/*
	 *	For "a" we have the range
	 *		(BASE - 1) * BASE ^ (a->len - 1) > a
	 *			a >= (BASE - 1) * BASE ^ (a->len - 2)
	 *	and for "b" we want
	 *		tden * BASE ^ tbias >= -b > (tden - 1) * BASE ^ tbias
	 */

	tden = est(b);
	tbias = b->len - 3;

	assert(a->len >= tbias + 2);
	res.len = a->len - tbias;
	res.val = (char *)mpalc(res.len);
	rp = &res.val[res.len - 1];
	*rp-- = 0;
	shift = a->len - (tbias + 2);

	do {
		*rp = guess(tden, tbias + shift, a);
		msma(b, *rp, shift, a);
		if (snc(b, shift, a)) {
			++*rp;
			msma(b, 1, shift, a);
		}
		--rp;
	} while (shift-- != 0);
	norm(a);
	norm(&res);
	mpfree(q->val);
	*q = res;
}




/*
 *	Est estimates the mint pointed to by "a".  Its result
 *	satisfies the following:
 *		1.  result * BASE ^ (a->len - 3) >= -a
 *			&& -a > (result - 1) * BASE ^ (a->len - 3)
 *		2.  BASE ^ 2 >= mant > BASE
 *	Note that "a" is assumed to be negative and have length atleast
 *	three.
 */
static int
est(a)
register mint *a;
{
	register int result;

	assert(!ispos(a) && a->len >= 3);
	result = (NEFL - a->val[a->len - 2]) << L2BASE;
	result += NEFL - a->val[a->len - 3] + 1;
	assert(BASE * BASE >= result && result > BASE);
	return (result);
}


/*
 *	Guess returns an integer which is the integral part of
 *		"a" / ("den" * BASE ^ "shift")
 *	Note that it is assumed that the following hold:
 *		1.  "a" is positive.
 *		2.  BASE ^ 2 >= "den" > BASE
 *		3.  the quotient is a single "digit" (ie. NEFL > quotient >= 0)
 */

static int
guess(den, shift, a)
int den;
unsigned shift;
mint *a;
{
	register char *ap = &a->val[a->len - 1];
	long result;

	if (*ap == 0)
		--ap;
	assert(ap + 1 - a->val <= shift + 3);
	if (ap + 1 - a->val < shift + 2)
		return (0);
	result = (ap[0] << L2BASE) + ap[-1];
	if (ap + 1 - a->val == shift + 3)
		result = (result << L2BASE) + ap[-2];
	assert(result < den * (long)BASE);
	return (result / den);
}


/*
 *	Snc does a shift-negate-compare.  Specifically, it assumes that
 *	"b" is negative, "a" is positive and it returns wether or not
 *		a  >=  -b * BASE ^ shift
 *	(ie. a + b * BASE ^ shift is positive (ie. >= 0).)
 */

static
snc(b, shift, a)
mint *a, *b;
unsigned shift;
{
	register char *ap;
	register char *bp;
	register char *limit;
	int aleng, bleng;
	int temp;

	/*
	 * correct length of a
	 */
	limit = a->val;
	for (ap = &limit[a->len - 1]; *ap == 0 && ap > limit; --ap)
		;
	a->len = ap - limit + 1;
	if (*ap == NEFL)
		++a->len;
	aleng = ap - limit + 1;
	bleng = b->len + shift - 1;

	/*
	 *	BASE ^ aleng  >  a  >=  BASE ^ (aleng - 1)
	 *	BASE ^ (b->len - 1)  >=  -b  >  BASE ^ (b->len - 2)
	 *	so multiplying by BASE ^ shift gives
	 *	BASE ^ bleng  >=  -b * BASE ^ shift  >  BASE ^ (bleng - 1)
	 *	This means that
	 *	IF	bleng  >  aleng
	 *	THEN	bleng - 1  >=  aleng
	 *	SO	-b * BASE ^ shift  >  BASE ^ (bleng - 1)
	 *				   >= BASE ^ aleng
	 *				   >  a
	 *	and thus we return FALSE.
	 *	Also we have
	 *	IF	aleng  >  bleng
	 *	THEN	aleng - 1  >=  bleng
	 *	SO	a  >=  BASE ^ (aleng - 1)
	 *		   >=  BASE ^ bleng
	 *		   >=  -b * BASE ^ shift
	 *	and thus we return TRUE.
	 *	Finally
	 *	If aleng = bleng then the two numbers line up and
	 *	it is just a matter of returning wether or not their
	 *	sum would cause a carry.
	 */
	if (aleng != bleng)
		return (aleng > bleng);
	limit = b->val;
	for (bp = &limit[b->len - 2]; bp >= limit; --bp, --ap) {
		temp = *ap + *bp;
		if (temp >= BASE)
			return (TRUE);
		if (temp < NEFL)
			return (FALSE);
	}
	return (FALSE);
}
