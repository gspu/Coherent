/*
 * expr.c
 * Nroff/Troff.
 * Expression reader.
 */

#include <ctype.h>
#include "roff.h"

/* Local variables. */
static	int	experr;			/* Got an error */
static	int	expmul;			/* Default unit multiplier */
static	int	expdiv;			/* Default unit divisor */
static	char	*expp;			/* Pointer in expression */

/*
 * Return the int value of an expression.
 * Given:
 *	Name	Description				Used
 *	str	string containing an expression
 *	mul/div	default unit multiplier, divisor	if no unit given
 *	num	initial value				if leading sign
 *	hvf	horizontal/vertical flag		if initial '|'
 *	def	default value				if no expression
 * The expression is evaluated from left to right with no priorites
 * except parentheses.
 */
int
number(str, mul, div, num, hvf, def) char *str; long mul, div; int num, hvf, def;
{
	register int n, c;

	expp = str;
	expmul = mul;
	expdiv = div;
	experr = 0;
	while (isascii(*expp) && isspace(*expp))
		expp++;
	if ((c = *expp) == '\0')
		return def;
	if (index("+-|", *expp))
		++expp;
	n = expseq();
	if (*expp != '\0')
		experr++;
	if (experr) {
		printe("syntax error");
		return 0;
	}
	switch (c) {
	case '+':
		n = num + n;
		break;
	case '-':
		n = num - n;
		break;
	case '|':
		n -= hvf ? cdivp->d_rpos : nlinsiz;
		break;
	}
	return n;
}

/*
 * Like number(), but fewer args, for efficiency.
 */
int
numb(str, mul, div) char *str; long mul, div;
{
	return number(str, mul, div, 0, 0, 0);
}

/*
 * Compute an expression sequence.
 */
expseq()
{
	register int n1, n2, c;

	n1 = expval();
	if (experr)
		return 0;
	for (;;) {
		while ((c = *expp++)==' ' || c == '\t')
			;
		switch (c) {
		case '<':
			if (*expp == '=') {
				expp++;
				c = 'l';
			}
			break;
		case '>':
			if (*expp == '=') {
				expp++;
				c = 'g';
			}
			break;
		case '=':
			if (*expp == '=')
				expp++;
			break;
		default:
			if ((c != 0) && index("+-/*%&:", c))
				break;
			--expp;
			return n1;
		}
		n2 = expval();
		if (experr)
			return 0;
		switch (c) {
		case '+':
			n1 += n2;
			break;
		case '-':
			n1 -= n2;
			break;
		case '*':
			n1 *= n2;
			break;
		case '/':
			if (n2 == 0) {
				printe("attempted zero divide");
				experr++;
				return 0;
			}
			n1 /= n2;
			break;
		case '%':
			if (n2 == 0) {
				printe("attempted zero modulus");
				experr++;
				return 0;
			}
			n1 %= n2;
			break;
		case '<':
			n1 = n1 < n2;
			break;
		case '>':
			n1 = n1 > n2;
			break;
		case 'l':
			n1 = n1 <= n2;
			break;
		case 'g':
			n1 = n1 >= n2;
			break;
		case '=':
			n1 = n1 == n2;
			break;
		case '!':
			n1 = n1 != n2;
			break;
		case '&':
			n1 = n1 && n2;
			break;
		case ':':
			n1 = n1 || n2;
			break;
		}
	}
}

/*
 * Get an operand.
 */
expval()
{
	long mul, div, m, d;
	register int n, c, s;

	while (isascii(c = *expp++) && isspace(c))
		;
	if (c == '(') {
		n = expseq();
		if (*expp++ != ')') {
			--expp;
			experr++;
			n = 0;
		}
		return n;
	}
	for (s = 1; c == '-'; c = *expp++)
		s = -s;
	for (m = 0; isascii(c) && isdigit(c); c = *expp++)
		m = m * 10 + c - '0';
	d = 1;
	if (c == '.') {
		while (isascii(c = *expp++) && isdigit(c)) {
			m = m * 10 + c - '0';
			d *= 10;
		}
	}
	switch (c) {
	case 'i':
		mul = SMINCH;
		div = SDINCH;
		break;
	case 'c':
		mul = SMCENT;
		div = SDCENT;
		break;
	case 'P':
		mul = SMPICA;
		div = SDPICA;
		break;
	case 'm':
		mul = SMEMSP;
		div = SDEMSP;
		break;
	case 'n':
		mul = SMENSP;
		div = SDENSP;
		break;
	case 'p':
		mul = SMPOIN;
		div = SDPOIN;
		break;
	case 'u':
		mul = SMUNIT;
		div = SDUNIT;
		break;
	case 'v':
		mul = SMVLSP;
		div = SDVLSP;
		break;
	default:
		--expp;
		mul = expmul;
		div = expdiv;
	}
	while (isascii(c = *expp) && isalpha(c))
		expp++;
	return unit(s*m*mul, d*div);
}

/*
 * Given a long numerator and denominator, divide the numerator by
 * the denominator and return an int.
 */
int
unit(mul, div) long mul, div;
{
	return ((div == 1) ? ((int) mul) : ((int) (mul/div)));
}

/* end of expr.c */
