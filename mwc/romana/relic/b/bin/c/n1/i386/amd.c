/*
 * n1/i386/amd.c
 * Access mode determination.
 * This routine sets the addressing flags for a tree node.
 * There is no provision for double indexed address modes;
 * the compiler does not attempt to use them at this time.
 * i386.
 */

#ifdef	vax
#include "INC$LIB:cc1.h"
#else
#include "cc1.h"
#endif

TREE	*findoffs();

/*
 * Determine access mode and store it into the 't_flag' field.
 * The subtrees have already been marked.
 * This routine is usually called by 'walk'.
 */
amd(tp, ptp) register TREE *tp; TREE *ptp;
{
	register FLAG	flag;
	register int	op;
	register TREE	*xp;
	register FLAG	mask;
	register lval_t	n;
	register int	i;

	if ((op=tp->t_op) == LEAF) {
		/* Leaf nodes inherit flags of subtree. */
		tp->t_flag = tp->t_lp->t_flag;
		return;
	}
	flag = 0;
	if (op==ICON || op==LCON) {		/* integer constants */
		flag |= T_NUM;
		if ((n = grabnval(tp)) >= 0) {
			if (n == 0)
				flag |= T_0|T_SHCNT|T_SBYTE|T_UBYTE|T_SWORD|T_UWORD;
			else if (n == 1)
				flag |= T_1|T_SHCNT|T_SBYTE|T_UBYTE|T_SWORD|T_UWORD;
			else if (n <= 31)
				flag |= T_SHCNT|T_SBYTE|T_UBYTE|T_SWORD|T_UWORD;
			else if (n <= 127)
				flag |= T_SBYTE|T_UBYTE|T_SWORD|T_UWORD;
			else if (n <= 255)
				flag |= T_UBYTE|T_SWORD|T_UWORD;
			else if (n <= 32767L)
				flag |= T_SWORD|T_UWORD;
			else if (n <= 65535L)
				flag |= T_UWORD;
		} else {
			if (n >= -128)
				flag |= T_SBYTE|T_SWORD;
			else if (n >= -32768L)
				flag |= T_SWORD;
		}
	} else if (op == DCON) {		/* double constants */
		flag |= T_DCN;
		for (i = 0; i < sizeof(dval_t) && tp->t_dval[i]==0; i++)
			;
		if (i == sizeof(dval_t))
			flag |= T_0;
	} else if (op == REG)			/* registers */
		flag |= T_REG;
	else if (op == ADDR)			/* address of ... */
		flag |= T_ADDR;
	else if (op==LID || op==GID)		/* identifiers */
		flag |= T_DIR;
	else if ((op == ADD || op == SUB) && ispoint(tp->t_type)) {
		/* Check for effective address, e.g. %ebp+12 or &i+1. */
		mask = (op == ADD) ? T_CON : T_NUM;
		xp = tp;
		do {
			if ((xp->t_rp->t_flag & mask) == 0)
				break;
			xp = xp->t_lp;
		} while (xp->t_op==ADD || xp->t_op==SUB);
		if (isreg(xp))
			flag |= T_LEA;
	} else if (op == STAR) {
		xp = findoffs(tp);
		if ((xp->t_flag & T_CON) != 0 || isreg(xp))
			flag |= T_DIR;
		flag |= T_OFS;
	}
	tp->t_flag = flag;
}

/*
 * Given a pointer to an offset type tree, return a pointer to the tree
 * that must be loaded into a register to get an addressable tree.
 */
TREE *
findoffs(tp) register TREE *tp;
{
	register flag, n;

	while (tp->t_op == LEAF)
		tp = tp->t_lp;
	for (n = 0;;) {
		tp = tp->t_lp;
		switch (tp->t_op) {
		case SUB:		/* can't load neg addr */
			if (tp->t_rp->t_flag & T_NUM)
				break;
			return tp;
		case ADD:		/* add one addr max */
			if ((flag = (tp->t_rp->t_flag)) & T_NUM)
				break;
			else if ((flag & T_CON) && n++ == 0)
				break;
			/* else fall through... */
		default:
			return tp;
		}
	}
}

/*
 * Return true if xp represents a dword register other than ESP.
 */
isreg(xp) register TREE *xp;
{
	register REGNAME r;

	if ((xp->t_flag&T_REG) == 0)
		return 0;
	while (xp->t_op == LEAF)
		xp = xp->t_lp;
	r = xp->t_reg;
	return (r==EAX || r==EBX || r==ECX || r==EDX
	     || r==ESI || r==EDI || r==EBP);
}

/* end of n1/i386/amd.c */
