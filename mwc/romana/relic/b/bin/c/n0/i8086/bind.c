/*
 * This file contains the machine-specific parts of the parser.
 * It is used in both the SMALL and the LARGE models of segmentation.
 */
#ifdef   vax
#include "INC$LIB:cc0.h"
#else
#include "cc0.h"
#endif

typedef	struct  locals	{
	struct  locals *l_lstp;
	int     l_lofs;
	int     l_mask;
}	LOCALS;

LOCALS	*clstp;		        /* Local stack */
int	clofs;                  /* Current local offset */
int	cmask;			/* Current mask */
int	llofs;			/* Last local offset reported */
int	lmask;			/* Last register mask reported */
int	lastword = T_PTR;	/* Last 16 bit type */
char	inbtr[]	 = "identifier \"%s\" not bound to register";

/*
 * This table is indexed by C type to obtain the size
 * in bytes of a machine object.
 * The T_PTR entry is 2; this is for ONLYSMALL.
 * The entry is patched based on the model.
 */
int     mysizes[] = {
	0,
	1,	1,
	2,	2,
	2,	2,
	2,
	4,	4,
	4,	8,
	0,
	0,	0,
	0,	0,
	0,	0
};

/*
 * This table is indexed by C type to obtain the machine type,
 * as defined in 'mch.h'.
 */
char    mytypes[] = {
	0,
	S8,     U8,
	S16,    U16,
	S16,    U16,
	SPTR,
	S32,    U32,
	F32,    F64,
	S16,
	BLK,    0,
	BLK,    0,
	0,      0
};

/*
 * main() calls this routine to perform machine-dependent setup.
 * On the i8086, it changes the size of a pointer and some other things
 * that alter the allocation of register variables.
 */
vinit()
{
#if !ONLYSMALL
	lastword = T_PTR;
	mysizes[T_PTR] = 2;
	mytypes[T_PTR] = SPTR;
	if (notvariant(VSMALL)) {
		lastword = T_UINT;
		mysizes[T_PTR] = 4;
		mytypes[T_PTR] = LPTR;
	}
#endif
}

/*
 * Empty the local binding stack and mark the registers usable
 * for register variables (SI and DI) as available.
 */
initlocals()
{
	clstp = NULL;
	clofs = 0;
	cmask = BCS|BSS|BSP|BBP;
	llofs = 0;
	lmask = BCS|BSS|BSP|BBP;
	bput(AUTOS);
	iput((ival_t)clofs);
	iput((ival_t)cmask);
}

/*
 * Save the current state of the local bindings.
 */
savelocals()
{
	register LOCALS	*lstp;

	lstp = (LOCALS *) new(sizeof(LOCALS));
	lstp->l_lstp = clstp;
	lstp->l_lofs = clofs;
	lstp->l_mask = cmask;
	clstp = lstp;
}

/*
 * Restore locals.  Tell the code generator if it changes.
 */
restlocals()
{
	register LOCALS	*lstp;

	if ((lstp=clstp) == NULL)
		cbotch("restlocals");
	clstp = lstp->l_lstp;
	clofs = lstp->l_lofs;
	cmask = lstp->l_mask;
	putautos();
	free((char *) lstp);
}

/*
 * Bind locals to the correct place on the stack frame.
 * Where things end up is a function of the model of compilation.
 */
bindlocal(sp)
register SYM	*sp;
{
	register int	c, regno;

	if ((c=sp->s_class) == C_REG) {
		if ((regno=grabreg(sp)) >= 0) {
			sp->s_value = regno;
			return;
		}
		sp->s_class = C_AUTO;
		if (isvariant(VSNREG))
			cstrict(inbtr, sp->s_id);
	}
	if (c==C_AUTO || c==C_REG) {
		clofs -= ssize(sp);
#if	OMF286
		/*
		 * The 80287 does not function properly for arguments
		 * on odd boundaries.
		 * The OMF286 compiler therefore rounds up the offsets
		 * of autos to keep them on even boundaries;
		 * however, char and unsigned char autos are not word-aligned.
		 * The compiler driver must also setvariant(VALIGN) to keep
		 * the stack word-aligned.
		 * fieldalign() should also keep structure members
		 * on word boundaries within the struct, but it does not;
		 * this is a kludge to maintain compatability with PL/M-286,
		 * which does not word-align structure members.
		 * The UDI interface routines fail if structure members
		 * are forced to word boundaries.
		 */
		if (ssize(sp) != 1 && (clofs & 1) != 0)
			--clofs;
#endif
		sp->s_value = clofs;
		if (clofs >= 0)
			cerror("auto \"%s\" is not addressable", sp->s_id);
	}
}

/*
 * Attempt to get a register for symbol 'sp'.
 * Only word things go in registers;
 * this includes pointers in SMALL model.
 */
