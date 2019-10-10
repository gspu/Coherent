/*
 * n0/expr.c
 * C compiler.
 * Expressions et al.
 */

#ifdef   vax
#include "INC$LIB:cc0.h"
#else
#include "cc0.h"
#endif

#define  LOW    0
#define	 HIGH	0177
#define	 INARGS	0200

#define	bcvint(p)	bconvert((p), T_INT, NULL, NULL, NULL)
#define	isfloat(t)	((t)==T_FLOAT || (t)==T_DOUBLE)
#define	isshift(op)	((op)==ASHL || (op)==ASHR || (op)==SHL || (op)==SHR)
#define	issgnint(t)	((t)== T_CHAR || (t)== T_SHORT || (t)== T_INT || (t)== T_LONG)
#define	isunsint(t)	((t)==T_UCHAR || (t)==T_USHORT || (t)==T_UINT || (t)==T_ULONG)

static	char	tclash[] = "type clash";

/*
 * Read a tree.
 * This is the top level routine that is called by the user.
 * The mysterious call to "build" with opcode "NOP" is to get any function or
 * array name conversions applied, even if the top of the tree is a term.
 * Ditto for getting types converted to computables.
 */
TREE *
expr()
{
	if (notvariant(VCPP)) {
		bput(LINE);
		iput((ival_t) line);
	}
	return build(NOP, tree(LOW), NULL);
}

/*
 * Read in a tree.
 * The "n" argument is a firewall precedence.
 * You continue reading until the precedence of the operator
 * in the input is less than the priority "n".
 * The "INARGS" flag is or'ed into the priority to indicate
 * that the expression is an argument list, and that "," is special.
 * Passing it in this unconventional way lets you pass a single
 * parameter to "tree".
 */
TREE *
tree(n)
{
	register TREE	*lp, *rp;
	register SYM	*sp;
	DIM		*dp;
	TREE		*tp, *fp;
	int		c, d, op, p, isarray, isinargs;

	isinargs = 0;
	if ((n&INARGS) != 0) {
		++isinargs;
		n &= ~INARGS;
	}
	lp = term();
	for (;;) {
		switch (s) {

		case LPAREN:
			lex();
			rp = NULL;
			if (s != RPAREN)
				rp = tree(LOW|INARGS);
			mustbe(RPAREN);
			lp = build(CALL, lp, rp);
			break;

		case LBRACK:
			lex();
			lp = build(ADD, lp, tree(LOW));
			adjust(lp, lp->t_type, lp->t_dp, lp->t_ip);
			lp = build(STAR, lp, NULL);
			mustbe(RBRACK);
			break;

		case DOT:
		case ARROW:
			lp = build((s==DOT?ADDR:NOP), lp, NULL);
			if (lex() != ID) {
				cerror("missing member");
				break;
			}
			sp = moslookup(lp);
			if (bitcompat(T_PTR, tltype(lp)) == 0)
				cerror("left side of \"%s\" not usable",
					s==DOT?".":"->");
			if (sp==NULL || ((c=sp->s_class)!=C_MOS && c!=C_MOU)) {
				cerror("member \"%s\" is not defined", id);
				sp = fakedef(SL_MOS);
			} else if (isvariant(VSMEMB))
				csmemb(lp, sp);
			lex();
			tp = talloc();
			tp->t_op = ADD;
			tp->t_lp = lp;
			tp->t_rp = bicon(sp->s_value);
			lp = tp;
			isarray = 0;
			dp = sp->s_dp;
			if (dp!=NULL && dp->d_type==D_ARRAY) {
				++isarray;
				dp = tdalloc(dp->d_dp, D_MOSAR, dp->d_bound);
			} else
				dp = tdalloc(dp, D_PTR, (sizeof_t)0);
			adjust(lp, sp->s_type, dp, sp->s_ip);
			if (!isarray)
				lp = build(STAR, lp, NULL);
			if (sp->s_width != 0) {
				fp = talloc();
				fp->t_op = FIELD;
				fp->t_type = lp->t_type;
				fp->t_lp = lp;
				fp->t_width = sp->s_width;
				fp->t_base = sp->s_offset;
				lp = fp;
			}
			break;

		case INCBEF:
		case DECBEF:
			op = s+INCAFT-INCBEF;
			lex();
			lp = build(op, lp, bicon((ival_t)1));
			break;

		default:
			if ((s==COMMA && ininit!=0 && isinargs==0)
			||  (s==COLON))
				goto out;
			if (s<MIOBASE || s>=MDOBASE)
				goto out;
			d = opdope[s-MIOBASE];
			if ((p=(d&PRIO)) == 0)
				goto out;
			if (p<n || ((d&RAS)==0 && p==n))
				goto out;
			if ((op = s) == QUEST) {
				lex();
				rp = tree(LOW);
				mustbe(COLON);
				rp = build(COLON, rp, tree(--p));
				lp = build(QUEST, lp, rp);
				break;
			}
			if (op==COMMA && isinargs!=0)
				op = ARGLST;
			lex();
			lp = build(op, lp, tree(p));
		}
	}
out:
	return lp;
}

