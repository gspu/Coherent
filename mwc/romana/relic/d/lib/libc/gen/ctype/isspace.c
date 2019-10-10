/*
 * C character handling library.
 * isspace()
 * ANSI 4.3.1.9.
 * Test if character is space.
 */

#include <ctype.h>
#undef	isspace

int isspace(c) int c;
{
	return ((_ctype+1)[c]&_S);
}
