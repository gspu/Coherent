#include <scn.h>
#include <ctype.h>
/*
 * Check that field has no whitespace.
 */
noWhite(s)
register unsigned char *s;
{
	unsigned char c;

	while (c = *s++) {
		if (isspace(c)) {
			showError("White space in field use \\s for space");
			return(0);
		}
	}
	return(1);
}
