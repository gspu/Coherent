/*
 * libc/stdio/gets.c
 * ANSI-compliant C standard i/o library.
 * gets()
 * ANSI 4.9.7.7.
 * Read string from stdin.
 * Do not retain newline.
 */

#include <stdio.h>

char *
gets(s) char *s;
{
	register c;
	register char *cp;

	cp = s;
	while ((c = getchar()) != EOF && c != '\n')
		*cp++ = c;
	if (c == EOF && (s == cp || ferror(stdin)))
		return NULL;		/* ANSI says leave *s unchanged */
	*cp = '\0';			/* else NUL-terminate */
	return s;
}

/* end of libc/stdio/gets.c */
