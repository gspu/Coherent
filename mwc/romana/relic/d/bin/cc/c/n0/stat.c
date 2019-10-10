/*
 * C compiler.
 * Statement compilation.
 * The routines in this file compile C statements.
 * This file also contains a number of small routines that are
 * only used by the statement compilation code.
 * This is the new VAX/VMS base version.
 */
#ifdef   vax
#include "INC$LIB:cc0.h"
#else
#include "cc0.h"
#endif

/*
 * Compile one statement.
 * On entry, 's' contains the first symbol of the new statement.
 * On exit, 's' contains the first token beyond the end of the statement.
 */
statement()
{
	register TREE	*tp;
	register int	lab1, lab2;
	TREE 		*lp, *rp;
	TREE 		*itree, *ctree, *stree;
	SBLOCK		*ssbp;
	int 		i, sclab, sblab, tt;
	ival_t		v;

loop:
	dbstat(s, line);
	switch (s) {

	case EOF:
		cerror("unexpected end of file");
		break;

	case SEMI:
		lex();
		break;

	case LBRACE:
		lex();
		savelocals();
		++llex;
		locals();
		putautos();
		while (s!=EOF && s!=RBRACE)
			statement();
		mustbe(RBRACE);
		--llex;
		downlex();
		dbstat(RBRACE, line);
		restlocals();
		break;

	case BREAK:
		lex();
		if (cblab == 0)
			cerror("break not in a loop");
		jump(cblab);
		mustbe(SEMI);
		break;

	case CONTINUE:
		lex();
		if (cclab == 0)
			cerror("continue not in a loop");
		jump(cclab);
		mustbe(SEMI);
		break;

	case CASE:
		lex();
		newtree(sizeof(TREE));
		++incase;
		v = iconexpr();
		--incase;
		mustbe(COLON);
		if (sbp == NULL)
			cerror("case not in a switch");
		else
			newcase(v);
		goto loop;

	case DEFAULT:
		lex();
		mustbe(COLON);
		if (sbp == NULL)
			cerror("default label not in a switch");
		else {
			if (sbp->sb_dlab != 0)
				cerror("only one default label allowed");
			sbp->sb_dlab = here();
		}
		goto loop;

	case GOTO:
		lex();
		if (s != ID) {
			cerror("missing label name in goto");
			skip();
		} else {
			dogoto();
			lex();
			mustbe(SEMI);
		}
		break;

	case IF:
		lex();
		tp = pexpr();
		truth(tp);
		tput(FEXPR, lab1=newlab(), tp);
		statement();
		if (s == ELSE) {
			lab2 = newlab();
			jump(lab2);
			label(lab1);
			lex();
			statement();
			label(lab2);
			break;
		}
		label(lab1);
		break;

	case DO:
		sblab = cblab;
		sclab = cclab;
		cblab = newlab();
		cclab = newlab();
		lab1 = here();
		lex();
		statement();
		label(cclab);
		dbstat(s, line);
		mustbe(WHILE);
		tp = pexpr();
		truth(tp);
		tput(TEXPR, lab1, tp);
		label(cblab);
		mustbe(SEMI);
		cclab = sclab;
		cblab = sblab;
		break;

	case WHILE:
		sblab = cblab;
		sclab = cclab;
		cblab = newlab();
		cclab = here();
		lex();
		tp = pexpr();
		truth(tp);
		tput(FEXPR, cblab, tp);
		statement();
		jump(cclab);
		label(cblab);
		cclab = sclab;
		cblab = sblab;
		break;

	case SWITCH:
		lex();
		tp = pexpr();
		tt = tltype(tp);
		if (tt > T_UINT)
			cerror("switch of non integer");
		if (tt != T_INT) {
			rp = NULL;
			if (tt == T_PTR)
				rp = bzcon(psize(tp));
			tp = bconvert(tp, T_INT, NULL, NULL, rp);
		}
		tput(SEXPR, 0, tp);
		ssbp = sbp;
		sbp = (SBLOCK *) new(sizeof(SBLOCK));
		sbp->sb_dlab = 0;
		sbp->sb_ncase = 0;
		sblab = cblab;
		cblab = newlab();
		statement();
		jump(cblab);
		if (sbp->sb_dlab==0 && sbp->sb_ncase==0)
			cwarn("empty switch");
		if (sbp->sb_dlab == 0)
			sbp->sb_dlab = cblab;
		bput(SBODY);
		iput((ival_t) sbp->sb_dlab);
		iput((ival_t) sbp->sb_ncase);
		for (i=0; i<sbp->sb_ncase; ++i) {
			iput((ival_t) sbp->sb_case[i].sc_val);
			iput((ival_t) sbp->sb_case[i].sc_lab);
		}
		label(cblab);
		cblab = sblab;
		free((char *) sbp);
		sbp = ssbp;
		break;

	case FOR:
		lex();
		mustbe(LPAREN);
		newtree(sizeof(TREE));
		itree = NULL;
		if (s != SEMI)
			itree = expr();
		mustbe(SEMI);
		ctree = NULL;
		if (s != SEMI) {
			ctree = expr();
			truth(ctree);
		}
		mustbe(SEMI);
		stree = NULL;
		if (s != RPAREN)
			stree = expr();
		mustbe(RPAREN);
		sblab = cblab;
		sclab = cclab;
		cblab = newlab();
		cclab = newlab();
		if (itree != NULL) {
			tput(EEXPR, 0, itree);
		}
		if (stree != NULL)
			jump(lab1 = newlab());
		label(cclab);
		if (stree != NULL) {
			tput(EEXPR, 0, stree);
			label(lab1);
		}
		if (ctree != NULL) {
			tput(FEXPR, cblab, ctree);
		}
		statement();
		jump(cclab);
		label(cblab);
		cclab = sclab;
		cblab = sblab;
		break;

	case RETURN:
		lex();
		if (s != SEMI) {
			newtree(sizeof(TREE));
			tp = expr();
			lp = talloc();
			lp->t_op   = CONVERT;
			lp->t_type = cfsym->s_type;
			lp->t_dp   = cfsym->s_dp->d_dp;  /* Skip D_FUNC */
			lp->t_ip   = cfsym->s_ip;
			if (lp->t_dp==NULL && lp->t_type==T_VOID) {
				cerror("return(e) illegal in void function");
				mustbe(SEMI);
				break;
			}
			if ((tltype(lp)==T_STRUCT || tltype(tp)==T_STRUCT)
			   && (lp->t_type!=tp->t_type||lp->t_dp!=tp->t_dp||lp->t_ip!=tp->t_ip)) {
				cerror("return type/function type mismatch");
				mustbe(SEMI);
				break;
			}
			if (bitcompat(tltype(lp), tltype(tp)))
				adjust(tp, lp->t_type, lp->t_dp, lp->t_ip);
			else {
				lp->t_lp = tp;
				tp = lp;
			}
			tput(REXPR, 0, tp);
		}
		jump(cflab);
		mustbe(SEMI);
		break;

	case ID:
		if (spnextis(':')) {
			dolabel();
			lex();
			lex();
			goto loop;
		}

	default:
		newtree(sizeof(TREE));
		tp = expr();
		tput(EEXPR, 0, tp);
		mustbe(SEMI);
	}
}

