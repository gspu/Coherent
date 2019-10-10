/*
 * C preprocessor.
 * This version of cpp runs as a module of cc0.
 */
#ifdef   vax
#include "INC$LIB:cc0.h"
#else
#include "cc0.h"
#endif

/*
 * Define a macro.
 */
define()
{
	register int c, narg;
	register CPPSYM *sp;
	register TOK *tp;
	TOK *targ[NARGS];

	c = getnb();
	if (c < 0 || ct[c] != ID)
		goto bad;
	getid(c);
	tp = idp;
	if ((sp = tp->t_sym) != NULL && sp->s_slevel == SL_CPP)
		;
	else
		sp = NULL;
	/*
 	 * Gather up the arguments.
	 */
	narg = -1;
	if ((c = get()) == '(') {
		++narg;
		do {
			if ((c = getnb()) == ')')
				break;
			if (c<0 || ct[c]!=ID)
				goto bad;
			getid(c);
			if (narg < NARGS)
				targ[narg] = idp;
			++narg;
			c = getnb();
		} while (c == ',');
		if (c != ')')
			goto bad;
		c = getnb();
	} else if (c == ' ' || c == '\t')
		c = getnb();
	/*
	 * Gather macro body.
	 */
	unget(c);
	while ((c = get()) >= 0) {
		switch (ct[c]) {
		case QUOTE:
		case STRING:
			dputstr(c);
			continue;
		case SKIP:
			unget(c = getnb());
			dputc(' ');
			continue;
		case SHARP:
			if (nextis(c)) {
				if (dputp > dbuf && dputp[-1] == ' ')
					--dputp;
				if (dputp == dbuf) {
					cerror("## at beginning of macro");
					continue;
				}
				c = getnb();
				if (c < 0) {
					cerror("## at end of macro");
					continue;
				}
				unget(c);
				dputc('#');
				dputc('#');
				continue;
			}
			c = getnb();
			if (ct[c] != ID) {
				unget(c);
				c = narg;
			} else {
				getid(c);
				for (c = 0; c < narg; c += 1)
					if (idp == targ[c])
						break;
			}
			if (c >= narg)
				cerror("parameter must follow #");
			else {
				dputc('#');
				dputc(ARG0+c);
			}
			continue;
		case CON:
		case DOT:
			dputnum(c);
			continue;
		case ID:
			getid(c);
			for (c = 0; c < narg; c += 1)
				if (idp == targ[c])
					break;
			if (c >= narg)
				dputs(id);
			else
				dputc(ARG0+c);
			continue;
		default:
			dputc(c);
			continue;
		}
	}
	if (dputp > dbuf && dputp[-1] == ' ')
		--dputp;
	dputc(0);
	if (sp != NULL) {
		if (sp->s_value > XUSERA
		 || strcmp(dbuf, sp->s_body) != 0
		 || sp->s_narg != narg)
			cerror("\"%s\" redefined", tp->t_id);
#if ANSI_STUPID
		else
			for (c = 0; c < narg; c += 1)
				if (targ[c] != sp->s_targ[c])
					cerror("\"%s\" redefined", tp->t_id);
#endif
	} else {
		if (narg > NARGS) {
			cerror("\"%s\" has too many arguments", tp->t_id);
			narg = NARGS;
		}
		sp = newcpp(narg, dbuf, dputp-dbuf);
		sp->s_sp = tp->t_sym;
		tp->t_sym = sp;
#if ANSI_STUPID
		if (narg > 0) {
			sp->s_targ = new(narg * sizeof(TOK *));
			for (c = 0; c < sp->s_narg; c += 1)
				sp->s_targ[c] = targ[c];
		}
#endif
	}
	dputp = dbuf;
	return;

 bad:
	cerror("error in #define syntax");
	while (c >= 0)
		c = get();
	dputp = dbuf;
}

/*
 * Read a file name for #include or #line
 * into dbuf.
 * Return quote character on success,
 * 0 for any kind of error.
 */
getfname()
{
	register int	c, d;
	register char *spshp, *p;

	d = 0;
	spshp = dpshp;
	while ((c = getnb()) >= 0 && ct[c] == ID)
		if ( ! expand(c))
			goto done;
	if (c == '<')
		d = '>';
	else if (c == '"')
		d = c;
	else
		goto done;
	instring = d;
	while ((c=get()) >= 0 && c != d)
		dpshc(c);
	instring = 0;
	if (c != d)
		d = 0;
 done:	while (c >= 0)
		c = get();
	for (p = spshp; --p >= dpshp; )
		dputc(*p);
	dputc(0);
	dpshp = spshp;
	return d;
}

/*
 * File inclusion.
 * incdirs[0] is always "".
 */
