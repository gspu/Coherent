/*
 * This routine allocates temporary
 * locations on the stack, adjusting the stack
 * frame temporaty location if necessary.
 */
#ifdef   vax
#include "INC$LIB:cc1.h"
#else
#include "cc1.h"
#endif

/*
 * Allocate a new temporary and
 * return a pointer to a TREE node that describes
 * it. The type is set from the type of the node that
 * is going to be stored. The "flag" is true if the
 * node needs to be allocated.
 */
TREE *
tempnode(tp, flag)
register TREE	*tp;
{
	register TREE	*tp1;
	register TYPE	ntype;
	register int	nbase;

	if (flag != 0)
		curtemp += pertype[tp->t_type].p_size;
	ntype = SPTR;
	nbase = BP;
#if !ONLYSMALL
	if (notvariant(VSMALL)) {
		ntype = LPTR;
		nbase = SSBP;
	}
#endif
	tp1 = makenode(REG, ntype);
	tp1->t_reg = nbase;
	tp1 = leafnode(tp1);
	tp1 = leftnode(SUB, tp1, ntype, 0);
	tp1->t_rp = ivalnode(curtemp);
	tp1 = leftnode(STAR, tp1, tp->t_type, tp->t_size);
	return (tp1);
}
