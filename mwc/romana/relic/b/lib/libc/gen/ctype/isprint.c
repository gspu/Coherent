/*
 * C character handling library.
 * isprint()
 * ANSI 4.3.1.7.
 * Test if character is printable.
 */

#include <ctype.h>
#undef	isprint

int isprint(c) int c;
{
	return ((_ctype+1)[c]&(_P|_B|_A|_N));
}
