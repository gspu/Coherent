/*
 * libm/tanh.c
 * Hyperbolic tangent.
 */

#include <math.h>

#if	EMU87
#include "emumath.h"
#endif

/*
 * To get a correct result for very small |x|,
 * the code below just returns x for |x| < THRESHOLD.
 * We can derive a theoretical value for THRESHOLD from the series:
 *	tanh(x)  = x - x^3/3 + 2*x^5/17 - ...
 * so if |x| < sqrt(3 * DBL_EPSILON) then x^3/3 < x * DBL_EPSILON
 * and the low-order terms must be insignificant.
 * The threshold value below, arrived at empirically, is somewhat larger;
 * it is for IEEE fp, the DECVAX value must be slightly different but...
 */
#define	THRESHOLD	7.000900090031528e-06

double
tanh(x) double x;
{
	double r;
	register int s;

	if (fabs(x) < THRESHOLD)
		return x;
	/*
	 * Hart (6.2.28) gives an alternative formula for |x| < 1,
	 * unused here; should it be used?
	 */
	if (s = (x < 0.0))
		x = -x;
	r = exp(-2.0*x);
	r = (1.0-r) / (1.0+r);
	return s ? -r : r;
}

/* end of tanh.c */
