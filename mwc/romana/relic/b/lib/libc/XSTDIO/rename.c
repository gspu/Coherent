/*
 * libc/stdio/rename.c
 * ANSI-compliant C standard i/o library.
 * ANSI 4.9.4.2.
 * Rename a file.
 * Implementation defined behavior:
 *	rename(old, new) fails and returns -1 if new exists.
 */

#include <stdio.h>

int
rename(old, new) char *old, *new;
{
	return (link(old, new) == -1 || unlink(old) == -1) ? -1 : 0;
}

/* end of libc/stdio/rename.c */
