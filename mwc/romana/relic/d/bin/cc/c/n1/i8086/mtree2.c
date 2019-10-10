/*
 * This file contains the machine dependent parts of the tree modifier.
 * This version handles both the SMALL and the LARGE model iAPX86.
 * A conditionalization handles machines that have an 8087.
 */
#ifdef   vax
#include "INC$LIB:cc1.h"
#else
#include "cc1.h"
#endif

int	blkflab;

static	char	modoptab[] = {
	'i',	'u',	'i',	'u',	'l',	'v',	'f',
	'd',	'b',	'i',	'i',	'p',	'p',	'p',	'p'
};


#if SPLIT_CC1
/*
 * Function prolog.
 * Clear "BLK function" label.
 */
doprolog()
{
	blkflab = 0;
}

/*
 * Copy auto information.
 */
doautos()
{
	iput(iget());
	iput(iget());
}
#endif

/*
 * This function performs machine specific tree modifications.
 * It is called from "modtree" after all of the machine
 * independent transformations have been done.
 * It returns either a pointer to the new tree
 * (telling "modtree" to do another pass)
 * or NULL (which implies that no changes were made).
 */
TREE *
modoper(tp, ac, ptp)
register TREE	*tp;
TREE		*ptp;
{
	register TREE	*lp;
	register TREE	*rp;
	register TREE	*tp1;
	register TREE	*tp2;
	register TREE	*tp3;
	register int	c;
	register int	o;
	register int	op;
	register int	tt;
	register int	nbase;
	register int	ntype;
	register int	lt;
	register int	rt;
	register int	seg;
	register int	lab;
	register int	makecall;

	c = ac;
	if (c==MRETURN || c==MSWITCH || c==MINIT)
		c = MRVALUE;
#if !ONLYSMALL&&!NDP
	/*
	 * All fetches of float or double values are rewritten as
	 * routine calls in LARGE model.
	 * This makes the code a great deal smaller,
	 * and solves the problem of addressability on the _fpac_,
	 * which is not in the same segment as the data of the file.
	 */
	if (isvariant(VLARGE)
	&& (ac!=MINIT && ac!=MLADDR && ac!=MRADDR)
	&& (tp->t_flag&T_LEAF) != 0
	&& isflt(tp->t_type)) {
		tp1 = makenode(GID, LPTR);
		if (tp->t_type == F32)
			tp1->t_sp = gidpool("dfload");
		else
			tp1->t_sp = gidpool("ddload");
		tp1->t_seg = SANY;
		tp1 = leftnode(CALL, tp1, F64);
		storedcon(tp);
		if (tp->t_op == STAR)
			tp1->t_rp = tp->t_lp;
		else
			tp1->t_rp = leftnode(ADDR, tp, LPTR);
		return (tp1);
	}
#endif
	op = tp->t_op;
	if (isleaf(op)) {
		if (op==AID || op==PID) {
			ntype = SPTR;
			nbase = BP;
#if !ONLYSMALL
			if (isvariant(VLARGE)) {
				ntype = LPTR;
				nbase = SSBP;
			}
#endif
			o = tp->t_offs;
			tp->t_op = STAR;
			tp->t_rp = NULL;
			tp->t_lp  = tp2 = makenode(ADD, ntype);
			tp2->t_lp = tp3 = makenode(REG, ntype);
			tp3->t_reg = nbase;
			tp2->t_rp = ivalnode(o);
			return (tp);
		}
#if !ONLYSMALL
		/* Generate links to double constants under LARGE RAM option. */
		if ((ac != MINIT) && (op == DCON) && isvariant(VRAM) && isvariant(VLARGE)) {
			pool(tp);	/* the dcon */
			tp->t_type = LPTR;
			tp->t_size = 0;
			pool(tp);	/* the label */
			tp = leftnode(STAR, tp, F64, 0);
			return (tp);
		}
		/*
		 * Build up indirect links to any LID or GID items
		 * that you cannot access in a direct fashon.
		 */
		if (ac != MINIT
		&& (op==LID || op==GID)
		&& (ptp==NULL || ptp->t_op!=CALL || tp!=ptp->t_lp)
		&& isvariant(VLARGE)) {
			seg = tp->t_seg;
			if (seg==SANY || seg==SDATA || seg==SBSS
			|| (seg==SPURE && isvariant(VRAM))
			|| (seg==SSTRN && notvariant(VROM))) {
				pool(tp);
				tp1=leftnode(STAR, tp, tp->t_type, tp->t_size);
				tp->t_type = LPTR;
				tp->t_size = 0;
				return (tp1);
			}
		}
#endif
		goto done;
	}
	/*
 	 * Beat on lvalue fields.
	 */
	if ((op==ASSIGN || (op>=AADD && op<=ASHR)
	||  (op>=INCBEF && op<=DECAFT))
	&&   tp->t_lp->t_op==FIELD && tp->t_lp->t_type<FLD8)
		return (modlfld(tp, c));
	/*
 	 * Non leaf.
	 * Gather up subtrees.
	 * Rewrite some things as calls to library routines.
	 */
	tt = tp->t_type;
	lp = tp->t_lp;
	if (lp != NULL)
		lt = lp->t_type;
	rp = NULL;
	if (op != FIELD) {
		rp = tp->t_rp;
		if (rp != NULL)
			rt = rp->t_type;
	}
	/*
	 * Long MUL, DIV and REM are always a function call.
	 * If there is no NDP in the machine,
	 * then floating point is a routine call.
	 * If there is, we rewrite conversions from bytes and unsigned things
	 * and all conversions from float to fixed
	 * (a mode switch may be necessary).
	 */
	makecall = 0;
	if (islong(tt) && op>=MUL && op<=REM) {
		++makecall;
#if NDP
	} else if (isflt(tt)) {
		if (op==CONVERT || op==CAST) {
			if ((lp->t_flag&T_REG) != 0
			|| (lp->t_flag&T_LEAF) == 0
			|| (lt!=S16 && lt!=S32 && lt!=F32))
				++makecall;
		}
	} else if ((op>=GT && op<=LT) && isflt(lt)) {
		tp->t_op += UGT-GT;
#else
	} else if (isflt(tt)) {
		if (op==CONVERT || op==CAST) {
			if (!isflt(lt))
				++makecall;
		} else if ((op>=ADD && op<=REM)
#if !ONLYSMALL
		|| (isvariant(VLARGE) && op==NEG)
		|| (isvariant(VLARGE) && op==ASSIGN)
#endif
		|| (op>=AADD && op<=AREM))
			++makecall;
	} else if (isrelop(op) && isflt(lt)) {
		++makecall;
#endif
	} else if ((op==CONVERT || op==CAST) && isflt(lt))
		++makecall;
	if (makecall != 0)
		return (modxfun(tp));
	switch (op) {

	case FIELD:
		if (c != MLADDR)
			return (modefld(tp->t_lp, tp, c, 1));
		break;

	case ASSIGN:
		if (tt == BLK) {
			tp = modsasg(lp, rp, tp->t_size);
			if (c != MEFFECT)
				tp = leftnode(STAR, tp, BLK, tp->t_size);
			return (tp);
		}
		break;

	case CONVERT:
	case CAST:
		if (modkind(tt) == modkind(lp->t_type)) {
			lp->t_type = tt;
			return (lp);
		}
	}
	/*
	 * If this tree is the return value of a structure function,
	 * arrange to copy the value into a secret place
	 * and return the address of the place.
	 */
done:
	if (tp->t_type==BLK && ac==MRETURN) {
		if (blkflab == 0) {
			blkflab = newlab();
			o = newseg(SBSS);
			genlab(blkflab);
			bput(BLOCK);
			iput((ival_t) tp->t_size);
			newseg(o);
		}
		lp = makenode(LID, BLK, tp->t_size);
		lp->t_label = blkflab;
		lp->t_seg = SBSS;
		return (modsasg(lp, tp, tp->t_size));
	}
	return (NULL);
}

