/*
 * libc/gen/strftime.c
 * C standard date and time library.
 * difftime()
 * ANSI 4.12.2.2.
 * Compute difference between two times.
 * Requires floating point.
 */

#include <time.h>

double
difftime(t1, t0) time_t t1, t0;
{
	return ((double)t1-t0);
}
