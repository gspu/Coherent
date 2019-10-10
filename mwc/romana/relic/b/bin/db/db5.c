/*
 * db/db5.c
 * A debugger.
 * Expression evaluator.
 */

#include "db.h"

#define	VALUE	(-2)		/* must be distinct from chars		*/
#define	VILLEGAL (-3)		/* ditto				*/
#define	PRCFULL	(0<<8)		/* precedence wall for full expression 	*/
#define	PRECADD	(1<<8)		/* precedence of additive ops		*/
#define	PRECMUL	(2<<8)		/* precedence of multiplicative ops	*/
#define	PRIMARY	(4<<8)		/* precedence of primary expression	*/
#define	tk_preced(op)	((op)&(~0<<8))
#define	tk_char(op)	((op)&0377)

/*
 * Recursive expression reader;
 * returns -1 for error, 0 for null expr, 1 for good expr.
 */
int
expr(left, wall) register VAL *left; int wall;
{
	VAL v;
	register VAL *right;
	int token, result;
	WORD_T word;

	right = &v;
	token = lextoken(left);
	if (token==VALUE)
		;
	else if (token==VILLEGAL)
		return -1;
	else switch (tk_char(token)) {
	case '~':
		if (expr(left, PRIMARY)<=0)
			return -1;
		left->v_nval = ~left->v_nval;
		break;
	case '-':
		if (expr(left, PRIMARY)<=0)
			return -1;
		left->v_nval = -left->v_nval;
		break;
	case '*':
		/* Indirection. */
		if (expr(left, PRIMARY)<=0)
			return -1;
		add = left->v_nval;
		/* Fetch what the left points to. */
		getb(val_segn(left), (char *)&word, sizeof word);
		left->v_nval = (long)word;
		left->v_flag = 0;
		break;
	case '(':
		if (expr(left, PRCFULL)<=0)
			return -1;
		if ((token = lextoken((VAL *)NULL))==')')
			break;
		else {
			unlex(token);
			printe("Missing ')'");
			return -1;
		}
	default:
		unlex(token);
		left->v_flag = VNULL;
		return 0;
	}
	for (;;) {
		token = lextoken((VAL *)NULL);
		if (tk_preced(token) <= wall) {
			unlex(token);
			return 1;
		} else switch (tk_char(token)) {
		case '*':
			if ((result=expr(right, PRECMUL))<=0)
				break;
			left->v_nval *= right->v_nval;
			continue;
		case '/':
			if ((result=expr(right, PRECMUL))<=0)
				break;
			left->v_nval /= right->v_nval;
			continue;
		case '+':
			if ((result=expr(right, PRECADD))<=0)
				break;
			left->v_nval += right->v_nval;
			continue;
		case '-':
			if ((result=expr(right, PRECADD))<=0)
				break;
			left->v_nval -= right->v_nval;
			continue;
		default:
			unlex(token);
			printe("Unimplemented operator");
			return -1;
		}
		if (result==0)
			printe("Missing operand");
		return -1;
	}
}

/*
 * Evaluate an expression list.
 */
int
expr_list(vlist) VAL vlist[VALSIZE];
{
	register VAL *vp;
	register int c, n;

	vp = vlist;
	n = VALSIZE;
	do {
		if (expr(vp, PRCFULL)<0)
			return 0;
		vp++;
		--n;
	} while ((c=getn()) == ',');
	ungetn(c);
	while (n--)
		vp++->v_flag = VNULL;
	return 1;
}

/*
 * Lex a token. If value, store in given val ptr.
 */
int
lextoken(vp) VAL *vp;
{
	register int	c;

	for (;;) switch (c=getn()) {
	case ' ':
	case '\t':
		continue;
	case '.':
		if (vp == (VAL *)NULL) {
			printe("Missing operand before '.'");
			ungetn(c);
			return VILLEGAL;
		}
		vp->v_flag = VSEGN;
		vp->v_segn = cseg;
		vp->v_nval = dot;
		return VALUE;
		/*
		 * Only binary ops need be mentioned explicitly
		 * so precedences can be added.
		 */
	case '*':
	case '/':
		return c|PRECMUL;
	case '+':
	case '-':
		return c|PRECADD;
	default:
		if ('0'<=c && c<='9')
			return readval(vp, c);
		else if ('a'<=c && c<='z' || 'A'<=c && c<='Z' || c=='_' || c=='%')
			return readvar(vp, c);
		else
			return c;
	}
}

/*
 * Evaluate a value as an lvalue and return it.
 * If the value is VNULL, return 'v'.
 */
ADDR_T
lvalue(vp, v) register VAL *vp; ADDR_T v;
{
	return (vp->v_flag & VNULL) ? v : (ADDR_T)vp->v_nval;
}

#if	0			/* Covered by macro in db.h */
/*
 * If the given value is null, return 1, else 0.
 */
int
nvalue(vp) VAL *vp;
{
	return vp->v_flag & VNULL;
}
#endif

/*
 * Read a number.
 */
int
readval(vp, c) VAL *vp; register int c;
{
	long	l;
	register int	i, base;

	if (vp == (VAL *)NULL) {
		printe("Missing operand before number");
		ungetn(c);
		return VILLEGAL;
	}
	base = 10;
	if (c=='0') {
		base = 8;
		if ((c = getn())=='x') {
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
		if ((10 <= (i=(c = getn())-('a'-10))
		    || 10 <= (i=c-('A'-10))
		    ||  0 <= (i=c-'0') && i <= 9)
		 && i < base)
			l = l*base - i;
		else
			break;
	}
	ungetn(c);
	vp->v_flag = 0;
	vp->v_nval = -l;
	return VALUE;
}

/*
 * Read symbol and place value in given val struct.
 */
int
readvar(vp, c) VAL *vp; int c;
{
	ungetn(c);
	if (vp == (VAL *)NULL) {
		printe("Missing operand before symbol");
		return VILLEGAL;
	}
	return (symval(vp)) ? VALUE : VILLEGAL;
}

/*
 * Evaluate a value as an rvalue and return it.
 * If the value is null, return 'v'.
 */
long
rvalue(vp, v) register VAL *vp; long v;
{
	return (vp->v_flag&VNULL) ? v : vp->v_nval;
}

/*
 * Push token back on input stream.
 */
void
unlex(c) int c;
{
	if (c != VILLEGAL && c != VALUE)
		ungetn(tk_char(c));
}

/*
 * Return the segment associated with a value.
 * If there is no segment associated with it,
 * look at the segmentation map to see if it is found.
 * If the value is null or not found, return the current segment.
 */
int
val_segn(vp) register VAL *vp;
{
	register int s;

	if (vp->v_flag & VNULL)
		return cseg;
	if (vp->v_flag & VSEGN)
		return vp->v_segn;		/* in known segment */
	else if ((s = find_seg((ADDR_T)vp->v_nval)) != SEG_NONE)
		return s;			/* in existing segment */
	else
		return cseg;			/* take a guess... */
}

/* end of db/db5.c */
