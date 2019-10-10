/*
 * lex1.c
 * main and parsing routines
 */
#include "lex.h"

main(argc, argv)
char **argv;
{
	register i, s;
	int pflag = 0, tflag = 0, vflag = 0;

	while (argc>1 && **++argv=='-') {
		while (*++*argv) switch (**argv) {
		case 'p':
			++pflag;
			break;
		case 't':
			++tflag;
			break;
		case 'v':
			++vflag;
			break;
		default:
			usage();
		}
		--argc;
	}
	if (argc > 2)
		usage();
	if (!tflag) {
		if ((fileout=fopen(OUTFILE, "w")) == NULL)
			error(opnerr, OUTFILE);
	}
	if (argc > 1) {
		if ((filein=fopen(*argv, "r")) == NULL)
			error(opnerr, *argv);
		loutput(0, "#line 1 %s", *argv);
	}
	/*
	 * write header section of output
	 */
	loutput(0, "#include <stdio.h>");
	loutput(0, "extern\tchar\t\tyytext[];");
	loutput(0, "extern\tint\t\tyyleng;");
	loutput(0, "extern\tint\t\tyyscon;");
	loutput(0, "extern\tint\t\tyyline;");
	loutput(0, "#define\tinput()\t\tgetchar()");
	loutput(0, "#define\toutput(c)\tputchar(c)");
	loutput(0, "#define\tunput(c)\tyyback(c)");
	loutput(0, "#define\tECHO\t\t%s",
		"{register n=0; while (n<yyleng) output(yytext[n++]);}");
	loutput(0, "#define\tREJECT\t\tyyrjct()");
	loutput(0, "#define\tBEGIN\t\tyyscon = ");
	/*
	 * set up context and start condition lists
	 */
	ctxstart = alloc(sizeof(struct def));
	ctxstart->d_next = NULL;
	ctxstart->d_name = "0";
	scnstart = alloc(sizeof(struct def));
	scnstart->d_next = NULL;
	scnstart->d_name = "0";
	scnstart->d_data = 0;
	/*
	 * parse definitions section
	 * definitions are nfa segments that get copied wherever
	 * they are used, classes used in definitions are stored once.
	 * definition nfa's start at 0, each ends with LX_TERM
	 */
	setltype();
	indefs = 1;
	yyparse();
	indefs = 0;
	/*
	 * set beginning of context 0 to where definitions end
	 */
	ctxstart->d_data = nxt;
	/*
	 * write #defines for start conditions and contexts
	 */
	sdefns();
	xdefns();
	/*
	 * these two routines are used by the generated atuomata
	 * to access the user-provided i/o routines
	 */
	loutput(0, "_llic()");
	loutput(0, "{");
	loutput(1, "return (input());");
	loutput(0, "}");
	loutput(0, "_lloc(c)");
	loutput(0, "{");
	loutput(1, "output(c);");
	loutput(0, "}");
	/*
	 * parse rules section, each rule gets a case in yylex()
	 */
	loutput(0, "yylex()");
	loutput(0, "{");
	outlnum(1);
	while (ltype == LN_LSPC)
		lcopy();
	loutput(0, "yyloop:");
	loutput(1, "switch (_lltk()) {");
	loutput(1, "case 0:");
	loutput(2, "if (yywrap())");
	loutput(3, "return (EOF);");
	loutput(2, "break;");
	yyparse();
	loutput(1, "}");
	loutput(1, "goto yyloop;\n");
	loutput(0, "}");
	nfa[nxt++][0] = LX_STOP;
	/*
	 * don't need definitions anymore, copy nfa down to 0
	 */
	s = ctxstart->d_data;
	i = 0;
	while (s < nxt) {
		nfa[i][0] = nfa[s][0];
		nfa[i++][1] = nfa[s++][1];
	}
	nxt = i;
	freedef(defstart);
	outlnum(1);
	/*
	 * copy the rest of the input spec through
	 */
	while (ltype != LN_EOFL)
		lcopy();
	/*
	 * class tables
	 * context table
	 * nfa
	 * workspace
	 */
	btable();
	xtable();
	ptable();
	ltable();
	if (pflag) {
		printnfa();
	}
	if (vflag)
		stats();
	fclose(fileout);
}

/*
 * interpret input according to type of line
 */
