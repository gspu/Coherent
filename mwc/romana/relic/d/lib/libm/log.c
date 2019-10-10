/*
 * Natural logarithm.
 */
#include <math.h>

#if	EMU87
#include "emumath.h"
#endif

double
log(x)
double x;
{
	return (log10(x)*LOG10BE);
}
