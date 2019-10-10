/*
 * lex3.c
 * output and table generating routines
 */
#include "lex.h"

/* VARARGS */
output(s)
char *s;
{
	fprintf (fileout, "%r", &s);
}

/* VARARGS 2 */
loutput(i, s)
char *s;
{
	while (i--)
		putc('\t', fileout);
	fprintf (fileout, "%r\n", &s);
}

/*
 * estimate the size of the working space
 * the generated automata would require, worst case.
 * this is non-optimum, but fast.
 * optimum could be a thousand times slower to save a few bytes
 */
ltable()
{
	register c, i, l;
	register lookflag;

	l = 0;
	i = 0;
	lookflag = 0;
	while (i < nxt) {
		c = 0;
		do {
			switch(nfa[i][0]) {
			case LX_LINK:
			case LX_JUMP:
				c += mark(nfa[i][1]);
			case LX_BLIN:
			case LX_ELIN:
			case LX_SCON:
				if (nfa[i][0] != LX_JUMP)
					c += mark(i+1);
				++c;
				nfa[i][0] = -nfa[i][0];
				break;
			case LX_LOOK:
			case -LX_LOOK:
				lookflag = 1;
				if (c > l)
					l = c;
				c = mark(i+1);
				break;
			case LX_STOP:
				if (nfa[i][1])
					nfa[i][1] = 0;
				else
					++c;
			}
		} while (nfa[i++][0] != LX_STOP);
		if (c > l)
			l = c;
	}
	++l;
	for (i=0; i<nxt; ++i)
		if (nfa[i][0] < 0)
			nfa[i][0] = -nfa[i][0];
	loutput(0, "int *yy_clist[0%o];", l);
	loutput(0, "int *yy_nlist[0%o];", l);
	if (lookflag == 0)
		return;
	loutput(0, "int *yy_llist[0%o];", l);
}

/*
 * used by ltable, mark a state as counted
 */
mark(state)
{
	switch (nfa[state][0]) {
	case LX_STOP:
		if (nfa[state][1]++)
			break;
	case LX_LOOK:
	case LX_ACPT:
	case LX_CHAR:
	case LX_CLAS:
	case LX_ANYC:
		nfa[state][0] = -nfa[state][0];
		return (1);
	}
	return (0);
}

stats()
{
	fprintf(stderr, "rules: %d\tstates: %d\tclasses: %d\n",
		actn, nxt, clas);
}

/*
 * put out #defines for start conditions
 */
sdefns()
{
	register struct def *pd;

	pd = scnstart;
	while ((pd=pd->d_next) != NULL)
		loutput(0, "#define\t%s\t\t0%o", pd->d_name, pd->d_data);
}

/*
 * put out #defines for contexts
 */
xdefns()
{
	register struct def *pd;
	register i = 0;

	for (pd=ctxstart; pd!=NULL; pd=pd->d_next)
		++i;
	loutput(0, "#define\tyyswitch(x)\tyyctxt((x),(0%o))", i);
	i = 0;
	pd = ctxstart;
	while ((pd=pd->d_next) != NULL)
		loutput(0, "#define\t%s\t\t0%o", pd->d_name, ++i);
}

/*
 * table of nfa starting locations for contexts
 */
xtable()
{
	register struct def *pd;

	loutput(0, "int yy_ctxtab[] = {");
	output("00");
	pd = ctxstart;
	while ((pd=pd->d_next) != NULL)
		output(",0%o", pd->d_data);
	output("\n");
	loutput(0, "};");
}

/*
 * character class tables, here we convert the vertical
 * class storage to horizontal to save space
 */
btable()
{
	register unsigned char *index;
	register bit;
	register b, n, i, t;

	if (classptr == NULL)
		return;
	b = 1;
	loutput(0, "int yy_lxctab[] = {");
	for (n=0; n<clas; ++n) {
		index = classptr + classindex(n);
		bit = classbit(n);
		t = 0;
		i = 0;
		while (i <= MAXUCHAR) {
			if (index[i]&bit)
				t |= b;
			if ((b<<=1) == 0) {
				output("0%o", t);
				if (n+1<clas || i<MAXUCHAR)
					output(",");
				if (++i%(8*NBINT) == 0)
					output("\n");
				b = 1;
				t = 0;
			} else
				++i;
		}
	}
	loutput(0, "};");
	/*
	 * output the bit selector table
	 */
	loutput(0, "int yy_lxbtab[] = {");
	i = 0;
	while (i < NBINT) {
		output("01<<0%02o", i);
		if (++i < NBINT)
			output(",");
		if (i%(NBINT/2) == 0)
			output("\n");
	}
	loutput(0, "};");
}

/*
 * output the actual automata table
 * each state is packed into an int with the low four bits
 * being the opcode, the high twelve being the data
 */
ptable()
{
	register i,s;

	loutput(0, "int yy_lextab[] = {");
	for (s=0,i=0; i<nxt; ++s,++i) {
		if (i)
			output(",");
		if (s && s%8 == 0)
			output("\n");
		output("0%o", nfa[i][0] | (nfa[i][1]<<LR_SHFT));
	}
	output("\n");
	loutput(0, "};");
}

/*
 * put a human readable form of the automaton onto stderr
 * undocumented, but useful for debugging and sort of interesting
 */
printnfa()
{
	register i;

	for (i=0; i<nxt; ++i) {
		fprintf(stderr, "%d:\t", i);
		switch (nfa[i][0]) {
		case LX_STOP:
			fprintf(stderr, "stop");
			break;
		case LX_LINK:
			fprintf(stderr, "link ");
			if (nfa[i][1] > 0)
				putc('+', stderr);
			fprintf(stderr, "%d", nfa[i][1]);
			break;
		case LX_JUMP:
			fprintf(stderr, "jump ");
			if (nfa[i][1] > 0)
				putc('+', stderr);
			fprintf(stderr, "%d", nfa[i][1]);
			break;
		case LX_LOOK:
			fprintf(stderr, "look");
			break;
		case LX_ACPT:
			fprintf(stderr, "acpt %d", nfa[i][1]);
			break;
		case LX_CHAR:
			fprintf(stderr, "char ");
			if (nfa[i][1] == EOF)
				fprintf(stderr, "EOF");
			else {
				putc('\'', stderr);
				if (iscntrl(nfa[i][1])) switch (nfa[i][1]) {
				case '\b':
					fprintf(stderr, "\\b");
					break;
				case '\f':
					fprintf(stderr, "\\f");
					break;
				case '\n':
					fprintf(stderr, "\\n");
					break;
				case '\r':
					fprintf(stderr, "\\r");
					break;
				case '\t':
					fprintf(stderr, "\\t");
					break;
				default:
					fprintf(stderr, "\\%03o", nfa[i][1]);
				} else
					putc(nfa[i][1], stderr);
				putc('\'', stderr);
			}
			break;
		case LX_CLAS:
			fprintf(stderr, "clas %d", nfa[i][1]);
			break;
		case LX_BLIN:
			fprintf(stderr, "blin");
			break;
		case LX_ELIN:
			fprintf(stderr, "elin");
			break;
		case LX_ANYC:
			fprintf(stderr, "anyc");
			break;
		case LX_SCON:
			fprintf(stderr, "scon %d", nfa[i][1]);
			break;
		case LX_TERM:
			fprintf(stderr, "term");
			break;
		default:
			fprintf(stderr, "undefined");
		}
		putc('\n', stderr);
	}
}
