/*
 * lex/lex2.c
 * lexical analyser and utilities
 */

#include "lex.h"
#define	CMAX	3

char	cbuf[CMAX];
int	bufc = 0;
int	lstchr = '\n';
int	line = 0;

char *
alloc(n)
register n;
{
	register char *pc;

	if ((pc=malloc(n)) == NULL)
		error(outmem);
	return (pc);
}

char *
ralloc(pc, n)
char *pc;
{
	if ((pc=realloc(pc, n)) == NULL)
		error(outmem);
	return (pc);
}

/*
 * lexical ananlyser for regular expressions
 */
yylex()
{
	switch (yylval = next()) {
	case '\\':
		yylval = escape();
		return (LX_CHAR);
	case '"':
		inquotes = 1-inquotes;
		return (LX_OPER);
	case EOF:
		error(eoferr);
	default:
		if (inquotes)
			if (yylval == '\n')
				error("%s in quoted string", illnln);
			else
				return (LX_CHAR);
	}
	switch (yylval) {
	case '[':
		yylval = getclas();
		return (LX_CLAS);
	case '.':
		return (LX_ANYC);
	case ' ':
	case '\t':
	case '\n':
		return (LX_TERM);
	case '/':
		return (indefs? LX_OPER : LX_TERM);
	case '$':
		if (!indefs && isspace(look(0)))
			return (LX_TERM);
	case '^':
	case '<':
	case '(':
	case ')':
	case '{':
	case '*':
	case '?':
	case '+':
	case '|':
		return (LX_OPER);
	default:
		if (!isascii(yylval) || iscntrl(yylval))
			error("%s in regular expression", illchr);
		return (LX_CHAR);
	}
}

/*
 * get a backslashed character
 */
escape()
{
	register c, i;

	switch (c = next()) {
	case 'b':
		return ('\b');
	case 'f':
		return ('\f');
	case 'n':
		return ('\n');
	case 'r':
		return ('\r');
	case 't':
		return ('\t');
	default:
		if (isoctl(c)) {
			for (i=0; c-='0', i<2 && isoctl(look(0)); ++i)
				c = c * 8 + next();
			if (c > MAXUCHAR)
				error(illoct);
		}
		return (c);
	}
}

/*
 * read and store a class specification
 */
getclas()
{
	register unsigned int c,d;
	register unsigned char *index;
	register int bit;
	register int invert;

	if (clas == 0) {
		classptr = alloc(MAXUCHAR+1);
		index = classptr + (c = MAXUCHAR + 1);
		while (c--)
			*--index = 0;
	} else if ((clas % NBCHAR) == 0) {
		c = classindex(clas + NBCHAR);
		classptr = ralloc(classptr, c);
		index = classptr + c;
		c = MAXUCHAR + 1;
		while (c--)
			*--index = 0;
	} else
		index = classptr + classindex(clas);
	bit = classbit(clas);
	if (look(0) == '^') {
		invert = 1;
		next();
	} else
		invert = 0;
	while ((c=next()) != ']') {
		if (iscntrl(c))
			error("%s in class", illchr);
		if (c == EOF)
			error(eoferr);
		if (c == '.') {
			c = index['\n'] & bit;
			for (d=0; d<=MAXUCHAR; ++d)
				index[d] |= bit;
			if (!c)
				index['\n'] ^= bit;
		} else {
			if (c == '\\')
				c = escape();
			d = c;
			if (look(0) == '-')
				if (next(), look(0) != ']') {
					if ((d=next()) == '\\')
						d = escape();
					if (d < c)
						error(illrng);
				} else
					index['-'] |= bit;
			do {
				index[c] |= bit;
			} while (c++ < d);
		}
	}
	if (invert) {
		for (d=0; d<=MAXUCHAR; ++d)
			index[d] ^= bit;
	}
	return (clas++);
}

/*
 * get next character from input
 */
next()
{
	static l = '\n';
	register c, i;

	if (l == '\n')
		++line;
	if (bufc == 0)
		c = lstchr = getc(filein);
	else {
		c = cbuf[0];
		for (i=1; i < bufc; i++)
			cbuf[i-1] = cbuf[i];
		--bufc;
	}
	return (l=c);
}

/*
 * look into input for the n+1th character
 * practically, n never exceeds 2
 */
look(n)
register int n;
{
	while (bufc <= n)
		cbuf[bufc++] = lstchr = getc(filein);
	return (cbuf[n]);
}

/*
 * delete input up to and including the next newline
 */
dnl()
{
	while (next() != '\n')
		;
	setltype();
}

/*
 * eat up white space
 */
eatspc()
{
	register c;

	while (c=look(0), c==' ' || c=='\t')
		next();
	if (c == EOF)
		error(eoferr);
}

/*
 * eat input until white space
 */
eatlbl()
{
	register c;

	while (c=look(0), !isspace(c))
		next();
	if (c == '\n')
		return (1);
	return (0);
}

/*
 * copy an input line to the output file
 */
lcopy()
{
	register c;

	do {
		putc(c=next(), fileout);
	} while (c != '\n');
	setltype();
}

/*
 * set external variable ltype to the type of the next line
 */
setltype()
{
	ltype = getltype();
}

/*
 * return line type based on the first couple of characters
 */
getltype()
{
	switch (look(0)) {
	case '\t':
	case '\n':
	case ' ':
		return (LN_LSPC);
	case EOF:
		return (LN_EOFL);
	case '%':
		switch (look(1)) {
		case '%':
			return (LN_DLIM);
		case '{':
			return (LN_LCOM);
		case '}':
			return (LN_RCOM);
		case 's':
		case 'S':
			return (LN_SCON);
		case 'c':
		case 'C':
			return (LN_CTXT);
		case 'x':
		case 'X':
			return (LN_OPTN);
		}
	}
	return (LN_DFLT);
}