/*
 * Given a type, return a kind that is used to see
 * if two objects are just different names for the same bits.
 */
modkind(t)
register t;
{
	if (t == U16)
		t = S16;
	else if (t == U32)
		t = S32;
	return (t);
}

/*
 * Build a call node for the assignment of structure data.
 * The rep and a copy operation are not used so that the ES need not be used.
 * The type of the CALL is PTR.
 * The size is valid.
 */
TREE *
modsasg(lp, rp, s)
register TREE	*lp;
register TREE	*rp;
{
	register TREE	*tp;
	int		nptdt;

	nptdt = SPTR;
#if !ONLYSMALL
	if (isvariant(VLARGE))
		nptdt = LPTR;
#endif
	rp = leftnode(ADDR, rp, nptdt);
	rp = leftnode(ARGLST, rp, nptdt);
	rp->t_rp = ivalnode(s);
	lp = leftnode(ADDR, lp, nptdt);
	lp = leftnode(ARGLST, lp, nptdt);
	lp->t_rp = rp;
	tp = makenode(GID, nptdt);
	tp->t_sp = gidpool("blkmv");
	tp->t_seg = SANY;
	tp = leftnode(CALL, tp, nptdt);
	tp->t_size = s;
	tp->t_rp = lp;
	return (tp);
}

