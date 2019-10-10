/*
 * n1/i386/altemp.c
 * Allocate a temporary on the stack,
 * adjusting the stack frame temporary location if necessary.
 */

#ifdef   vax
#include "INC$LIB:cc1.h"
#else
#include "cc1.h"
#endif

/*
 * Allocate a new temporary and return a pointer to a TREE node
 * which describes it: "*(%EBP - n)".
 * The type is set from the type of the node that is going to be stored.
 * The "flag" is true if the node needs to be allocated.
 */
TREE *
tempnode(tp, flag) register TREE *tp; int flag;
{
	register TREE	*tp1;

	if (flag != 0)
		curtemp += mapssize(pertype[tp->t_type].p_size);
	tp1 = makenode(REG, PTR);
	tp1->t_reg = EBP;
	tp1 = leafnode(tp1);
	tp1 = leftnode(SUB, tp1, PTR, 0);
	tp1->t_rp = ivalnode((ival_t)curtemp);
	tp1 = leftnode(STAR, tp1, tp->t_type, tp->t_size);
	return tp1;
}

/* end of n1/i386/altemp.c */
