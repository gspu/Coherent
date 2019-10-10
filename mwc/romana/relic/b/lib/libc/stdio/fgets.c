/*
 * libc/stdio/fgets.c
 * ANSI-compliant C standard i/o library.
 * fgets()
 * ANSI 4.9.10.2.
 * Get string from stream.
 * Read up to n-1 characters, including newline.
 */

#include <stdio.h>

char *
fgets(s, n, stream) char *s; register int n; FILE *stream;
{
	register c;
	register char *cp;

	cp = s;
	while (--n > 0 && (c = getc(stream)) != EOF)
		if ((*cp++ = c) == '\n')
			break;
	if (c == EOF && (cp == s || ferror(stream)))
		return NULL;		/* ANSI says leave *cp unchanged */
	*cp = '\0';			/* else NUL-terminate */
	return s;
}

/* end of libc/stdio/fgets.c */
