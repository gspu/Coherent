/*
 * n1/i386/mtree4.c
 * Machine specific tree modification functions.
 * i386.
 */

#ifdef   vax
#include "INC$LIB:cc1.h"
#else
#include "cc1.h"
#endif

/*
 * This routine figures out the left subgoal context.
 * It watches for 'test' instructions on the 8086.
 */
getldown(tp, c)
TREE *tp;
{
	return (c==MFLOW && (tp->t_rp->t_flag&T_IMM)!=0) ? MRADDR : MRVALUE;
}

/*
 * This routine figures out the right hand subgoal contexts
 * for the shift instruction.
 */
getrdown(tp, c)
TREE *tp;
{
	register int op;

	return ((op=tp->t_rp->t_op)==ICON || op==LCON) ? MRADDR : MRVALUE;
}

/*
 * Fix up the type of a reordered ADD node.
 * Watch for pointers.
 */
fixaddtype(tp)
TREE *tp;
{
	register tt;
	register lt, rt;

	lt = tp->t_lp->t_type;
	rt = tp->t_rp->t_type;
	if (ispoint(lt))
		tt = lt;
	else if (ispoint(rt))
		tt = rt;
	else if (islong(lt))
		tt = lt;
	else if (islong(rt))
		tt = rt;
	else
		tt = lt;
	tp->t_type = tt;
}

/*
 * Zap the type field of tree node 'tp'
 * into the type used for offset addressing trees.
 */
setofstype(tp)
TREE *tp;
{
	tp->t_type = OFFS;
}

/* end of n1/i386/mtree4.c */
