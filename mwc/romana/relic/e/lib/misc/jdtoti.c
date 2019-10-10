/*
 * Coherent time to Julian day in integers.
 */
#include "misc.h"

time_t
jday_to_time(jd)
jday_t jd;
{
	return (((jd.j_d-COHEPOCH)*86400L)+jd.j_s-86400L/2);
}
