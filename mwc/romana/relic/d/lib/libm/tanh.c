/*
 * Hyperbolic tangent.
 */
#include <math.h>

#if	EMU87
#include "emumath.h"
#endif

double
tanh(x)
double x;
{
	double r;
	register int s;

	s = 0;
	if (x < 0.0) {
		x = -x;
		s = 1;
	}
	r = exp(-2.0*x);
	r = (1.0-r) / (1.0+r);
	return (s?-r:r);
}
