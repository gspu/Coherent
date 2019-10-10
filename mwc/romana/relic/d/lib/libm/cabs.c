/*
 * Complex absolute value.
 */
#include <math.h>

#if	EMU87
#include "emumath.h"
#endif

double
cabs(z)
CPX z;
{
	return (hypot(z.z_r, z.z_i));
}
