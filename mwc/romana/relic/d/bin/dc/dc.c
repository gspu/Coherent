/*
 * DC - Reverse Polish desk calculator (multi-precision)
 * Depends on mint value being defined as a `char *'
 * so mint may also be char string
 */
#include <stdio.h>
#include "bc.h"

char	*version = "DC Version 1.00.\n";

#define	NREG	256
#define	NSTACK	256

struct	reg_t	{
	struct	reg_t	*next;
	rvalue	regval;
}	*reg[NREG];

rvalue	stack[NSTACK],
	*sp = &stack[0];

static	char	*soflmsg = "Out of pushdown",
		*suflmsg = "stack empty",
		*oospmsg = "Out of space",
		*nregmsg = "Missing reg name";

#define	skiperr(m) {fprintf(stderr,"%s\n",m);return(-1);}

#define	push(x)	{if(sp==&stack[NSTACK]){skiperr(soflmsg);}else{x=sp++;}}

#define	pop(x)	{if(sp==&stack[0]){skiperr(suflmsg);}else{x=--sp;}}

#define	new(x)	{push(x);minit(&(x)->mantissa);}

#define	temp(x)	{push(x);pop(x);}

#define	tos(x)	{pop(x);push(x);}

#define	getreg(c,r){if((c=getc(infile))==EOF){skiperr(nregmsg);}else{r=reg[c];}}

#define	newreg(c, r) {\
		struct reg_t *nr=(struct reg_t *)malloc(sizeof(struct reg_t));\
		if (nr == NULL) {\
			skiperr(oospmsg);\
		} else {\
			nr->next = r;\
			r = reg[c] = nr;\
		}\
	}

#define	execute(x, y) if((x)->scale<0){\
	FILE f;char *s=(x)->mantissa.val;if(y)minit(&(x)->mantissa);\
	_stropen(s,strlen(s),&f);c=interp(&f);infile=fp;if(y)mpfree(s);\
	if(c)return(c-1);}

main(argc, argv)
int	argc;
char	*argv[];
{
	FILE	*fp;

	init();
	if (argc > 1)
		if (!strcmp(argv[1], "-V")) {
			fprintf(stderr, version);
			exit(0);
		} else if ((fp=fopen(argv[1], "r"))==NULL) {
			fprintf(stderr, "Dc: can't open %s\n", argv[1]);
			return (1);
		} else if (interp(fp) > 0)
			return (0);
		else
			fclose(fp);
	while (interp(stdin) < 0)
		;
	return (0);
}

