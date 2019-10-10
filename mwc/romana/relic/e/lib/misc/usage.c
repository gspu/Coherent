/*
 * Put message and die.
 */
#include <stdio.h>
usage(s)
char *s;
{
	fprintf(stderr, "usage: %r\n", &s);
	exit(1);
}