/*
 * Check that a structure member is
 * indeed a member of the left hand structure.
 * If the member is untagged, don't object to any clash.
 * The structure declaration reader sets the S_TAG bit
 * in the flags word of all tagged structures.
 */
csmemb(lp, sp)
TREE	*lp;
SYM	*sp;
{
	register INFO	*ip;
	register SYM	*tsp;
	register int	type;
	char		*msg;
	int 		i;

	type = lp->t_type;
	if (lp->t_dp==NULL || lp->t_dp->d_type!=D_PTR
	|| type<T_STRUCT || type>T_FUNION) {
		cstrict("potentially nonportable structure access");
		return;
	}
	if ((type==T_STRUCT || type==T_UNION) && (sp->s_flag&S_TAG)!=0) {
		ip = lp->t_ip;
		if (ip != NULL) {
			for (i=0; i<ip->i_nsp && ip->i_sp[i]!=sp; ++i)
				;
			if (i >= ip->i_nsp) {
				tsp = taglookup(ip);
				if (tsp != NULL) {
					msg = (tsp->s_class == C_UTAG) ? "union" : "structure";
					if (tsp->s_id[0] != '.')
						cstrict("%s \"%s\" does not contain member \"%s\"",
							msg, tsp->s_id, sp->s_id);
					else
						cstrict("%s does not contain member \"%s\"",
							msg, sp->s_id);
				}
			}
		}
	}
}

/*
 * Read a term.
 * Handle all of the leaf nodes and the unary operations here.
 */
