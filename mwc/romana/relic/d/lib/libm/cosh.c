/*
 * Hyperbolic cosine.
 */
#include <math.h>

#if	EMU87
#include "emumath.h"
#endif

double
cosh(x)
double x;
{
	double r;
	register int e;

	e = errno;
	r = exp(x);
	errno = e;
	r = (r+1.0/r) / 2.0;
	return (r);
}
