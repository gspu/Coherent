/*
 * C general utilities library.
 * atoi()
 * ANSI 4.10.1.2.
 * Convert ASCII to int (the old fashioned way).
 */

#include <stdlib.h>
#include <ctype.h>

int
atoi(nptr) register char *nptr;
{
	register int	val;
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

/* end of atoi.c */
