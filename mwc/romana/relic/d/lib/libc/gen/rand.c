/*
 * Random number generator, of period 2 ^ 32. Returns random numbers (ints) in
 *	the interval [0, 2^15-1].
 * The algorithm used is a linear congruential method, where the internal
 *	computation is done in unsigned longs (mod 2^32). The numbers from
 *	this sequence are right shifted 17 bits so that the most random 15
 *	bits are returned.
 * All this is from Knuth Vol 2, 2nd ed., ch 3. The choice of multiplier
 *	is made from the table in 3.3.4.E,  pp102.
 */

#define	A	1664525L		/* Multiplicative generator */
#define	C	907633387L		/* Additive generator */


static long	seed	= 1;

srand(n)
unsigned int n;
{
	seed = n;
}

rand()
{
	seed = seed * A + C;
	return ((seed >> 17) & 077777);
}
