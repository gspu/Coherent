/*
 * n1/i386/fixtop.c
 * Adjust types at the top of a tree node.
 * i386.
 */

#ifdef   vax
#include "INC$LIB:cc1.h"
#else
#include "cc1.h"
#endif

/*
 * Make the top of a tree a computational type.
 */
fixtoptype(tp)
register TREE *tp;
{
	register type;

	type = tp->t_type;
	if (type == S8 || type == S16)
		tp->t_type = S32;
	else if (type == U8 || type == U16)
		tp->t_type = U32;
	else if (type == F32)
		tp->t_type = F64;
}

/* end of n1/i386/fixtop.c */
