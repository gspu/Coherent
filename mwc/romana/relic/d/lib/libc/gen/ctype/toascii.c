/*
 * C character handling library.
 * toascii()
 * Not in ANSI 4.3 but allowed by 4.13.2.
 * Convert character to ASCII.
 */

#include <ctype.h>
#undef toascii

int toascii(c) int c;
{
	return (c&0x7F);
}
