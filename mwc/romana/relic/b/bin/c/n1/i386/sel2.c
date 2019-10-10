/*
 * n1/i386/sel2.c
 * Pattern selection machine dependencies.
 * i386.
 */

#ifdef   vax
#include "INC$LIB:cc1.h"
#else
#include "cc1.h"
#endif

subgoal(c, stemp)
{
	register int sgoal;

	if (isrealreg(stemp)) {
		sgoal = reg[stemp].r_goal;
		if (sgoal < 0)
			cbotch("goal");
		return sgoal;
	} else if (c == MLVALUE)
		return MLVALUE;
	else
		return MRVALUE;
}

/* end of n1/i386/sel2.c */
