/*
 * n1/mtree1.c
 * A portable associative tree reorderer and constant expression folder.
 * This may not be general enough for all machines.
 */

#ifdef   vax
#include "INC$LIB:cc1.h"
#else
#include "cc1.h"
#endif

#if	_I386
#define  NLNODE  128
#define  NONODE  128
#else
#define  NLNODE  20
#define  NONODE  20
#endif

static	char	cltc[]	= "associative expression too complex";
TREE *fold1();
TREE *foldaddr();

/*
 * Fancy folder.
 * This gathers up all the constants that it can
 * by digging through the subtrees of commutative/associative operations.
 * Simpler things are done for operations that are not quite so friendly.
 * Returns a pointer to the new tree.
 */
TREE *
modfold(tp)
register TREE *tp;
{
	register TREE *lp, **rp;
	int i, nl, nn, op, cop;
	lval_t c1;
	TREE *leaves[NLNODE], *onodes[NONODE];

	op = tp->t_op;
	if (op==NEG && tp->t_lp->t_op==DCON) {
		tp = tp->t_lp;
#if IEEE|DECVAX
		tp->t_dval[DVALIS] ^= DVALMS;
#else
#if MCFFP
		if ((((int)tp->t_dval[3])&0377) != 0)
			tp->t_dval[3] ^= 0200;
#else
		dvalneg((char *)tp->t_dval);
#endif
#endif
		return tp;
	}
	if (isleaf(op) || isflt(tp->t_type))
		return tp;
	if (op==CONVERT || op==CAST) {
		lp = tp->t_lp;
		if ((cop=lp->t_op)==ICON || cop==LCON) {
			c1 = constcvt(tp->t_type, grabnval(lp));
			if (islong(tp->t_type))
				lp = lvalnode(c1);  else
				lp = ivalnode((ival_t) c1);
			lp->t_type = tp->t_type;
			lp->t_size = tp->t_size;
			return lp;
		}
	}
	if (op!=ADD && op!=MUL && op!=AND && op!=OR && op!=XOR) {
		lp = fold1(op, tp->t_lp, tp->t_rp);
		if (lp != NULL)
			return lp;
		return tp;
	}
	nn = nl = 0;
	cluster(tp, op, tp->t_type, &nn, &nl, onodes, leaves);
	rp = &leaves[--nl];
	for (i = nl; i > 0; ) {
		lp = fold1(op, rp[0], rp[-1]);
		if (lp == NULL)
			break;
		--nl;
		--rp;
		rp[0] = lp;
		--i;
	}
	tp = rp[0];
	if (op==ADD || op==OR || op==XOR) {
		if (nl>0 && isnval(tp, 0))
			--nl;
		if (nl <= 0)
			return leaves[0];
	}
	if (op==MUL || op==AND) {
		if (isnval(tp, 0))
			return tp;
		if (op==MUL && nl>0 && isnval(tp, 1))
			--nl;
	}
	if (nl <= 0)
		return leaves[0];
	rp = &leaves[0];
	tp =  leaves[0];
	for (i=0; i<nl; ++i) {
		lp = onodes[i];
		lp->t_rp = *++rp;
		lp->t_lp = tp;
		tp = lp;
		if (op==ADD && isblkp(tp->t_type)==0)
			fixaddtype(tp);
	}
	return tp;
}

/*
 * Collect up an associative operator cluster.
 * Pack it into the supplied buffers.
 */
cluster(tp, op, type, ann, anl, onodes, leaves)
register TREE *tp;
int *ann, *anl;
register TREE *onodes[], *leaves[];
{
	register i, nl;
	int nn;
	TREE *xp;

	if (tp->t_op==op && tp->t_type==type) {
		nn = *ann;
		if ((*ann)++ >= NONODE)
			cfatal(cltc);
		onodes[nn] = tp;
		cluster(tp->t_lp, op, type, ann, anl, onodes, leaves);
		cluster(tp->t_rp, op, type, ann, anl, onodes, leaves);
		return;
	}
	nl = *anl;
	if ((*anl)++ >= NLNODE)
		cfatal(cltc);
	if (isncon(tp->t_op)) {
		leaves[nl] = tp;
		return;
	}
	for (i = nl; i > 0; ) {
		xp = leaves[i-1];
		if (tp->t_op==ADDR && !isncon(xp->t_op))
			break;
		if (tp->t_op==STAR && xp->t_op==STAR)
			break;
		leaves[i] = xp;
		--i;
	}
	leaves[i] = tp;
}

