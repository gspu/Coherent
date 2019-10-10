/*
 * libm/sin.c
 * Evaluate the sine function.
 */

#include <math.h>

#if	EMU87
#include "emumath.h"
#endif

#define	THRESHOLD	1.111111111111111111e-3

/*
 * When |x| is very small, use the series
 *	x - x**3 / 6 + x**5 / 120 - ...
 * to get an accurate result while avoiding loss of precision.
 * The THRESHOLD value above is determined empirically.
 */
static readonly double sintab[] ={
	 0.000000000000000000000000,
	 1.000000000000000000000000,
	 0.000000000000000000000000,
	-0.166666666666666666666666,
	 0.000000000000000000000000,
	 0.008333333333333333333333
};

double
sin(x) double x;
{
	if (fabs(x) < THRESHOLD)
		return _pol(x, sintab, 6);
	return cos(PI/2.0 - x);
}

/* end of sin.c */
