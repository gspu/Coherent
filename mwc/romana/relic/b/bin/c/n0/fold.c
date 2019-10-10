/*
 * n0/fold.c
 * Constant expression folder for the C compiler parser.
 * More of this is done in the code generator.
 * This one exists only to allow constant expressions in case labels,
 * in array bounds, and in initializers.
 */

#ifdef   vax
#include "INC$LIB:cc0.h"
#else
#include "cc0.h"
#endif

extern	lval_t	grablval();

#if	FOLD_DOUBLES
/*
 * The double folding code below assumes that the compiler host and target fp
 * representations are identical,
 * an assumption the compiler source carefully avoids making everywhere else.
 * If host fp == target fp,
 * ordinary C fp operations (+, -, *, / etc.) can be used in folding.
 * If host fp != target fp, folding requires format translation or emulation.
 * A compiler produced with this option uses fp,
 * so in particular the verison of fp it uses must run on all target systems;
 * e.g., if you compile the compiler -VNDP then the target will need
 * either an NDP or NDP emulation.
 */
extern	TREE	*foldd();
extern	double	grabdval();
#endif

/*
 * Try to fold constants.
 * The given op must be appropriate for folding,
 * the left subtree must be a constant (ICON/LCON/ZCON/DCON), and
 * the right subtree (if any) must be a constant (ICON/LCON/ZCON/DCON)
 * for folding to work.
 * Return pointer to a constant tree node,
 * or NULL if folding is impossible.
 * This code is very casual about lval_t/ulval_t bit equivalence.
 * Folds double constants if FOLD_DOUBLES.
 */
TREE *
fold0(op, lp, rp) int op; TREE *lp, *rp;
{
	register TREE *tp;
	register lval_t lv, rv;
	int bool;
	register int lt, rt, gt, uflag, zflag, lflag;

	/* Check op dope table to make sure op has a chance of folding. */
	if ((opdope[op - MIOBASE] & FOLD) == 0)
		return NULL;

	/* Check for constant left subtree. */
	if (!isconst(lp))
		return NULL;
	lt = lp->t_type;

	/* Handle ?: without further ado. */
	if (op == QUEST) {
		if (rp->t_op != COLON)
			return NULL;		/* parsing must be fubar */
		return (grablval(lp, 0)) ? rp->t_lp : rp->t_rp;
	}

	/* Handle ',' with constant on lhs without further ado. */
	if (op == COMMA)
		return rp;

	if (rp != NULL) {
		/* Check for constant right subtree. */
		if (!isconst(rp))
			return NULL;
		rt = rp->t_type;
	} else
		rt = lt;			/* Unary ops: no right subtree */

	/*
	 * Find the goal type in the conversion dope table.
	 * Watch out for shifts, right type implicitly T_INT.
	 */
	if (op == SHL || op == SHR)
		rt = T_INT;
	gt = cvdope[13 * (lt - T_CHAR) + (rt - T_CHAR)] & GOAL;

	/* Set unsigned, long, ZCON flags based on goal type. */
	uflag = (gt == T_UINT || gt == T_ULONG);
	lflag = (gt == T_LONG || gt == T_ULONG);
	zflag = (lp->t_op == ZCON || (rp != NULL && rp->t_op == ZCON));

#if	FOLD_DOUBLES
	/* Fold as doubles if goal type is double. */
	if (gt == T_DOUBLE) {
		if (opdope[op - MIOBASE] & NFLT)
			return NULL;		/* op is illegal in any case */
		return foldd(op, lp, rp);
	}
#endif

	/* Grab the left and right values as [u]lval_t. */
	lv = grablval(lp, uflag);
	if (rp != NULL)
		rv = grablval(rp, uflag);

	/* Perform the folding, computing result in lv or in bool (0 or 1). */
	bool = -1;
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

	/* Ops /, % and >> are different for signed and unsigned cases. */
	case DIV:
		if (rv == 0) {
			cwarn("divide by zero");
			return NULL;
		}
		if (uflag)
			lv = ((ulval_t)lv / (ulval_t)rv);
		else
			lv /= rv;
		break;

	case REM:
		if (rv == 0) {
			cwarn("zero modulus");
			return NULL;
		}
		if (uflag)
			lv = ((ulval_t)lv % (ulval_t)rv);
		else
			lv %= rv;
		break;

	case SHR:
		if (uflag)
			lv = ((ulval_t)lv >> (ulval_t)rv);
		else
			lv >>= rv;
		break;

	/* Relations and conditionals return T_INT result in bool. */
	case NOT:	bool = !lv;		break;
	case EQ:	bool = lv == rv;	break;
	case NE:	bool = lv != rv;	break;
	case ANDAND:	bool = (lv && rv);	break;
	case OROR:	bool = (lv || rv);	break;
	/* Nonequality relations are different for signed and unsigned cases. */
	case LT:
		bool = (uflag) ? ((ulval_t)lv <  (ulval_t)rv) : (lv <  rv);
		break;
	case LE:
		bool = (uflag) ? ((ulval_t)lv <= (ulval_t)rv) : (lv <= rv);
		break;
	case GT:
		bool = (uflag) ? ((ulval_t)lv >  (ulval_t)rv) : (lv >  rv);
		break;
	case GE:
		bool = (uflag) ? ((ulval_t)lv >= (ulval_t)rv) : (lv >= rv);
		break;

	default:		/* shouldn't happen if opdope is right */
		return NULL;
	}

	/* Allocate a TREE for the result, store result, return it. */
	tp = talloc();
	if (bool != -1) {			/* boolean result */
		tp->t_op   = ICON;
		tp->t_type = T_INT;
		tp->t_ival = bool;
	} else if (lflag) {			/* [u]long result */
		tp->t_op   = LCON;
		tp->t_type = (uflag) ? T_ULONG : T_LONG;
		tp->t_lval = lv;
	} else if (zflag) {			/* size_t result */
		tp->t_op   = ZCON;
		tp->t_type = T_UINT;
		tp->t_zval = lv;
	} else {				/* [u]int result */
		tp->t_op   = ICON;
		tp->t_type = (uflag) ? T_UINT : T_INT;
		tp->t_ival = lv;
	}
	return tp;
}

