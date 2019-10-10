/*
 * Korn expression evaluation
 */

static char *RCSid = "$Header: expr.c,v 3.1 88/11/03 09:15:55 egisin Exp $";

#include <stddef.h>
#include <errno.h>
#include <setjmp.h>
#include "sh.h"
#include "table.h"

#define	ef	else if		/* fashion statement */

#define	VAR	0x01
#define	LIT	0x02
#define	LEQ	0x03
#define	LNE	0x04
#define	LLE	0x05
#define	LGE	0x06

static void token();		/* read next token */
static Const char *expression;	/* expression being evaluated */
static Const char *tokp;	/* lexical position */
static int tok;			/* token from token() */
static struct tbl *val;		/* value from token() */

static struct tbl *tempvar(), *intvar();
static struct tbl *asn(), *e6(), *e5(), *e3(), *e2(), *e0();

/*
 * parse and evalute expression
 */
void
evalerr(err)
	char *err;
{
	errorf("%s: %s\n", expression, err);
}

long
evaluate(expr)
	Const char *expr;
{
	struct tbl *v;

	expression = tokp = expr;
	token();
	v = intvar(asn());
	if (!(tok == 0))
		evalerr("bad expression");
	return v->val.i;
}

static struct tbl *
asn()
{
	register struct tbl *vl, *vr;

	vr = vl = e6();
	if ((tok == '=')) {
		Area * olastarea = lastarea;
		token();
		if ((vl->flag&RDONLY)) /* assign to rvalue */
			evalerr("bad assignment");
		vr = intvar(asn());
		lastarea = olastarea;
		setint(vl, vr->val.i);
		if ((vl->flag&INTEGER) && vl->type == 0) /* default base? */
			vl->type = vr->type;
	}
	return vr;
}

static struct tbl *
e6()
{
	register struct tbl *vl, *vr;

	vl = e5();
	while ((tok == LEQ) || (tok == LNE)) {
		int op = tok;
		token();
		vl = intvar(vl);
		vr = intvar(e5());
		vl->val.i = vl->val.i == vr->val.i;
		if (op == LNE)
			vl->val.i = ! vl->val.i;
	}
	return vl;
}

static struct tbl *
e5()
{
	register struct tbl *vl, *vr;

	vl = e3();
	while ((tok == LLE) || (tok == '<') || (tok == '>') || (tok == LGE)) {
		int op = tok;
		token();
		vl = intvar(vl);
		vr = intvar(e3());
		if (op == LLE)
			vl->val.i = vl->val.i <= vr->val.i;
		ef (op == '<')
			vl->val.i = vl->val.i < vr->val.i;
		ef (op == LGE)
			vl->val.i = vl->val.i >= vr->val.i;
		ef (op == '>')
			vl->val.i = vl->val.i > vr->val.i;
	}
	return vl;
}

static struct tbl *
e3()
{
	register struct tbl *vl, *vr;

	vl = e2();
	while ((tok == '+') || (tok == '-')) {
		int op = tok;
		token();
		vl = intvar(vl);
		vr = intvar(e2());
		if (op == '+')
			vl->val.i += vr->val.i;
		ef (op == '-')
			vl->val.i -= vr->val.i;
	}
	return vl;
}

static struct tbl *
e2()
{
	register struct tbl *vl, *vr;

	vl = e0();
	while ((tok == '*') || (tok == '/') || (tok == '%')) {
		int op = tok;
		token();
		vl = intvar(vl);
		vr = intvar(e0());
		if (op != '*' && vr->val.i == 0)
			evalerr("zero divisor");
		if (op == '*')
			vl->val.i *= vr->val.i;
		ef (op == '/')
			vl->val.i /= vr->val.i;
		ef (op == '%')
			vl->val.i %= vr->val.i;
	}
	return vl;
}

static struct tbl *
e0()
{
	register struct tbl *v;

	if ((tok == '!') || (tok == '-')) {
		int op = tok;
		token();
		v = intvar(e0());
		if (op == '!')
			v->val.i = !v->val.i;
		ef (op == '-')
			v->val.i = -v->val.i;
	} else
	if ((tok == '(')) {
		token();
		v = asn();
		if (!(tok == ')'))
			evalerr("missing )");
		token();
	} else
	if ((tok == VAR) || (tok == LIT)) {
		v = val;
		token();
	} else
		evalerr("bad expression");
	return v;
}

static void
token()
{
	register char *cp = (char *) tokp;
	register int c, c2;

	/* skip white space */
	do c = *cp++;	while (c != '\0' && (c == ' ' || c == '\t'));
	tokp = cp-1;

	if (letter(c)) {
		for (; letnum(c); c = *cp++)
			;
		c = *--cp;
		*cp = 0;
		val = global(tokp);
		*cp = c;
		tok = VAR;
	} else
	if (digit(c)) {
		for (; letnum(c) || c == '#'; c = *cp++)
			;
		c = *--cp;
		*cp = 0;
		val = tempvar();
		setstr(val, tokp);
		val->flag |= RDONLY;
		*cp = c;
		tok = LIT;
	} else {
		c2 = *cp++;
		if (c == '=' && c2 == '=')
			c = LEQ;
		ef (c == '!' && c2 == '=')
			c = LNE;
		ef (c == '<' && c2 == '=')
				c = LLE;
		ef (c == '>' && c2 == '=')
				c = LGE;
		else
			cp--;
		tok = c;
	}
	tokp = cp;
}

static struct tbl *
tempvar()
{
	register struct tbl *vp;

	vp = alloc(sizeof(struct tbl), ATEMP);
	lastarea = ATEMP;
	vp->flag = ISSET|INTEGER;
	vp->type = 0;
	vp->name[0] = '\0';
	return vp;
}

/* cast (string) variable to temporary integer variable */
static struct tbl *
intvar(vp)
	register struct tbl *vp;
{
	register struct tbl *vq;

	vq = tempvar();
	vq->type = 10;
	return strint(vq, vp);
}

