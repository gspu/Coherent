/*
 * Floating point output conversion routines for 'printf'.
 * Conditionalized #if NDP to do 8087 conversion.
 */

#include <math.h>

extern	char	*_dtoa();
extern	char	*_dtof();
extern	double	frexp();
extern	double	modf();
extern	double	_pow10();

#if	NDP
#include <stdio.h>

extern	char	*strcpy();

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
 * into 'e', 'f' or 'g' format ASCII in the buffer 'buf'.
 * The 'fmt' argument is the conversion type.
 * The 'prec' argument is the precision.
 * Return a pointer past the last character.
 */
char *
_dtefg(fmt, dp, prec, buf)
int		fmt;
double		*dp;
int		prec;
register char	*buf;
{
	register char	*cp;
	int		decexp, sign;
	char		tbuf[64];
	double		d;

	d = *dp;
#if	NDP
	/* Print given string if 8087 format is special. */
	if ((cp=fxamsg[_fxam(d)]) != NULL) {
		strcpy(buf, cp);
		return(buf + strlen(buf));
	}
#endif

	if (prec < 0)
		prec = 6;				/* Default precision */
	if (d < 0.0) {
		d = -d;					/* Force d nonnegative */
		*buf++ = '-';				/* Leading '-' */
	}
	cp = _dtoa(fmt, d, prec, &decexp, &sign, tbuf);
	if (fmt == 'e'
	 || (fmt == 'g' && (decexp > 4 || decexp < -prec))) {	/* 'e' format */
		buf = _dtof(buf, cp, prec, 0, fmt=='g');	/* mantissa */
		if (decexp >= 0)
			sprintf(buf, "e+%02u", decexp);		/* exponent */
		else
			sprintf(buf, "e-%02u", -decexp);
		return(buf + strlen(buf));
	}
	return(_dtof(buf, cp, prec, decexp, fmt=='g'));	/* 'f' format */
}

/*
 * Copy ASCII number from 'cp' to 'buf' in %f format
 * with precision 'prec' and decimal exponent 'decexp'.
 * The 'isgfmt' flag determines whether trailing zeros are suppressed.
 * Return a pointer past the last character.
 */
static
char *
_dtof(buf, cp, prec, decexp, isgfmt)
register char *buf;
register char *cp;
register int prec;
register int decexp;
int isgfmt;
{
	if (decexp < 0)
		*buf++ = '0';			/* Units digit '0' */
	else do
		*buf++ = *cp ? *cp++ : '0';	/* or integral part */
	while (decexp--);
	if (prec == 0 || (isgfmt && *cp == '\0'))
		return(buf);
	*buf++ = '.';				/* '.' */
	while (prec-- > 0) {
		if (isgfmt && *cp == '\0')
			break;			/* suppress trailing zeros */
		if (++decexp < 0)
			*buf++ = '0';		/* put leading zero */
		else
			*buf++ = *cp ? *cp++ : '0';
	}
	return (buf);
}

/*
 * Convert nonnegative double 'd' to string of ASCII digits with no leading zeros
 * (unless "0") and no trailing zeros, with precision 'prec' in format 'fmt'.
 * Return a pointer to the converted string, usually (not always) in 'buf'.
 * Store the decimal exponent indirectly through 'decexpp'.
 * The first digit of the return value is implicitly followed by '.'
 * and the return value is implicitly multiplied by 10 to the decimal exponent.
 */
char *
_dtoa(fmt, d, prec, decexpp, signp, buf)
int	fmt;
double	d;
int	prec;
int	*decexpp;
int	*signp;
char	*buf;
{
	register char	*cp;
	register int	digit;
	register int	decexp;
	int		ndigits;
	int		binexp;
	double		dexp;

	/*
	 * Force d >= 0.0.  The "signp" arg is extraneous in the _dtefg() call,
	 * but is retained for the calls from ecvt() and fcvt().  Bah.
	 */
	if (d < 0.0) {
		*signp = 1;
		d = -d;
	}
	else
		*signp = 0;

	/* Handle 0.0 as a special case. */
	if (d == 0.0) {
		*decexpp = 0;
		return("0");
	}

	/* Reduce d to range [1., 10) and set decexp accordingly. */
	/* Approximate the decimal exponent from the binary exponent. */
	/* Obscure but it makes floating output much more efficient. */
	frexp(d, &binexp);			/* Find binary exponent */
	modf((--binexp)/LOG10B2, &dexp);	/* Scale, take integer part */
	decexp = dexp;				/* Convert to integer */
	d *= _pow10(-decexp);			/* Reduce d by power of 10 */
	if (d >= 10.) {				/* May be off by 1 place */
		++decexp;
		d *= 0.10;
	}
	*decexpp = decexp;			/* Store the decimal exponent */

	/* Compute the desired number of result digits. */
	if (fmt == 'e' || (fmt == 'g' && (decexp > 4 || decexp < -prec)))
		ndigits = prec + 1;		/* For 'e' format */
	else
		ndigits = prec + decexp + 1;	/* For 'f' format */
	if (ndigits <= 0) {			/* No significant digits */
		if (ndigits == 0 && d > 5.0) {	/* Round up to one digit */
			++*decexpp;
			return("1");
		}
		*decexpp = 0;
		return("0");
	}
	else if (ndigits > L10P)
		ndigits = L10P;			/* Maximum precision */

	/* Compute the result digits. */
	for (cp = buf; cp < &buf[ndigits] && d != 0.0; ) {
		digit = (int) d;
		*cp++ = digit + '0';		/* Store next digit */
		d = 10.0 * (d-digit);		/* and reduce d accordingly */
	}
	*cp = '\0';				/* NUL-terminate result */

	/* Round up the result if appropriate. */
	if (d <= 5.0) {				/* Do not round up */
		while (--cp != buf && *cp == '0')
			*cp = '\0';		/* Strip a trailing '0' */
		return (buf);
	}
	while (cp-- != buf) {			/* Round up */
		if (++*cp <= '9')		/* Bump last digit */
			return (buf);
		*cp = '\0';			/* Strip a trailing '0' */
	}
	++*decexpp;				/* Bump exponent */
	return("1");				/* and return "1" */
}
