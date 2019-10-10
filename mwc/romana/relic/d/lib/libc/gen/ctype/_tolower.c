/*
 * C character handling library.
 * _tolower()
 * Not in ANSI 4.3 but allowed by 4.13.2.
 * Convert character to lower case.
 */

#include <ctype.h>
#undef _tolower

int _tolower(c) int c;
{
	return ((c)|('a'-'A'));
}
