/*
 * Evaluate the Bessel function of the first kind for the
 * n'th order.
 */
#include <math.h>

double
jn(n, x)
register int n;
double x;
{
	double r, ja, jb;
	register int m, i;

	if (x == 0.0)
		return (0.0);
	m = 0;
	if (x < 0.0) {
		x = -x;
		m = 1;
	}
	if (n < 0) {
		n = -n;
		if (n&1)
			m ^= 1;
	}
	switch (n) {
	case 0:
		r = j0(x);
		break;
	case 1:
		r = j1(x);
		break;
	default:
		jb = j0(x);
		ja = j1(x);
		for (i=1; i<n; i++) {
			r = (2.0*i/x)*ja - jb;
			jb = ja;
			ja = r;
		}
		break;
	}
	if (m)
		r = -r;
	return (r);
}
