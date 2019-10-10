/*
 * Convert COHERENT time into julian date
 */
#include "time.h"
#include "misc.h"
jday_t
time_to_jday(t)
time_t t;
{
	jday_t jd;
	jd.j_s = (t += 86400L/2) % 86400L;
	jd.j_d = (t / 86400L) + COHEPOCH;
	return jd;
}
