/*
 * Given a pointer to an argv entry, parse the variants into
 * the array of characters.
 */
#ifdef	vax
#include	"INC$LIB:var.h"
#else
#include "var.h"
#endif


makvariant(p)
register char *p;
{
	register int i, j;
	unsigned int u;
	static char digit[] = "0123456789ABCDEF";

	for (i = 0; i < VMAXIM/VGRANU; i += 1) {
		for (j = 4; j >= 0; j -= 4) {
			u = (variant[i] >> j) & 0xF;
			*p++ = digit[u];
		}
	}
	*p = 0;
}
