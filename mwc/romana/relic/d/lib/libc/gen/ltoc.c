/*
 * Convert long to comp_t style number.
 * A comp_t contains 3 bits of base-8 exponent
 * and a 13-bit mantissa.  Only unsigned
 * numbers can be comp_t numbers.
 */

#include <sys/types.h>

#define	MAXMANT		017777		/* 2^13-1 = largest mantissa */

comp_t
ltoc(l)
long l;
{
	register exp;

	if (l < 0)
		return (0);
	for (exp = 0; l > MAXMANT; exp++)
		l >>= 3;
	return ((exp<<13) | l);
}
