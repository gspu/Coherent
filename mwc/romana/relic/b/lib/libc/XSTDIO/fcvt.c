/*
 * fcvt.c
 * Historical "f"-format floating point output conversion.
 * Non-ANSI.
 */

#include <math.h>

char *
fcvt(d, width, decp, signp) double d; int width; int *decp; int *signp;
{
	static char buf[L10P+1];

	if (d < 0) {
		*signp = 1;
		d = -d;
	} else
		*signp = 0;
	_dtoa('f', &d, width<=0 ? 0 : width, decp, buf);
	++*decp;
	return buf;		
}
