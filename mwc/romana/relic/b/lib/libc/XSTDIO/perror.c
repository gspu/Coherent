/*
 * libc/stdio/perror.c
 * ANSI-compliant C standard i/o library.
 * perror()
 * ANSI 4.9.10.4.
 * Write an error message to stderr.
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>

void
perror(s) const char *s;
{
	if (s != NULL && *s != '\0')
		fprintf(stderr, "%s: ", s);
	fprintf(stderr, "%s\n", strerror(errno));
}

/* end of libc/stdio/perror.c */
