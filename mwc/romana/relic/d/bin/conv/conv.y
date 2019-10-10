/* conv - programmer's calculator
 * conv evaluates expressions in c style syntax and reports the results
 * in hexadecimal, decimal, octal, binary, and ascii character constants.
 * If arguments are present, they are each evaluated as independent
 * expressions, otherwise each line of standard input is evaluated.
 * Input numbers follow the style of the old conv, ie
 *	#digits	- hexadecimal
 *	0xdigits - hexadecimal
 *	0Xdigits - hexadecimal
 *	0digits - octal
 *	$digits - binary
 *	digits - decimal
 *	'character - ascii character constant
 * The only expressions not allowed are those involving arrays, pointers,
 * structure members, and addresses.  All of the integer arithmetic ops
 * are supported including variables, assignment ops, conditional expressions,
 * and the comma operator.
 * In addition, there is an iterative conditional expression
 *	<expr1> ?? <expr2> :: <expr3>
 * which evaluates to while (<expr1>) <expr2>; <expr3>, that is evaluating
 * <expr2> as often as <expr1> evaluates to true, then returning the value
 * of <expr3>.
 * Bug: function definition and evaluation should be permitted.
 */
%{
#include <ctype.h>

#define	NNODES	1000

char	*version = "Conv Version 2.00.\n";

typedef union yynode {
	struct {
		int	n_op;
		union yynode *n1_ln;
		union yynode *n1_rn;
	} yyn1;
	struct {
		int	n_op;
		long	n2_val;
	} yyn2;
	struct {
		char	n3_name[];
	} yyn3;
} NODE;
#define n_ln	yyn1.n1_ln
#define n_rn	yyn1.n1_rn
#define n_val	yyn2.n2_val
#define n_name	yyn3.n3_name

#define NULL ((NODE *)0)
NODE *node();
NODE *vnode();
NODE *nnode();
NODE *hash();
NODE *dotp;
long getcc();
long getv();

%}

%union {
	int	yy_op;
	long	yy_val;
	NODE	*yy_nod;
}

%start list

%token NUMBER NAME CALL LINK
%token INC DEC INCBEF DECBEF INCAFT DECAFT

%left ','
%right '=' ADDEQ SUBEQ MULEQ DIVEQ REMEQ ANDEQ IOREQ EOREQ SHLEQ SHREQ
%left QUQU COCO
%left '?' ':'
%left OR
%left AND
%left '|'
%left '^'
%left '&'
%left EQ NE
%left '<' '>' LE GE
%left SHL SHR
%left '+' '-'
%left '*' '/' '%'
%right '~' '!' UMINUS

%type <yy_op> '=' ADDEQ SUBEQ MULEQ DIVEQ REMEQ ANDEQ IOREQ EOREQ SHLEQ SHREQ
%type <yy_op> '?' ':' OR AND '|' '^' '&'
%type <yy_op> EQ NE '<' '>' LE GE
%type <yy_op> SHL SHR '+' '-' '*' '/' '%'
%type <yy_op> '~' '!' UMINUS '(' ')' ','
%type <yy_op> INC DEC INCAFT INCBEF DECAFT DECBEF
%type <yy_nod> NAME
%type <yy_val> NUMBER
%type <yy_nod> expr

%%

list:	/* empty */
|	list '\n'
|	list expr '\n' {
		eval($2, dotp->n_ln);
			switch ($2->n_op) {
		case IOREQ:
		case EOREQ:
		case ANDEQ:
		case SHLEQ:
		case SHREQ:
		case REMEQ:
		case DIVEQ:
		case MULEQ:
		case SUBEQ:
		case ADDEQ:
		case '=':
			break;
		default:
			putv(dotp->n_ln);
		}
		nfree($2);
	}
|	list error '\n' {
		yyerrok;
	}
;


expr:	expr ',' expr {
		$$ = node($2, $1, $3);
	}
