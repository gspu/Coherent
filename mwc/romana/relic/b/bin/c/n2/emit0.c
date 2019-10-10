/*
 * This file contains one machine independent code generation routine.
 */

#ifdef   vax
#include "INC$LIB:cc2.h"
#else
#include "cc2.h"
#endif

/*
 * Generate a segment switch.
 * Reset the values in 'dot' and 'dotseg' to correspond to the new segment.
 */
genseg(sn)
{
	register char	*s;

	if (dotseg >= 0)
		seg[dotseg].s_dot = dot;
	if (dotseg != sn) {
		dotseg = sn;
		if (dotseg >= 0) {
			dot = seg[dotseg].s_dot;
			if (isvariant(VASM)) {
				bput(ENTER);
				bput(dotseg);
			} else
				outseg(dotseg);
		}
	}
}