/*
 * put out a #line command for the C preprocessor
 */
outlnum(i)
{
	loutput(0, "#line %d", line+i);
}

/*
 * copy a user-specified action to the output
 */
getactn()
{
	enum cstate { /* states of c source */
		start, slash, comment, star, bsl, dquote, squote
	} pstate;
	register enum cstate state;
	register c;
	int bct;

	for (state = start, bct = 0;;) {
		if (EOF == (c = next()))
			error(eoferr);
		putc(c, fileout);

		switch (state) {
		case slash:	/* maybe a comment */
			if ('*' == c) {
				state = comment;
				break;
			}
			state = start;
		case start:	/* normal text state */
			switch (c) {
			case '/':
				state = slash;
				break;
			case '\\':
				pstate = state;
				state = bsl;
				break;
			case '"':
				state = dquote;
				break;
			case '\'':
				state = squote;
				break;
			case '{':
				bct++;
				break;
			case '}':
				if (--bct < 0)
					error(actsyn);
				break;
			case '\n':
				if (!bct) {
					setltype();
					return;
				}
			}
			break;
		case star:	/* saw * in comment */
			if ('/' == c) {
				state = start;
				break;
			}
			state = comment;
		case comment:	/* in comment */
			if ('*' == c)
				state = star;
			break;
		case bsl:	/* char after backslash */
			state = pstate;
			break;
		case dquote:	/* in double quoted string */
		case squote:	/* in single quoted string */
			switch (c) {
			case '"':
				if (dquote == state)
					state = start;
				break;
			case '\'':
				if (squote == state)
					state = start;
				break;
			case '\n':
				error(actsyn);
				break;
			case '\\':
				pstate = state;
				state = bsl;
			}
		}
	}
}

/*
 * read an identifier
 */
char *
getident()
{
	register c, i = 0;
	register char *pc;

	eatspc();
	pc = alloc(NCBLK);
	while (c=look(0), isalnum(c) || c=='_') {
		pc[i++] = next();
		if (i%NCBLK == 0)
			pc = ralloc((char *)pc, i+NCBLK);
	}
	pc[i] = '\0';
	eatspc();
	return (pc);
}

/*
 * interpret an identifier as the name of a
 * start condition, try to return its value
 */
getstart()
{
	register struct def *pd;
	register char *pc;

	pd = scnstart;
	pc = getident();
	do {
		if (strcmp(pc, pd->d_name) == 0) {
			free(pc);
			return (pd->d_data);
		}
	} while ((pd=pd->d_next) != NULL);
	error(undstc);
}

/*
 * add a string of identifiers to the start condition
 * list, numbering them as we go, too bad if they
 * are duplicates
 */
addstart()
{
	register struct def *pd;

	if (eatlbl())
		return;
	pd = scnstart;
	while (look(0) != '\n') {
		pd->d_next = alloc(sizeof(struct def));
		pd->d_next->d_data = pd->d_data + 1;
		pd = pd->d_next;
		pd->d_next = NULL;
		pd->d_name = getident();
	}
}

/*
 * like addstart, except with names of contexts
 */
addcontext()
{
	register struct def *pd;

	if (eatlbl())
		return;
	pd = ctxstart;
	while (pd->d_next != NULL)
		pd = pd->d_next;
	pd->d_next = alloc(sizeof(struct def));
	pd = pd->d_next;
	for (;;) {
		pd->d_name = getident();
		pd->d_data = 0;
		if (look(0) == '\n') {
			pd->d_next = NULL;
			break;
		} else {
			pd->d_next = alloc(sizeof(struct def));
			pd = pd->d_next;
		}
	}
}

/*
 * this is called to mark the context name with its
 * associated section of the nfa
 */
markcontext(t)
{
	register char *pc;
	register struct def *pd;

	eatlbl();
	pc = getident();
	pd = ctxstart;
	while ((pd=pd->d_next) != NULL)
		if (strcmp(pc, pd->d_name) == 0) {
			pd->d_data = t;
			free(pc);
			return;
		}
	error(undctx);
}

/*
 * read the name of the definition, try
 * to return where it starts
 */
getdefn()
{
	register char *pc;
	register struct def *pd;

	pc = getident();
	if (look(0) == '}')
		next();
	else
		error("%s in definition name", illchr);
	for (pd=defstart; pd!=NULL; pd=pd->d_next)
		if (strcmp(pc, pd->d_name) == 0) {
			free(pc);
			return (pd->d_data);
		}
	error(unddef);
}

/*
 * recursive, costly but elegant
 */
freedef(pd)
register struct def *pd;
{
	if (pd != NULL) {
		freedef(pd->d_next);
		if (pd->d_name != NULL)
			free(pd->d_name);
		free((char *)pd);
	}
}

/*
 * cleanup and abort
 */
/* VARARGS */
error(s)
{
	fprintf(stderr, "lex: ");
	if (line)
		fprintf(stderr, "%d: ", line);
	fprintf(stderr, "%r\n", &s);
	if (filein == stdin)
		while (lstchr!='\n' && lstchr!=EOF)
			next();
	if (fileout != stdout)
		unlink(OUTFILE);
	exit (1);
}

usage()
{
	fprintf(stderr, "Usage: lex [-tv] [filename]\n");
	exit (1);
}

/* end of lex2.c */
