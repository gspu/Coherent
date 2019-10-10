/*
 * Floating absolute value.
 */
#include <math.h>

#if	EMU87
#include "emumath.h"
#endif

double
fabs(x)
double x;
{
	if (x < 0.0)
		x = -x;
	return (x);
}
