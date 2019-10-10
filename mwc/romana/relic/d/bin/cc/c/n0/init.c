/*
 * C compiler.
 * Initializers.
 * (All storage classes).
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
again:				while ((c = getmap('\"')) >= 0) {
					newtree(sizeof(TREE));
					icollect(bicon(c), bo, sp, ndp);
					++ninit;
					++bo;
				}
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
				icollect(bicon(0), bo, sp, ndp);
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
sinit(sp, bo)
SYM	*sp;
sizeof_t bo;
{
	register SYM	*mp;
	register INFO	*ip;
	register int	n;
	int		brace, c, t, f;
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
			if (mp->s_width != 0)
				cerror("cannot initialize fields");
			if (!isauto(&sym) && lo!=mo) {
				bput(BLOCK);
				zput(mo-lo);
				lo = mo;
			}
		}
		init(&sym, sym.s_dp, bo+mo, 0);
		lo += ssize(&sym);
		if (s != COMMA || (n>=ip->i_nsp && !brace))
			break;
		itrail();
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
isauto(sp)
register SYM	*sp;
{
	register int	c;

	c = sp->s_class;
	if (c==C_AUTO || c==C_PAUTO || c==C_REG)
		return (1);
	return (0);
}

/*
 * This routine actually performs the initialization.
 * Either an assignment node is built up and evaluated for effect,
 * or an initialization tree is output.
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
