/*
 * ecvt.c
 * Historical "e"-format floating point output conversion.
 * Non-ANSI.
 */

#include <math.h>

char *
ecvt(d, width, decp, signp) double d; int width; int *decp; int *signp;
{
	static char buf[L10P+1];

	if (d < 0) {
		*signp = 1;
		d = -d;
	} else
		*signp = 0;
	_dtoa('e', &d, width<=0 ? 0 : width-1, decp, buf);
	++*decp;
	return buf;		
}

/* end of ecvt.c */
