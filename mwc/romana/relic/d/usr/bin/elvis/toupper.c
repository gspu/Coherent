#include <ctype.h>

toupper(c)
register c;
{
	return ((isascii(c) && islower(c)) ? _toupper(c) : c);
}
