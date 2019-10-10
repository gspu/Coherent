/* $Header: /src386/bin/db/RCS/trace5.c,v 1.1 92/06/10 14:37:04 bin Exp Locker: bin $
 *
 *	The information contained herein is a trade secret of Mark Williams
 *	Company, and  is confidential information.  It is provided  under a
 *	license agreement,  and may be  copied or disclosed  only under the
 *	terms of  that agreement.  Any  reproduction or disclosure  of this
 *	material without the express written authorization of Mark Williams
 *	Company or persuant to the license agreement is unlawful.
 *
 *	COHERENT Version 2.3.35
 *	Copyright (c) 1982, 1983, 1984.
 *	An unpublished work by Mark Williams Company, Chicago.
 *	All rights reserved.
 */
/*
 * A debugger.
 * Expression evaluator.
 *
 * $Log:	trace5.c,v $
 * Revision 1.1  92/06/10  14:37:04  bin
 * Initial revision
 * 
 * Revision 1.1	88/10/17  04:05:45 	src
 * Initial revision
 * 
 * Revision: 386 version 92/05/01 
 * Bernard Wald, Wald Software Consulting, Germany
 *
 */
#include <stdio.h>
#include <ctype.h>
#include <l.out.h>
#include "trace.h"

#define	VALUE	(-2)		/* must be distinct from chars */
#define	VILLEGAL (-3)		/* ditto */
#define	PRCFULL	(0<<8)		/* precedence wall for full expression */
#define	PRECADD	(1<<8)		/* precedence of addops */
#define	PRECMUL	(2<<8)		/* precedence of mulops */
#define	PRIMARY	(4<<8)		/* precedence of primary expression */
#define	tk_preced(op)	((op)&(~0<<8))
#define	tk_char(op)	((op)&0377)

/*
 * Evaluate an expression list.
 */
int
explist(vlist)
VAL vlist[VALSIZE];
{
	register VAL *vp;
	register int c, n;

	vp = vlist;
	n = VALSIZE;
	do {
		if (expressn(vp, PRCFULL)<0)
			return (0);
		vp++;
		--n;
	} while ((c=getn()) == ',');
	ungetn(c);
	while (n--)
		vp++->v_flag = VNULL;
	return (1);
}

/*
 * Recursive expression reader;
 * returns -1 for error, 0 for null expr, 1 for good expr.
 */
int
expressn(left, wall)
register VAL	*left;
int	wall;
{
	VAL	v;
	register VAL	*right = &v;
	int	token, result;

	token = lextoken(left);
	if (token==VALUE)
		;
	else if (token==VILLEGAL)
		return (-1);
	else switch (tk_char(token)) {
	case '~':
		if (expressn(left, PRIMARY)<=0)
			return (-1);
		left->v_nval = ~left->v_nval;
		break;
	case '-':
		if (expressn(left, PRIMARY)<=0)
			return (-1);
		left->v_nval = -left->v_nval;
		break;
	case '*':
		if (expressn(left, PRIMARY)<=0)
			return (-1);
		add = left->v_nval;
		getb(left->v_flag&VLVAL ? left->v_segn : 0,
			&result, sizeof result);
		left->v_nval = (long)(unsigned int)result;
		left->v_flag = VLVAL;
		left->v_segn = 0;
		break;
	case '(':
		if (expressn(left, PRCFULL)<=0)
			return (-1);
		if ((token=lextoken(NULL))==')')
			break;
		else {
			unlex(token);
			printe("Missing ')'");
			return (-1);
		}
	default:
		unlex(token);
		left->v_flag = VNULL;
		return (0);
	}
	for (;;) {
		token = lextoken(NULL);
		if (tk_preced(token) <= wall) {
			unlex(token);
			return (1);
		} else switch (tk_char(token)) {
		case '*':
			if ((result=expressn(right, PRECMUL))<=0)
				break;
			left->v_nval *= right->v_nval;
			continue;
		case '/':
			if ((result=expressn(right, PRECMUL))<=0)
				break;
			left->v_nval /= right->v_nval;
			continue;
		case '+':
			if ((result=expressn(right, PRECADD))<=0)
				break;
			left->v_nval += right->v_nval;
			continue;
		case '-':
			if ((result=expressn(right, PRECADD))<=0)
				break;
			left->v_nval -= right->v_nval;
			continue;
		default:
			unlex(token);
			printe("Unimplemented operator");
			return (-1);
		}
		if (result==0)
			printe("Missing operand");
		return (-1);
	}
}

