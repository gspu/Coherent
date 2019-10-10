/*
 * libm/floor.c
 * Floor.
 */

#include <math.h>

#if	EMU87
#include "emumath.h"
#endif

double
floor(x) double x;
{
	double r;

	return (modf(x, &r) < 0.0) ? r - 1.0 : r;
}

/* end of libm/floor.c */