/*
 * Return true if tp represents a constant (ICON, LCON, ZCON, DCON).
 */
int
isconst(tp) register TREE *tp;
{
	register int op;

	return (tp->t_dp == NULL
	     && ((op = tp->t_op) == ICON
		 || op == LCON
		 || op == ZCON
#if	FOLD_DOUBLES
		 || op == DCON
#endif
		));
}

/*
 * Grab the value of a constant as an lval_t (or ulval_t, really).
 * Zero extend if the goal type is unsigned.
 */
lval_t
grablval(tp, uflag) register TREE *tp; int uflag;
{
	switch (tp->t_op) {
	case ICON:
		return (uflag) ? (ulval_t)(uival_t)tp->t_ival : tp->t_ival;
	case ZCON:
		return (ulval_t)tp->t_zval;
	case LCON:
		return tp->t_lval;
#if	FOLD_DOUBLES
	case DCON:
		return dval_to_d(tp);
#endif
	}
	cbotch("grablval");
}

#if	FOLD_DOUBLES

/*
 * Fold double constants.
 */
TREE *
foldd(op, lp, rp) int op; TREE *lp, *rp;
{
	register TREE *tp;
	int bool;
	double lv, rv;

	lv = grabdval(lp);
	if (rp != NULL)
		rv = grabdval(rp);
 

	/* Perform the folding, leaving result in lv or in bool (0 or 1). */
	bool = -1;
	switch(op) {

	case NEG:	lv = -lv;		break;
	case ADD:	lv += rv;		break;
	case SUB:	lv -= rv;		break;
	case MUL:	lv *= rv;		break;
	case DIV:	lv /= rv;		break;

	/* Relations and conditionals return T_INT result in bool. */
	case NOT:	bool = !lv;		break;
	case EQ:	bool = lv == rv;	break;
	case NE:	bool = lv != rv;	break;
	case LT:	bool = lv <  rv;	break;
	case LE:	bool = lv <= rv;	break;
	case GT:	bool = lv >  rv;	break;
	case GE:	bool = lv >= rv;	break;
	case ANDAND:	bool = (lv && rv);	break;
	case OROR:	bool = (lv || rv);	break;

	default:		/* shouldn't happen if opdope is right */
		return NULL;
	}

	/* Allocate a TREE for the result and store the result. */
	tp = talloc();
	if (bool != -1) {			/* boolean result */
		tp->t_op  = ICON;
		tp->t_type = T_INT;
		tp->t_ival = bool;
	} else {				/* double result */
		tp->t_op  = DCON;
		tp->t_type = T_DOUBLE;
		d_to_dval(tp, lv);
	}
	return tp;
}

/*
 * Grab the value of a constant as a double.
 */
double
grabdval(tp) register TREE *tp;
{
	register int t;

	switch (tp->t_op) {
	case ICON:
		return ((t = tp->t_type) == T_UINT || t == T_ULONG)
				? (uival_t)tp->t_ival : tp->t_ival;
	case ZCON:
		return (uival_t)tp->t_zval;
	case LCON:
		return ((t = tp->t_type) == T_UINT || t == T_ULONG)
				? (ulval_t)tp->t_lval : tp->t_lval;
	case DCON:
		return dval_to_d(tp);
	}
	cbotch("grabdval");
}

#endif

/* end of n0/fold.c */