|	NAME '=' expr {
		$$ = node($2, $1, $3);
	}
|	NAME ADDEQ expr {
		$$ = node($2, $1, $3);
	}
|	NAME SUBEQ expr {
		$$ = node($2, $1, $3);
	}
|	NAME MULEQ expr {
		$$ = node($2, $1, $3);
	}
|	NAME DIVEQ expr {
		$$ = node($2, $1, $3);
	}
|	NAME REMEQ expr {
		$$ = node($2, $1, $3);
	}
|	NAME ANDEQ expr {
		$$ = node($2, $1, $3);
	}
|	NAME IOREQ expr {
		$$ = node($2, $1, $3);
	}
|	NAME EOREQ expr {
		$$ = node($2, $1, $3);
	}
|	NAME SHLEQ expr {
		$$ = node($2, $1, $3);
	}
|	NAME SHREQ expr {
		$$ = node($2, $1, $3);
	}
|	NAME INC {
		$$ = node(INCAFT, $1, NULL);
	}
|	NAME DEC {
		$$ = node(DECAFT, $1, NULL);
	}
|	INC NAME {
		$$ = node(INCBEF, $2, NULL);
	}
|	DEC NAME {
		$$ = node(DECBEF, $2, NULL);
	}
|	expr QUQU expr COCO expr {
		$$ = node($2, $1, node($4, $3, $5));
	}
|	expr '?' expr ':' expr {
		$$ = node($2, $1, node($4, $3, $5));
	}
|	expr OR expr {
		$$ = node($2, $1, $3);
	}
|	expr AND expr {
		$$ = node($2, $1, $3);
	}
|	expr '|' expr {
		$$ = node($2, $1, $3);
	}
|	expr '^' expr {
		$$ = node($2, $1, $3);
	}
|	expr '&' expr {
		$$ = node($2, $1, $3);
	}
|	expr EQ expr {
		$$ = node($2, $1, $3);
	}
|	expr NE expr {
		$$ = node($2, $1, $3);
	}
|	expr '<' expr {
		$$ = node($2, $1, $3);
	}
|	expr '>' expr {
		$$ = node($2, $1, $3);
	}
|	expr LE expr {
		$$ = node($2, $1, $3);
	}
|	expr GE expr {
		$$ = node($2, $1, $3);
	}
|	expr SHR expr {
		$$ = node($2, $1, $3);
	}
|	expr SHL expr {
		$$ = node($2, $1, $3);
	}
|	expr '+' expr {
		$$ = node($2, $1, $3);
	}
|	expr '-' expr {
		$$ = node($2, $1, $3);
	}
|	expr '*' expr {
		$$ = node($2, $1, $3);
	}
|	expr '/' expr {
		$$ = node($2, $1, $3);
	}
|	expr '%' expr {
		$$ = node($2, $1, $3);
	}
|	'-' expr %prec UMINUS {
		$$ = node(UMINUS, $2, NULL);
	}
|	'~' expr %prec UMINUS {
		$$ = node($1, $2, NULL);
	}
|	'!' expr %prec UMINUS {
		$$ = node($1, $2, NULL);
	}
|	NAME {
		$$ = $1;
	}
|	NUMBER {
		$$ = vnode($1);
	}
|	'(' expr ')' {
		$$ = node($1, $2, NULL);
	}
;


%%

