/*
 * libm/ceil.c
 * Ceiling.
 */

#include <math.h>

#if	EMU87
#include "emumath.h"
#endif

double
ceil(x) double x;
{
	double r;

	return (modf(x, &r) > 0.0) ? r + 1.0 : r;
}

/* end of libm/ceil.c */
