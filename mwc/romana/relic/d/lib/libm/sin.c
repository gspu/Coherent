/*
 * Evaluate the sine function.
 */
#include <math.h>

#if	EMU87
#include "emumath.h"
#endif

double
sin(x)
double x;
{
	return (cos(x-PI/2.0));
}
