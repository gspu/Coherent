/*
 * Regular expression routines.
 * These routines are currently used by grep but
 * they are somewhat general and thus could be
 * used elsewhere.
 */

#include <stdio.h>
#include <ctype.h>
#include "grep.h"

char	*newcc();
char	*e_exec();


static	char	repri[] = {
	0 /* REEND */,	2 /* STEND */,
	3 /* OR */,	6 /* LPAR */,
	1 /* RPAR */,	5 /* CLOS */,
	5 /* NECLOS */,	5 /* ZORO */,
	4 /* CONC */
};

RE	**rebuild();
RE	*renode();
static	union rebit	relval;
static	char	cc[NCLASS];
static	(*reinf)();
static	(*reunf)();
static	char	*rein;

static	char	resyn[] = "Regular expression syntax error";
static	char	reoflo[] = "Regular expression overflow";
static	char	nospace[] = "Out of space";

char	*reerror;		/* Erro code */
int	redual;			/* Set for dual-case comparisons */
int	refull;			/* Full expressions accepted */

/*
 * Lexical token reader for
 * regular expressions.
 */
static
relex(ec)
{
	register c;

	if ((c = (*reinf)()) == ec)
		return (REEND);
	switch (c) {
	case EOF:
	case '\n':
		reerror = "Non-terminated regular expression";
		return (REEND);

	case '[':
		return (cclass(ec));

	case '^':
		return (BOL);

	case '$':
		return (EOL);

	case '(':
		if (!refull)
			goto def;
		return (LPAR);

	case ')':
		if (!refull)
			goto def;
		return (RPAR);

	case '.':
		return (ANY);

	case '|':
		if (!refull)
			goto def;
		return (OR);

	case '*':
		return (CLOS);

	case '?':
		if (!refull)
			goto def;
		return (ZORO);

	case '+':
		if (!refull)
			goto def;
		return (NECLOS);

	case '\\':
		c = (*reinf)();
	default:
	def:
		relval.u_ival = c;
		return (redual ? DCONC : CONC);
	}
}

/*
 * Read in a character class from
 * an RE (called from relex).
 */
static
cclass(ec)
int ec;
{
	register c, i, pc;
	int comp;

	for (i=0; i<sizeof cc; i++)
		cc[i] = 0;
	if ((c = (*reinf)()) != '^') {
		comp = 0;
		(*reunf)(c);
	} else
		comp = 1;
	pc = ec;
	while ((c = (*reinf)()) != ']') {
		if (c == ec) {
			reerror = "Non-terminated character class";
			return (REEND);
		}
		if (c=='-' && pc!=ec) {
			if ((c = (*reinf)()) == ']')
				break;
			for (i=pc; i<=c; i++)
				cc[i/NBPC] |= 1<<(i%NBPC);
			pc = ec;
		} else {
			cc[c/NBPC] |= 1<<(c%NBPC);
			pc = c;
		}
	}
	if (comp)
		for (i=0; i<sizeof cc; i++)
			cc[i] ^= -1;
	if ((relval.u_cptr = newcc(cc)) == NULL)
		return (REEND);
	return (redual ? DCCLASS : CCLASS);
}

/*
 * Allocate space for a new character class
 * and copy into it.
 */
static char *
newcc(occ)
register char *occ;
{
	register char *ncc;
	register unsigned n;
	char *rcc;

	n = NCLASS;
	if ((rcc = ncc = malloc(n)) == NULL) {
		reerror = nospace;
		return (NULL);
	}
	do {
		*ncc++ = *occ++;
	} while (--n);
	return (rcc);
}

/*
 * Parse a regular expression.
 * The arguments are an input string to parse (`in'),
 * the end character to terminated the expression (`ec'),
 * and (if `in' is NULL) functions to get and unget characters,
 * getf and ungetf, respectively.
 */
RE *
reparse(in, ec, getf, ungetf)
char *in;
int (*getf)(), (*ungetf)();
{
	RE *restk[NRE];
	struct opstk {
		char	o_op;
		char	o_pri;
	} opstk[NRE];
	register struct opstk *osp;
	register op;
	register RE **rsp;
	int concflg, op1, pri;

	if ((rein = in) != NULL) {
		reinf = reget;
		reunf = reunget;
	} else {
		reinf = getf;
		reunf = ungetf;
	}
	reerror = NULL;
	concflg = 0;
	osp = opstk;
	osp->o_op = STEND;
	osp->o_pri = repri[STEND];
	rsp = restk;
	*rsp++ = NULL;

	for (;;) {
		if (termop(op = relex(ec))) {
			if ((*rsp++ = renode(op, relval.u_cptr, NULL)) == NULL)
				return (NULL);
			if (rsp >= &restk[NRE-1]) {
				reerror = reoflo;
				return (NULL);
			}
			if (!concflg) {
				concflg++;
				continue;
			} else
				op = CONC;
		} else if (op == OR) {
			if (!concflg) {
				reerror = resyn;
				return (NULL);
			}
			concflg = 0;
		} else if (op==CLOS || op==NECLOS || op==ZORO) {
			if (!concflg) {
				reerror = resyn;
				return (NULL);
			}
		} else if (op == LPAR) {
			if (concflg) {
				(++osp)->o_op = CONC;
				osp->o_pri = repri[CONC];
				concflg = 0;
			}
		}
		pri = repri[op];
		for (;;) {
			if (reerror != NULL)
				return (NULL);
			if (pri>osp->o_pri || (op==CONC && osp->o_op==CONC)) {
				if (op == LPAR)
					pri = repri[RPAR];
				if (osp >= &opstk[NRE-1]) {
					reerror = reoflo;
					return (NULL);
				}
				(++osp)->o_op = op;
				osp->o_pri = pri;
				if (!postop(op))
					break;
				else
					pri = repri[REEND];
			}
			switch (op1 = (osp--)->o_op) {
			case STEND:
				if (op == REEND)
					return (*--rsp);
				osp++;
				break;

			case LPAR:
				if (op != RPAR) {
					reerror = "Unbalanced parentheses";
					return (NULL);
				}
				break;

			default:
				rsp = rebuild(rsp, op1);
				continue;
			}
			break;
		}
	}
}


