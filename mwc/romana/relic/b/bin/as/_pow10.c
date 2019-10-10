/*
 * C general utilities library internals.
 * _pow10()
 * Compute 10.0^n.
 */

/*
 * There are lots of ways to do this, with varying accuracy and speed.
 * Using recursion on all negative exponents eliminates the divide
 * but introduces greater roundoff errors.
 */

static double powtab[] = {
	1e-16,	1e-15,	1e-14,	1e-13,	1e-12,	1e-11,	1e-10,	1e-9,
	1e-8,	1e-7,	1e-6,	1e-5,	1e-4,	1e-3,	1e-2,	1e-1,
	1e0,
	1e1,	1e2,	1e3,	1e4,	1e5,	1e6,	1e7,	1e8,
	1e9,	1e10,	1e11,	1e12,	1e13,	1e14,	1e15,	1e16
};

double
_pow10(exp) register int exp;
{

	if (exp < -16)
		return (1.0/_pow10(-exp));
	else if (exp <= 16)
		return (powtab[exp+16]);
	else if (exp <= 64)
		return (1e16 * _pow10(exp - 16));
	else
		return (1e64 * _pow10(exp - 64));
}
