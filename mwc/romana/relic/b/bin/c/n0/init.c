/*
 * n0/init.c
 * C compiler.
 * Initializers.
 * All storage classes.
 */

#ifdef   vax
#include "INC$LIB:cc0.h"
#else
#include "cc0.h"
#endif

init(sp, dp, bo, flex)
register SYM	*sp;
register DIM	*dp;
sizeof_t	bo;
int		flex;
{
	register int	t;
	DIM		*ndp;
	TREE		*tp;
	sizeof_t	width, nbe, nel, ninit;
	int		brace, c, adj;

	t  = sp->s_type;
	width = sdsize(sp, dp);
	ndp = dp;
	if (dp!=NULL && dp->d_type==D_ARRAY) {
		if (dp->d_bound != 0)
			flex = 0;
		ndp = dp->d_dp;
		if (width==0 && !flex)
			cerror("array row has 0 length");
	} else
		flex = 0;
	nel = 0;
	nbe = sdsize(sp, ndp);
	if (nbe != 0)
		nel = width/nbe;
	brace = 0;
	if (s==LBRACE && (dp!=NULL || !istruct(t))) {
		++brace;
		lex();
	} else if (s!=LBRACE && (dp!=NULL || istruct(t)))
		notbook();
	ninit = 0;
	while (s != RBRACE) {
		if (ndp==NULL && istruct(t))
			sinit(sp, bo);
		else if (ndp!=NULL && ndp->d_type==D_ARRAY)
			init(sp, ndp, bo, 0);
		else {
			adj = 0;
			if (s==STRING && t<T_INT && ndp==NULL) {
again:
				instring = '"';
				while ((c = getmap('\"')) >= 0) {
					newtree(sizeof(TREE));
					icollect(bicon((ival_t)c), bo, sp, ndp);
					++ninit;
					++bo;
				}
				instring = 0;
				if (lex() == STRING) {
					++adj;
					goto again;
				}
				if (adj)
					notbook();
				if (!flex && ninit==nel) {
					cwarn("string initializer not terminated by NUL");
					break;
				}
				newtree(sizeof(TREE));
				icollect(bicon((ival_t)0), bo, sp, ndp);
				++ninit;
				break;
			}
			newtree(sizeof(TREE));
			++ininit;
			tp = expr();
			--ininit;
			icollect(tp, bo, sp, ndp);
		}
		++ninit;
		bo += nbe;
		if (s != COMMA)
			break;
		if (ninit>=nel && !flex) {
			if (brace) {
				lex();
				if (s == RBRACE)
					cwarn("trailing ',' in initialization list");
				else {
					cerror("too many initializers");
					iskip();
				}
			}
			break;
		}
		itrail();
	}
	if (brace)
		mustbe(RBRACE);
	if (ninit < nel) {
		c = sp->s_class;
		if (c==C_GDEF || c==C_SIN || c==C_SEX) {
			bput(BLOCK);
			zput(nbe*(nel-ninit));
		}
	}
	if (ninit > nel) {
		if (!flex)
			cerror("too many initializers");
		if (dp != NULL)
			dp->d_bound = ninit;
	}
}

/*
 * Initialize a structure.
 * Create a fake variable for each member;
 * this is necessary for the call to build() in iassign()
 * to do the correct things.
 */
sinit(sp, bo) SYM *sp; sizeof_t bo;
{
	register SYM	*mp;
	register INFO	*ip;
	int		n, brace, c, t, f, bfflag;
	sizeof_t	mo, lo;
	SYM		sym;

	c = sp->s_class;
	t = sp->s_type;
	if (t==T_UNION || t==T_FUNION)
		cerror("cannot initialize unions");
	if (t==T_FSTRUCT || t==T_FUNION)
		return;
	ip = sp->s_ip;
	sym.s_class = c;
	sym.s_value = sp->s_value;
	brace = 0;
	if (s == LBRACE) {
		++brace;
		lex();
	}
	lo = n = f = 0;
	while (s != RBRACE) {
		bfflag = 0;
		if (n >= ip->i_nsp) {
			if (f == 0) {
				cerror("too many structure initializers");
				++f;
			}
			sym.s_type = T_INT;
			sym.s_dp = sym.s_ip = NULL;
			mo = 0;
		} else {
			mp = ip->i_sp[n++];
			sym.s_type = mp->s_type;
			sym.s_dp   = mp->s_dp;
			sym.s_ip   = mp->s_ip;
			mo = mp->s_value;
			if (!isauto(&sym) && lo!=mo) {
				/* Output BLOCK item for padding. */
				bput(BLOCK);
				zput(mo-lo);
				lo = mo;
			}
			if (mp->s_width != 0)		/* bitfield initializer */
				bfflag = bfinit(&sym, mp, ip, &n, bo+mo);
		}
		if (!bfflag)
			init(&sym, sym.s_dp, bo+mo, 0);
		lo += ssize(&sym);
		if (bfflag != 2 && s != COMMA)
			break;
		if (n >= ip->i_nsp && !brace)
			break;
		if (bfflag != 2)
			itrail();	/* skip COMMA, check for RBRACE */
	}
	if (brace)
		mustbe(RBRACE);
	if (t==T_STRUCT && (c==C_GDEF || c==C_SIN || c==C_SEX)) {
		if ((mo = ip->i_size-lo) > 0) {
			bput(BLOCK);
			zput(mo);
		}
	}
}

