/*
 * Convert a comp_t style number to a long.
 * A comp_t contains 3 bits of base-8 exponent
 * and a 13-bit mantissa.  Only unsigned
 * numbers can be comp_t numbers.
 */

#include <sys/types.h>
#include <acct.h>
#define	MAXMANT		017777		/* 2^13-1 - largest mantissa */

long
ctol(c)
comp_t c;
{
	register exp;
	long mant;

	mant = c&MAXMANT;
	exp = (c>>13)&07;
	while (exp--)
		mant <<= 3;
	return (mant);
}
