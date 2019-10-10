/*
 * libc/string/strdup.c
 * Not in ANSI draft standard.
 * Return a malloc()'ed copy of a string.
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

char *
strdup(s) register char *s;
{
	register char *cp;

	return ((cp = malloc(strlen(s) + 1)) == NULL) ? cp : strcpy(cp, s);
}

/* end of strdup.c */