TREE *
term()
{
	register TREE	*tp;
	register SYM	*sp;
	register int	op;
	TREE 		*cp;
	DIM		*dp;
	int		tt, lt, n;

	switch (op = s) {

	case ICON:
		tp = bicon(ival);
		tp->t_type = tval;
		lex();
		break;

	case LCON:
		tp = talloc();
		tp->t_op = LCON;
		tp->t_type = tval;
		tp->t_lval = lval;
		lex();
		break;

	case DCON:
		tp = talloc();
		tp->t_op = DCON;
		tp->t_type = tval;
		memcpy((char *)tp->t_dval, (char *)dval, sizeof(dval_t));
		lex();
		break;

	case STRING:
		tp = bstring();
		break;

	case ID:
		if ((sp=reflookup(SL_VAR)) == NULL) {
			/*
			 * An undefined name followed by a "(" is declared
			 * to be a function that returns an integer.
			 * So says the manual.
			 */
			TOK *tp;
			tp = idp;
			if (spnextis('(')) {
				setid(tp->t_id);
				sp = deflookup(SL_VAR, 0);
				dp = tackdim(NULL, D_FUNC, 0);
				sp = declare(sp, C_CXT, T_INT, dp, NULL, 0);
			} else
				setid(tp->t_id);
			if (sp == NULL) {
				cerror("identifier \"%s\" is not defined", id);
				sp = fakedef(SL_VAR);
			}
		}
		tp = bid(sp);
		lex();
		break;

	case LPAREN:
		lex();
		if ((cp=cast()) != NULL) {
			if (cp->t_type==T_FLOAT && cp->t_dp==NULL)
				cp->t_type = T_DOUBLE;
			cp->t_lp = build(NOP, tree(HIGH), NULL);
			tt = tltype(cp);
			lt = tltype(cp->t_lp);
			if (tt == T_STRUCT)
				cerror("cannot cast to structure or union");
			if (lt == T_STRUCT)
				cerror("cannot cast structure or union");
			if (tt == T_PTR && isfloat(lt))
				cerror("cannot cast double to pointer");
			if (isfloat(tt) && lt == T_PTR)
				cerror("cannot cast pointer to double");
			if (bitcompat(tt, lt)) {
				tp = cp->t_lp;
				adjust(tp, cp->t_type, cp->t_dp, cp->t_ip);
				if (tp->t_op == CONVERT && tp->t_rp == NULL)
					tp->t_op = CAST;
			} else
				tp = cp;
			break;
		}
		n = ininit;
		ininit = 0;
		tp = tree(LOW);
		ininit = n;
		mustbe(RPAREN);
		break;

	case SIZEOF:
		lex();
		if (s == LPAREN) {
			lex();
			if ((tp=cast()) == NULL) {
				tp = tree(LOW);
				mustbe(RPAREN);
			}
		} else
			tp = tree(HIGH);
		tp = build(SIZEOF, tp, NULL);
		break;

	case NOT:
		lex();
		tp = build(EQ, tree(HIGH), bicon((ival_t)0));
		break;

	case SUB:
		op = NEG;
		goto unary;
	case ADD:	/* Unary plus == no rearrangement FIX.ME */
		lex();
		tp = tree(HIGH);	/* ignored for present */
		break;
		op = POS;
		goto unary;
	case MUL:
		op = STAR;
		goto unary;
	case AND:
		op = ADDR;
		goto unary;
	case COM:
	unary:
		lex();
		tp = build(op, tree(HIGH), NULL);
		break;

	case INCBEF:
	case DECBEF:
		lex();
		tp = build(op, tree(HIGH), bicon((ival_t)1));
		break;

	default:
		cerror("error in expression syntax");
		while (s!=EOF && s!=SEMI && s!=LBRACE && s!=RBRACE)
			skip();
		return bicon((ival_t)0);
	}
	return tp;
}

/*
 * Given an operator "op" and left and right subtrees "lp" and "rp",
 * build a tree node, folding any constant expressions
 * that can be folded, supplying any required type conversions.
 * All of the conversions that are possible are added;
 * the routine "transform" may remove some of them.
 */
