/*
 * libc/stdio/_dtefg.c
 * ANSI-compliant C standard i/o library internals.
 * _dtefg(), _dtoa()
 * ANSI 4.9.6.1.
 * Floating point output conversion routines for 'printf'.
 * Conditionalized #if _IEEE to do 8087 conversion.
 */

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>

#if	0
#include <locale.h>
#else
#define	_decimal_point	'.'
#endif

extern	char	*_dtof();

#if	_IEEE
#include <stddef.h>
#include <string.h>

/*
 * This table is indexed by the return value of the "_fxam" routine.
 * The number is converted if the entry is NULL,
 * otherwise the entry gives the string to print.
 */
static readonly char *fxamsg[] = {
	"{+ Unnormal}",
	"{+ NAN}",
	"{- Unnormal}",
	"{- NAN}",
	NULL,			/* + Normal	*/
	"{+ Infinity}",
	NULL,			/* - Normal	*/
	"{- Infinity}",
	NULL,			/* +0		*/
	NULL,			/* Empty	*/
	NULL,			/* -0		*/
	NULL,			/* Empty	*/
	"{+ Denormal}",
	NULL,			/* Empty	*/
	"{- Denormal}",
	NULL			/* Empty	*/
};
#endif

/*
 * Convert a floating point number 'd' from binary
 * into 'e', 'E', 'f', 'g', or 'G' format ASCII in the buffer 'cp'.
 * 'fmt' is the conversion type.
 * 'prec' is the precision.
 * 'aflag' is the '#' flag from the conversion specication.
 * 'signp' points to the returned sign (-1 negative, 1 nonnegative).
 * Return a pointer past the last character.
 *
 * Called from printf() for fp conversions and from gcvt().
 */
char *
_dtefg(cp, dp, fmt, prec, aflag, signp)
register char *cp; double *dp; int fmt, prec, aflag; int *signp;
{
	int	eflag, decexp;
	char	tbuf[DBL_DIG+1];
	double	d;

	d = *dp;
#if	_IEEE
	{
		register char *cp2;

		/* Print given string if 8087 format is special. */
		if ((cp2=fxamsg[_fxam(d)]) != NULL) {
			strcpy(cp, cp2);
			return cp + strlen(cp);
		}
	}
#endif

	if (prec == 0 && (fmt == 'g' || fmt == 'G'))
		prec = 1;
	else if (prec == -1)
		prec = 6;			/* Default precision */

	if (d < 0.0) {
		d = -d;				/* Force d nonnegative */
		*signp = -1;
	} else
		*signp = 1;
	_dtoa(fmt, &d, prec, &decexp, tbuf);
	eflag = (fmt=='e' || fmt=='E'
	      || ((fmt=='g' || fmt=='G') && (decexp < -4 || decexp >= prec)));
	cp = _dtof(cp, tbuf, prec, eflag ? 0 : decexp, fmt, aflag);
	if (eflag) {
		*cp++ = (fmt == 'E' || fmt == 'G') ? 'E' : 'e';
		cp += sprintf(cp, "%+03d", decexp);
	}
	return cp;
}

/*
 * Copy ASCII number from 'cp' to 'buf' in %f format
 * with precision 'prec' and decimal exponent 'decexp'.
 * The 'fmt' determines whether trailing zeros are suppressed.
 * Return a pointer past the last character.
 */
static
char *
_dtof(buf, cp, prec, decexp, fmt, aflag)
register char *buf, *cp; register int prec, decexp, fmt, aflag;
{
	if (decexp < 0)
		*buf++ = '0';			/* Units digit '0' */
	else
		do
			*buf++ = *cp ? *cp++ : '0';	/* integral part */
		while (decexp--);
	if (!aflag && (prec == 0 || ((fmt=='g'|| fmt=='G') && *cp == '\0')))
		return buf;
	*buf++ = _decimal_point;		/* '.' */
	while (prec-- > 0) {
		if ((fmt=='g' || fmt=='G') && *cp == '\0' && !aflag)
			break;			/* suppress trailing zeros */
		if (++decexp < 0)
			*buf++ = '0';		/* put leading zero */
		else
			*buf++ = *cp ? *cp++ : '0';
	}
	return buf;
}

