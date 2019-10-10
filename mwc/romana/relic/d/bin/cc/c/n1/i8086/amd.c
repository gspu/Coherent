/*
 * This routine sets the addressing flags for a tree node.
 * This version is for both the SMALL and LARGE model iAPX-86.
 * There is no provision for double indexed address modes;
 * the compiler does not attempt to use them at this time.
 */
#ifdef   vax
#include "INC$LIB:cc1.h"
#else
#include "cc1.h"
#endif

TREE	*findoffs();

/*
 * Determine access mode.
 * The mode set is stored into the 't_flag' field.
 * The subtrees have already been marked.
 * Knows how the output writer will map
 * the logical segments into physical segments.
 * This routine is usually called by 'walk'.
 */
amd(tp, ptp)
register TREE	*tp;
TREE		*ptp;
{
	register TREE	*xp;
	register ival_t	half;
	register int	op;
	register REGNAME r;
	register FLAG	flag;
	register FLAG	mask;
	register KIND	kind;
	register TYPE	t;
	register lval_t	n;
	register int	i;
	register int	seg;

	if ((op=tp->t_op) == LEAF) {
		tp->t_flag = tp->t_lp->t_flag;
		return;
	}
	flag = 0;
	if (op==ICON || op==LCON) {
		if ((n=grabnval(tp)) == 0)
			flag |= T_0;
		else if (n == 1)
			flag |= T_1;
		else if (n == 2)
			flag |= T_2;
		if (n>=-128 && n<=127)
			flag |= T_BYTE;
		if (op == LCON) {
			flag |= T_LCN;
			half = lower(n);
			if (half == 0)
				flag |= T_LHC;
			if (half == -1)
				flag |= T_LHS;
			half = upper(n);
			if (half == 0)
				flag |= T_UHC;
			if (half == -1)
				flag |= T_UHS;
		} else
			flag |= T_ICN;
	}
	if (op == DCON) {
		flag |= T_DCN;
		i = 0;
		while (i<sizeof(dval_t) && tp->t_dval[i]==0)
			++i;
		if (i == sizeof(dval_t))
			flag |= T_0;
	}
	if (op == REG) {
		flag |= T_DIR;
		kind = pertype[tp->t_type].p_kind;
		if ((reg[tp->t_reg].r_lvalue&kind) != 0)
			flag |= T_LREG;
		if ((reg[tp->t_reg].r_rvalue&kind) != 0)
			flag |= T_RREG;
#if !ONLYSMALL
		if (tp->t_reg==SSSP || tp->t_reg==SSBP)
			flag |= T_SREG;
#endif
	} else if (op == ADDR) {
		if (isvariant(VSMALL))
			flag |= T_ADS;
		else {
			xp = tp->t_lp;
			if (xp->t_op==LID || xp->t_op==GID) {
				switch (xp->t_seg) {
				case SCODE:
				case SLINK:
					flag |= T_ACS;
					break;
				case SPURE:
					if (isvariant(VRAM))
						flag |= T_ADS;
					else
						flag |= T_ACS;
					break;
				case SSTRN:
					if (notvariant(VROM))
						flag |= T_ADS;
					else
						flag |= T_ACS;
					break;
				case SDATA:
				case SBSS:
					flag |= T_ADS;
					break;
				}
			} 	
		}
	} else if (op==LID || op==GID) {
#if ONLYSMALL
		flag |= T_DIR;
#else
		seg = tp->t_seg;
		if (isvariant(VSMALL)
		|| (ptp!=NULL && ptp->t_op==CALL && tp==ptp->t_lp)
		|| seg==SCODE
		|| seg==SLINK
		|| (seg==SPURE && notvariant(VRAM)))
			flag |= T_DIR;
#endif
	} else if ((op==ADD || op==SUB) && ispoint(t=tp->t_type)) {
		mask = T_CON;
		if (op == SUB)
			mask = T_NUM;
		xp = tp;
		do {
			if ((xp->t_rp->t_flag&mask) == 0)
				break;
			xp = xp->t_lp;
		} while (xp->t_op==ADD || xp->t_op==SUB);
		if ((xp->t_flag&T_REG) != 0) {
			while (xp->t_op == LEAF)
				xp = xp->t_lp;
			r = xp->t_reg;
#if !ONLYSMALL
			if (notvariant(VSMALL)) {
				if (r == SSBP)
					flag |= T_LSS;
			} else {
#endif
				if (r==SI || r==DI || r==BX || r==BP)
					flag |= T_LEA;
#if !ONLYSMALL
			}
#endif
		}
	} else if (op == STAR) {
		xp = findoffs(tp);
		if ((xp->t_flag&T_CON) != 0 && notvariant(VLARGE))
			flag |= T_DIR;
		else if ((xp->t_flag&T_REG) != 0) {
			while (xp->t_op == LEAF)
				xp = xp->t_lp;
			r = xp->t_reg;
#if !ONLYSMALL
			if (notvariant(VSMALL)) {
				if (r==SSBP
				||  r==ESSI || r==ESDI || r==ESBX
				||  r==DSSI || r==DSDI || r==DSBX)
					flag |= T_DIR;
			} else {
#endif
				if (r==BP
				||  r==SI || r==DI || r==BX)
					flag |= T_DIR;
#if !ONLYSMALL
			}
#endif
		}
		flag |= T_OFS;
	}
	tp->t_flag = flag;
}

/*
 * Given a pointer to an offset type
 * tree, run down the chain of indirections
 * and additions, looking for the tree that
 * must be loaded into a register to get
 * an addressable tree. Return a pointer to
 * this tree.
 */
TREE *
findoffs(tp)
register TREE *tp;
{
	register flag, ct=0, op;

	while (tp->t_op == LEAF)
		tp = tp->t_lp;
	for (;;) {
		tp = tp->t_lp;
		switch (op=tp->t_op) {
			case SUB:	/* can't load neg addr */
			    if(tp->t_rp->t_flag&T_NUM)
				break;
			    return tp;
			case ADD: /* add one addr max */
			    if((flag=(tp->t_rp->t_flag))&T_NUM)
				break;
			    else
				if(flag&T_CON&&0==ct++)
				    break;
			default:
			    return tp;
		}
	}
}
