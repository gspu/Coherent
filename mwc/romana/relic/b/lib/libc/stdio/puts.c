/*
 * libc/stdio/puts.c
 * ANSI-compliant C standard i/o library.
 * puts()
 * ANSI 4.9.7.10.
 * Write string s to stdout.
 * Append newline.
 */

#include <stdio.h>

int
puts(s) const char *s;
{
	while (*s)
		if (putchar(*s++) == EOF && ferror(stdout))
			return EOF;
	if (putchar('\n') == EOF)
		return EOF;
	return 1;
}

/* end of libc/stdio/puts.c */
