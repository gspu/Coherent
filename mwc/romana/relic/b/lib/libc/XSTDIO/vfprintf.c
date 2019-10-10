/*
 * libc/stdio/vfprintf.c
 * ANSI-compliant C standard i/o library.
 * vfprintf()
 * ANSI 4.9.6.7, 4.9.6.1.
 * Do the work for fprintf(), printf(), sprintf(), vprintf(), vsprintf().
 * Thanks to the ANSI committee for all the complication.
 *
 * Implementation-defined behavior:
 *	"%p" format is same as "%#.4X" or "%#.8X"
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>

/* Compile-time options. */
#define	LONGDOUBLE	0	/* iff sizeof(double) != sizeof(long double) */
#define	VA_PURE	0		/* iff va_list treated purely for doubles */

static	char	*convert();

/* Manifest constants. */
/* ANSI says any conversion item can be up to 509 characters. The only	*/
/* case where cbuf needs to be big is for floating point (%f).		*/
#define	CBUFMAX		512		/* conversion buffer size	*/
#define	NDIGITS		12		/* ASCII digits in octal long	*/
					/* i.e. "37777777777\0"		*/
#define	PRINTNULL	"{NULL}"	/* for %s with NULL arg		*/

int
vfprintf(fp, format, args) FILE *fp; register char *format; va_list args;
{
	char		cbuf[CBUFMAX];
	register char *	cbp;
	char *		cbs;
	char *		s;
	register int	count, c;
	long		l;
	int		fwidth, prec, base, len;
	int		leftjustify, plusflag, spaceflag, altflag, longflag;
	int		padchar, padwidth, issigned, prefix, ispfx, nzeros;
	va_list		rargs;
#if	VA_PURE
	double		d;
#if	LONGDOUBLE
	long double	ld;
#endif
#endif

	count = 0;			/* characters printed */
	for (;;) {

		/* Nonconversion characters. */
		while ((c = *format++) != '%') {
			if (c == '\0')
				return count;
			++count;
			putc(c, fp);
		}

		/* Optional flags "-+ #0". */
		leftjustify = plusflag = spaceflag = altflag = 0;
		padchar = ' ';
		for (;;) {
			switch(c = *format++) {
			case '-':	++leftjustify;	continue;
			case '+':	++plusflag;	continue;
			case ' ':	++spaceflag;	continue;
			case '#':	++altflag;	continue;
			case '0':	padchar = '0';	continue;
			default:	break;
			}
			break;
		}

		/* Optional field width ('*' or decimal integer). */
		if (c == '*') {
			if ((fwidth = va_arg(args, int)) < 0) {
				leftjustify = 1;
				fwidth = -fwidth;
			}
			c = *format++;
		} else
			for (fwidth = 0; c>='0' && c<='9'; c = *format++)
				fwidth = fwidth*10 + c-'0';

		/* Optional precision ('.' followed by '*' or decimal integer). */
		if (c == '.') {
			c = *format++;
			if (c == '*') {
				prec = va_arg(args, int);
				if (prec < 0)
					prec = -1;
				c = *format++;
			} else
				for (prec=0; c>='0' && c<='9'; c = *format++)
					prec = prec*10 + c-'0';
		} else
			prec = -1;

		/* Optional 'h', 'l' or 'L' flag. */
		if (c == 'l' || c == 'h' || c == 'L') {
			longflag = c;
			c = *format++;
		} else
			longflag = 0;

		/* Convert an item. */
		cbp = cbs = cbuf;
		issigned = nzeros = prefix = ispfx = 0;
		switch (c) {

		case 'd':
		case 'i':
			base = 10;
			if (longflag=='l')
				l = va_arg(args, long);
			else
				l = (long) va_arg(args, int);
			if (longflag == 'h')
				l = (short) l;
			if (l < 0L) {
				l = -l;
				--issigned;		/* -1 for negative */
			} else
				++issigned;		/* +1 for positive */
			goto conv;

		case 'o':
			base = 8;
			goto unsconv;

		case 'u':
			base = 10;
			goto unsconv;
	
		case 'x':
		case 'X':
			base = 16;
unsconv:
			if (longflag=='l')
				l = va_arg(args, long);
			else
#if	__STDC__
				/* The i8086 compiler sign-extends this. */
				l = (unsigned long) va_arg(args, int);
#else
				/* Kludge. */
				l = va_arg(args, int) & 0x0000FFFFL;
#endif
			if (longflag == 'h')
				l = (unsigned short) l;
			if (altflag && ((l != 0L && base == 8) || base == 16))
				prefix = c;		/* 'o', 'x' or 'X' */
conv:
			if (prec == 0 && l == 0L)
				break;			/* ANSI says so */
			if (prec != -1)
				padchar = ' ';		/* ignore 0 flag */
			cbp = convert(cbp, l, base, c);
			if ((nzeros = prec - (cbp - cbs)) < 0)	/* number of leading '0's */
				nzeros = 0;
			break;

		/*
		 * Floating point output.
		 * A simple floating point operation may have considerable
		 * overhead in some environments (e.g. MSDOS, where a large
		 * 8087 emulator gets linked into executables which require
		 * software floating point).  But there is only one
		 * ideologically pure way to get the fp arg from the arg list,
		 * namely with va_arg(), and that requires a fp fetch.
		 * The code below is conditionalized to do it the pure way
		 * (generating a fp fetch) or with a pointer to double (no fp).
		 */
		case 'f':
		case 'e':
		case 'E':
		case 'g':
		case 'G':
#if	VA_PURE
#if	LONGDOUBLE
			if (longflag == 'L') {
				ld = va_arg(args, long double);
				cbp = _ldtefg(cbp, &ld, c, prec, altflag, &issigned);
				break;
			}
#endif
			d = va_arg(args, double);
			cbp = _dtefg(cbp, &d, c, prec, altflag, &issigned);
			break;
#else
#if	LONGDOUBLE
			if (longflag == 'L') {
				cbp = _ldtefg(cbp, (long double *)args, c,
						 prec, altflag, &issigned);
				args = ((char *)args) + sizeof(long double);
				break;
			}
#endif
			cbp = _dtefg(cbp, (double *)args, c, prec, altflag, &issigned);
			args = ((char *)args) + sizeof(double);
			break;
#endif
		case 'c':
			*cbp++ = (unsigned char) va_arg(args, int);
			break;
	
		case 's':
			if ((s = va_arg(args, char *)) == NULL)
				s = PRINTNULL;		/* not strictly ANSI */
			cbp = cbs = s;
			while (*cbp++ != '\0')
				if (prec>=0 && cbp-s>prec)
					break;
			cbp--;
			break;

		/* Implementation-defined '%p' format: %#.4X or %#.8X */
		case 'p':
#if	_LARGE
			longflag = 'l';
			prec = 8;
#else
			longflag = 0;
			prec = 4;
#endif
			++altflag;
			c = 'X';
			base = 16;
			goto unsconv;

		case 'n':
			if (longflag == 'h')
				*(va_arg(args, short *)) = (short)count;
			else if (longflag == 'l')
				*(va_arg(args, long *)) = (long)count;
			else
				*(va_arg(args, int *)) = count;
			break;

		/*
		 * The '%r' recursive printf conversion is non-ANSI.
		 * The item is a va_list; its first member is the format.
		 */
		case 'r':
			rargs = va_arg(args, va_list);
			s = va_arg(rargs, char *);
			count += vfprintf(fp, s, rargs);
			break;
	
		default:
			++count;
			putc(c, fp);
			continue;
		}

		/* Output an item. */
		len = cbp - cbs;		/* length of converted item */
		if (issigned && (issigned == -1 || plusflag || spaceflag)) {
			++len;			/* for '-', '+', ' ' */
			++ispfx;
		}
		if (prefix) {
			++len;			/* for '0' */
			++ispfx;
			if (prefix != 'o')
				++len;		/* for 'x', 'X' */
		}
		if ((padwidth = fwidth - nzeros - len) < 0)
			padwidth = 0;		/* length of padding required */
		count += len + padwidth + nzeros;
		if (!leftjustify && padwidth > 0) {
			if (ispfx && padchar == '0')
				nzeros += padwidth;	/* prefix before 0-padding */
			else
				while (padwidth-- > 0)	/* pad on the left */
					putc(padchar, fp);
		}
		if (issigned) {
			if (issigned == -1)
				putc('-', fp);
			else if (plusflag)
				putc('+', fp);
			else if (spaceflag)
				putc(' ', fp);
		}
		if (prefix) {
			putc('0', fp);
			if (prefix != 'o')
				putc(prefix, fp);
		}
		while (nzeros-- > 0)
			putc('0', fp);		/* leading '0's */
		len = cbp - cbs;
		while (len-- > 0)
			putc(*cbs++, fp);	/* converted item */
		if (leftjustify)
			while (padwidth-- > 0)
				putc(' ', fp);	/* pad on the right */
	}
}

static const char digits[] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'A', 'B', 'C', 'D', 'E', 'F'
};
static const char ldigits[] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'a', 'b', 'c', 'd', 'e', 'f'
};

/*
 * Convert unsigned long 'n' to ASCII in base 'b' for conversion 'c'.
 * Store the result through 'cp' and return a pointer past the end.
 */
static
char *
convert(cp, n, b, c) register char *cp; unsigned long n; unsigned int b; int c;
{
	register char *	ep;
	char *		dp;
	unsigned int	u;
	char		pbuf[NDIGITS];

	dp = (c == 'x') ? &ldigits[0] : &digits[0];
	ep = &pbuf[NDIGITS-1];
	*ep = '\0';			/* NUL-terminate */
	if (n <= UINT_MAX) {
		u = n;			/* Use int arithmetic for efficiency */
		do {			/* Store next digit */
			*--ep = dp[u%b];
		} while ((u /= b) != 0);
	} else {
		do {			/* Store next digit */
			*--ep = dp[n%b];
		} while ((n /= b) != 0);
	}
	while (*ep)
		*cp++ = *ep++;		/* Copy result */
	return cp;
}


/* end of libc/stdio/vfprintf.c */