yylex()
{
	register int c;
	char *seq = "=";
	char tmpbuf[64];
	register char *p;

again:
	while ((c=getc()) == ' ' || c == '\t')
		;

	if (isalnum(c) || c == '#' || c == '$') {
		tmpbuf[0] = c;
		c = getc();
		for (p = tmpbuf+1; p < &tmpbuf[64] && isalnum(c); c = getc())
			*p++ = c;
		ungetc(c);
		*p++ = 0;
		c = tmpbuf[0];
		if (c == '#' || c == '$' || isdigit(c)) {
			yylval.yy_val = getv(tmpbuf);
			return (NUMBER);
		}
		yylval.yy_nod = hash(tmpbuf);
		return (NAME);
	}
	switch (c) {

	case '\'':
		yylval.yy_val = getcc();
		return NUMBER;
	case '\\': if (substr("\n", 1, 0))
			goto again;
		return yylval.yy_op = c;
	case '+': return substr("=+", ADDEQ, INC, c);
	case '-': return substr("=-", SUBEQ, DEC, c);
	case '*': return substr(seq, MULEQ, c);
	case '/': return substr(seq, DIVEQ, c);
	case '%': return substr(seq, REMEQ, c);
	case '&': return substr("=&", ANDEQ, AND, c);
	case '|': return substr("=|", IOREQ, OR, c);
	case '^': return substr(seq, EOREQ, c);
	case '=': return substr(seq, EQ, c);
	case '!': return substr(seq, NE, c);
	case '<': if ((c = substr("<=", SHL, LE, c)) == SHL)
			return substr(seq, SHLEQ, c);
		return yylval.yy_op = c;
	case '>': if ((c = substr(">=", SHR, GE, c)) == SHR)
			return substr(seq, SHREQ, c);
		return yylval.yy_op = c;
	case '.':
		yylval.yy_nod = dotp;
		return NAME;
	}
	return yylval.yy_op = c;
}

NODE *reg[32];

NODE *
hash(bp)
char *bp;
{
	int hash;
	{
		register char *p;
		register c;

		hash = 0;
		p = bp;
		while (c = *p++)
			hash += c;
	}
	{
		register NODE *rp, **ep;

		bp[sizeof(NODE)] = 0;
		for (ep = &reg[hash & 31]; rp = *ep; ep = &rp->n_rn) {
			if (strcmp(bp, rp->n_ln->n_rn->n_name) == 0)
				return (rp->n_ln);
		}
		rp = node(NAME, vnode((long) 0), nnode(bp));
		*ep = node(LINK, rp, NULL);
		return (rp);
	}
}

/*
 * Depending on the next character, return one of several tokens.
 */
substr(cp, t1)
register char *cp;
int t1;
{
	register int cn;
	register int *tp;

	tp = &t1;
	cn = getc();
	while (*cp)
		if (*cp++ == cn)
			return yylval.yy_op = *tp;
		else
			tp += 1;
	ungetc(cn);
	return yylval.yy_op = *tp;
}

int peekc;
char *inputs = 0;

getc()
{
	register int c;

	if (peekc) {
		c = peekc;
		peekc = 0;
	} else if (inputs != 0) {
		if (c = *inputs)
			inputs += 1;
		else
			c = -1;
	} else
		c = getchar();
	return c;
}

ungetc(c)
{
	peekc = c;
}

main(argc, argv)
char *argv[];
{
	dotp = node(NAME, vnode((long) 0), NULL);
	if (argc > 1) {
		while (--argc > 0) {
			++argv;
			if (!strcmp(*argv, "-V")) {
				printf(version);
				exit(0);
			}
			inputs = malloc(strlen(*argv)+2);
			strcpy(inputs, *argv);
			strcat(inputs, "\n");
			yyparse();
		}
	} else {
		inputs = 0;
		yyparse();
	}
	return 0;
}
yyerror() { puts("Syntax"); }

NODE _nodes_[NNODES];
NODE *_np_ = &_nodes_[0];

NODE *
alloc()
{
	register NODE *np;

	np = _np_;
	for (;;) {
		if (np->n_op == 0) {
			_np_ = np;
			return (np);
		}
		if (++np >= &_nodes_[NNODES])
			np = &_nodes_[0];
		if (np == _np_)
			break;
	}
	puts("No space");
	exit(1);
}

