/*
 * Square root function.
 */
#include <math.h>

#if	EMU87
#include "emumath.h"
#endif

double
sqrt(x)
double x;
{
	double s;
	int i;
	register int n;

	if (x < 0.0) {
		errno = EDOM;
		return (0.0);
	}
	if (x == 0.)
		return(0.);
	n = L2L2P;
	/* The assignment in the following line is to avoid compiler bug. */
	s = frexp(x, &i);
	s = ldexp(1.0, i/2);
	do {
		s = (s + x/s) / 2.0;
	} while (--n);
	return (s);
}