static RE **
rebuild(rsp, op)
register RE **rsp;
{
	register RE *left, *right;

	switch (op) {
	case OR:
		right = *--rsp;
		left = *--rsp;
		break;

	case CLOS:
	case NECLOS:
	case ZORO:
		left = *--rsp;
		right = NULL;
		break;

	case CONC:
		right = *--rsp;
		left = *--rsp;
		if (left->r_next == NULL)
			left->r_next = right;
		else {
			for (; left->r_next->r_next!=NULL; left = left->r_next)
				;
			left->r_next->r_next = right;
		}
		return (++rsp);

	default:
		reerror = "RE botch in rebuild";
	}
	*rsp++ = renode(op, left, right);
	return (rsp);
}

/*
 * Build regular expression node.
 */
static RE *
renode(op, left, right)
RE *left, *right;
{
	register RE *rep;

	if ((rep = (RE *)malloc(sizeof (RE))) == NULL) {
		reerror = nospace;
		return (NULL);
	}
	rep->r_next = (RE*)NULL;
	rep->r_op = op;
	rep->r_left.u_re = left;
	rep->r_right.u_re = right;
	return (rep);
}

static	char	*sb;		/* string beginning */

/*
 * User-callable driver for regular expression
 * execution.  Called with regular expression code and
 * text string.
 */
reinterp(rep, s)
RE *rep;
register char *s;
{
	sb = s;
	if (rep!=NULL && rep->r_op==BOL)
		return(e_exec(rep, s) != NULL);
	for (; *s!='\0'; s++)
		if (e_exec(rep, s) != NULL)
			return (1);
	return (0);
}

/*
 * Internal regular expression
 * execution routines
 */
static char *
e_exec(rep, s)
register RE *rep;
register char *s;
{
	register c;
	char *ss, *es;

	for ( ; rep!=NULL; rep = rep->r_next)
		switch (rep->r_op) {
		case BOL:
			if (s != sb)
				return (NULL);
			break;

		case EOL:
			if (*s != '\0')
				return (NULL);
			break;

		case ANY:
			if (*s++ == '\0')
				return (NULL);
			break;

		case CONC:
			if (*s++ != rep->r_left.u_ival)
				return (NULL);
			break;

		case DCONC:
			if (islower(rep->r_left.u_ival)) {
				if (isupper(c = *s++))
					c = tolower(c);
				if (c != rep->r_left.u_ival)
					return (NULL);
			} else
				if (*s++ != rep->r_left.u_ival)
					return (NULL);
			break;

		case CCLASS:
			c = *s++;
			if ((rep->r_left.u_cptr[c/NBPC] & (1<<(c%NBPC))) == 0)
				return (NULL);
			break;

		case DCCLASS:
			if ((c = *s++)>='A' && c<='Z')
				c |= 'a'-'A';
			if ((rep->r_left.u_cptr[c/NBPC] & (1<<(c%NBPC))) == 0)
				return (NULL);
			break;

		case OR:
			ss = s;
			if ((s = e_exec(rep->r_left.u_re, s)) == NULL)
				if ((s = e_exec(rep->r_right.u_re, ss))==NULL)
					return (NULL);
			break;

		case CLOS:
			ss = s;
			while ((es = e_exec(rep->r_left.u_re, s)) != NULL)
				s = es;
			while (s >= ss)
				if ((es = e_exec(rep->r_next, s--))!=NULL)
					return(es);
			return (NULL);

		case NECLOS:
			ss = s;
			while ((es = e_exec(rep->r_left.u_re, s)) != NULL)
				s = es;
			while (s > ss)
				if ((es = e_exec(rep->r_next, s--))!=NULL)
					return (es);
			return (NULL);

		case ZORO:
			ss = s;
			if ((es = e_exec(rep->r_left.u_re, s)) != NULL)
				s = es;
			while (s >= ss)
				if ((es = e_exec(rep->r_next, s--))!=NULL)
					return (es);
			return (NULL);

		default:
			fprintf(stderr, "Regular expression botch\n");
			exit(2);
		}
	return (s);
}

/*
 * Default functions to get and unget
 * characters from the regular expression.
 */
static
reget()
{
	return (*rein++);
}

static
reunget(c)
{
	--rein;
}
