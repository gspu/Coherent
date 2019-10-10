/*
 * C character handling library.
 * iscntrl()
 * ANSI 4.3.1.3.
 * Test if character is control character.
 */

#include <ctype.h>
#undef	iscntrl

int iscntrl(c) int c;
{
	return ((_ctype+1)[c]&_C);
}
