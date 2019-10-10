/*
 * libc/stdio/getchar.c
 * ANSI-compliant C standard i/o library.
 * getchar()
 * ANSI 4.9.7.6.
 * Get character from stdin.
 */

#include <stdio.h>
#undef	getchar

int
getchar()
{
	return getc(stdin);
}

/* end of libc/stdio/getchar.c */