grabreg(sp)
register SYM	*sp;
{
	register DIM	*dp;
	register int	type;

	if ((dp=sp->s_dp) != NULL) {
#if !ONLYSMALL
		if (isvariant(VSMALL)) {
#endif
			if (dp->d_type == D_PTR) {
				if ((cmask&BSI) == 0) {
					cmask |= BSI;
					return (SI);
				}
				if ((cmask&BDI) == 0) {
					cmask |= BDI;
					return (DI);
				}
			}
#if !ONLYSMALL
		}
#endif
		return (-1);
	}
	type = sp->s_type;
	if (type>=T_SHORT && type<=lastword) {
		if ((cmask&BSI) == 0) {
			cmask |= BSI;
			return (SI);
		}
		if ((cmask&BDI) == 0) {
			cmask |= BDI;
			return (DI);
		}
	}
	return (-1);
}

/*
 * Bind arguments.
 */
bindargs()
{
	register SYM	*sp;
	register int	i;
	register sizeof_t offset;

#if !ONLYSMALL
	if (isvariant(VSMALL))
		offset = 8;
	else
		offset = 10;
#else
	offset = 8;
#endif
	for (i=0; i<nargs; ++i) {
		sp = args[i];
		if (sp->s_class != C_PREG)
			sp->s_class = C_PAUTO;
		sp->s_value = offset;
		offset += ssize(sp);
		if (isvariant(VALIGN) && (offset & 1) != 0)
			++offset;
		if ((short)offset != offset)
		    cerror("parameter \"%s\" is not addressible", sp->s_id);
	}
}

/*
 * Copy register arguments from the stack into the register.
 * Change class of the argument to plain register.
 * Note that bindargs() has put the stack displacement
 * into the 's_value' field of the argument symbol.
 */
loadargs()
{
	register SYM	*sp;
	register int	i, r, v;

	for (i=0; i<nargs; ++i) {
		sp = args[i];
		if (sp->s_class == C_PREG) {
			if ((r=grabreg(sp)) >= 0) {
				v = sp->s_value;
				sp->s_class = C_REG;
				sp->s_value = r;
				loadreg(sp, v);
				continue;
			}
			sp->s_class = C_PAUTO;
			if (isvariant(VSNREG))
				cstrict(inbtr, sp->s_id);
		}
	}
}

/*
 * Load symbol 'sp' from offset 'offset' in the frame.
 * Since register variables are always words,
 * the type can always be setup to T_INT.
 */
loadreg(sp, offset)
SYM	*sp;
{
	register TREE	*lp, *rp, *tp;

	newtree(sizeof(TREE));
	lp = talloc();
	lp->t_op = REG;
	lp->t_type = T_INT;
	lp->t_reg = sp->s_value;
	rp = talloc();
	rp->t_op = PID;
	rp->t_type = T_INT;
	rp->t_offs = offset;
	tp = build(ASSIGN, lp, rp);
	putautos();
	tput(EEXPR, 0, tp);
}

/*
 * Inform the code generator (and perhaps the optimizer)
 * about the current automatic variable allocation.
 * Keep the stack word-aligned if isvariant(VALIGN).
 */
putautos()
{
	if (clofs != llofs || cmask != lmask) {
		if (isvariant(VALIGN) && (clofs & 1) != 0)
			--clofs;
		llofs = clofs;
		lmask = cmask;
		bput(AUTOS);
		iput((ival_t)-clofs);
		iput((ival_t)cmask);
	}
}

/*
 * Look at type 't', DIM list 'dp' and field width 'w' (0 for non fields)
 * and return the bit offset of the base of the structure member.
 * The 'offset' argument is the current bit offset in the structure or union.
 * Check for fields that are too wide, bad field base types, and arrange
 * that the byte or word that spans the field can be grabbed in one grab.
 * See comments in bindlocal() above concerning reals and bitfield alignment.
 */
long
fieldalign(t, dp, ip, w, offset)
register DIM	*dp;
INFO		*ip;
register int	w;
long		offset;
{
	register int	maxwidth;

	if (w != 0) {					/* nonzero width */
		if (dp != 0)
			cerror("non scalar field");
		if ((maxwidth=8*mysizes[t]) > 16)	/* 8 for char, 16 for int */
			cerror("bad base type for field");
		if (w > maxwidth)
			cerror("field too wide");
		if ((offset&7)+w > maxwidth)
			offset = (offset+7) & ~7L;
	} else {					/* zero width */
		offset = (offset+7) & ~7L;
	}
	return (offset);
}

/*
 * Convert a tree into its low level form.
 * Fill in the machine type byte looking at the C type
 * and the segment information.
 */
