/*
 * C character handling library.
 * _toupper()
 * Not in ANSI 4.3 but allowed by 4.13.2.
 * Convert character to upper case.
 */

#include <ctype.h>
#undef _toupper

int _toupper(c) int c;
{
	return ((c)&~('a'-'A'));
}