include()
{
	register int	c, d;
	char *fbuf;
	FILE *fp;

	if (istackp == istack)
		cfatal("include stack overflow");
	if ((d = getfname()) == 0) {
		dputp = dbuf;
		cerror("error in #include syntax");
		return;
	}
	fp = NULL;
	fbuf = dputp;
	dputp = dbuf;
	for (c = (d != '"'); c < ndirs; c += 1) {
		strcpy(fbuf, incdirs[c]);
#ifdef COHERENT
		if (fbuf[0] != '\0')
			strcat(fbuf, "/");
#endif
#if GEMDOS || MSDOS
		if (fbuf[0] != 0 && fbuf[strlen(fbuf)-1] != '\\')
			strcat(fbuf, "\\");
#endif
		strcat(fbuf, dbuf);
		if ((fp=fopen(fbuf, "r")) != NULL)
			break;
	}
	if (fp == NULL)
		return cfatal("cannot open include file \"%s\"", dbuf);
	--istackp;
	istackp->i_fp = ifp;
	istackp->i_file = tfile;
	istackp->i_line = line;
	istackp->i_cstackp = cstackp;
	ifp = fp;
	line = 1;
	notskip = 0;
	setid(fbuf);
	setfname();
	emptyfilep();
	return;
}

/*
 * Undefine the name in 'id'.
 */
undefine()
{
	register CPPSYM *sp;

	if ((sp = idp->t_sym) != NULL
	 && sp->s_slevel == SL_CPP
	 && sp->s_value <= XUSERA) {
		idp->t_sym = sp->s_sp;
#if ANSI_STUPID
		if (sp->s_narg > 0)
			free(sp->s_targ);
#endif
		free(sp);
	}
}

/*
 * Read a constant expression in cpp.
 * As it turns out, this will accept
 * sizeof() and casts if the terms are
 * defined at the time of the reference.
 */
cppexpr()
{
	TREE *stp;
	register TREE *tp;
	register int con;

	++incpp;
	stp = talloc();
	lex();
	tp = expr();
	if (tp->t_op == ICON)
		con = tp->t_ival == 0;
	else if (tp->t_op == LCON)
		con = tp->t_lval == 0;
	else if (tp->t_op == ZCON)
		con = tp->t_zval == 0;
	else
		cerror("constant expression required");
	treset(stp);
	--incpp;
	return con;
}

/*
 * Entry to process a control line
 * after # has been read.
 * Called from within get().
 */
control()
{
    register int c;
    register SYM *sp;
    register int true;
    register char *p;
    TOK *tp;

    unget(c = getnb());
    if (c < 0)
	return;
    getid('#');
    tp = idp;
    if ((sp = idp->t_sym) == NULL || sp->s_slevel != SL_CPP)
	goto bad;
    if (cstate == 0 || sp->s_value >= XIF)
	switch (sp->s_value) {

	case XDEFINE:
		define();
		break;

	case XINCLUDE:
		include();
		return;

	case XASSERT:
		if (cppexpr())
			cerror("#assert failure");
		break;

	case XERROR:
		p = dputp;
		while ((c = get()) > 0)
			dputc(c);
		dputc(0);
		cfatal("#error: %s", p);
		break;

	case XPRAGMA:
		break;

	case XIFDEF:
	case XIFNDEF:
		c = getnb();
		if (ct[c] != ID)
			goto bad;
		getid(c);
		if (sp->s_value == XIFDEF)
		true = ! ((sp = idp->t_sym) != NULL && sp->s_slevel == SL_CPP);
		else
		true = ((sp = idp->t_sym) != NULL && sp->s_slevel == SL_CPP);
		goto new_if;

	case XIF:
		/* 0 indicates a true section */
		/* 1 indicates a false section */
		/* 2 indicates a section which was true already */
		true = cstate!=0 ? 1 : cppexpr();
	new_if:
		if (cstackp == cstack)
			cfatal("conditional stack overflow");
		--cstackp;
		cstackp->c_op = XIF;
		cstackp->c_state = cstate;
		cstate = cstate!=0 ? 2 : true;
		break;

	case XELSE:
		if (cstackp == cstack+NLEV)
			cerror("#else used without #if or #ifdef");
		else if (cstackp->c_op == XELSE)
			cerror("multiple #else's");
		else {
			cstackp->c_op = XELSE;
			cstate = cstate==1 ? 0 : 2;
		}
		break;

	case XELIF:
		if (cstackp == cstack+NLEV)
			cerror("#elif used without #if or #ifdef");
		else if (cstackp->c_op == XELSE)
			cerror("#elif used after #else");
		else {
			cstackp->c_op = XELIF;
			cstate = cstate==1 ? cppexpr() : 2;
		}
		break;

	case XENDIF:
		if (cstackp == cstack+NLEV)
			cerror("#endif used without #if or #ifdef");
		else {
			cstate = cstackp->c_state;
			++cstackp;
		}
		break;

	case XUNDEF:
		if ((c = getnb()) < 0 || ct[c] != ID)
			goto bad;
		getid(c);
		undefine();
		break;

	case XLINE:
		while ((c = getnb()) >= 0 && ct[c] == ID)
			if ( ! expand(c))
				goto bad;
		if (c < 0 || ct[c] != CON)
			goto bad;
		getnum(c, 0);
		line = ival;
		c = getfname();
		dputp = dbuf;
		if (c != 0) {
			setid(dbuf);
			setfname();
		}
		break;

	default:
	bad:	cerror("illegal control line");
	}
    while ((c = get()) >= 0)	/* incpp, new line follows EOF */
	;
}