TREE *
build(op, lp, rp) int op; TREE *lp; TREE *rp;
{
	TREE	*tp, *gtp, gt;
	DIM	*dp;
	int	rt, lt, d, cv;
	sizeof_t ll, lr;

	if (op == SIZEOF)
		return bzcon(tsize(lp));
	rt = T_NONE;
	if (rp != NULL) {
		chmos(rp);
		rp = chfun(charray(rp));
		rt = tltype(rp);
	}
	chmos(lp);
	lp = charray(lp);
	if (op!=ADDR && op!=CALL)
		lp = chfun(lp);
	lt = tltype(lp);
	if (op == NOP)
		goto conversion;
	/*
	 * Make sure that the operands are
	 * suitable as operands of the specified operation.
	 * Look mainly at the type, although some consideration
	 * is made of storage class.
	 */
	if (lt==T_VOID || rt==T_VOID) {
		if ((op!=COMMA && op!=QUEST && op!=COLON)
		 || (op==COLON && lt!=rt)
		 || (op==QUEST && lt==T_VOID)) {
			cerror("illegal operation on \"void\" type");
			lt = T_INT;
			rt = T_INT;
		}
	}
	if (op==ADDR && lp->t_op==REG)
		cerror("cannot apply unary '&' to a register variable");
	if (op==ADDR && lp->t_op==FIELD)
		cerror("cannot apply unary '&' to a bit field");
	if (op==ADDR && lp->t_op==GID && lp->t_seg==SALIEN)
		cerror("cannot apply unary '&' to an alien function");
	d = opdope[op-MIOBASE];
	if ((d&RLVL) != 0
	 && (lp->t_op<=DCON
	  || (lp->t_op>=MIOBASE && lp->t_op!=STAR && lp->t_op!=FIELD
	   && (lp->t_op!=CALL || lp->t_type!=T_STRUCT || lp->t_dp!=NULL)
	   )))
		cerror("lvalue required");
	if ((d&NFLT)!=0 && (isfloat(lt) || isfloat(rt)))
		cerror("illegal use of floating point");
	if (((d&NPTL)!=0 && lt==T_PTR) || ((d&NPTR)!=0 && rt==T_PTR))
		cerror("illegal use of a pointer");
	if (((d&NSTL)!=0 && lt==T_STRUCT) || ((d&NSTR)!=0 && rt==T_STRUCT))
		cerror("illegal use of a structure or union");
	if ((d&RTOL) != 0)
		truth(lp);
	if ((d&RTOR) != 0)
		truth(rp);
	/*
	 * The operation may just fold away to nothing.
	 * If so, return a pointer to the new, constant tree node.
	 */
	if ((tp=fold0(op, lp, rp)) != NULL)
		return tp;
	/*
	 * Insert conversion nodes.
	 */
conversion:
	gt.t_op = CONVERT;
	gt.t_dp = NULL;
	gt.t_ip = NULL;
	switch (op) {

	case QUEST:
		if (rp->t_op != COLON)
			cerror("mismatched conditional");
		/* Make sure the condition type 'lt' is computational. */
		gt.t_type = (cvdope[13 * (lt - T_CHAR) + lt - T_CHAR] & GOAL);
		gt.t_dp = lp->t_dp;
		gt.t_ip = lp->t_ip;
		if (bitcompat(tltype(&gt), lt) == 0)
			lp = bcvt(lp, &gt);
		/* then fall through... */

	case COMMA:
	case ARGLST:
		gt.t_type = rp->t_type;
		gt.t_dp = rp->t_dp;
		gt.t_ip = rp->t_ip;
		break;

	case ANDAND:
	case OROR:
	case NOT:
		gt.t_type = T_INT;
		break;

	case CALL:
		if ((dp = lp->t_dp)!=NULL && dp->d_type==D_FUNC)
			gt.t_dp = dp->d_dp;
		else if ((dp = lp->t_dp)!=NULL
		      && dp->d_type==D_PTR
		      && dp->d_dp->d_type==D_FUNC) {
			cwarn("implicit '*' added to function call");
			lp = build(STAR, lp, NULL);
			gt.t_dp = dp->d_dp->d_dp;
		} else
			cerror("call of non function");
		if (rp != NULL && rp->t_op != ARGLST) {
			rp = build(NOP, rp, NULL);
			rt = tltype(rp);
		}
		gt.t_type = lp->t_type;
		gt.t_ip = lp->t_ip;
		break;

	case STAR:
		if ((dp = lp->t_dp)!=NULL && dp->d_type==D_PTR)
			gt.t_dp = dp->d_dp;
		else
			cerror("indirection through non pointer");
		gt.t_type = lp->t_type;
		gt.t_ip = lp->t_ip;
		break;

	case ADDR:
		gt.t_type = lp->t_type;
		gt.t_dp = tdalloc(lp->t_dp, D_PTR, (sizeof_t)0);
		gt.t_ip = lp->t_ip;
		break;

	default:
		if (rt == T_NONE)
			rt = lt;
		if (isshift(op)) {
			/*
			 * Shift ops: convert right op to T_INT,
			 * read dope table accordingly.
			 */
			if (bitcompat(T_INT, rt) == 0)
				rp = bcvint(rp);
			rt = T_INT;
		}
		cv = cvdope[13*(lt-T_CHAR) + rt - T_CHAR];
		if (noconvert(op, lt, rt))
			cv &= ~(CVL|CVR|CVRA|CARA);
		else if (isshift(op))
			cv &= ~(CVR|CVRA);
		gt.t_type  = cv&GOAL;
		gt.t_rp = NULL;
		if ((cv&(GTL|GTR)) != 0) {
			gtp = ((cv&GTL)!=0) ? lp : rp;
			gt.t_type = gtp->t_type;
			gt.t_dp = gtp->t_dp;
			gt.t_ip = gtp->t_ip;
			if ((cv&(CVL|CVR|CVRA)) != 0)
				gt.t_rp = bzcon(psize(gtp));
		}
		if (gt.t_type == T_NONE && op!=NOP && lt!=T_VOID)
			cerror(tclash);
		if ((d&ASGN) != 0) {
			gt.t_type = lp->t_type;
			gt.t_dp = lp->t_dp;
			gt.t_ip = lp->t_ip;
			if (gt.t_dp == NULL) {
				if (gt.t_type==T_CHAR
				 || gt.t_type==T_UCHAR
				 || gt.t_type==T_SHORT
				 || gt.t_type==T_ENUM
				 || gt.t_type==T_FENUM)
					gt.t_type = T_INT;
				if (gt.t_type == T_USHORT)
					gt.t_type = T_UINT;
				if (gt.t_type == T_FLOAT)
					gt.t_type = T_DOUBLE;
			}
/*
 * The following ADIV and AREM type conversion code is somewhat bogus.
 * The basic problem is that C requires "a op= b;" to give the same
 * result as "a = a op b;".
 * The code below makes sure that an unsigned right op remains unsigned,
 * so the code tables can generate an unsigned op rather than a signed op.
 * Actually, the problem is more general; "i *= 1.5;" should have the same
 * effect as "i = (double)i * 1.5;" but it currently does "i *= (int) 1.5;".
 * This code the most obvious inconsistencies, though.
 */
			if ((cv&CVRA) != 0) {
				if ((op == ADIV || op == AREM)
				 && issgnint(gt.t_type) && isunsint(rt)) {
					gt.t_type = sgn2uns(gt.t_type);
					rp = bcvt(rp, &gt);
					gt.t_type = uns2sgn(gt.t_type);
				} else
					rp = bcvt(rp, &gt);
			} else if ((cv&CARA) != 0) {
				gt.t_op = CAST;
				gt.t_rp = NULL;
				rp = bcvt(rp, &gt);
			}
		} else {
			if (op>=INCBEF && op<=DECAFT)
				cv &= ~CVL;
			if (op == COLON && lt != rt) {
				if (lt == T_PTR) {
					if (iszero(rp)) {
						gt.t_op = CAST;
						gt.t_rp = NULL;
					} else
						cerror(tclash);
				} else if (rt == T_PTR) {
					if (iszero(lp)) {
						gt.t_op = CAST;
						gt.t_rp = NULL;
					} else
						cerror(tclash);
				}
			} else if (op == COLON && lt==T_VOID && rt==T_VOID)
				gt.t_type = T_VOID;
			if ((cv&CVL) != 0)
				lp = bcvt(lp, &gt);
			if (op==SHL || op==SHR) {
				gt.t_type = lp->t_type;
				gt.t_dp = lp->t_dp;
				gt.t_ip = lp->t_ip;
			} else if (rp!=NULL && (cv&CVR)!=0)
				rp = bcvt(rp, &gt);
			if ((cv&UREL)!=0 && (op>=GT && op<=LT))
				op += UGT-GT;
			if (op>=EQ && op<=ULT) {
				gt.t_type = T_INT;
				gt.t_dp = gt.t_ip = NULL;
			}
		}
	}
	if (op==NOP)
		return lp;
	tp = talloc();
	tp->t_op = op;
	tp->t_type = gt.t_type;
	tp->t_dp = gt.t_dp;
	tp->t_ip = gt.t_ip;
	tp->t_lp = lp;
	tp->t_rp = rp;
	if (lt==T_PTR && rt==T_PTR) {
		if (op == ADD)
			cerror("cannot add pointers");
		if (op == SUB) {
			if ((ll=psize(lp)) != (lr=psize(rp)))
				cerror("illegal subtraction of pointers");
			tp = bconvert(tp, T_INT, NULL, NULL, bzcon(ll));
		}
	}
	if (op==ASSIGN && lt==T_STRUCT && rt==T_STRUCT) {
		notbook();
		if ((ll=tsize(lp)) != (lr=tsize(rp)))
			cerror("illegal structure assignment");
	}
	if (isvariant(VSPVAL) && lt != rt
	 && ((lt==T_PTR && !iszero(rp)) || (rt==T_PTR && !iszero(lp)))) {
		if (op == ASSIGN)
			cstrict("integer pointer pun");
		else if (op >= EQ && op <= ULT)
			cstrict("integer pointer comparison");
	}
	return tp;
}

