/*
 * C character handling library.
 * isdigit()
 * ANSI 4.3.1.4.
 * Test if character is digit.
 */

#include <ctype.h>
#undef	isdigit

int isdigit(c) int c;
{
	return ((_ctype+1)[c]&_N);
}