/*
 * Lex a token. If value, store in given val ptr.
 */
int
lextoken(vp)
VAL	*vp;
{
	register int	c;

	for (;;) switch (c=getn()) {
	case ' ':
	case '\t':
		continue;
	case '.':
		if (vp==NULL) {
			printe("Missing opr before '.'");
			ungetn(c);
			return (VILLEGAL);
		}
		vp->v_flag = VLVAL;
		vp->v_segn = cseg;
		vp->v_nval = dot;
		return (VALUE);
		/*
		 * only binary ops need be mentioned explicitly
		 * so precedences can be added
		 */
	case '*':
	case '/':
		return (c|PRECMUL);
	case '+':
	case '-':
		return (c|PRECADD);
	default:
		if ('0'<=c && c<='9')
			return (readval(vp, c));
		else if ('a'<=c && c<='z' || 'A'<=c && c<='Z' || c=='_')
			return (readvar(vp, c));
		else
			return (c);
	}
}
/*
 * Push token back on input stream
 */
unlex(c)
int	c;
{
	if (c!=VILLEGAL && c!=VALUE)
		ungetn(tk_char(c));
}

/*
 * Read a number.
 */
readval(vp, c)
VAL	*vp;
register int	c;
{
	long	l;
	register int	i, base;

	if (vp==NULL) {
		printe("Missing opr before number");
		ungetn(c);
		return (VILLEGAL);
	}
	base = 10;
	if (c=='0') {
		base = 8;
		if ((c=getn())=='x') {
			base = 16;
		} else {
			ungetn(c);
		}
		c = '0';
	}
	if (c == '#')
		base = 16;
	else
		l = '0' - c;
	for (;;) {
		if ((10<=(i=(c=getn())-('a'-10))
		  || 10<=(i=c-('A'-10))
		  ||  0<=(i=c-'0') && i<=9
		 )&& i<base)
			l = l*base - i;
		else
			break;
	}
	ungetn(c);
	vp->v_flag = 0;
	vp->v_nval = -l;
	return (VALUE);
}

/*
 * Read symbol and place value in given val struct
 */
readvar(vp, c)
VAL	*vp;
{
	ungetn(c);
	if (vp==NULL) {
		printe("Missing opr before symbol");
		return (VILLEGAL);
	}
	return (getsval(vp) ? VALUE : VILLEGAL);
}

/*
 * Given a value, return it's segment.  If there is no segment associated
 * with it, return `v'.
 */
vsegno(vp, v)
register VAL *vp;
{
	if ((vp->v_flag&VLVAL) == 0)
		return (v);
	return (vp->v_segn);
}

/*
 * Evaluate a value as an lvalue and return it.  If the value is
 * null, return `v'.
 */
long
lvalue(vp, v)
register VAL *vp;
long v;
{
	if (vp->v_flag&VNULL)
		return (v);
	return (vp->v_nval);
}

/*
 * Evaluate a value as an rvalue and return it.  If the value is
 * null, return `v'.
 */
long
rvalue(vp, v)
register VAL *vp;
long v;
{
	if (vp->v_flag&VNULL)
		return (v);
	return (vp->v_nval);
}

/*
 * If the given value is null, return 1, else 0.
 */
nvalue(vp)
VAL *vp;
{
	return (vp->v_flag&VNULL);
}
