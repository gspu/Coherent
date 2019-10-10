/*
 * Julian date to year, month, mday, hour, min, sec, yday, wday fields
 * Algorithm from Duffett-Smith, Practical Astronomy With Your Calculator
 * Also Meeus, Astronomical Formulae for Calculators
 */
#include "local_misc.h"
tm_t *
jday_to_tm(jd)
jday_t jd;
{
	register tm_t *tp;
	register long d, s, A, B, F;
	static tm_t t;

	tp = &t;
	d = jd.j_d;
	s = jd.j_s;
	if ((s += 86400L/2) >= 86400L) {
		s -= 86400L; d += 1;
	}
	tp->tm_isdst = 0;		/* no telling who stepped here */
	tp->tm_wday = (d+1) % 7;
	if ((A = d) > 2299160L) {
		A = (d*100 - 186721625L) / 3652425L;
		A = d + 1 + A - (A / 4);
	}
	B = A + 1524;
	tp->tm_year = (B*100 - 12210) / 36525L;
	B -= (tp->tm_year * 36525L) / 100;
	tp->tm_mon = (B * 10000) / 306001L;
	F = (306001L * tp->tm_mon) / 10000;
	tp->tm_mday = B - F;
	tp->tm_yday = B - (tp->tm_mon >= 14 ? 428+1 : 61+2+1);
	tp->tm_mon -= tp->tm_mon > 13 ? 13 : 1;
	tp->tm_year -= tp->tm_mon > 2 ? 4716 :  4715;
	if (tp->tm_mon > 2 && isleapyear(tp->tm_year))
		++tp->tm_yday;
	tp->tm_sec = s % 60;
	s /= 60;
	tp->tm_min = (unsigned int)s % 60;
	tp->tm_hour = (unsigned int)s / 60;
	/* Correct for tm_t bias */
	tp->tm_year -= 1900;
	tp->tm_mon -= 1;
	return tp;
}