/*
 * Modify function calls.
 * Handle functions that return objects of type "BLK"
 * by adding a free indirection node.
 */
TREE *
modcall(tp, c)
register TREE *tp;
{
	tp->t_lp = modtree(tp->t_lp, MLADDR, tp);
	tp->t_rp = modargs(tp->t_rp, tp);
	if (tp->t_type == BLK) {
#if !ONLYSMALL
		if (isvariant(VLARGE))
			tp->t_type = LPTR;
		else
			tp->t_type = SPTR;
#else
		tp->t_type = SPTR;
#endif
		if (c != MEFFECT)
			tp = leftnode(STAR, tp, BLK, tp->t_size);
	}
	return (tp);
}

/*
 * Modify argument lists.
 */
TREE *
modargs(tp, ptp)
register TREE	*tp;
TREE		*ptp;
{
	if (tp == NULL)
		return (NULL);
	if (tp->t_op == ARGLST) {
		tp->t_lp = modargs(tp->t_lp, tp);
		tp->t_rp = modargs(tp->t_rp, tp);
		return (tp);
	}
	if (tp->t_type == BLK) {
#if !ONLYSMALL
		tp = leftnode(ADDR, tp,
			(isvariant(VLARGE))?LPTB:SPTB,
			tp->t_size);
#else
		tp = leftnode(ADDR, tp, SPTB, tp->t_size);
#endif
		return (modtree(tp, MRVALUE, ptp));
	}
	return (modtree(tp, MFNARG, ptp));
}

/*
 * Given a pointer to a TREE node that describes an operation
 * that the machine cannot directly perform,
 * rewrite the node as a CALL to a magic routine.
 * On the iAPX-86 we rewrite floating point, long multiply and divide
 * and unsigned long multiply and divide.
 * This routine only has to handle the ASSIGN
 * operation if IEEE format; when using DECVAX format
 * the double=>float conversion is easy.
 */