/*
 * Fetch the next symbol.
 * The current symbol is a COMMA.
 * If the new symbol is '}', the program has said ", }"; this is legal.
 * If you pull the comments here, it becomes not quite so legal.
 */
static
itrail()
{
	lex();
	if (s == RBRACE)
		cwarn("trailing ',' in initialization list");
}

/*
 * An initialization starting with '{' contains too many initializers.
 * Skip initializers to the matching '}' or EOF.
 */
static
iskip()
{
	register int braces;

	braces = 0;
	for(;;) {
		if (s == LBRACE)
			++braces;
		else if (s == RBRACE) {
			if (braces == 0)
				return;
			else
				--braces;
		}
		else if (s == EOF)
			return;
		lex();
	}
}

/*
 * Return true if symbol "sp" is an automatic,
 * in the sense that an assigment statement must be generated
 * for the initialization item.
 * Registers are considered auto.
 */
int
isauto(sp) register SYM *sp;
{
	register int c;

	c = sp->s_class;
	return (c==C_AUTO || c==C_PAUTO || c==C_REG);
}

/*
 * This routine actually performs the initialization.
 * Either an build an assignment node and evaluate it for effect,
 * or output an initialization tree.
 */
icollect(tp, bo, sp, dp)
register TREE	*tp;
sizeof_t	bo;
register SYM	*sp;
DIM		*dp;
{
	register TREE	*ip;

	if (isauto(sp)) {
		iassign(sp, dp, bo, tp);
		return;
	}
	ip = talloc();
	ip->t_op = INIT;
	ip->t_type = sp->s_type;
	ip->t_dp = dp;
	ip->t_ip = sp->s_ip;
	ip->t_lp = tp;
	tput(IEXPR, 0, ip);
}

/*
 * Actually do the assignment for an automatic initialization thing.
 */
iassign(sp, dp, bo, tp)
register SYM	*sp;
register TREE	*tp;
sizeof_t	bo;
DIM		*dp;
{
	register TREE	*ip;
	DIM		*ssdp;

	ssdp = sp->s_dp;
	sp->s_dp = dp;
	ip = bid(sp);
	sp->s_dp = ssdp;
	if (sp->s_class != C_REG)
		ip->t_offs += bo;
	tp = build(ASSIGN, ip, tp);
	tput(EEXPR, 0, tp);
}

/*
 * Collect one or more bitfield initializers.
 * Each bitfield initializer must be an integer constant.
 * Multiple bitfield initializers get output as a single initialization item:
 * look for more initializers if the next
 * structure member is another bitfield at the same offset (s_value).
 * This may or may not need to look ahead to the next initializer,
 * so it returns 1 if it has not read COMMA, 2 if it has read the COMMA.
 */
int
bfinit(sp, mp, ip, np, off) SYM *sp; register SYM *mp; INFO *ip; register int *np; sizeof_t off;
{
	register unsigned long val, mval, mask;
	int status;

	for (val = (unsigned long)0; ; ) {

		status = 1;			/* COMMA not read yet */

		/* Widen the initialized symbol base type if necessary. */
		if (ssize(sp) < ssize(mp))
			sp->s_type = mp->s_type;

		/* Grab the initializer value. */
		++ininit;
		mval = iconexpr();
		--ininit;

		/* Make sure the initializer is appropriate. */
		mask = (((unsigned long)1) << mp->s_width) - 1;
		if ((mval & ~mask) != (unsigned long)0 && (mval & ~mask) != ~mask)
			cwarn("bitfield initializer out of range");
		mval &= mask;			/* mask value to bitfield width */

		/* Build up the initializer in val. */
		val |= (mval << mp->s_offset);	/* put value in the right place */

		/* Check whether more bitfield initializers follow. */
		if (s != COMMA)			/* no more initializers */
			break;
		itrail();			/* skip COMMA */
		++status;			/* COMMA has been read */
		if (s == LBRACE || s == RBRACE	/* { or }, no more bitfield inits */
		 || *np >= ip->i_nsp		/* no more members */
		 || ip->i_sp[*np]->s_width==0	/* next not a bitfield */
		 || ip->i_sp[*np]->s_value != mp->s_value)	/* next at different value */
			break;
		mp = ip->i_sp[(*np)++];		/* repeat for next bitfield */
	}

	/*
	 * Output the bitfield initializer built up in val.
	 * FIX_ME The initializer value collected in unsigned long 'val'
	 * is passed to icollect() as bicon((ival_t)val),
	 * this does not seem not quite right.
	 */
	icollect(bicon((ival_t)val), off, sp, NULL);
	return status;
}

/* end of n0/init.c */
