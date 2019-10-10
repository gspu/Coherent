/*
 * Given a pointer to an argv entry, parse its contents into
 * this phase's variant data structure.
 */
#ifdef	vax
#include	"INC$LIB:var.h"
#else
#include "var.h"
#endif


getvariant(p)
register char *p;
{
	register int i, j;
	unsigned int u;

	for (i = 0; i < VMAXIM/VGRANU; i += 1) {
		for (j = 4; j >= 0; j -= 4) {
			if ((u = *p++) != 0
			 && ((u -= '0') < 10
			 || (u += '0' - 'A' + 10) < 16))
				variant[i] |= u << j;
			else
				return;
		}
	}
}
