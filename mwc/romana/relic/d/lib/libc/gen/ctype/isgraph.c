/*
 * C character handling library.
 * isgraph()
 * ANSI 4.3.1.5.
 * Test if character is printing nonspace.
 */

#include <ctype.h>
#undef	isgraph

int isgraph(c) int c;
{
	return ((_ctype+1)[c]&(_P|_A|_N));
}
