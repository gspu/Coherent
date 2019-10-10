/*
 * Hypotenuse function.
 */
#include <math.h>

#if	EMU87
#include "emumath.h"
#endif

double
hypot(x, y)
double x;
double y;
{
	double r;

	if (x == 0.)
		r = y;
	else {
		r = y/x;
		r = x * sqrt(1.0 + r*r);
	}
	return (r < 0. ? -r : r);
}
