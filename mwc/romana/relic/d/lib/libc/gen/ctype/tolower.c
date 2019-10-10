/*
 * C character handling library.
 * tolower()
 * ANSI 4.3.2.1.
 * Convert character to lower case.
 */

#include <ctype.h>

int tolower(c) int c;
{
	return (isupper(c) ? _tolower(c) : c);
}