TREE *
transform(tp, why, above)
register TREE	*tp;
{
	register TREE	*lp;
	register int	t;
	register int	wd;

	/*
	 * The way that this code looks at the DIM lists to decide
	 * what type of conversion is inserted makes me sick.
	 * The correct type of conversion node (MUL, DIV, etc.)
	 * should really be inserted when the tree is constructed,
	 * guided by a table.
	 */
	if (tp->t_op == CONVERT) {
		lp = tp->t_lp;
		if (tp->t_dp!=NULL && lp->t_dp==NULL
		&&  why!=REXPR) {
			tp->t_op = MUL;
			tp->t_dp = NULL;
			if (lp->t_type != T_INT)
				tp->t_lp = bconvert(lp, T_INT, NULL,NULL,NULL);
			tp->t_type = T_INT;
		} else if (tp->t_dp==NULL && lp->t_dp!=NULL && tp->t_rp!=NULL) {
			tp->t_op = DIV;
			lp->t_dp = NULL;
			lp->t_type = T_INT;
		}
	}
	/* Fixup any remaining ZCON's */
	if (tp->t_op == ZCON) {
		tp->t_op = ICON;
		if (tp->t_zval > 0xFFFFL)
			cwarn("sizeof truncated to unsigned");
		if (tp->t_zval < 0x8000L)
			tp->t_type = T_INT;
		tp->t_ival = tp->t_zval;
	}
	if (tp->t_dp != NULL)
		tp->t_type = mytypes[T_PTR];
	else if ((t=tp->t_type)==T_FSTRUCT || t==T_FUNION || t==T_FENUM) {
		unksize(t, tp->t_ip);
		tp->t_type = S16;
	} else {
		if (t == T_ENUM)
			t = tp->t_ip->i_type;
		tp->t_type = mytypes[t];
	}
	if (tp->t_op >= MIOBASE) {
		wd = why;
		if (why == REXPR)
			wd = EEXPR;
		tp->t_lp = transform(tp->t_lp, wd, tp->t_op);
		if (tp->t_op!=FIELD && tp->t_rp!=NULL)
			tp->t_rp = transform(tp->t_rp, wd, -1);
		if (tp->t_op==CONVERT && tp->t_rp==NULL) {
			lp = tp->t_lp;
			if (tp->t_type == lp->t_type)
				tp = lp;
		}
	}
	return (tp);
}

/*
 * Check if the operator 'op', when applied to C types 'lt' and 'rt'
 * (which are outputs from tltype(), which means that all pointers
 * have been converted to type T_PTR), involves no conversions.
 * What this means is that the code generator
 * has to be prepared to deal with the type mismatch.
 * This code has been edited too many times; I am really
 * quite ashamed of the number of bugs that have been
 * tracked down to this code. It is no longer amusing.
 */
noconvert(op, lt, rt)
register int	op;
register int	lt;
register int	rt;
{
	if (op==ASSIGN || op==CAST || (op>=EQ && op<=ULT)) {
		if ((lt>=T_CHAR && lt<=T_UCHAR)
		&&  (rt>=T_CHAR && rt<=T_UCHAR))
			return(1);
		if ((lt>=T_SHORT && lt<=T_UINT)	
		&&  (rt>=T_SHORT && rt<=T_UINT))
			return (1);
		if ((lt>=T_LONG  && lt<=T_ULONG)
		&&  (rt>=T_LONG  && rt<=T_ULONG))
			return (1);
		if (lt==T_PTR && rt==T_PTR)
			return (1);
#if !ONLYSMALL
		if (notvariant(VSMALL)
		&&   op != CAST) {
			if ((lt>=T_SHORT && lt<=T_ULONG)
			&&  (rt>=T_SHORT && rt<=T_ULONG)
			&&  (lt==T_PTR || rt==T_PTR))
				return (1);
		} else if (isvariant(VSMALL)) {
#endif
			if ((lt>=T_SHORT && lt<=T_PTR)
			&&  (rt>=T_SHORT && rt<=T_PTR))
				return (1);
#if !ONLYSMALL
		}
#endif
		if (op==CAST && lt==T_FLOAT && rt==T_DOUBLE)
			return (1);
	}
	return (0);
}

/*
 * Put out an ALIGN object of the correct flavor to align
 * the location counter well enough for the object to which 'sp' points.
 * On the i8086 there is only one type of alignment,
 * so the argument is unused.
 */
align(sp)
SYM	*sp;
{
	bput(ALIGN);
	bput(0);
}

/*
 * Check object sizes for overflow.
 */
sizeof_t szcheck(n, a, s) sizeof_t n; int a; char *s;
{
	if (n < 0x10000L)
		return n;
	if (a == 0 || (n & ~MAXUV) != 0) {
		cerror("size of %s too large", s);
		return(0);
	}
	if (isvariant(VSLCON))
		cstrict("size of %s overflows fsize_t", s);
	return (n);
}
