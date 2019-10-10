/*
 * C compiler.
 * Intel 8086/8088.
 * Adjust types at the top of a tree node.
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
	if (type == S8)
		tp->t_type = S16;
	else if (type == U8)
		tp->t_type = U16;
	else if (type == F32)
		tp->t_type = F64;
}
