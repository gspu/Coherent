/*
 * C character handling library.
 * isupper()
 * ANSI 4.3.1.10.
 * Test if character is upper case letter.
 */

#include <ctype.h>
#undef	isupper

int isupper(c) int c;
{
	return ((_ctype+1)[c]&_U);
}
