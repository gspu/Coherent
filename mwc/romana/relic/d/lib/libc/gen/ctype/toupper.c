/*
 * C character handling library.
 * toupper()
 * ANSI 4.3.2.2.
 * Convert character to upper case.
 */

#include <ctype.h>

int toupper(c) int c;
{
	return (islower(c) ? _toupper(c) : c);
}
