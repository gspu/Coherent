/*
 * libm/tan.c
 * Evaluate the tangent function.
 * (Hart 4245, 17.08)
 */

#include <math.h>

#if	EMU87
#include "emumath.h"
#endif

static readonly double tanntab[] ={
	-0.16045331195592187943926861e+05,
	 0.12697548376580828837860720e+04,
	-0.17135185514886110932101000e+02,
	 0.28208772971655103151400000e-01
};
static readonly double tanmtab[] ={
	-0.20429550186600697853114142e+05,
	 0.58173599554655686739034190e+04,
	-0.18149310354089045993457500e+03,
	 0.10000000000000000000000000e+01
};

double
tan(x) double x;
{
	double r;
	register int i, s;

	if ((x = modf(x/(2.0*PI), &r)) < 0.0) {
		x += 1.0;
		r -= 1.0;
	}
	i = 0;
	s = 0;
	if (x > 0.5)
		x -= 0.5;
	if (x > 0.25) {
		s = 1;
		x = 0.5 - x;
	}
	if (x > 0.125) {
		i = 1;
		x = 0.25 - x;
	}
	x *= 8.0;
	r = x * x;
	r = x * (_pol(r, tanntab, 4)/_pol(r, tanmtab, 4));
	if (i) {
		if (r < 1.0/HUGE_VAL) {
			errno = ERANGE;
			return HUGE_VAL;
		}
		r = 1/r;
	}
	return s ? -r : r;
}

/* end of libm/tan.c */