/*
 * If the first entry in the dimensions of tree "tp" is a
 * MOS array, stomp it into the pointer it truly is.
 */
chmos(tp)
TREE *tp;
{
	register struct dim *dp;

	if ((dp = tp->t_dp)!=NULL && dp->d_type==D_MOSAR) {
		dp->d_type = D_PTR;
		dp->d_bound = 0;
	}
}

/*
 * Read constant expression.
 * Call the standard expression reader, and make sure that
 * the folder has squashed it down to nothing.
 * Return the value.
 */
ival_t
iconexpr()
{
	register TREE *tp;

	tp = expr();
	while (tp->t_op == CAST || tp->t_op == CONVERT)
		tp = tp->t_lp;
	if (tp->t_op == ICON)
		return tp->t_ival;
	if (tp->t_op == LCON && (ival_t)tp->t_lval == tp->t_lval)
		return (ival_t)tp->t_lval;
	if (tp->t_op == ZCON && (ival_t)tp->t_zval == tp->t_zval)
		return (ival_t)tp->t_zval;
	cerror("int constant expression required");
	return (ival_t)0;
}

/*
 * Look for a constant zero of integral type.
 */
iszero(tp)
register TREE *tp;
{
	register int op;

	if (tp != NULL) {
		while ((op = tp->t_op) == CONVERT || op == CAST)
			tp = tp->t_lp;
		if (op == ICON && tp->t_ival == 0)
			return 1;
		if (op == LCON && tp->t_lval == 0L)
			return 1;
	}
	return 0;
}

