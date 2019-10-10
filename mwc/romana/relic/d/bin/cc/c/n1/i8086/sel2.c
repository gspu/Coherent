/*
 * C compiler.
 * Pattern selection machine dependencies.
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
		return(sgoal);
	} else if (c == MLVALUE)
		return(MLVALUE);
	else
		return(MRVALUE);
}
