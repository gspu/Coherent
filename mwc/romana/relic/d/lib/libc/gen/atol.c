/*
 * C general utilities library.
 * atol()
 * ANSI 4.10.1.3.
 * Convert ASCII to long (the old fashioned way).
 */

#include <stdlib.h>
#include <ctype.h>

long
atol(nptr) register char *nptr;
{
	register long	val;
	register int	c;
	register int	sign;

	val = sign = 0;

	/* Leading whitespace. */
	while (isspace(c = *nptr++))
		;

	/* Optional sign. */
	if (c == '-') {
		sign = 1;
		c = *nptr++;
	} else if (c == '+')
		c = *nptr++;

	/* Process digit string. */
	for ( ; isdigit(c); c = *nptr++)
		val = val * 10 + c - '0';
	return (sign ? -val : val);
}

/* end of atol.c */