/*
 * Convert the mantissa of nonnegative double 'd' to a string of ASCII digits
 * in the supplied buffer 'buf'.
 * Store the decimal exponent indirectly through 'decexpp'.
 * The first digit of the mantissa is implicitly followed by '.'
 * The result has no leading zeros (unless d=0.0) and no trailing zeros.
 * The precision 'prec' and format 'fmt' determine the digit count.
 * The maximum length of the result is DBL_DIG+1 (for the NUL).
 * For example, if *dp==123.456789 and prec==3:
 *	fmt=='e'	"1234"		decexp==2	1.234e+02
 *	fmt=='f'	"123456"	decexp==2	123.456
 *	fmt=='g'	"123"		decexp==2	123
 * This is called directly by ecvt() and fcvt(), as well as from _dtefg().
 */
void
_dtoa(fmt, dp, prec, decexpp, buf) int fmt; double *dp; int prec; int *decexpp; char *buf;
{
	register char *	cp;
	register int	digit;
	register int	decexp;
	int		ndigits;
	int		binexp;
	double		d;
	double		dexp;

	/* Handle 0.0 as a special case. */
	cp = buf;
	if ((d = *dp) == 0.0) {
ret0:
		*decexpp = 0;
		*cp++ = '0';
		*cp ='\0';
		return;
	}

	/* Reduce d to range [1., 10) and set decexp accordingly. */
	/* Approximate the decimal exponent from the binary exponent. */
	/* Obscure but it makes floating output much more efficient. */
	frexp(d, &binexp);			/* Find binary exponent */
	if (modf((--binexp)/LOG10B2, &dexp) < 0.0)
		dexp -= 1.0;			/* Scale, take integer part */
	decexp = dexp;				/* Convert to integer */
	d *= _pow10(-decexp);			/* Reduce d by power of 10 */
	if (d >= 10.) {				/* May be off by 1 place */
		++decexp;
		d *= 0.10;
	}
	*decexpp = decexp;			/* Store the decimal exponent */

	/* Compute the desired number of result digits. */
	if (fmt == 'e' || fmt == 'E')
		ndigits = prec + 1;		/* For 'e' or 'E' format */
	else if (fmt == 'f')
		ndigits = prec + decexp + 1;	/* For 'f' format */
	else
		ndigits = prec;			/* For 'g' or 'G' format */
	if (ndigits <= 0) {			/* No significant digits */
		if (ndigits == 0 && d > 5.0)	/* Round up to one digit */
			goto ret1;		/* and return "1" */
		else
			goto ret0;		/* return "0" */
	} else if (ndigits > DBL_DIG)
		ndigits = DBL_DIG;		/* Maximum precision */
	
	/* Compute the result digits. */
	for ( ; cp < &buf[ndigits] && d != 0.0; ) {
		digit = (int) d;
		*cp++ = digit + '0';		/* Store next digit */
		d = 10.0 * (d-digit);		/* and reduce d accordingly */
	}
	*cp = '\0';				/* NUL-terminate result */

	/* Round up the result if appropriate. */
	if (d <= 5.0) {				/* Do not round up */
		while (--cp != buf && *cp == '0')
			*cp = '\0';		/* Strip a trailing '0' */
		return;
	}
	while (cp-- != buf) {			/* Round up */
		if (++*cp <= '9')		/* Bump last digit */
			return;
		*cp = '\0';			/* Strip a trailing '0' */
	}
ret1:
	++*decexpp;				/* and return "1" */
	*cp++ = '1';
	*cp = '\0';
}

/* end of libc/stdio/_dtefg.c */
