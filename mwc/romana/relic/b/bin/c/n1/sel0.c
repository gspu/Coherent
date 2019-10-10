/*
 * C compiler.
 * Pattern selection.
 */
#ifdef   vax
#include "INC$LIB:cc1.h"
#else
#include "cc1.h"
#endif

#if !TINY
#define consnap(x, p)		if (sflag>x) snapf(p)
#define consnapv(x, p, v)	if (sflag>x) snapf(p, v)
#else
#define consnap(x, p)		/* consnap */
#define consnapv(x, p, v)	/* consnapv */
#endif

/*
 * Code selection.
 * This routine is a near duplicate of output().
 * This routine must succeed or a nomatch fatal error will result.
 */
select(tp, c, r)
register TREE *tp;
{
	register TREE *ap;
	register op;
	register TREE *xp;
	int flag;
	int rel;
	int r1;

#if !TINY
	if (sflag > 1) {
		snapf("select(%P, %C, ", tp, c);
		snapf(c<MFLOW ? "%R" : c<MEQ ? "%d" : "L%d", r);
		snapf(") %A\n", tp->t_op);
	}
#endif
	/* Process a few operators by hand */
	op = tp->t_op;
	if (op == COMMA) {
		if (code1(&tp->t_lp, MEFFECT, ANYR) == 0
		 || code1(&tp->t_rp, c, r) == 0)
			return (0);
		tp->t_used = tp->t_lp->t_used | tp->t_rp->t_used;
		tp->t_treg = tp->t_rp->t_treg;
		tp->t_rreg = tp->t_rp->t_rreg;
		return (1);
	}
	if (op == QUEST) {
		if (code1(&tp->t_lp, MFLOW, 0) == 0)
			return (0);
		ap = tp->t_rp;
		if (code1(&ap->t_lp, c, r) == 0)
			return (0);
		ap->t_rreg = NONE;
		if (c==MLVALUE || c==MRVALUE) {
			r = ap->t_lp->t_rreg;
			ap->t_rreg = r;
		}
		if (code1(&ap->t_rp, c, r) == 0)
			return (0);
		ap->t_treg = NONE;
		ap->t_used = ap->t_lp->t_used | ap->t_rp->t_used;
		tp->t_rreg = ap->t_rreg;
		tp->t_treg = NONE;
		tp->t_used = tp->t_lp->t_used | ap->t_used;
		return (1);
	}
	/* Conditionals may be unravelled to generate simpler code.
	 * 'r' is true or false depending on which condition should goto 'l'. */
	if (c == MFLOW) {
		if (op == NOT) {
			if (code1(&tp->t_lp, c, !r) == 0)
				return(0);
			tp->t_used = tp->t_lp->t_used;
			return(1);
		}
		if (op == LEAF) {
			ap = tp->t_lp;
			if (ap->t_op==ICON || ap->t_op==LCON)
				return (1);
		}
		if (op==ANDAND || op==OROR) {
			r1 = r;
			if ((op==OROR) != r)
				r1 = !r1;
			if (code1(&tp->t_lp, MFLOW, r1) == 0
			 || code1(&tp->t_rp, MFLOW, r) == 0)
				return (0);
			tp->t_used = tp->t_lp->t_used | tp->t_rp->t_used;
			tp->t_rreg = NONE;
			tp->t_treg = NONE;
			return (1);
		}
		rel = NE;
		flag = 0;
		if (isrelop(op)) {
			rel = op;
			if (isnval(tp->t_rp, 0) && tp->t_lp->t_op != QUEST) {
				if (rel == ULE)
					rel = EQ;
				else if (rel == UGT)
					rel = NE;
				ap = tp->t_lp;
				flag = 1;
				while ((op = ap->t_op) == COMMA)
					ap = ap->t_rp;
				if (isrelop(op) || isflow(op)) {
					switch (rel) {
					case EQ:
					case LE:
					case ULE:
						r = !r;
					case NE:
					case GT:
					case UGT:
						if (code1(&tp->t_lp, c, r) == 0)
							return(0);
						tp->t_used = tp->t_lp->t_used;
						return(1);
					}
				}
			}
		}
		if (r == 0)
			rel = otherel[rel-EQ];
		c = MEQ + rel - EQ;
		r = ANYR;
		if (flag) {
			if (code1(&tp->t_lp, c, r) == 0)
				return (0);
			tp->t_used = tp->t_lp->t_used;
			return (1);
		}
	}
	if (op == CALL) {
#if 0
		if (c != pertype[tp->t_type].p_frcxt
		 && (c==MRVALUE || c==MLVALUE))
			return (0);
#endif
		tp->t_used = FNUSED;
		if (tp->t_rp != NULL) {
			if (selargs(&tp->t_rp) == 0)
				return (0);
			tp->t_used |= tp->t_rp->t_used;
		}
		ap = tp->t_lp;
		if (!isadr(ap->t_flag)) {
#if ICALLS
		/* Does the machine allow
		 * an extra level of indirection
		 * on calls? */
			if (ap->t_op != STAR)
				cbotch("call no star");
			xp = ap->t_lp;
			if (!isadr(xp->t_flag)) {
				if (!isofs(xp->t_flag))
					store(ap, 0);
				else {
					xp = findoffs(xp);
					if (select(xp, MLVALUE, ANYL) == 0)
						return (0);
					tp->t_used |= xp->t_used;
				}
			}
#else
			if (!isofs(ap->t_flag))
				cbotch("call not ofs");
			ap = findoffs(ap);
			if (select(ap, MLVALUE, ANYL) == 0)
				return (0);
			tp->t_used |= ap->t_used;
#endif
		}
		tp->t_treg = NONE;
		tp->t_rreg = pertype[tp->t_type].p_frreg;
		if ((c!=MLVALUE&&c!=MRVALUE)||(isrealreg(r)&&r!=tp->t_rreg))
			selfix(tp, c, r);
		return (1);
	}
	/* Everything not previously dispatched by hand
	 * must be found in the code tables by seltree */
	if (seltree(tp, c, r)) {
		if ((c==MLVALUE||c==MRVALUE) && isrealreg(r) && r!=tp->t_rreg)
			selfix(tp, c, r);
		return (1);
	}
	/* RVALUE patterns can be fixed up into anything */
	if (c != MRVALUE && seltree(tp, MRVALUE, ANYR)) {
		selfix(tp, c, r);
		return (1);
	}
#if 0
	/* The m68000 needed this, maybe it won't next time */
	if (c != MLVALUE && seltree(tp, MLVALUE, ANYL)) {
		if (c != MEFFECT)
			selfix(tp, c, r);
		return (1);
	}
#endif
	/* FAILURE is a fatal error */
	return (0);
}

