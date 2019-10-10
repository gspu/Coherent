/*
 * Coherent Standard I/O Library.
 * Default error routine - not a portable
 * part of stdio.
 */

#include <stdio.h>

error(x)
{
	fprintf(stderr, "%r", &x);
	exit(1);
}