yyparse()
{
	register s = nxt;
	register struct def *nd, *pd = NULL;
	register char *pc;

	for (;;) switch (ltype) {
	case LN_DFLT:
		if (indefs) {
			s = nxt;
			pc = getident();
			rexparse(0);
			nfa[nxt++][0] = LX_TERM;
			nd = alloc(sizeof(struct def));
			if (pd != NULL)
				pd->d_next = nd;
			else
				defstart = nd;
			pd = nd;
			pd->d_name = pc;
			pd->d_data = s;
			pd->d_next = NULL;
			if (yylval != '\n')
				dnl();
			else
				setltype();
		} else {
			if (look(0) == '<') {
				next();
				inscons();
				if (next() != '>')
					error("%s in start list", illchr);
			}
			if (look(0) == '^') {
				next();
				nfa[nxt++][0] = LX_BLIN;
			}
			rexparse(0);
			if (yylval == '/') {
				nfa[nxt++][0] = LX_LOOK;
				rexparse(0);
			}
			switch (yylval) {
			case '$':
				nfa[nxt++][0] = LX_ELIN;
				break;
			case ' ':
			case '\t':
				break;
			case '\n':
				error(noactn);
			default:
				error(rulsyn);
			}
			nfa[nxt][0] = LX_ACPT;
			nfa[nxt++][1] = ++actn;
			loutput(1, "case 0%o:", actn);
			outlnum(0);
			eatspc();
			if (look(0) == '\n')
				error(noactn);
			if (look(0) == '|')
				dnl();
			else {
				output("\t\t");
				getactn();
				loutput(2, "break;");
			}
			nfalink(s);
			s = nxt;
			while (ltype == LN_LSPC)
				lcopy();
		}
		break;
	case LN_LSPC:
		outlnum(1);
		while (lcopy(), ltype==LN_LSPC);
		break;
	case LN_CTXT:
		if (indefs)
			addcontext();
		else {
			nfa[nxt++][0] = LX_STOP;
			markcontext(nxt - ctxstart->d_data);
			s = nxt;
		}
		dnl();
		break;
	case LN_SCON:
		if (!indefs)
			error(illstc);
		addstart();
		dnl();
		break;
	case LN_LCOM:
		dnl();
		outlnum(1);
		while (ltype != LN_RCOM)
			lcopy();
		dnl();
		break;
	case LN_OPTN:
		dnl();
		break;
	case LN_DLIM:
		dnl();
		return;
	case LN_EOFL:
		if (indefs)
			error(eoferr);
		return;
	}
}

/*
 * parse regular expressions into nfa segments
 */
rexparse(p)
{
	register c, t, s;

	s = nxt;
	for (c=yylex(); c!=LX_TERM; c=nfaclose(t)) {
		t = nxt;
		if (c != LX_OPER) {
			nfa[nxt][0] = c;
			nfa[nxt++][1] = yylval;
		} else switch (c=yylval) {
		case '|':
			nfa[t=nxt++][0] = LX_JUMP;
			nfalink(s);
			c = rexparse(p);
			++t;
			nfa[t][1] = nxt - t;
			return (c);
		case '"':
			if (!inquotes)
				return (p);
		case '(':
			if (rexparse(c) == c)
				break;
			error(unmopr, c);
		case ')':
			if (p)
				return (p);
			error(unmopr, c);
		case '{':
			for (c=getdefn(); nfa[c][0]!=LX_TERM; ++c) {
				nfa[nxt][0] = nfa[c][0];
				nfa[nxt][1] = nfa[c][1];
				++nxt;
			}
			break;
		default:
			error(regsyn);
		}
	}
	return (c);
}

/*
 * look for and apply a closure operator to the
 * nfa segement starting at t, ending at nxt
 */
nfaclose(t)
register t;
{
	register c;
	int v0, v1;

	if ((c=yylex()) == LX_OPER) switch (yylval) {
	case '*':
	case '+':
		nfa[nxt][0] = LX_LINK;
		nfa[nxt][1] = t - nxt;
		++nxt;
	case '?':
		if (yylval != '+')
			nfalink(t);
		c = yylex();
		break;
	case '{':
		if (!isdigit(look(0)))
			break;
		v0 = v1 = 0;
		do {
			v0 *= 10;
			v0 += next()-'0';
		} while (isdigit(look(0)));
		if (look(0) == ',') {
			next();
			while (isdigit(look(0))) {
				v1 *= 10;
				v1 += next()-'0';
			}
			v1 -= v0;
		} else
			v1 = 0;
		if (next()!='}' || v0<0 || v1<0)
			error(reperr);
		if (v0) {
			while (--v0)
				t = nfacopy(t);
			if (v1)
				t = nfacopy(t);
		} else if (v1 == 0)
			while (--nxt > t)
				nfa[nxt][0] = nfa[nxt][1] = 0;
		if (v1) {
			nfalink(t);
			c = (nxt - t) * v1 + t;
			nfa[t][1] = c - t;
			while (--v1) {
				t = nfacopy(t);
				nfa[t][1] = c - t;
			}
		}
		c = yylex();
	}
	return (c);
}

/*
 * create an epsilon transition at nfa state s to nxt
 */
nfalink(s)
register s;
{
	register i, j;

	j = nxt++;
	while ((i=j--) > s) {
		nfa[i][0] = nfa[j][0];
		nfa[i][1] = nfa[j][1];
	}
	nfa[s][0] = LX_LINK;
	nfa[s][1] = nxt - s;
}

/*
 * replicate the nfa segment starting at t ending at nxt
 */
nfacopy(t)
register t;
{
	register s;

	for (s=nxt; t<s; ++t,++nxt) {
		nfa[nxt][0] = nfa[t][0];
		nfa[nxt][1] = nfa[t][1];
	}
	return (t);
}

/*
 * insert conditional epsilon transitions for
 * each applicable start condition
 */
inscons()
{
	register t;

	nfa[nxt][0] = LX_SCON;
	nfa[t=nxt++][1] = getstart();
	if (look(0) == ',') {
		next();
		nfa[nxt++][0] = LX_JUMP;
		nfalink(t++);
		inscons();
		++t;
		nfa[t][1] = nxt - t;
	}
}