/*
 * If the selector cannot find a template that will satisfy the context or the
 * requested register, an MRVALUE case will do if the result is 'fixed up'.
 * A FIXUP tree node tells the output part of the coder that a fixup is
 * needed at this point in the tree.
 * Context mismatch FIXUP's are generally benign:
 * they either push a register onto the stack as a function argument,
 * or test a register and generate a conditional branch.
 * Register mismatch FIXUP's appear to be caused
 * by table errors or overzealous node coercion
 * and should to be avoided at all costs;
 * they often lead to shuffling values back and forth between registers.
 * The typical culprit is a code table pattern
 * which specifies both a node register and an ADR subnode
 * rather than a TREG subnode shared with the node register.
 * seltree() coerces the ADR node into the wrong register
 * and the fixup copies it to the requested reg.
 */
selfix(tp, c, r)
register TREE *tp;
{
	register TREE *ap;
	TREE rnode;

#if !TINY
	if (sflag > 1) {
		snapf("selfix(%P, %C, ", tp, c);
		snapf(c<MFLOW ? "%R" : c<MEQ ? "%d" : "L%d", r);
		snapf(") %A\n", tp->t_op);
	}
#endif
	ap = copynode(tp);
	fixtoptype(tp);
	rnode.t_op   = REG;
	rnode.t_type = tp->t_type;
	rnode.t_size = tp->t_size;
	rnode.t_reg  = tp->t_rreg;
	tp->t_op = FIXUP;
	tp->t_lp = &rnode;
	tp->t_rp = NULL;
	walk(tp, amd);
#if !TINY
	rnode.t_treg = NONE;
	rnode.t_rreg = NONE;
	rnode.t_used = 0;
	rnode.t_patp = NULL;
	if (sflag > 2) snapf("%W%E%W", "Selfix", tp, NULL);
#endif
	if (select(tp, c, r) == 0)
		cbotch("node will not fix");
	tp->t_lp = ap;
	tp->t_used = ap->t_used;
	if (isrealreg(r))
		setused(tp, r);
}

/*
 * Add register 'r' to the used set of the tree node pointed to by 'tp'.
 */
setused(tp, r)
TREE *tp;
{
	tp->t_used |= reg[r].r_phys;
}

/*
 * Run down the (possibly null)
 * argument list of a function, making
 * the required recursive calls to the
 * walker and/or the code selector.
 * If the node is a BLK pointer,
 * it the address of a structure argument and
 * must get special treatment.
 */
