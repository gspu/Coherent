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
extern	TREE *ripout();

/*
 * This table, indexed by a context name,
 * returns the appropriate bit set used in the pattern match.
 */
static	PATFLAG	nflagtab[] = {
	PEFFECT,
	PLVALUE,
	PRVALUE,
	PFNARG,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	PEQ,
	PNE,
	PGT,
	PGE,
	PLE,
	PLT,
	PUGT,
	PUGE,
	PULE,
	PULT
};

/*
 * This routine is the heart of the code selector.
 * It looks for a pattern that covers the top level
 * operation of tree 'tp' and is in conformance with
 * (or can be pulled into conformance with)
 * the pattern's shape requirements.
 */
seltree(tp, c, r)
TREE *tp;
{
	register TREE *lp, *rp;
	register PAT  *patp;
	PATX *patxp;
	TREE *ap;
	FLAG lflag, rflag, lpflag, rpflag;
	TYPESET ltype, rtype, ntype;
	PATFLAG pflag, nflag;
	PREGSET savbusy, savxreg;
	int ltemp, rtemp, ntemp, stemp, ptemp;
	int sgoal;
	int clash, h;
	int rreg, op, npat, lindex, rindex;
	int didstore;

	/* Confirm that the context is consistent
	 * with the register specified. */
	if (c == MLVALUE && isrealreg(r)
	&& (pertype[tp->t_type].p_kind&reg[r].r_lvalue)==0)
		c = MRVALUE;
	/* Jump here if the original match was accomplished by coercing a
	 * subtree into addressibility by a store into an auto temp.
	 * The tree has been rewritten and must
	 * be matched again for an earlier ADR|MMX pattern. */
rematch:
#if !TINY
	if (sflag > 1) {
		snapf("seltree(%P, %C, ", tp, c);
		snapf(c<MFLOW ? "%R" : c<MEQ ? "%d" : "L%d", r);
		snapf(") %A curbusy=%M, curxreg=%M\n",
			tp->t_op, curbusy, curxreg);
		if (sflag > 2)
			snapf("%W%E%W", "Seltree", tp, NULL);
	}
#endif
	nflag = nflagtab[c];
	ntype = pertype[tp->t_type].p_type;
	if (tp->t_op < MIOBASE)
		return (0);
	/* Try for matches with widening conversions deleted.
	 * Shrink type conversions must remain as
	 * they may clear or sign extend the operand. */
	lp = tp->t_lp;
	if (iswiden(lp)) {
		ap = lp->t_lp;
		if (ap->t_op >= MIOBASE) {
			tp->t_lp = ap;
			if (seltree(tp, c, r))
				return (1);
			tp->t_lp = lp;
		}
	}
	ltype = pertype[lp->t_type].p_type;
	if (lp->t_op == FIELD)
		lp = lp->t_lp;
	lflag = lp->t_flag;
	rtype = 0;
	rflag = 0;
	if ((op = tp->t_op) != FIELD) {
		rp = tp->t_rp;
		if (rp != NULL) {
			if (iswiden(rp)) {
				ap = rp->t_lp;
				if (ap->t_op >= MIOBASE) {
					tp->t_rp = ap;
					if (seltree(tp, c, r))
						return (1);
					tp->t_rp = rp;
				}
			}
			rtype = pertype[rp->t_type].p_type;
			rflag = rp->t_flag;
		}
	}
	patxp = &patx[op - MIOBASE];
	patp = patxp->px_pp - 1;
	npat = patxp->px_npat;
	savbusy = curbusy;
	savxreg = curxreg;
	/* Main search.
	 * A flying jump to 'again' will try the next pattern.
	 * The 'patp' is predecremented.
	 * Yes, this is a bit ugly. */
again:
	curbusy = savbusy;
	curxreg = savxreg;
	tp->t_patp = NULL;
	if (--npat < 0)
		return (0);
	++patp;
	consnapv(10, "%I: ", patp);
	/* Pattern flag: specifies whether this pattern
	 * can satisfy the context specified for this tree
	 * we must have a hit to match. */
	pflag = 0;
	if (patp->p_flag != 0)
		pflag = patcache[patp->p_flag-1];
	if ((nflag&pflag) == 0) {
		consnap(10, "pflag\n");
		goto again;
	}
	/* Node type: unless pattern specifies no type
	 * we must have a hit to match. */
	if (patp->p_ntype != 0
	 && (typecache[patp->p_ntype-1]&ntype) == 0) {
		consnap(10, "ntype\n");
		goto again;
	}
	/* Left tree type: unless pattern specifies no type
	 * we must have a hit to match. */
	if (patp->p_ltype != 0
	 && (typecache[patp->p_ltype-1]&ltype) == 0) {
		consnap(10, "ltype\n");
		goto again;
	}
	/* Right tree type: unless pattern specifies no type
	 * we must have a hit to match. */
	if (patp->p_rtype != 0
	 && (typecache[patp->p_rtype-1]&rtype) == 0) {
		consnap(10, "rtype\n");
		goto again;
	}
	/* Left tree node type: a mismatch will be coerced
	 * unless the pattern specifies MMX.
	 * bytereg(tp) || ... is dgc's kludge to avoid ADR coercion
	 * of byte oriented operations which actually require a
	 * register, at least so I (rec) think (thinks). */
	lpflag = 0;
	if (patp->p_lflag != 0)
		lpflag = flagcache[patp->p_lflag-1];
	if (lpflag!=0 && (lpflag&lflag)==0
	&& ((lpflag&T_MMX) != 0
	||  (bytereg(tp) && lflag==0 && (lpflag&T_TREG)==0))) {
		consnap(10, "lflag\n");
		goto again;
	}
	/* Right tree node type: a mismatch will be coerced
	 * unless the pattern rspecifies MMX. */
	rpflag = 0;
	if (patp->p_rflag != 0)
		rpflag = flagcache[patp->p_rflag-1];
	if (rpflag!=0 && (rpflag&rflag)==0 && (rpflag&T_MMX)!=0) {
		consnap(10, "rflag\n");
		goto again;
	}
	/* This pattern is acceptable.
	 * The simplest case of a match requires
	 * only that the pattern pointer be identified.
	 * The rest of the code in this routine is activated
	 * by requests for registers, TREG loads, and node type coercion. */
	tp->t_patp = patp;
	tp->t_used = 0;
	/* The pattern may explicitly specify temp
	 * registers for a subtrees, or the
	 * subtree temp may have to be allocated
	 * before the other subtree is selected. */
	rtemp = NONE;
	if ((rpflag&T_TREG) != 0) {
		rtemp = patp->p_rtemp;
		if (isrealreg(rtemp)) {
			if (isbusy(rtemp)) {
				consnap(10, "rtemp busy\n");
				goto again;
			}
			setused(tp, rtemp);
		} else if ((lpflag&T_TREG)!=0 && (pflag&P_SRT)==0) {
			rtemp = rallo(rp, tp->t_used, 0);
			if (rtemp < 0) {
				consnap(10, "rtemp allo\n");
				goto again;
			}
			consnapv(15, "rtemp = %R\n", rtemp);
			setused(tp, rtemp);
		}
	}
	ltemp = NONE;
	if ((lpflag&T_TREG) != 0) {
		ltemp = patp->p_ltemp;
		if (isrealreg(ltemp)) {
			if (isbusy(ltemp)) {
				consnap(10, "ltemp busy\n");
				goto again;
			}
			setused(tp, ltemp);
		} else if ((rpflag&T_TREG)!=0 && (pflag&P_SLT)==0) {
			ltemp = rallo(lp, tp->t_used, 0);
			if (ltemp < 0) {
				consnap(10, "ltemp allo\n");
				goto again;
			}
			consnapv(15, "ltemp = %R\n", ltemp);
			setused(tp, ltemp);
		}
	}
	/* The pattern may require a node temp register.
	 * If the caller cares, use his preference.
	 * If PAIR, allocate a pair so that the right and left
	 * preferences (usually LOTEMP) will work out right.
	 * The ANY specifier stays around for a while. */
	ntemp = patp->p_ntemp;
	if (isrealreg(ntemp)) {
		if (isbusy(ntemp)) {
			consnap(10, "ntemp busy\n");
			goto again;
		}
	} else if (ntemp != NONE) {
		if ((c==MLVALUE || c==MRVALUE) && isrealreg(r)) {
			ptemp = r;
			clash = 0;
			if (ntemp == PAIR) {
				ptemp = enpair(ptemp);
				if ((h=lohalf(ptemp))!=r && isbusy(h))
					++clash;
				if ((h=hihalf(ptemp))!=r && isbusy(h))
					++clash;
			}
			if (!clash && isusable(tp, c, ptemp))
				ntemp = ptemp;
		}
		/* Bind only if we must. */
		if (!isrealreg(ntemp)) {
			if (ntemp == PAIR
			|| (ltemp==LOTEMP || ltemp==HITEMP)
			|| (rtemp==LOTEMP || rtemp==HITEMP)) {
				ntemp = regselect(tp, ntemp, 1);
				if (ntemp < 0) {
					consnap(10, "ntemp select\n");
					goto again;
				}
			}
		}
	}
	if (isrealreg(ntemp) && rp!=NULL) {
		consnapv(15, "ntemp = %R\n", ntemp);
		setused(tp, ntemp);
	}
	/* If the pattern specifies addressible
	 * subtrees and the subtrees are not directly
	 * addressible, then index registers will be
	 * required to hold the offset expression. */
	lindex = rindex = -1;
	if (ishlvadr(rpflag) && !isadr(rflag)) {
		if (!isofs(rflag))
			cbotch("no rofs");
		ap = findoffs(rp);
		if ((rindex = rallo(ap, tp->t_used, 2)) < 0) {
			consnap(10, "rindex allo\n");
			goto again;
		}
		consnapv(15, "rindex = %R\n", rindex);
		curxreg |= reg[rindex].r_phys;
	}
	if (ishlvadr(lpflag) && !isadr(lflag)) {
		if (!isofs(lflag))
			cbotch("no lofs");
		ap = findoffs(lp);
		if ((lindex = rallo(ap, tp->t_used, 2)) < 0) {
			consnap(10, "lindex allo\n");
			goto again;
		}
		consnapv(15, "lindex = %R\n", lindex);
		curxreg |= reg[lindex].r_phys;
	}
	consnap(10, "\n");
	/* If the pattern specifies T_TREG, then
	 * the subtree must be selected. */
	if ((rpflag&T_TREG) != 0) {
		/* In case two TREG's are specified the ltemp
		 * must be identified and setbusy before the rtemp
		 * is loaded or ltemp may be clobbered. */

		/* Select an rtemp register based on the share flag,
		 * the rtemp preference, and the node preference */
		if ((pflag&P_SRT) != 0
		&& (rtemp==ANYL || rtemp==ANYR)
		&&  isrealreg(r)
		&&  isusable(tp, c, r))
			stemp = r;
		else
			stemp = reguse(rtemp, ntemp);
		/* Select the subcontext depending on the register selected
		 * or the currently specified context */
		sgoal = subgoal(c, stemp);
		/* Now load the specified register or preferred type */
		if (select(rp, sgoal, stemp) == 0)
			goto again;
		tp->t_used |= rp->t_used;
		if ((pflag&P_SRT) != 0) {
			if ((ntemp==ANYL || ntemp==ANYR)
			&&   isusable(tp, c, rp->t_rreg))
				ntemp = rp->t_rreg;
			else if (patp->p_ntemp == patp->p_rtemp
			 && ntemp != rp->t_rreg) {
				consnap(10, "rtemp share\n");
				goto again;
			}
		} else
			setbusy(rp->t_rreg);	/* This is wrong */
			/* Should be set busy after the ltemp load */

		/* If ltemp was setbusy above, it should now be freed */
	}
	if ((lpflag&T_TREG) != 0) {
		if ((pflag&P_SLT) != 0
		&& (ltemp==ANYL || ltemp==ANYR)
		&&  isrealreg(r)
		&&  isusable(tp, c, r))
			stemp = r;
		else
			stemp = reguse(ltemp, ntemp);
		sgoal = subgoal(c, stemp);
		if (select(lp, sgoal, stemp) == 0)
			goto again;
		tp->t_used |= lp->t_used;
		if ((pflag&P_SLT) != 0) {
			if ((ntemp==ANYL || ntemp==ANYR)
			&&   isusable(tp, c, lp->t_rreg))
				ntemp = lp->t_rreg;
			else if (patp->p_ntemp == patp->p_ltemp
			 && ntemp != lp->t_rreg) {
				consnap(10, "ltemp share\n");
				goto again;
			}
		} else
			setbusy(lp->t_rreg);
	}
	/* Now bind the node temporary if it hasn't already happened. */
	if (!isrealreg(ntemp) && ntemp!=NONE) {
		ntemp = regselect(tp, ntemp, 1);
		if (ntemp < 0) {
			consnap(10, "ntemp select\n");
			goto again;
		}
	}
	tp->t_treg = ntemp;
	/* Keep temps used in node coercion off target path */
	if (ntemp!=NONE && rp!=NULL)
		setused(tp, ntemp);
	/* Force ltemp and rtemp used */
	if ((lpflag&T_TREG)==0 && isrealreg(patp->p_ltemp))
		setused(tp, patp->p_ltemp);
	if ((rpflag&T_TREG)==0 && isrealreg(patp->p_rtemp))
		setused(tp, patp->p_rtemp);
	/* Now coerce the subtrees into rv or lv addressibility
	 * if they do not naturally conform. */
	didstore = 0;
	if (selmiss(rpflag, rflag) && isrvadr(rpflag) && !isadr(rflag))
		selrv(tp, 1, &didstore);
	if (selmiss(lpflag, lflag) && isrvadr(lpflag) && !isadr(lflag))
		selrv(tp, 0, &didstore);
	if (selmiss(rpflag, rflag) && islvadr(rpflag) && !isadr(rflag))
		sellv(tp, rindex, 1, &didstore);
	if (selmiss(lpflag, lflag) && islvadr(lpflag) && !isadr(lflag))
		sellv(tp, lindex, 0, &didstore);
	/* If coercion generated a stack temp,
	 * then we must search the table again
	 * for a simpler pattern, after cleaning
	 * up any FIXUP's */
	if (didstore) {
		tp = ripout(tp, 0);
		curbusy = savbusy;
		curxreg = savxreg;
		consnap(1, "Goto rematch\n");
		goto rematch;
	}
	/* Make node temp used */
	if (ntemp != NONE)
		setused(tp, ntemp);
	/* Compute the result register */
	if ((rreg = patp->p_result) == TEMP)
		rreg = ntemp;
	else if (rreg == LOTEMP)
		rreg = lohalf(ntemp);
	else if (rreg == HITEMP)
		rreg = hihalf(ntemp);
	tp->t_rreg = rreg;
	if (rreg != NONE)
		setused(tp, rreg);
	curbusy = savbusy;
	curxreg = savxreg;
	return (1);
}
