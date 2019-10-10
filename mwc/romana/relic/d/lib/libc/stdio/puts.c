/*
 * C input/output library.
 * puts()
 * ANSI 4.9.7.10.
 * Write string s to stdout.
 * Append newline.
 */

#include <stdio.h>

/* Should be "const char *s;". */
int
puts(s) register char *s;
{
	while (*s)
		if (putchar(*s++) == EOF && ferror(stdout))
			return EOF;
	if (putchar('\n') == EOF)
		return EOF;
	return 1;
}