TREE *
modxfun(tp)
TREE *tp;
{
	register TREE	*lp, *rp;
	register char	*p1, *p2;
	register TREE	*tp1;
	register int	tt, lt, op;
	register int	nptct, nptdt;

	static	char	*name[] = {
		"add",
		"sub",
		"mul",
		"div",
		"rem"
	};

	nptdt = SPTR;
#if !ONLYSMALL
	if (isvariant(VLARGE))
		nptdt = LPTR;
#endif
	tp1 = makenode(GID, nptdt);
	op  = tp->t_op;
	lp  = tp->t_lp;
	rp  = tp->t_rp;
	tt  = tp->t_type;
	if (lp != NULL)
		lt  = lp->t_type;
	p1  = id;
	if (op==CONVERT || op==CAST || op==NEG) {
		p2 = "cvt";
		if (op == NEG)
			p2 = "neg";
		*p1++ = modoptab[tt];
		if (lt==F32 && tt!=F64)
			lt = F64;
		*p1++ = modoptab[lt];
	} else {
		walk(tp, amd);
		*p1++ = modoptab[tt];
		if ((op==ADD || op==MUL || isrelop(op))
		&& ((lp->t_flag&T_LEAF)!=0 && (rp->t_flag&T_LEAF)==0
		||   lp->t_op==DCON && rp->t_op!=DCON)) {
			lp = rp;
			rp = tp->t_lp;
			if (isrelop(op))
				op = fliprel[op-EQ];
		}
		if (op==ASSIGN || (op>=AADD && op<=AREM)) {
			*p1++ = modoptab[lt];
			if (lp->t_op != STAR)
				lp = leftnode(ADDR, lp, nptdt);
			else
				lp = lp->t_lp;
			if (op == ASSIGN)
				p2 = "asg";
			else
				p2 = name[op-AADD];
		} else if (isrelop(op))
			p2 = "cmp"; 
		else if (op>=ADD && op<=REM)
			p2 = name[op-ADD];
		else
			cbotch("modxfun");
		storedcon(rp);
		if (uselvalueform(op, tt, rp)) {
			*p1++ = 'l';
			if (rp->t_op != STAR)
				rp = leftnode(ADDR, rp, nptdt);
			else
				rp = rp->t_lp;
		} else
			*p1++ = 'r';
	}
	while (*p1++ = *p2++)
		;
	*p1 = 0;
	tp1->t_sp = gidpool(id);
	tp1->t_seg = SANY;
	lp = leftnode(ARGLST, lp, nptdt);
	lp->t_rp = rp;
	tp->t_op = CALL;
	tp->t_lp = tp1;
	tp->t_rp = lp;
	fixtoptype(tp);
	if (tp->t_type!=tt && tt!=F32)
		tp = leftnode(CONVERT, tp, tt);
	if (isrelop(op)) {
		tp = leftnode(op, tp, TRUTH);
		tp->t_rp = ivalnode(0);
	}
	return (tp);
}

/*
 * Zap a DCON into a block of memory with a double in it.
 */
storedcon(tp)
register TREE	*tp;
{
	if (tp->t_op != DCON)
		return;
	pool(tp);
	tp->t_flag = T_DIR;
}

/*
 * This routine, used only by the "modxfun" routine,
 * checks if an lvalue form of an operator routine can be used.
 * True return if it can.
 */
uselvalueform(op, tt, rp)
register TREE *rp;
{
	if (isrelop(op)) {
		if (rp->t_type != F64)
			return (0);
	} else {
		if (rp->t_type !=  tt)
			return (0);
	}
	if (rp->t_op==STAR || (rp->t_flag&T_DIR)!=0)
		return (1);
	return (0);
}

/*
 * Test if 1) the tree pointed to by "tp" is a register
 * and     2) the operation "op" can be computed in it.
 */
isokareg(tp, op)
register TREE *tp;
register op;
{
	if (op==MUL || op==DIV || op==REM)
		return (0);
	if (tp->t_op!=REG || !isword(tp->t_type))
		return (0);
	return (1);
}

/*
 * Modify bit fields in lvalue contexts.
 * The "tp" argument is a pointer to the tree node
 * with the FIELD operation on the left side.
 * This routine has two tasks.
 * First, it rewrites the type in the FIELD node to be the type used by
 * select to match the tree; this is also used as a flag
 * to prevent this routine from being called twice on a node.
 * Second, it inserts explict shift nodes to
 * the operands and results so that all of the optimizations
 * applied to shifts work for fields.
 * A pointer to the new top of the tree is returned.
 */
