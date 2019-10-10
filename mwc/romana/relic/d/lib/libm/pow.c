/*
 * libm/pow.c
 * Raise x to the power y.
 */

#include <math.h>

#if	EMU87
#include "emumath.h"
#endif

double
pow(x, y)
double x, y;
{
	double r;
	register unsigned s, i, e;

	s = 0;
	i = 0;
	if (x == 0.0) {
		if (y <= 0.0)
			errno = EDOM;
		return 0.0;
	}
	if ((r = modf(y, &r)) < 0.0)
		r += 1.0;
	if (x < 0.0) {
		if (r != 0.0) {
			errno = EDOM;
			return 0.0;
		}
		x = -x;
		if (((int) y) & 1)
			s = 1;
	}
	if (y < 0.0) {
		y = -y;
		i = 1;
	}
	if (r!=0.0 || y>16384.0)
		r = _two(y*log10(x)*LOG10B2);
	else {
		r = 1.0;
		for (e=y; e; e>>=1) {
			if (e&01)
				r *= x;
			x *= x;
		}
	}
	if (i)
		r = 1/r;
	return s ? -r : r;
}

/* end of libm/pow.c */