selargs(tpp)
register TREE **tpp;
{
	register TREE *tp;

	if ((tp = *tpp) == NULL)
		return (1);
	if (tp->t_op == ARGLST) {
		if (selargs(&tp->t_lp)==0)
			return (0);
		if (selargs(&tp->t_rp)==0)
			return (0);
		tp->t_used = 0;
		if (tp->t_lp != NULL)
			tp->t_used |= tp->t_lp->t_used;
		if (tp->t_rp != NULL)
			tp->t_used |= tp->t_rp->t_used;
		return (1);
	}
	if (isblkp(tp->t_type))
		return (code1(tpp, MBLARG, MBLREG));
	return (code1(tpp, MFNARG, ANYR));
}

/*
 * Set up an address in an rvalue context.
 * This routine knows how to pool
 * constants on demand, how to use registers
 * as indexing temporaries and as plain old
 * value temporaries, and how to give up and
 * put a value into memory.
 * If the top operator is one of the leaf
 * nodes the register used in the setup of an
 * offset address is not made busy.
 * It is marked used by the tree.
 * These trees are more like a TREG than a RHS
 * in their use of a register.
 */
selrv(ttp, flag, didstorep)
TREE *ttp;
int *didstorep;
{
	register TREE *tp, *ap;
	register op, leaf;
	int lab, r;

	leaf = 1;
	tp = ttp;
	op = tp->t_op;
	if (op==CONVERT || op==CAST || op==FIXUP || op==LEAF)
		leaf = 2;
#if !TINY
	if (sflag > 1)
		snapf("selrv(%P, %d, ...) leaf=%d\n", tp, flag, leaf);
#endif
	tp = flag ? tp->t_rp : tp->t_lp;
	/* Pool constants into memory */
	if ((tp->t_flag&T_IMM) != 0) {
		for (ap=tp; ap->t_op==LEAF; ap=ap->t_lp)
			;
		pool(ap);
		walk(tp, amd);
		return;
	}
	/* The tree is offset addressible,
	 * load the offset node into an index reg */
	if (isofs(tp->t_flag)) {
		ap = findoffs(tp);
		consnapv(2, "Rv offs %P\n", ap);
		if ((r = rallo(ap, ttp->t_used, leaf)) >= 0) {
			consnapv(2, "Offs %R\n", r);
			if (leaf == 1)
				setbusy(r);
			if (select(ap, MLVALUE, r)) {
				ttp->t_used |= ap->t_used;
				return;
			}
			if (leaf == 1)
				clrbusy(r);
		}
	}
	/* Try to load the tree into a register */
	if ((r = rallo(tp, ttp->t_used, 0)) >= 0) {
		setbusy(r);
		if (select(tp, MRVALUE, r)) {
			tp->t_flag   = 0;
			ttp->t_used |= tp->t_used;
			return;
		}
		clrbusy(r);
	}
	/* Store the tree into a stack temporary */
	store(ttp, flag);
	walk(tp, amd);
	*didstorep = 1;
}

/*
 * This function sets up a left context subtree in an addressing context.
 * The 'index' argument is a preferred register; if negative,
 * then indirection is legal.
 * This routine is the pits, comparable
 * in pittiness to the part of output()
 * responsible for undoing this madness.
 */
sellv(ttp, index, flag, didstorep)
TREE *ttp;
int *didstorep;
{
	register TREE *tp, *tp1, *tp2;
	int newindex;

	tp = flag ? ttp->t_rp : ttp->t_lp;
#if !TINY
	if (sflag > 1)
		snapf("sellv(%P, %R, %d, ...)\n", tp, index, flag);
#endif
	if (tp->t_op == FIELD) {
		tp->t_used = ttp->t_used;
		sellv(tp, index, 0, didstorep);
		ttp->t_used |= tp->t_used;
		return;
	}
	if (!isofs(tp->t_flag))
		cbotch("sellv: not an ofs");
	/* If we can indirect through a stack temporary,
	 * we still try first to do it in a register,
	 * but we give up with much less grief. */
	if (index < 0) {
		tp1 = findoffs(tp);
		consnapv(2, "Lv offs %P\n", tp1);
		if ((index = rallo(tp1, ttp->t_used, 1)) >= 0) {
			consnapv(2, "Index %R\n", index);
			setbusy(index);
			if (select(tp1, MLVALUE, index)) {
				tp->t_used  |= tp1->t_used;
				ttp->t_used |= tp1->t_used;
				return;
			}
			clrbusy(index);
		}
		/* The store puts a stack temp under tp
		 * which must have tp->t_op == STAR */
		store(tp, 0);
		walk(tp, amd);
		*didstorep = 1;
		return;
	}
	/* We cannot indirect through a temp,
	 * so we may have to fake it. */
	tp2 = findoffs(tp);
	consnapv(2, "Offs %P\n", tp2);
	for (tp1 = tp; tp1->t_lp != tp2; tp1 = tp1->t_lp);
	/* If the offset is not directly addressible
	 * then we will have to generate code to load
	 * it into the index register. */
	if (!isadr(tp2->t_flag)) {
		/* If the preferred index is useable
		 * then attempt to use it */
		if ((ttp->t_used&reg[index].r_phys) == 0) {
			if (select(tp2, MLVALUE, index)) {
				tp->t_used  |= tp2->t_used;
				ttp->t_used |= tp2->t_used;
				return;
			}
		}
		/* Since the preferred index is already used,
		 * try to use another one. */
		if ((newindex = rallo(tp2, ttp->t_used, 1)) >= 0) {
			setbusy(newindex);
			if (select(tp2, MLVALUE, newindex)) {
				ttp->t_used |= tp2->t_used;
				tp->t_used  |= tp2->t_used;
				return;
			}
			clrbusy(newindex);
		}
		/* We can't do it ourselves, but an index has been
		 * reserved to load from a stack temp, so give up
		 * and store the offset. */
		store(tp1, 0);
		walk(tp, amd);
		*didstorep = 1;
	}
	/* tp2 either was addressible to begin with,
	 * or was just made into an addressible temp */
	tp2 = tp1->t_lp;
	if (tp2->t_op != LEAF) {
		tp2 = leafnode(tp2);
		tp2->t_flag = tp1->t_lp->t_flag;
		tp1->t_lp = tp2;
	}
	/* Select the addressible offset node
	 * into an index register */
	if (select(tp2, MLVALUE, index) == 0)
		cbotch("sellv");
	/* This flag is for the benefit of output() */
	tp->t_flag |= T_INDIR;
	tp->t_used |= tp2->t_used;
	ttp->t_used |= tp2->t_used;
}

