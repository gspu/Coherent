/*
 * libc/crt/modf.c
 */

/*
 * double
 * modf(real, dp)
 * double real, *dp;
 *
 * modf() finds g = greatest integer <= real,
 * then stores g through dp and returns real - g.
 * This version uses a machine dependent subroutine
 *	double _modf(real, dp, e)
 * which assumes real >= 0, 1 <= e = exponent(real) <= DMBITS+1,
 * where DMBITS is the number of mantissa bits in a double.
 */

#if	_IEEE
#define	DEBITS	11
#define	DMBITS	52
#define	FEBITS	 8
#define	FMBITS	23
#endif

#if	_DECVAX
#define	DEBITS	 8
#define	DMBITS	55
#define	FEBITS	 8
#define	FMBITS	23
#endif

extern	double	_modf();
extern	double	frexp();

double
modf(d, dp) double d; register double *dp;
{
	int e;

 	frexp(d, &e);
	if (e >= DMBITS+1) {			/* very large d */
		*dp = d;
		return 0.0;
	} else if (e <= 0) {			/* -1. < d < 1. */
		*dp = 0.0;
		return d;
	} else if (d >= 0.0)
		return _modf(d, dp, e);		/* d >= 0., 1 <= e < DMBITS+1 */

	/* d is negative. */
	d = _modf(-d, dp, e);
	*dp = -*dp;
	return -d;
}

/* end of modf.c */