nfree(np)
register NODE *np;
{
	if (np->n_op != NAME && np->n_op != NUMBER) {
		if (np->n_rn != NULL)
			nfree(np->n_rn);
		nfree(np->n_ln);
	}
	if (np->n_op != NAME) {
		np->n_op = 0;
		if (np < _np_)
			_np_ = np;
	}
}

NODE *
node(op, nl, nr)
NODE *nl, *nr;
{
	register NODE *np;

	np = alloc();
	np->n_op = op;
	np->n_ln = nl;
	np->n_rn = nr;
	return (np);
}

NODE *
vnode(va)
long va;
{
	register NODE *np;

	np = alloc();
	np->n_op = NUMBER;
	np->n_val = va;
	return (np);
}

NODE *
nnode(cp)
register char *cp;
{
	register NODE *np;
	register char *p;

	np = alloc();
	p = np->n_name;
	while (p < np+1)
		*p++ = *cp++;
	return (np);
}

eval(np, dp)
register NODE *np, *dp;
{
	NODE lval, rval, *lptr;

	dp->n_op = NUMBER;
again:
	switch (np->n_op) {
	case '=':
	case ADDEQ:
	case SUBEQ:
	case MULEQ:
	case DIVEQ:
	case REMEQ:
	case SHREQ:
	case SHLEQ:
	case ANDEQ:
	case EOREQ:
	case IOREQ:
		eval(np->n_rn, &rval);
		/* Fall through */
	case INCBEF:
	case DECBEF:
	case INCAFT:
	case DECAFT:
		lptr = np->n_ln->n_ln;
		break;
	case NAME:
		lptr = np->n_ln;
		break;
	case ',':
	case '|':
	case '^':
	case '&':
	case EQ:
	case NE:
	case '<':
	case '>':
	case LE:
	case GE:
	case SHR:
	case SHL:
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
		eval(np->n_rn, &rval);
		/* Fall through */
	case UMINUS:
	case '~':
	case '!':
	case QUQU:
	case '?':
	case OR:
	case AND:
		eval(np->n_ln, &lval);
		break;
	case NUMBER:
		lptr = np;
		break;
	case '(':
		np = np->n_ln;
		goto again;
	default:
		printf("Bad operator: %d\n", np->n_op);
		return;
	}
	switch (np->n_op) {
	case ',':
		np = np->n_rn;
		goto again;
	case '=':
		lptr->n_val = rval.n_val;
		*dp = *lptr;
		break;
	case ADDEQ:
		lptr->n_val += rval.n_val;
		*dp = *lptr;
		break;
	case SUBEQ:
		lptr->n_val -= rval.n_val;
		*dp = *lptr;
		break;
	case MULEQ:
		lptr->n_val *= rval.n_val;
		*dp = *lptr;
		break;
	case DIVEQ:
		lptr->n_val /= rval.n_val;
		*dp = *lptr;
		break;
	case REMEQ:
		lptr->n_val %= rval.n_val;
		*dp = *lptr;
		break;
	case SHREQ:
		lptr->n_val >>= rval.n_val;
		*dp = *lptr;
		break;
	case SHLEQ:
		lptr->n_val <<= rval.n_val;
		*dp = *lptr;
		break;
	case ANDEQ:
		lptr->n_val &= rval.n_val;
		*dp = *lptr;
		break;
	case EOREQ:
		lptr->n_val ^= rval.n_val;
		*dp = *lptr;
		break;
	case IOREQ:
		lptr->n_val |= rval.n_val;
		*dp = *lptr;
		break;
	case INCBEF:
		++lptr->n_val;
		*dp = *lptr;
		break;
	case DECBEF:
		--lptr->n_val;
		*dp = *lptr;
		break;
	case INCAFT:
		*dp = *lptr;
		lptr->n_val++;
		break;
	case DECAFT:
		*dp = *lptr;
		lptr->n_val++;
		break;
	case NAME:
		*dp = *lptr;
		break;
	case NUMBER:
		*dp = *lptr;
		break;
	case '|':
		dp->n_val = lval.n_val | rval.n_val;
		break;
	case '^':
		dp->n_val = lval.n_val ^ rval.n_val;
		break;
	case '&':
		dp->n_val = lval.n_val & rval.n_val;
		break;
	case EQ:
		dp->n_val = lval.n_val == rval.n_val;
		break;
	case NE:
		dp->n_val = lval.n_val != rval.n_val;
		break;
	case '<':
		dp->n_val = lval.n_val < rval.n_val;
		break;
	case '>':
		dp->n_val = lval.n_val > rval.n_val;
		break;
	case LE:
		dp->n_val = lval.n_val <= rval.n_val;
		break;
	case GE:
		dp->n_val = lval.n_val >= rval.n_val;
		break;
	case SHR:
		dp->n_val = lval.n_val >> rval.n_val;
		break;
	case SHL:
		dp->n_val = lval.n_val << rval.n_val;
		break;
	case '+':
		dp->n_val = lval.n_val + rval.n_val;
		break;
	case '-':
		dp->n_val = lval.n_val - rval.n_val;
		break;
	case '*':
		dp->n_val = lval.n_val * rval.n_val;
		break;
	case '/':
		dp->n_val = lval.n_val / rval.n_val;
		break;
	case '%':
		dp->n_val = lval.n_val % rval.n_val;
		break;
	case UMINUS:
		dp->n_val = - lval.n_val;
		break;
	case '~':
		dp->n_val = ~ lval.n_val;
		break;
	case '!':
		dp->n_val = ! lval.n_val;
		break;
	case QUQU:
		if (lval.n_val)
			eval(np->n_rn->n_ln, &rval);
		else
			np = np->n_rn->n_rn;
		goto again;
	case '?':
		if (lval.n_val)
			np = np->n_rn->n_ln;
		else
			np = np->n_rn->n_rn;
		goto again;
	case OR:
		if (lval.n_val != 0) {
			*dp = lval;
			break;
		}
		np = np->n_rn;
		goto again;
	case AND:
		if (lval.n_val == 0) {
			*dp = lval;
			break;
		}
		np = np->n_rn;
		goto again;
	}
}

