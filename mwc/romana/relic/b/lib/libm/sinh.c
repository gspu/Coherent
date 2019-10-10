/*
 * libm/sinh.c
 * Hyperbolic sine.
 */

#include <math.h>

#if	EMU87
#include "emumath.h"
#endif

/*
 * To get a correct result for very small |x|,
 * the code below just returns x for |x| < THRESHOLD.
 * We can derive a theoretical value for THRESHOLD from the series:
 *	sinh(x)  = x + x^3/3! + x^5/5! + ...
 * so if |x| < sqrt(6 * DBL_EPSILON) then x^3/3! < x * DBL_EPSILON
 * and the low-order terms must be insignificant.
 * The threshold value below, arrived at empirically, is somewhat larger;
 * it is for IEEE fp, the DECVAX value must be slightly different but...
 */
#define	THRESHOLD	8.899087910006383e-6

double
sinh(x) double x;
{
	double r;
	register int e;

	if (fabs(x) < THRESHOLD)
		return x;
	/*
	 * Hart (6.2.27) gives an alternative formula for |x| < 1,
	 * unused here; should it be used?
	 */
	e = errno;
	r = exp(x);
	errno = e;
	r = (r-1.0/r) / 2.0;
	return r;
}

/* end of sinh.c */
