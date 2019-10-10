/*
 * Year, month, day to julian day in integers.
 * Algorithm from Duffett-Smith, Practical Astronomy With Your Calculator
 * Also Meeus, Astronomical Formulae for Calculators
 */
#include "local_misc.h"

/* Pack year, month, and day into long package for easy compares */
#define pymd(y,m,d)	(((long)(y)<<16)|((m)<<8)|(d))

jday_t
tm_to_jday(tp)
register tm_t *tp;
{
	register int y, m, B;
	register long C, D;
	jday_t jd;

	y = tp->tm_year + 1900;
	m = tp->tm_mon + 1;
	if (m <= 2) {
		y -= 1; m += 12;
	}
	/* Gregorian correction */
	if (pymd(y, m, tp->tm_mday) > pymd(1582, 10, 15)) {
		B = 2 - (y/100) + (y/400);
	} else
		B = 0;
	C = (y * 36525L) / 100;
	D = ((m + 1) * 306001L) / 10000;
	jd.j_d = tp->tm_mday + B + C + D + 1720994L;
	jd.j_s = tp->tm_sec + 60L * (tp->tm_min + 60 * tp->tm_hour);
	if ((jd.j_s += 86400L/2) >= 86400L) {
		jd.j_s -= 86400L; jd.j_d += 1;
	}
	return jd;
}