/*
 * Build a conversion node.
 * Used by build and stat when bcvt is unsuitable.
 */
TREE *
bconvert(lp, gt, gdp, gip, rp)
TREE	*lp, *rp;
DIM	*gdp;
INFO	*gip;
{
	TREE t;

	t.t_op = CONVERT;
	t.t_type = gt;
	t.t_dp = gdp;
	t.t_ip = gip;
	t.t_rp = rp;
	return bcvt(lp, &t);
}

/*
 * Build a conversion or cast node.
 */
TREE *
bcvt(lp, gtp)
TREE *lp, *gtp;
{
	register TREE *tp;

	tp = lp;
	tp = talloc();
	tp->t_op = gtp->t_op;
	tp->t_type = gtp->t_type;
	tp->t_dp = gtp->t_dp;
	tp->t_ip = gtp->t_ip;
	tp->t_lp = lp;
	tp->t_rp = gtp->t_rp;
	return tp;
}

/*
 * Given a pointer to its symbol table node,
 * return a pointer to the tree node for an identifier.
 * Enumeration tags get changed into integer constants here.
 */
TREE *
bid(sp)
register SYM	*sp;
{
	register TREE *tp;

	sp->s_flag |= S_USED;
	if (sp->s_class == C_MOE)
		return bicon(sp->s_value);
	tp = talloc();
	switch (sp->s_class) {

	case C_AUTO:
		tp->t_op = AID;
		tp->t_offs = sp->s_value;
		break;

	case C_PAUTO:
		tp->t_op = PID;
		tp->t_offs = sp->s_value;
		break;

	case C_SIN:
		tp->t_op = LID;
		tp->t_seg = sp->s_seg;
		tp->t_label = sp->s_value;
		break;

	case C_REG:
		tp->t_op = REG;
		tp->t_reg = sp->s_value;
		break;

	case C_GDEF:
	case C_GREF:
	case C_CXT:
	case C_SEX:
		tp->t_op = GID;
		tp->t_seg = sp->s_seg;
		tp->t_sp = sp;
		break;

	default:
		cerror("identifier \"%s\" not usable", sp->s_id);
		tp->t_op = ICON;
		tp->t_type = T_INT;
		return tp;
	}
	tp->t_type = sp->s_type;
	tp->t_dp   = sp->s_dp;
	tp->t_ip   = sp->s_ip;
	return tp;
}

