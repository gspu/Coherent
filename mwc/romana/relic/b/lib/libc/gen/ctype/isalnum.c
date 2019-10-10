/*
 * C character handling library.
 * isalnum()
 * ANSI 4.3.1.1.
 * Test if character is alphanumeric.
 */

#include <ctype.h>
#undef	isalnum

int isalnum(c) int c;
{
	return ((_ctype+1)[c]&(_A|_N));
}
