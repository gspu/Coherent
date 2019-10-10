/*
 * C character handling library.
 * isalpha()
 * ANSI 4.3.1.2.
 * Test if character is alphabetic.
 */

#include <ctype.h>
#undef	isalpha

int isalpha(c) int c;
{
	return ((_ctype+1)[c]&_A);
}
