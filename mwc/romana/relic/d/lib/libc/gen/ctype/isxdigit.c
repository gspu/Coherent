/*
 * C character handling library.
 * isxdigit()
 * ANSI 4.3.1.11.
 * Test if character is hexadecimal digit.
 */

#include <ctype.h>
#undef	isxdigit

int isxdigit(c) int c;
{
	return ((_ctype+1)[c]&_X);
}
