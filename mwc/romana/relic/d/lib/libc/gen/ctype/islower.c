/*
 * C character handling library.
 * islower()
 * ANSI 4.3.1.6.
 * Test if character is lower case letter.
 */

#include <ctype.h>
#undef	islower

int islower(c) int c;
{
	return ((_ctype+1)[c]&_L);
}