/*
 * Build ICON tree node.
 */
TREE *
bicon(n) ival_t n;
{
	register TREE *tp;

	tp = talloc();
	tp->t_op = ICON;
	tp->t_type = T_INT;
	tp->t_ival = n;
	return tp;
}

TREE *
bzcon(n)
sizeof_t n;
{
	register TREE *tp;

	tp = talloc();
	tp->t_op = ZCON;
	tp->t_type = T_UINT;
	tp->t_zval = n;
	return tp;
}

/*
 * Read in a string and hide it in the string section.
 * Return a pointer to a local identifier node;
 * the local identifier is planted at the start of the string.
 * The type of the node is set to array of char.
 */
static
TREE *
bstring()
{
	register TREE	*tp;
	register int	c, nbb, nbs;
	int		old, lab, adj;
	char 		bb[11];

	old = newseg(SSTRN);
	lab = here();
	nbb = nbs = adj = 0;
again:
	instring = '"';
	while ((c=getmap('\"')) >= 0) {
		++nbs;
		if (nbb >= 10) {		/* Leave space for NUL */
			bsflush(bb, nbb);
			nbb = 0;
		}
		bb[nbb++] = c;
	}
	instring = 0;
	if (lex() == STRING) {			/* e.g. "Hello " "world" */
		++adj;
		goto again;
	}
	bb[nbb++] = 0;
	bsflush(bb, nbb);
	if (adj)
		notbook();
	newseg(old);
	tp = talloc();
	tp->t_op = LID;
	tp->t_type = T_CHAR;
	tp->t_dp = tdalloc(NULL, D_ARRAY, (sizeof_t)nbs+1);
	tp->t_label = lab;
	tp->t_seg = SSTRN;
	return tp;
}

/*
 * Dump out the string buffer.
 */
bsflush(bb, nbb)
register unsigned char	*bb;
register int	nbb;
{
	bput(IBLOCK);
	bput(nbb);
	while (nbb--)
		bput(*bb++);
}

/*
 * Make all the types and dims in a tree agree with the viewpoint at the top.
 * This is used by casts and the code that builds up structure access trees.
 */
adjust(tp, t, dp, ip)
register TREE	*tp;
register DIM	*dp;
INFO	 	*ip;
{
	register int	op;

again:
	tp->t_type = t;
	tp->t_dp   = dp;
	tp->t_ip   = ip;
	if ((op = tp->t_op) == ADDR) {
		if (dp==NULL || (dp->d_type!=D_PTR && dp->d_type!=D_MOSAR))
			return;
		dp = dp->d_dp;
		tp = tp->t_lp;
		goto again;
	}
	if (op == STAR) {
		dp = tdalloc(dp, D_PTR, (sizeof_t)0);
		tp = tp->t_lp;
		goto again;
	}
	if (dp!=NULL && dp->d_type==D_MOSAR)
		dp = tdalloc(dp->d_dp, D_PTR, (sizeof_t)0);
	if (op==CAST && bitcompat(tltype(tp), tltype(tp->t_lp))) {
		tp = tp->t_lp;
		goto again;
	}
}

/*
 * If the tree "tp" is a pointer,
 * return the size of the item to which the pointer points;
 * if it is not a pointer, return 1.
 */
sizeof_t psize(tp)
register TREE	*tp;
{
	register DIM	*dp;
	register sizeof_t n;

	dp = tp->t_dp;
	if (dp==NULL || dp->d_type!=D_PTR)
		return 1;
	tp->t_dp = dp->d_dp;
	n = tsize(tp);
	tp->t_dp = dp;
	return n;
}

