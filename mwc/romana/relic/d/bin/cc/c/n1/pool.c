/*
 * The routines in this file
 * are responsible for managing the pool of
 * literals. This is needed on a machine that
 * lacks immediate addressing modes.
 */
#ifdef   vax
#include "INC$LIB:cc1.h"
#else
#include "cc1.h"
#endif

/*
 * Data structure for
 * remembering the constants in the pool.
 * A chain of these is built up as the
 * compilation proceeds. The pool entries are
 * written out as new entries are added.
 */
typedef	struct	pool	{
	struct	pool	*p_pp;			/* Link */
	int		p_op;			/* Type of node */
	int		p_lab;			/* Literal label */
	union	{
		ival_t	p_ival;			/* Value of ICON */
		lval_t	p_lval;			/* Value of LCON */
		dval_t	p_dval;
		struct	{
			int	p_label;	/* LID t_label */
			sizeof_t p_loffs;	/* LID t_offs */
		}	pl;
		struct	{
			SYM	*p_sp;		/* GID t_sp */
			sizeof_t p_goffs;	/* GID t_offs */
		}	pg;
	}	pd;
}	POOL;

static	POOL	*poolp	= NULL;			/* The pool */

/*
 * Given a pointer to a tree node with opcode
 * ICON, LCON, DCON, LID or GID,
 * build an entry in the literal pool
 * and rewrite the tree to refer to the label.
 * The only potential fixups left to the caller
 * are a change to the type and a call to amd to set tflag.
 * The pool should ideally be in SLINK without exception,
 * but it isn't possible so we call poolseg(op)
 * (defined in cc1mch.h) to determine where we're going.
 */
pool(tp)
register TREE	*tp;
{
	register POOL	*pp;
	register int	i;
	register int	op;
	register int	old;

	op  = tp->t_op;
	if (op==ICON||op==LCON||op==DCON||op==LID||op==GID||op==ADDR) {
		pp = poolp;
		while (pp != NULL) {
			if (pp->p_op == op) {
				if (op == ICON) {
					if (pp->pd.p_ival == tp->t_ival)
						break;
				} else	if (op == LCON) {
					if (pp->pd.p_lval == tp->t_lval)
						break;
				} else	if (op == DCON) {
					i = 0;
					while (i < sizeof(dval_t)
					&& pp->pd.p_dval[i] == tp->t_dval[i])
						++i;
					if (i == sizeof(dval_t))
						break;
				} else	if (op == LID) {
					if (pp->pd.pl.p_label == tp->t_label
					&&  pp->pd.pl.p_loffs == tp->t_offs)
						break;
				} else	if (op == GID) {
					if (pp->pd.pg.p_sp == tp->t_sp
					&&  pp->pd.pg.p_goffs == tp->t_offs)
						break;
				}
			}
			pp = pp->p_pp;
		}
		if (pp==NULL && (pp=(POOL *)malloc(sizeof(POOL))) != NULL) {
			pp->p_pp = poolp;
			poolp = pp;
			pp->p_op = op;
			pp->p_lab = newlab();
			if (op == ICON)
				pp->pd.p_ival = tp->t_ival;
			else if (op == LCON)
				pp->pd.p_lval = tp->t_lval;
			else if (op == DCON) {
				for (i=0; i<sizeof(dval_t); ++i)
					pp->pd.p_dval[i] = tp->t_dval[i];
			} else if (op == LID) {
				pp->pd.pl.p_label = tp->t_label;
				pp->pd.pl.p_loffs = tp->t_offs;
			} else if (op == GID) {
				pp->pd.pg.p_sp = tp->t_sp;
				pp->pd.pg.p_goffs = tp->t_offs;
			}
			old = newseg(poolseg(op));
			genlab(pp->p_lab);
			if (op==LID || op==GID) {
				iexpr(tp, iptrtype());
			} else
				iexpr(tp, tp->t_type);
			newseg(old);
		} else if (pp == NULL) {
			cnomem("pool malloc");
		}
	} else
		cbotch("unpoolable tree");
	tp->t_op = LID;
	tp->t_label = pp->p_lab;
	tp->t_seg = poolseg(op);
	tp->t_offs = 0;
	if (op == ADDR) {
		poolp = pp->p_pp;
		free(pp);
	}
}

#if OVERLAID
/*
 * Free all of the entries in the
 * literal pool. Called at the end of the
 * code generator phase.
 */
freepool()
{
	register POOL	*p1;
	register POOL	*p2;

	p1 = poolp;
	while (p1 != NULL) {
		p2 = p1->p_pp;
		free((char *) p1);
		p1 = p2;
	}
	poolp = NULL;
}
#endif