long
getcc()
{
	register int c, d;
	c = getc();
	d = getc();
	if (d != '\'') ungetc(d);
	return c;
}

char digits[16] = "0123456789ABCDEF";

long
getv(dp)
register char *dp;
{
	register int d;
	register long v;
	register int b;

	v = 0;
	b = 10; d = *dp++;
	switch (d) {
	case '#': b = 16; d = *dp++; break;
	case '$': b = 2; d = *dp++; break;
	case '0': b = 8; d = *dp++;
		if (d == 'x' || d == 'X') {
			b = 16; d = *dp++;
		}
		break;
	}
	while ((isdigit(d) && (d -= '0') < b)
	    || (isupper(d) && (d += 10 - 'A') < b)
	    || (islower(d) && (d += 10 - 'a') < b)) {
		v = v * b + d;
		d = *dp++;
		if (d == 0)
			return v;
	}
	printf("Bad digit: 0%o\n", dp[-1]);
	return v;
}

putv(v) NODE *v;
{
	long	l;
	register	c;
	long		m;

	l = v->n_val;
	/*
	 * hex, decimal, octal
	 */
	printf( "#%X %D 0%O $", l, l, l);
	/*
	 * binary
	 */
	for (c=32, m=l; m; --c, m<<=1)
		if (m < 0)
			break;
	do {
		printf( m<0? "1": "0");
		m <<= 1;
	} while (--c);
	/*
	 * char
	 */
	printf( " '");
	if ((c=(char)l) < 0) {
		printf( "~");
		c &= 0177;
	}
	if (c<' ' || c=='\177') {
		printf( "^");
		if (c == '\177')
			c = '?';
		else
			c += '@';
	}
	printf( "%c'\n", c);
}