/*
 * Check if a tree has type "function returning ...".
 * If it does, modify the tree to add the free "address of".
 */
TREE *
chfun(tp)
register TREE *tp;
{
	register DIM	*dp;

	dp = tp->t_dp;
	if (dp!=NULL && dp->d_type==D_FUNC) {
		tp = build(ADDR, tp, NULL);
		adjust(tp, tp->t_type, tp->t_dp, tp->t_ip);
	}
	return tp;
}

/*
 * Check for arrays.
 * Add the free '&' operation.
 * We build the node ourselves (rather than calling build)
 * to avoid a recursive call to this routine on
 * a multidimensional structure.
 */
TREE *
charray(tp)
register TREE	*tp;
{
	register TREE	*ap;
	register DIM	*dp;

	dp = tp->t_dp;
	if (dp!=NULL && dp->d_type==D_ARRAY) {
		ap = talloc();
		ap->t_op = ADDR;
		ap->t_type = tp->t_type;
		ap->t_dp = tdalloc(dp->d_dp, D_PTR, (sizeof_t)0);
		ap->t_ip = tp->t_ip;
		ap->t_lp = tp;
		tp = ap;
		adjust(tp, tp->t_type, tp->t_dp, tp->t_ip);
	}
	return tp;
}

/*
 * Given a tree node, get a type for the tree that is
 * usable for indexing into the type conversion tables.
 * All non-scalars are pointers.
 * All structures and unions are structures.
 * All enumerations are set right.
 */
tltype(tp)
register TREE *tp;
{
	register int	type;

	if (tp->t_dp != NULL)
		return T_PTR;
	if ((type=tp->t_type) == T_FENUM)
		return T_INT;
	if (type == T_ENUM)
		return tp->t_ip->i_type;
	if (type > T_VOID)
		return T_STRUCT;
	return type;
}

/*
 * Check if two linear types are "bit compatable".
 */
bitcompat(t1, t2)
{
	return noconvert(CAST, t1, t2);
}

/*
 * Allocate a new dim structure in the tree area.
 * Used to create pointers in the tree.
 */
DIM *
tdalloc(p, t, b)
DIM	*p;
int	t;
sizeof_t b;
{
	register DIM	*dp;

	dp = (struct dim *) talloc();
	dp->d_dp = p;
	dp->d_type = t;
	dp->d_bound = b;
	return dp;
}

/*
 * Output a tree.
 */
tput(why, lab, tp)
register why;
register TREE *tp;
{
	tp = transform(tp, why, -1);
	bput(why);
	if (why==TEXPR || why==FEXPR)
		iput((ival_t) lab);
	tput1(tp);
}

/*
 * Output a tree, part II.
 */
tput1(tp)
register TREE *tp;
{
	register int	op;

	if (tp == NULL) {
		iput((ival_t) NIL);
		return;
	}
	op = tp->t_op;
	iput((ival_t) op);
	bput(tp->t_type);
	if (tp->t_type == BLK)
		iput((ival_t)tp->t_ip->i_size);
	switch (op) {

	case ICON:
		iput((ival_t) tp->t_ival);
		break;

	case LCON:
		lput(tp->t_lval);
		break;

	case ZCON:
		zput(tp->t_zval);
		break;

	case DCON:
		dput(tp->t_dval);			/* An array */
		break;

	case AID:
	case PID:
	case LID:
	case GID:
		zput(tp->t_offs);
		if (op==LID || op==GID) {
			bput(tp->t_seg);
			if (op == LID)
				iput((ival_t) tp->t_label);
			else
				nput(tp->t_sp->s_id);
		}
		break;

	case REG:
		iput((ival_t) tp->t_reg);
		break;

	case FIELD:
		bput(tp->t_width);
		bput(tp->t_base);
		tput1(tp->t_lp);
		break;

	default:
		tput1(tp->t_lp);
		tput1(tp->t_rp);
	}
}

/* end of n0/expr.c */