/*
 * Fold an operation.
 * Return a pointer to the folded tree,
 * or NULL if no fold is possible.
 * FIX_ME This should pay attention to unsigned types like n0/fold.c/fold0().
 */
TREE *
fold1(op, lp, rp) int op; TREE *lp, *rp;
{
	register TREE *fp;
	register int sop, lflag, tt, bool;
	lval_t lv, rv;

	if ((fp = foldaddr(op, lp, rp)) != NULL)
		return fp;
	if ((sop = lp->t_op)!=ICON && sop!=LCON)
		return NULL;
	lv = grabnval(lp);
	tt = lp->t_type;
	lflag = (sop == LCON);
	if (op == QUEST)
		return (lv) ? rp->t_lp : rp->t_rp;
	if (rp != NULL) {
		if ((sop = rp->t_op)!=ICON && sop!=LCON)
			return NULL;
		rv = grabnval(rp);
		if (rp->t_type > tt)
			tt = rp->t_type;
		if (sop == LCON)
			++lflag;
	}
	bool = -1;

	/* Perform the folding, result to lv or bool. */
	switch (op) {

	case COM:	lv = ~lv;		break;
	case NEG:	lv = -lv;		break;
	case ADD:	lv += rv;		break;
	case SUB:	lv -= rv;		break;
	case MUL:	lv *= rv;		break;
	case AND:	lv &= rv;		break;
	case OR:	lv |= rv;		break;
	case XOR:	lv ^= rv;		break;
	case SHL:	lv <<= rv;		break;
	case SHR:	lv >>= rv;		break;

	case DIV:
		if (rv == 0)
			return NULL;
		lv /= rv;
		break;

	case REM:
		if (rv == 0)
			return NULL;
		lv %= rv;
		break;

	case NOT:	bool = !lv;		break;
	case EQ:	bool = lv == rv;	break;
	case NE:	bool = lv != rv;	break;
	case LT:	bool = lv <  rv;	break;
	case LE:	bool = lv <= rv;	break;
	case GT:	bool = lv >  rv;	break;
	case GE:	bool = lv >= rv;	break;
	case ANDAND:	bool = (lv && rv);	break;
	case OROR:	bool = (lv || rv);	break;

	default:
		return NULL;
	}

	if (bool != -1)
		fp = ivalnode((ival_t)bool);
	else if (lflag)
		fp = lvalnode(lv);
	else
		fp = ivalnode((ival_t) lv);
	fp->t_type = tt;
	return fp;
}

/*
 * Fold things that look like '&array[constant]'
 * where the array is an external or a static.
 */
TREE *
foldaddr(op, lp, rp)
register TREE *lp, *rp;
{
	register TREE *xp;
	long val;

	if (op==ADD || op==SUB) {
		if (lp->t_op==ADDR
		 && (rp->t_op==LCON || rp->t_op==ICON)) {
			xp = lp->t_lp;
			if (xp->t_op==LID || xp->t_op==GID) {
				val = grabnval(rp);
				if (op == ADD)
					xp->t_offs += val;
				else
					xp->t_offs -= val;
				return lp;
			}
		}
		if (op==ADD &&  rp->t_op==ADDR
		 && (lp->t_op==LCON || lp->t_op==ICON)) {
			xp = rp->t_lp;
			if (xp->t_op==LID || xp->t_op==GID) {
				xp->t_offs += grabnval(lp);
				return rp;
			}
		}
	}
	return NULL;
}

/* end of n1/mtree1.c */
