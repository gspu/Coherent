#include "asm.h"

expr(esp, n)
register struct expr *esp;
{
	register c, p;
	address lv, rv;
	int lt, rt;
	struct expr re;

	term(esp);
	for (;;) {
		c = getnb();
		if (ctype[c] != BINOP)
			break;
		if (c == '|')
			p = 10;
		else if (c == '*')
			p = 9;
		else
			p = 8;
		if (p <= n)
			break;
		expr(&re, p);
		lt = esp->e_type;
		lv = esp->e_addr;
		rt = re.e_type;
		rv = re.e_addr;
		if (c == '|') {
			if (rt != E_ACON)
				aerr("non constant in segment construction");
			switch (lt) {
			case E_ACON:
				esp->e_type = E_ASEG;
				esp->e_base.e_segn = lv;
				break;

			case E_SYM:
				esp->e_type = E_SEG;
				break;

			default:
				aerr("bad segment construction");
			}
			esp->e_addr = rv;
			continue;
		}
		if (c == '^') {
			if ((esp->e_type = rt) == E_DIR)
				esp->e_base.e_lp = re.e_base.e_lp;
			else if (rt==E_SYM || rt==E_SEG)
				esp->e_base.e_sp = re.e_base.e_sp;
			else if (rt == E_ASEG)
				esp->e_base.e_segn = re.e_base.e_segn;
			continue;
		}
		if (c == '+') {
			if (lt == E_ACON) {
				esp->e_type = rt;
				if (rt == E_ASEG)
					esp->e_base.e_segn = re.e_base.e_segn;
				else if (rt == E_DIR)
					esp->e_base.e_lp = re.e_base.e_lp;
				else if (rt==E_SYM || rt==E_SEG)
					esp->e_base.e_sp = re.e_base.e_sp;
			} else if (rt != E_ACON)
				rerr("add of 2 non constants");
			esp->e_addr += rv;
			continue;
		}
		if (c == '-') {
			if (lt == E_ACON) {
				if (rt != E_ACON)
					rerr("constant - non-constant");
			} else if (rt != E_ACON) {
				if (lt!=E_DIR || rt!=E_DIR || 
				    esp->e_base.e_lp!=re.e_base.e_lp)
					rerr("cannot fold this expression");
				esp->e_type = E_ACON;
			}
			esp->e_addr -= rv;
			continue;
		}
		if (c == '*') {
			if (lt!=E_ACON || rt!=E_ACON)
				aerr("non-constant in multiply");
			esp->e_addr *= rv;
			continue;
		}
		fprintf(stderr, "Internal error, c=%d in expr.\n", c);
		exit( 1);
	}
	unget(c);
}

address
absexpr()
{
	struct expr e;

	expr(&e, 0);
	abscheck(&e);
	return (e.e_addr);
}

term(esp)
register struct expr *esp;
{
	register c;
	address  n;
	char id[NCPLN];
	struct sym  *sp;
	struct tsym *tp;
	int r, v;

	if ((c=getnb()) == '[') {
		expr(esp, 0);
		if (getnb() != ']')
			qerr("missing ']'");
		return;
	}
	if (c == '-') {
		expr(esp, 100);
		abscheck(esp);
		esp->e_addr = -esp->e_addr;
		return;
	}
	if (c == '~') {
		expr(esp, 100);
		abscheck(esp);
		esp->e_addr = ~esp->e_addr;
		return;
	}
	if (c == '\'') {
		esp->e_type = E_ACON;
		esp->e_addr = getmap(-1);
		return;
	}
	esp->e_type = E_ACON;
	esp->e_addr = 0;
	if (ctype[c] == DIGIT) {
		r = 10;
		if (c == '0') {
			r = 8;
			if ((c = *ip) != 0)
				++ip;
			if (c == 'x') {
				r = 16;
				if ((c = *ip) != 0)
					++ip;
			}
		}
		n = 0;
		while ((v=digit(c, r)) >= 0) {
			n = r*n + v;
			if ((c = *ip) != 0)
				++ip;
		}
		if (c=='f' || c=='b') {
			if (n < 10) {
				if (c == 'f')
					tp = tsymp[n].tp_fp; else
					tp = tsymp[n].tp_bp;
				if (tp != NULL) {
					esp->e_type = E_DIR;
					esp->e_base.e_lp = tp->t_lp;
					esp->e_addr = tp->t_addr;
					return;
				}
			}
			err('u', "undefined local symbol");
			return;
		}
		if (c != 0)
			--ip;
		esp->e_addr = n;
		return;
	}
	if (ctype[c] == LETTER) {
		getid(id, c);
		if ((sp=lookup(id, gflag)) != NULL) {
			if (sp->s_kind == S_NEW) {
				if ((sp->s_flag&S_GBL) != 0) {
					esp->e_type = E_SYM;
					esp->e_base.e_sp = sp;
					return;
				}
				uerr(id);
				return;
			}
			esp->e_type = sp->s_type;
			esp->e_addr = sp->s_addr;
			if (sp->s_type == E_ASEG)
				esp->e_base.e_segn = sp->s_base.s_segn;
			else if (sp->s_type == E_DIR)
				esp->e_base.e_lp = sp->s_base.s_lp;
			else if (sp->s_type==E_SYM || sp->s_type==E_SEG)
				esp->e_base.e_sp = sp->s_base.s_sp;
			return;
		}
		uerr(id);
		return;
	}
	qerr("invalid symbol");
}

/*
 * Look for an optional floating point register specification
 * and store an appropriate e_mode value through ep.
 * No operand: store ST and return 0.
 * One operand: store reg and return 1.
 */
fp_reg(ep)
register struct expr *ep;
{
	register c;
	char id[NCPLN];
	struct sym  *sp;

	ep->e_mode = ST;
	if (ctype[c=getnb()] == LETTER) {
		getid(id, c);
		if ((sp=lookup(id, 0)) != NULL) {
			if ((sp->s_addr&MMASK) != ST) {
				qerr("invalid floating-point register");
				return(0);
			}
			ep->e_mode = sp->s_addr;
			return(1);
		}
		uerr(id);
		return(0);
	}
	unget(c);
	return(0);
}

/*
 * Look for one or two optional floating point register operands
 * and store appropriate e_mode values through ep1, ep2.
 * Return a flag indicating whether any operands were supplied.
 * No operand: store ST1, ST, return 0.
 * One operand: store reg1, ST, return 1.
 * Two operands: store reg1, reg2, return 1; at least one must be ST.
 */
fp_reg2(ep1, ep2)
register struct expr *ep1, *ep2;
{
	register int c;

	ep2->e_mode = ST;
	if (fp_reg(ep1)) {
		if ((c = getnb()) == ',') {
			if (!fp_reg(ep2)
			  || (ep1->e_mode != ST && ep2->e_mode != ST))
				qerr("invalid floating-point register");
		}
		else
			unget(c);
		return(1);
	}
	else {
		ep1->e_mode = ST1;
		return(0);
	}
}

digit(c, r)
register c, r;
{
	if (r == 16) {
		if (c>='A' && c<='F')
			return (c-'A'+10);
		if (c>='a' && c<='f')
			return (c-'a'+10);
	}
	if (c>='0' && c<='9')
		return (c - '0');
	return (-1);
}

abscheck(esp)
register struct expr *esp;
{
	if (esp->e_type != E_ACON) {
		rerr("expected constant");
		esp->e_type = E_ACON;
	}
}