/*
 * Store a subtree of the node pointed to by 'ttp'.
 * The 'flag' argument tells which subtree;
 * true means store the right side.
 * Then rewrite the subtree to refer to the storage location.
 */
store(ttp, flag)
TREE	*ttp;
{
	register TREE	*ap;
	register TREE	*tp;
	register int	sideflag;

	if (nstorelist >= NSTORE)
		cbotch("more than %d stores", NSTORE);
	sideflag = MRADDR;
	if (ttp->t_op>=INCBEF && ttp->t_op<=DECAFT) {
		if (flag != 0)
			cbotch("store");
		tp = copynode(ttp);
		sideflag = MRVALUE;
	} else if (flag != 0)
		tp = ttp->t_rp;
	else
		tp = ttp->t_lp;
	ap = tempnode(tp, 1);
	if (sideflag == MRVALUE)
		ap = leafnode(ap);
	walk(ap, amd);
	if (sideflag == MRVALUE)
		*ttp = *ap;
	else if (flag != 0)
		ttp->t_rp = ap;
	else
		ttp->t_lp = ap;
	amd(ttp);
	ap = tempnode(tp, 0);
	ap = leftnode(ASSIGN, ap, ap->t_type, ap->t_size);
	while (tp->t_op == LEAF)
		tp = tp->t_lp;
	if (tp->t_type != ap->t_type)
		tp = leftnode(CONVERT, tp, ap->t_type, ap->t_size);
	else if ((tp->t_flag&T_LEAF) != 0)
		tp = leafnode(tp);
	ap->t_rp = ripout(tp, 0);
	fixtoptype(ap);
	walk(ap, amd);
	storelist[nstorelist++] = ap;
}

/*
 * Return true if the top of the argument tree is a conversion of some type
 * and the conversion makes the data wider (e.g. int to long).
 */
iswiden(tp)
register TREE *tp;
{
	register TREE *lp;

	if (tp->t_op!=CONVERT && tp->t_op!=CAST)
		return (0);
	lp = tp->t_lp;
	if (wtype[tp->t_type] < wtype[lp->t_type])
		return (0);
	return (1);
}

/*
 * This routine determines if a pattern and a set of expression flags mismatch,
 * and a call to 'sellv' or 'selrv' is needed.
 * Also used by output().
 */
selmiss(pflag, eflag)
FLAG pflag;
FLAG eflag;
{
	if ((pflag&eflag&~T_OFS) == 0)
		return (1);
	return (0);
}

/*
 * Remove any fix nodes which may have been generated
 * before a temp was substituted for an expression by store.
 */
TREE *
ripout(tp, flag)
register TREE *tp;
{
	register op;

	op = tp->t_op;
	if (!isleaf(op)) {
		tp->t_lp = ripout(tp->t_lp, flag);
		if (op!=FIELD && tp->t_rp!=NULL)
			tp->t_rp = ripout(tp->t_rp, flag);
	}
	if (op==FIXUP || (op==LEAF && flag!=0))
		tp = tp->t_lp;
	return (tp);
}

