/*
 * C general utilities library.
 * strtol(), strtoul()
 * ANSI 4.10.1.5, 4.10.1.6.
 * Convert ASCII to long or unsigned long (the System V way).
 * atol() was so simple before System V/ANSI got hold of it...
 */

#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

extern unsigned long _strtoul();

long
strtol(nptr, endptr, base) char *nptr; char **endptr; int base;
{
	return (long)_strtoul(nptr, endptr, base, 0);
}

unsigned long
strtoul(nptr, endptr, base) char *nptr; char **endptr; int base;
{
	return _strtoul(nptr, endptr, base, 1);
}

static
unsigned long
_strtoul(nptr, endptr, base, uflag) char *nptr; char **endptr; int base, uflag;
{
	register char *cp;
	register unsigned long val;
	register int c, sign, overflow, pflag, dlimit, ulimit, llimit;
	unsigned long quot, rem;

	cp = nptr;
	val = pflag = overflow = sign = 0;

	/* Leading white space. */
	while (isspace(c = *cp++))
		;

	/* Optional sign. */
	switch(c) {
	case '-':
		sign = 1;
		/* fall through... */
	case '+':
		c = *cp++;
	}

	/* Determine implicit base. */
	if (base == 0) {
		if (c == '0')
			base = (*cp == 'x' || *cp == 'X') ? 16 : 8;
		else if (isdigit(c))
			base = 10;
		else {			/* expected form not found */
			cp = nptr;
			goto done;
		}
	}

	/* Skip optional hex base "0x" or "0X". */
	if (base == 16 && c == '0' && (*cp == 'x' || *cp == 'X')) {
		++pflag;
		++cp;
		c = *cp++;
	}

	/* Initialize legal character limits. */
	dlimit = '0' + base;
	ulimit = 'A' + base - 10;
	llimit = 'a' + base - 10;

	/* The next character must be a legitimate digit; e.g. " +@" fails. */
	/* Watch out for e.g. "0xy", which is "0" followed by "xy". */
	if (!(   (isdigit(c) && c < dlimit)
	      || (isupper(c) && c < ulimit)
	      || (islower(c) && c < llimit))) {
		cp = (pflag) ? cp - 2 : nptr;
		goto done;
	}

	/* Determine limits for overflow computation. */
	/* This would use ldiv() if it worked for unsigned long. */
	if (uflag) {
		quot = ULONG_MAX / base;
		rem = ULONG_MAX % base;
	} else {
		quot = LONG_MAX / base;
		rem = LONG_MAX % base;
	}

	/* Process digit string. */
	for ( ; ; c = *cp++) {
		if (isdigit(c) && c < dlimit)
			c -= '0';
		else if (isupper(c) && c < ulimit)
			c -= 'A'-10;
		else if (islower(c) && c < llimit)
			c -= 'a'-10;
		else {
			--cp;
			break;
		}
		if (val < quot || (val == quot && c <= rem))
			val = val * base + c;
		else
			++overflow;
	}

done:
	/* Store end pointer and return approriate result. */
	if (endptr != (char **)NULL)
		*endptr = cp;
	if (overflow) {
		errno = ERANGE;
		if (uflag)
			return ULONG_MAX;
		return (unsigned long)(sign ? LONG_MIN : LONG_MAX);
	}
	return (sign) ? (unsigned long)(-(long)val) : val;
}

/* end of strtol.c */
