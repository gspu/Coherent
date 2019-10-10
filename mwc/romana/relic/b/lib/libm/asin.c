/*
 * Compute the inverse sine function.
 */
#include <math.h>

#if	EMU87
#include "emumath.h"
#endif

double
asin(x)
double x;
{
	if (x<-1.0 || x>1.0) {
		errno = EDOM;
		return (0.0);
	}
	if (x == 1.0)
		return (PI/2.0);
	if (x == -1.0)
		return (-PI/2.0);
	return (atan(x/sqrt(1.0-x*x)));
}
