/*
 * libc/stdio/remove.c
 * ANSI-compliant C standard i/o library.
 * ANSI 4.9.4.1.
 * Remove a file.
 */

#include <stdio.h>

int
remove(filename) char *filename;
{
	return unlink(filename);
}

/* end of libc/stdio/remove.c */
