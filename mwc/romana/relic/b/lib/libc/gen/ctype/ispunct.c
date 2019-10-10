/*
 * C character handling library.
 * ispunct()
 * ANSI 4.3.1.8.
 * Test if character is punctuation.
 */

#include <ctype.h>
#undef	ispunct

int ispunct(c) int c;
{
	return ((_ctype+1)[c]&_P);
}