/*
 * Check if the given tree can be used in a truth value context.
 * Put out the appropriate diagnostics.
 * What does this have to do with stat()?
 */
truth(tp)
TREE	*tp;
{
	register int	tt, op;

	tt = tltype(tp);
	op = tp->t_op;
	if (tt>=T_STRUCT && tt<=T_FUNION)
		cerror("structure or union used in truth context");
	if (incpp)
		return;
	if (isvariant(VSCCON) && (op==ICON || op==LCON || op==DCON))
		cstrict("constant used in truth context");
	if (isvariant(VSRTVC)) {
		if ((tt>=T_PTR && tt<=T_DOUBLE)
		 || (op==EQ && tp->t_lp->t_type==T_DOUBLE))
			cstrict("risky type in truth context");
	}
}

/*
 * Read an expression enclosed in parentheses.
 * Used to read the control expressions in 'if', 'do', 'while' and 'switch'.
 */
TREE *
pexpr()
{
	register TREE	*tp;

	mustbe(LPAREN);
	newtree(sizeof(TREE));
	tp = expr();
	mustbe(RPAREN);
	return (tp);
}

/*
 * Labels.
 */
dolabel()
{
	register SYM	*sp;
	register int	c;

	sp = deflookup(SL_LAB, 2);
	c  = sp->s_class;
	if (c==C_NONE || c==C_FREF) {
		if (c == C_NONE)
			sp->s_value = newlab();
		sp->s_class = C_LAB;
		label(sp->s_value);
		dblabel(sp);
	} else
		cerror("illegal label \"%s\"", sp->s_id);
}

/*
 * The dreaded 'goto' statement.
 */
dogoto()
{
	register SYM	*sp;
	register int	c;

	sp = deflookup(SL_LAB, 2);
	c  = sp->s_class;
	if (c == C_NONE) {
		sp->s_class = C_FREF;
		sp->s_value = newlab();
	} else if (c!=C_LAB && c!=C_FREF)
		cerror("identifier \"%s\" is not a label", sp->s_id);
	jump(sp->s_value);
	sp->s_flag |= S_USED;
}

/*
 * Append a new case to the current switch,
 * adjusting the size of the switch block if necessary and possible.
 */
newcase(v)
ival_t	v;
{
	SBLOCK *ssbp;
	register SCASE *scp, *ocp;
	register int i;

	i = sbp->sb_ncase;
	scp = sbp->sb_case;
	while (--i >= 0) {
		if (scp->sc_val == v) {
			cerror("duplicated case constant");
			return;
		} else
			scp += 1;
	}
	i = sbp->sb_ncase;
	scp = &sbp->sb_case[i];
	if ((i % 32) == 0) {
		ssbp = sbp;
		sbp = new(sizeof(SBLOCK) + (i+32) * sizeof(SCASE));
		sbp->sb_ncase = ssbp->sb_ncase;
		sbp->sb_dlab = ssbp->sb_dlab;
		scp = sbp->sb_case;
		ocp = ssbp->sb_case;
		while (--i >= 0) {
			scp->sc_val = ocp->sc_val;
			scp->sc_lab = ocp->sc_lab;
			scp += 1;
			ocp += 1;
		}
		free((char *) ssbp);
		scp = &sbp->sb_case[sbp->sb_ncase];
	}
	scp->sc_val = v;
	scp->sc_lab = here();
	sbp->sb_ncase += 1;
}
