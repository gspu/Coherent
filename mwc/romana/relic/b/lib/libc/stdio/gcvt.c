/*
 * gcvt.c
 * Historical "g"-format floating point output conversion.
 * Non-ANSI.
 */

#include <stdio.h>

char *
gcvt(d, prec, buf) double d; int prec; char *buf;
{
	int i;
	register char *cp;

	cp = _dtefg(buf, &d, 'g', prec, 0, &i);
	*cp = '\0';
	return buf;
}

/* end of gcvt.c */
