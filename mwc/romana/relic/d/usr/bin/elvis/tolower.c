#include <ctype.h>

tolower(c)
register c;
{
	if (isascii(c) && isupper(c))
		return (_tolower(c));
	return (c);
}
