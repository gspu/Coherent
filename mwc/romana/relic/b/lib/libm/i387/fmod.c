/*
 * libm/fmod.c
 * C mathematics library.
 * fmod()
 * ANSI 4.5.6.4.
 * Floating point remainder function.
 * Implementation-defined behavior:
 *	issues EDOM error and returns 0.0 when y==0.0.
 */

#include <math.h>

#if	EMU87
#include "emumath.h"
#endif

double
fmod(x, y) double x, y;
{
	register int s;

	if (y == 0.0) {
		errno = EDOM;
		return 0.0;
	}
	s = (x >= 0);
	x = fabs(x);
	y = fabs(y);
	x -= y * floor(x/y);
	return (s) ? x : -x;
}

/* end of libm/fmod.c */