TREE *
modlfld(tp, c)
register TREE *tp;
{
	register TREE *lp, *rp;
	register lt, tt;
	register op;
	register bmop;
	register MASK mask;

	op = tp->t_op;
	tt = tp->t_type;
	lp = tp->t_lp;
	if (lp != NULL)
		lt = lp->t_type;
	if (op!=AMUL && op!=ADIV && op!=ASHL && op!=ASHR) {
		rp = tp->t_rp;
		rp = leftnode(SHL, rp, rp->t_type);
		rp->t_rp = ivalnode(lp->t_base);
		tp->t_rp = rp;
	}
	if (op==AAND || op==AOR || op==AXOR || op==ASSIGN) {
		mask = ((MASK)01<<lp->t_width) - 1;
		mask = mask << lp->t_base;
		bmop = AND;
		if (op == AAND) {
			mask = ~mask;
			bmop = OR;
		}
		/* rp set above */
		rp = leftnode(bmop, rp, rp->t_type);
		rp->t_rp = ivalnode(mask);
		tp->t_rp = rp;
	}
	if (c != MEFFECT)
		tp = modefld(tp, lp, c, 0);
	if (isbyte(lt))
		lp->t_type = FLD8;
	else
		lp->t_type = FLD16;
	return (tp);
}

/*
 * This function rewrites any field extraction.
 * The argument "tp" is the base of the field.
 * The argument "fp" is a FIELD node that supplies the width and the base.
 * The argument "flag" is true to enable the mask off in unsigned field extract.
 */
TREE *
modefld(tp, fp, c, flag)
register TREE *tp;
register TREE *fp;
{
	register n;
	register tt;
	register mw;
	register mask;
	register ttold;

	mw = 16;
	if (isbyte(ttold = tt = tp->t_type)) {
		mw = 8;
		tp = leftnode(CONVERT, tp, tt);
		fixtoptype(tp);
		tt = tp->t_type;
	}
	if (c == MFLOW) {
		mask = (01<<fp->t_width) - 1;
		if ((n=fp->t_base) != 0)
			mask <<= n;
		tp = leftnode(AND, tp, tt);
		tp->t_rp = ivalnode((ival_t)mask);
		return (tp);
	}
	if (isuns(tt)) {
		if ((n=fp->t_base) != 0) {
			tp = leftnode(SHR, tp, tt);
			tp->t_rp = ivalnode(n);
		}
		if (flag && (n=fp->t_width)<mw) {
			tp = leftnode(AND, tp, tt);
			tp->t_rp = ivalnode(((ival_t)01<<n)-1);
		}
		return (tp);
	}
	if ((n=mw-(fp->t_base+fp->t_width)) != 0) {
		tp = leftnode(SHL, tp, tt);
		tp->t_rp = ivalnode(n);
	}
	if ((n=mw-fp->t_width) != 0) {
		tp = leftnode(SHR, tp, tt);
		tp->t_rp = ivalnode(n);
	}
	if (ttold != tt)
		tp = leftnode(CONVERT, tp, ttold);
	return (tp);
}

/*
 * Check if a tree should have its left and right subtrees swapped.
 * Do it if it is required.
 * Sometimes the relational operation must be adjusted.
 */
modswap(tp, ptp)
register TREE	*tp;
TREE		*ptp;
{
	register TREE *lp, *rp;
	FLAG lf, rf;

	switch (tp->t_op) {

	case ADD:
	case MUL:
	case AND:
	case OR:
	case XOR:
	case EQ:
	case NE:
	case GT:
	case GE:
	case LT:
	case LE:
	case UGT:
	case UGE:
	case ULT:
	case ULE:
		lp = tp->t_lp;
		rp = tp->t_rp;
		lf = lp->t_flag;
		rf = rp->t_flag;
		if (lf!=0 || rf!=0) {
			if ((lf&T_CON) != 0
			||  (rf&T_REG) != 0
			||  (lf!=0 && rf==0))
				swapit(tp);
		} else if (lp->t_cost > rp->t_cost)
			swapit(tp);
	}
}
	
/*
 * Swap subtrees.
 * Fix up relational ops.
 */
swapit(tp)
register TREE	*tp;
{
	register TREE *xp;
	register op;

	xp = tp->t_lp;
	tp->t_lp = tp->t_rp;
	tp->t_rp = xp;
	op = tp->t_op;
	if (isrelop(op))
		tp->t_op = fliprel[op-EQ];
}
