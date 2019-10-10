/*
 * libc/stdio/putchar.c
 * ANSI-compliant C standard i/o library.
 * putchar()
 * ANSI 4.9.7.9.
 * Write character c to stdout.
 */

#include <stdio.h>
#undef	putchar

int
putchar(c) int c;
{
	return putc(c, stdout);
}

/* end of libc/stdio/putchar.c */
