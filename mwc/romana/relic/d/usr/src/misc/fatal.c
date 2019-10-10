/*
 * Put message and die.
 */
#include <stdio.h>
fatal(s)
char *s;
{
	fprintf(stderr, "\nfatal: %r\n", &s);
	exit(1);
}
