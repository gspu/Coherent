/*
 * AWK - Common support routines.
 * node allocation, etc.
 */

#include "awk.h"
#include "y.tab.h"

static	int	eosflag;		/* for pgetc/pungetc */

/*
 * Allocate n bytes of
 * storage, checking for
 * running out of space.
 * This version doesn't call malloc, but
 * rather uses sbrk and doles out storage
 * itself.  This sames much time for
 * searching long alloc lists.  However,
 * i/o calls must all be buffered staticly.
 * A call with 0 bytes returns the pointer to
 * the end which can be saved for a call to xfree.
 * The following are 2 pointers maintained by the allocator/xfree.
 */
static	int	*xallrp;		/* Running pointer for xalloc */
static	int	*xallep;		/* End pointer for xfree */
CHAR *
xalloc(size)
register unsigned size;
{
	register int *rp;

	if ((rp = (int *)malloc(size)) == NULL)
		awkerr("Out of memory");
	return ((CHAR *)rp);
}

/*
 * Create an expression tree node.
 * The only required argument is the
 * operation code.  The other three
 * are operands.
 */
/* VARARGS 1 */
NODE *
node(op, o1, o2, o3, o4)
int op;
NODE *o1, *o2, *o3, *o4;
{
	register NODE *np;

	np = (NODE *)xalloc(sizeof (NODE));
	np->n_op = op;
	np->n_flag = 0;
	np->n_O1 = o1;
	np->n_O2 = o2;
	np->n_O3 = o3;
	np->n_O4 = o4;
	return (np);
}

/*
 * Produce a node with a character (int)
 * int it (mostly for regular expressions).
 */
NODE *
cnode(op, c)
int op, c;
{
	register NODE *np;

	np = (NODE *)xalloc(sizeof (NODE));
	np->n_op = op;
	np->n_flag = 0;
	np->n_o1.n_char = c;
	np->n_O2 = np->n_O3 = np->n_O4 = NULL;
	return (np);
}

/*
 * Produce a string node.
 * While running, this node
 * is considered temporary.
 */
NODE *
snode(s, type)
register CHAR *s;
int type;
{
	register NODE *np;

	np = (NODE *)xalloc(sizeof(TERM));
	np->t_op = ATERM;
	np->t_flag = type;
	np->t_un.t_str = s;
	if (runflag) {
		np->t_next = tempnodes;
		tempnodes = np;
	}
	return (np);
}

/*
 * Build a node for a (long)
 * integer.
 */
NODE *
inode(i)
INT i;
{
	register NODE *np;

	np = (NODE *)xalloc(sizeof(NODE));
	np->t_op = ATERM;
	np->t_flag = T_NUM|T_INT;
	np->t_un.t_int = i;
	if (runflag) {
		np->t_next = tempnodes;
		tempnodes = np;
	}
	return (np);
}

/*
 * Build a node for a (double)
 * floating point number.
 */
NODE *
fnode(f)
FLOAT f;
{
	register NODE *np;

	np = (NODE *)xalloc(sizeof(NODE));
	np->t_op = ATERM;
	np->t_flag = T_NUM;
	np->t_un.t_float = f;
	if (runflag) {
		np->t_next = tempnodes;
		tempnodes = np;
	}
	return (np);
}

/*
 * Free a terminal node.  This
 * looks at the flags to see if
 * it is a string, and frees it if
 * it is also an allocated string.
 */
freenode(np)
register NODE *np;
{
	if ((np->t_flag&(T_NUM|T_ALLOC)) == T_ALLOC)
		free(np->t_STRING);
	free(np);
}

/*
 * Checks to see if a node is numeric.
 * Here it must be not in string form
 * but in number form.  This is
 * used mostly for comparisons.
 */
isnumeric(np)
register NODE *np;
{
	if (np->t_flag & T_NUM)
		return (1);
	return (0);
}

/*
 * Return 1 if a number is of the floating
 * type.  This will check strings as well
 * as numbers.
 */
isfloat(np)
register NODE *np;
{
	register CHAR *cp;
	register int isfloat = 0, sawDigit = 0;

	if (np->t_flag & T_NUM)
		return (!(np->t_flag & T_INT));

	for (cp = np->t_STRING;;cp++) {
		switch (*cp) {
		case 0:
			return (isfloat);
		case 'e':
		case 'E':
			if (!sawDigit)
				return (0);
		case '.':
			sawDigit = isfloat = 1;
			break;
		case ' ':
		case '\t':
			if (sawDigit)
				return (0);
			break;
		default:
			if (isdigit(*cp) || '-' == *cp) {
				sawDigit = 1;
				break;
			}
			return (0);
		}
	}
}

/*
 * Fetch a character from
 * the program, either on
 * the command line or in a file.
 */
pgetc()
{
	register int c;

	if (parg != NULL) {
		if (eosflag)
			return (EOF);
		else if ((c = *parg++) == '\0') {
			eosflag = 1;
			return ('\n');
		}
	} else {
		while ((c = getc(pfp)) == '\\') {
			if ((c = getc(pfp)) != '\n') {
				ungetc(c, pfp);
				return ('\\');
			}
			/* bypass \ newline */
			if (lineno == 0)
				lineno++;
			lineno++;
		}
	}
	if (c == '\n') {
		if (lineno == 0)
			lineno++;
		lineno++;
	}
	return (c);
}

/*
 * Set up a new string for the
 * lexical analyser to read.
 */
pgetinit(s)
CHAR *s;
{
	parg = s;
	eosflag = 0;
	lineno = 0;
}

/*
 * Return a character to the
 * program input.
 */
pungetc(c)
int c;
{
	if (parg != NULL) {
		if (eosflag)
			c = '\0';
		*--parg = c;
		eosflag = 0;
	} else
		ungetc(c, pfp);
	if (c == '\n')
			--lineno;
}

/*
 * Error routines and usage
 * messages.
 */
usage()
{
	fprintf(stderr,
	   "Usage: awk [-y] [-Fc] [-f prog] [parameters] [prog] [file ...]\n");
	exit(1);
}

/* VARARGS */
awkerr(x)
{
	fprintf(stderr, "awk: ");
	if (FILENAME[0] != '\0')
		fprintf(stderr, "%s: ", FILENAME);
	if (lineno != 0)
		fprintf(stderr, "%d: ", lineno);
	fprintf(stderr, "%r", &x);
	putc('\n', stderr);
	exit(1);
}

/* VARARGS */
awkwarn(x)
{
	fprintf(stderr, "awk: ");
	if (FILENAME[0] != '\0')
		fprintf(stderr, "%s: ", FILENAME);
	if (lineno != 0)
		fprintf(stderr, "%d: ", lineno);
	fprintf(stderr, "Warning: %r", &x);
	putc('\n', stderr);
}

/* VARARGS */
yyerror(x)
{
	awkerr(x);
}
