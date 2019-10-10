/*
 * libm/two.c
 * Evaluate 2 to the power x.
 * (Hart 1067, 18.08)
 */

#include <math.h>

#if	EMU87
#include "emumath.h"
#endif

static readonly double twontab[] ={
	0.1513906799054338915894328e+04,
	0.2020206565128692722788600e+02,
	0.2309334775375023362400000e-01
};
static readonly double twomtab[] ={
	0.4368211662727558498496814e+04,
	0.2331842114274816237902950e+03,
	0.1000000000000000000000000e+01
};

double
_two(x) double x;
{
	double p, q, r, e;
	register int s;

	if (x > L2HUGE_VAL) {
		errno = ERANGE;
		return HUGE_VAL;
	}
	s = 0;
	if ((x = modf(x, &e)) < 0.0) {
		x += 1.0;
		e -= 1.0;
	}
	if (x > 0.5) {
		s = 1;
		x -= 0.5;
	}
	r = x*x;
	p = x*_pol(r, twontab, 3);
	q = _pol(r, twomtab, 3);
	r = (q+p)/(q-p);
	if (s)
		r *= SQRT2;
	return ldexp(r, (int) e);
}

/* end of libm/two.c */
