/*
 * Compute the inverse cosine function.
 */
#include <math.h>

#if	EMU87
#include "emumath.h"
#endif

double
acos(x)
double x;
{
	if (x<-1.0 || x>1.0) {
		errno = EDOM;
		return (0.0);
	}
	return (PI/2.0 - asin(x));
}
