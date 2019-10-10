/*
 * Evaluate a polynomial.
 * The polynomial must contain at least two terms.
 */
#include <math.h>

double
_pol(x, c, n)
double x;
double c[];
register int n;
{
	double r, d;
	register int i;

	r = c[0];
	d = x;
	--n;
	for (i=1; i<n; i++) {
		r += d*c[i];
		d *= x;
	}
	r += d*c[i];
	return (r);
}