interp(fp)
FILE	*fp;
{
	register int	c;
	register rvalue	*a, *b;
	struct	reg_t	*r;
	int	(*f)();
	int	d;
	extern int iseq(), isne(), islt(), isle(), isge(), isgt();
	extern int dcsub(), bcadd(), bcmul(), bcdiv(), bcrem(), bcexp();
	extern int sibase(), sobase(), output();

	infile = fp;
	for (f=NULL;;f=NULL) switch (c = getc(infile)) {
	case EOF:
		return (0);
	case ' ':
	case '\t':
	case '\n':
		continue;
	case '_':
		c = getc(infile);
		f = mneg;
		/* Fall through */
	case '0': case '1': case '2': case '3':
	case '4': case '5': case '6': case '7':
	case '8': case '9': case 'A': case 'B':
	case 'C': case 'D': case 'E': case 'F':
	case '.':
		push(a);
		b = getnum(c);
		*a = *b;
		mpfree((char *)b);
		if (f!=NULL)
			(*f)(&a->mantissa, &a->mantissa);
		continue;
	case '-':
		f = dcsub;
		goto binary;
	case '+':
		f = bcadd;
		goto binary;
	case '*':
		f = bcmul;
		goto binary;
	case '/':
		f = bcdiv;
		goto binary;
	case '%':
		f = bcrem;
		goto binary;
	case '^':
		f = bcexp;
		/* Fall through */
	binary:
		pop(b);
		pop(a);
		(*f)(b, a);
		push(a);
		continue;
	case '[':
		push(a);
		if (c = rdstring(a))
			return (c);
		continue;
	case '<':
		f = islt;
		goto compare;
	case '=':
		f = iseq;
		goto compare;
	case '>':
		f = isgt;
		goto compare;
	case '!':
		switch (c=getc(infile)) {
		case '<':
			f = isge;
			goto compare;
		case '=':
			f = isne;
			goto compare;
		case '>':
			f = isle;
			goto compare;
		default:
			ungetc(c, infile);
			temp(a);
			rdline(a, infile);
			system(a->mantissa.val);
			printf("!\n");
			mvfree(&a->mantissa);
			continue;
		}
	compare:
		pop(b);
		pop(a);
		d = (*f)(bccmp(b, a));
		getreg(c, r);
		if (d && r != NULL)
			execute(&r->regval, 0);
		continue;
	case '?':
		temp(a);
		rdline(a, stdin);
		execute(a, 1);
		continue;
	case 'c':
		while (sp != &stack[0]) {
			pop(a);
			mvfree(&a->mantissa);
		}
		continue;
	case 'd':
		tos(a);
		new(b);
		mcopy(&a->mantissa, &b->mantissa);
		b->scale = a->scale;
		continue;
	case 'f':
		for (c = 0;  c != NREG; c++)
			if ((r=reg[c]) != NULL) {
				printf("`%c': ", c);
				output(&r->regval);
			}
		printf("stack:\n");
		for (a = &stack[0];  a < sp;  a++)
			output(a);
		continue;
	case 'i':
		f = sibase;
		goto unary;
	case 'I':
	case 'K':
		new(a);
		mitom((c=='I' ? ibase : scale), &a->mantissa);
		a->scale = 0;
		continue;
	case 'k':
		pop(a);
		scale = rtoint(a);
		mvfree(&a->mantissa);
		if (scale < 0) {
			scale = 0;
			skiperr("Scale < 0");
		}
		continue;
	case 'l':
	case 'L':
		getreg(c, r);
		new(a);
		if (r == NULL) {
			newscalar(a);
		} else if (c == 'l') {
			mcopy(&r->regval.mantissa, &a->mantissa);
			a->scale = r->regval.scale;
		} else {
			reg[c] = r->next;
			*a = r->regval;
			free((char *)r);
		}
		continue;
	case 'o':
		f = sobase;
		goto unary;
	case 'O':
		new(a);
		mcopy(&outbase, &a->mantissa);
		a->scale = 0;
		continue;
	case 'p':
		tos(a);
		output(a);
		continue;
	case 'P':
		f = output;
		/* Fall through */
	unary:
		pop(a);
		(*f)(a);
		mvfree(&a->mantissa);
		continue;
	case 'q':
		return (1);
		continue;
	case 'Q':
		pop(a);
		c = rtoint(a);
		mvfree(&a->mantissa);
		return (--c > 0 ? c : 0);
		continue;
	case 's':
		getreg(c, r);
		pop(a);
		if (r != NULL) {
			mvfree(&r->regval.mantissa);
		} else
			newreg(c, r);
		r->regval = *a;
		continue;
	case 'S':
		getreg(c, r);
		pop(a);
		newreg(c, r);
		r->regval = *a;
		continue;
	case 'v':
		tos(a);
		bcsqrt(a);
		continue;
	case 'x':
		pop(a);
		execute(a, 1);
		continue;
	case 'X':
		tos(a);
		mitom(a->scale, &a->mantissa);
		a->scale = 0;
		continue;
	case 'z':
		new(a);
		mitom(sp - &stack[0], &a->mantissa);
		a->scale = 0;
		continue;
	case 'Z':
	{
		char	*s;

		tos(a);
		s = mtos(&a->mantissa);
		mitom(strlen(s), &a->mantissa);
		a->scale = 0;
		mpfree(s);
	}
		continue;
	default:
		fprintf(stderr, "`%c'", c);
		skiperr("?");
	}
}

iseq(x)
{
	return (x==0);
}

isne(x)
{
	return (x!=0);
}

islt(x)
{
	return (x<0);
}

isle(x)
{
	return (x<=0);
}

isge(x)
{
	return (x>=0);
}

isgt(x)
{
	return (x>0);
}

rdstring(v)
rvalue	*v;
{
	register int	c;
	register char	*s,
			*str;
	unsigned int	len,
			d = 0;		/* nesting depth */

	s = str = malloc(len=16);
	while ((c=getc(infile)) != EOF) {
		if (c == '[')
			++d;
		else if (c == ']' && d-- == 0)
			break;
		if (str != NULL)
			*s++ = c;
		if (s == &str[len]) {
			str = realloc(str, len*=2);
			s = &str[len/2];
		}
	}
	if (str == NULL) {
		skiperr(oospmsg);
	} else if (c == EOF) {
		skiperr("Missing ']'");
	} else {
		*s = '\0';
		v->mantissa.val = str;
		v->mantissa.len = len;
		v->scale = -1;
		return (0);
	}
}
rdline(v, fp)
rvalue	*v;
FILE	*fp;
{
	register int	c;
	register char	*s,
			*str;
	unsigned int	len;

	s = str = malloc(len=16);
	while ((c=getc(fp))!= EOF && c != '\n') {
		if (str != NULL)
			*s++ = c;
		if (s == &str[len]) {
			str = realloc(str, len*=2);
			s = &str[len/2];
		}
	}
	if (str == NULL) {
		skiperr(oospmsg);
	} else {
		*s = '\0';
		v->mantissa.val = str;
		v->mantissa.len = len;
		v->scale = -1;
		return (0);
	}
}

output(v)
rvalue	*v;
{
	if (v->scale < 0)
		printf("%s\n", v->mantissa.val);
	else {
		putnum(v);
		pnewln();
	}
}
