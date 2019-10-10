/*
 * C general utilities library.
 * strtod()
 * ANSI 4.10.1.4.
 * Convert ASCII to double (the System V way).
 * Builds significand in an unsigned long, for efficiency.
 * Does not use any knowledge about floating point representation,
 * except DBL_MIN_10_EXP, DBL_MAX_10_EXP, DBL_EXP_10_DIG.
 */

#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#if	__STDC__
#include <float.h>
#include <locale.h>
#else
#define	_decimal_point	'.'
#define	DBL_MIN_10_EXP	-37		/* DECVAX fp */
#define	DBL_MAX_10_EXP	38
#endif

#define	DBL_EXP_10_DIG	2		/* max dec digits in legal DECVAX exponent */

/* Flag bits. */
#define	NEG	1			/* negative significand */
#define	DOT	2			/* decimal point seen	*/
#define	NEGEXP	4			/* negative exponent	*/
#define	BIG	8			/* significand too big for ulong */
#define	OVER	16			/* overflow		*/
#define	UNDER	32			/* underflow		*/

double
strtod(nptr, endptr) char *nptr; char **endptr;
{
	register char *cp;
	register int c, flag, eexp;
	register unsigned long val;
	int exp, edigits, sdigits, vdigits;
	double d;
	char *savcp;

	cp = nptr;
	val = flag = exp = sdigits = vdigits = 0;
	d = 0.;

	/* Leading white space. */
	while (isspace(c = *cp++))
		;

	/* Optional sign. */
	switch (c) {
	case '-':
		flag |= NEG;
		/* fall through... */
	case '+':
		c = *cp++;
	}

	/* Next character must be decimal digit. */
	if (!isdigit(c)) {
		cp = nptr;
		goto done;
	}

	/*
	 * Significand: sequence of decimal digits with optional '.'.
	 * Compute chunks in val (for efficiency) until it overflows.
	 * d * _pow10(vdigits) + val contains the current significand.
	 */
	for (; ; c = *cp++) {
		if (isdigit(c)) {
			c -= '0';
			if (sdigits != 0 || c != 0)
				++sdigits;	/* significant digits seen */
#if	__STDC__
			if (val > (ULONG_MAX-9) / 10) {
#else
			/* The pre-ANSI compiler gets the test above wrong. */
			if (val > 429496728L) {
#endif
				/* Significand too big for val, use d. */
				if (flag & BIG)
					d = d * _pow10(vdigits) + val;
				else {
					d = val;
					flag |= BIG;
				}
				vdigits = 1;
				val = c;
			} else {
				++vdigits;	/* decimal digits in val */
				val = val * 10 + c;
			}
			if (flag & DOT)
				--exp;		/* implicit exponent */
		} else if (c == _decimal_point && (flag & DOT) == 0)
			flag |= DOT;
		else
			break;
	}

	/* Significand to d. */
	if (flag & BIG)
		d = d * _pow10(vdigits) + val;
	else
		d = val;

	/* Optional exponent: 'E' or 'e', optional sign, decimal digits. */
	if (c == 'e'  ||  c == 'E') {
		savcp = cp - 1;		/* save in case exponent absent */

		/* Optional sign. */
		switch (c = *cp++) {
		case '-':
			flag |= NEGEXP;
			/* fall through... */
		case '+':
			c = *cp++;
		}

		/* Next character must be decimal digit. */
		if (!isdigit(c)) {
			cp = savcp;		/* restore pre-'E' value */
			goto done;
		}

		/* Decimal digits. */
		for (eexp = edigits = 0; isdigit(c); c = *cp++) {
			if (edigits != 0 || c != '0')
				++edigits;	/* count exp digits for overflow */
			eexp = eexp * 10 + c - '0';
		}
		if (edigits > DBL_EXP_10_DIG) {
			flag |= ((flag & NEGEXP) ? UNDER : OVER);
			--cp;
			goto done;
		}

		/* Adjust explicit exponent for digits read after '.'. */
		if (flag & NEGEXP)
			exp -= eexp;
		else
			exp += eexp;
	}
	--cp;

	/* Reconcile the significand with the exponent. */
	if (exp <= DBL_MIN_10_EXP)
		flag |= UNDER;		/* exponent underflow */
	else if (exp + sdigits - 1 >= DBL_MAX_10_EXP)
		flag |= OVER;		/* exponent overflow */
	else if (exp != 0)
		d *= _pow10(exp);

done:
	if (endptr != (char **)NULL)
		*endptr = cp;
	if (flag & UNDER) {
		errno = ERANGE;
		return 0.0;
	}
	if (flag & OVER) {
		errno = ERANGE;
		d = HUGE_VAL;
	}
	return ((flag & NEG) ? -d : d);
}

/* end of strtod.c */
