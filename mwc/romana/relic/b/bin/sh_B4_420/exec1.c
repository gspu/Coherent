/*
 * sh/exec1.c
 * The Bourne shell.
 * Shell part of execution.
 */

#include "sh.h"

char *lastcmd = "";

char *skipredir();

/*
 * Execute the given node, wait for completion, return status.
 */
command(np)
register NODE *np;
{
	register int f;		/* Pid of command */
	int mynllflag;		/* Saved fork not forced flag */
	CON con;		/* Control link for break/continue */
	char *innp, **inlp;	/* for NFOR */
	NODE *cnode;		/* for NCASE */
	char *cname;		/* for NCASE */

	if (ret_done)
		return;
	mynllflag = nllflag;
	innp = inlp = NULL;
	cnode = NULL;
	f = 0;

	con.c_next = sesp->s_con;
	con.c_node = np;
	con.c_bpp = savebuf();
	sesp->s_con = &con;

	switch (setjmp(con.c_envl)) {
	case 0:		/* initial setjmp */
		break;
	case 1:		/* continue */
		np = con.c_node;
		break;
	case 2:		/* break */
		goto break2;
	default:
		panic(1);
		NOTREACHED;
	}

	for ( ; np != NULL; np=np->n_next) {
	if (ret_done)
		break;		/* "return" interrupts control flow */
	recover(ICMD);
	f = 0;
	nllflag = mynllflag;

	switch (np->n_type) {
	case NCOMS:
		f = comscom(np->n_auxp);
		break;
	case NFOR:
		if (innp == NULL) {
			innp = np->n_strp;
			nargc = 0;
			nargv = makargl();
			for (np = np->n_next->n_auxp; np; np = np->n_next)
				eval(np->n_strp, EARGS);
			inlp = nargv;
			np = con.c_node;
		}
		continue;
	case NFOR2:
		/* do_done_list->n_next == this node */
		if (*inlp == NULL || assnvar(innp, *inlp++) == NULL)
			break;
		continue;
	case NWHILE:
		/* do_done_list->n_next == this node */
		nllflag = 0;
		if (command(np->n_auxp))
			break;
		continue;
	case NUNTIL:
		/* do_done_list->n_next == this node */
		nllflag = 0;
		if ( ! command(np->n_auxp))
			break;
		continue;
	case NBRAC:
		command(np->n_auxp);
		break;
	case NPARN:
		if (nllflag || (f=clone()) == 0) {
			exit(command(np->n_auxp));
			NOTREACHED;
		}
		break;
	case NIF:
		nllflag = 0;
		if (!command(np->n_auxp->n_auxp))
			np = np->n_auxp;
		else if (np->n_next == NULL)
			slret = 0;		/* exit status 0 if no elsepart */
		continue;
	case NELSE:
		command(np->n_auxp);
		break;
	case NCASE:
		eval(np->n_strp, EWORD);
		if (errflag)
			break;
		cname = duplstr(strt, 0);
		continue;
	case NCASE2:
		cnode = np->n_auxp;
		continue;
	case NCASE3:
		/* end of pattern list == next NCASE2 node */
		eval(np->n_strp, EPATT);
		if (errflag)
			break;
		if (match(strt, cname)) {
			command(cnode);
			break;
		}
		continue;
	case NLIST:
		nllflag = 0;
		command(np->n_auxp);
		continue;
	case NANDF:
		nllflag = 0;
		if (command(np->n_auxp))
			break;
		continue;
	case NORF:
		nllflag = 0;
		if ( ! command(np->n_auxp))
			break;
		continue;
	case NBACK:
		if ((f=clone()) == 0) {
			static char *iov[] = { "0</dev/null", NULL };

			bckflag++;
			dflttrp(IBACK);
			redirect(iov, NULL);
			exit(command(np->n_auxp));
			NOTREACHED;
		}
		sback = f;
		prints("%d\n", f);
		f = 0;
		continue;
	case NPIPE:
		f = pipecoms(np);
		break;
	case NFUNC:
		def_shell_fn(np);
		break;
	case NRET:
		if (in_sh_fn == 0) {
			printe("return not inside shell function");
			break;
		}
		ret_done++;		/* to interrupt control flow */
		if (np->n_strp[0] != '\0') {
			eval(np->n_strp, EARGS);
			slret = atoi(strt);
		}
		return slret;
	default:
		panic(2);
		NOTREACHED;
	}
	break;

	}
break2:
	nllflag = mynllflag;
	if (sesp->s_con != NULL)
		sesp->s_con = sesp->s_con->c_next;
	if (f)
		waitc(f);
	if (slret)
		assnvar("LASTERROR", lastcmd);
	freebuf(con.c_bpp);
	if (eflag && (slret || errflag)) {
		reset(RUABORT);
		NOTREACHED;
	}
	return slret;
}

/*
 * Run a simple command.
 *	Anything but.
 */
#define	FARGS	1
#define	FIORS	2
#define	FASSG	4

comscom(np)
register NODE *np;
{
	register int f;
	register char **app, *s, *s1, *sp;
	int nputs, nargs;

	nargc = 1;
	nargv = makargl();
	nargv = addargl(nargv, "sh");
	niovp = makargl();
	nenvp = makargl();
	nctlp = NULL;
	/*
	 * Scan for arguments.
	 * or a control node.
	 */
	nputs = 0;
	f = 0;
	for ( ; np; np = np->n_next) {
		switch (np->n_type) {
		case NIORS:
			f |= FIORS;
#if	0
			/* Old code. */
			eval(np->n_strp, EWORD);
			niovp = addargl(niovp, duplstr(strt, 0));
			if (xflag)
				nputs += puta(nputs, strt);
#else
			/*
			 * New code by steve 1/24/91.
			 * This allows globs in redirection args.
			 */
			s = skipredir(np->n_strp);
			nargs = nargc;
			eval(s, EARGS);			/* expand as arg */
			for (s1 = np->n_strp, sp = strt; s1 < s; )
				*sp++ = *s1++;
			strcpy(sp, nargv[nargs]);	/* build redir arg */
			niovp = addargl(niovp, duplstr(strt, 0));
			if (xflag)
				nputs += puta(nputs, strt);
			--nargc;
			for (app = nargv+nargs; *app; app++) {
				*app = *(app + 1);	/* shift remaining args */
				if (xflag && *app != NULL)
					nputs += puta(nputs, *app);
			}
#endif
			continue;
		case NARGS:
			f |= FARGS;
			nargs = nargc;
			eval(np->n_strp, EARGS);
			if (xflag)
				for (app = nargv+nargs; *app; )
					nputs += puta(nputs, *app++);
			continue;
		case NASSG:
			if (kflag || (f&FARGS)==0) {
				f |= FASSG;
				eval(np->n_strp, EWORD);
				nenvp = addargl(nenvp, duplstr(strt, 0));
				if (xflag)
					nputs += puta(nputs, strt);
			} else {
				nargs = nargc;
				eval(np->n_strp, EARGS);
				if (xflag)
					for (app = nargv+nargs; *app; )
						nputs += puta(nputs, *app++);
			}
			continue;
		case NCTRL:
			if (nctlp!=NULL) {
				panic(3);
				NOTREACHED;
			}
			f |= FARGS;
			nctlp = np;
			continue;
		default:
			panic(4);
			NOTREACHED;
		}
	}
	if (xflag && nputs)
		prints("\n");
#ifdef VERBOSE
	if (xflag) {
		prints("\t<%o flag, %d put, %d arg, %o arv, %o env, %o iov>\n",
			f, nputs, nargc-1, *(nargv+1), *nenvp, *niovp);
	}
#endif

	nargv += 1;	/* Skip over "sh" */
	nargc -= 1;

	/* Last chance to quit */
#ifdef VERBOSE
	if (xflag) prints("errflag = %o\n", errflag);
#endif
	if (errflag || nflag)
		return (0);

	/* And away we go */
	sfree(lastcmd);
	if (*nargv)
		lastcmd = duplstr(*nargv, 1);
	else
		lastcmd = "";
	switch (f) {
	case	0:
		return (0);
	case	FARGS:
		if (nctlp) {
			command(nctlp->n_auxp);
			return (0);
		}
	case	FARGS|	FIORS:
	case	FARGS|		FASSG:
	case	FARGS|	FIORS|	FASSG:
		if (sh_fn() || inline())
			return (0);
		break;
	case		FIORS:
		break;
	case			FASSG:
	case		FIORS|	FASSG:
		for (app = nenvp; *app!=NULL; )
			setsvar(*app++);
		if ((f&~FASSG)==0) {
			slret = 0;
			return (0);
		}
		break;
	default:
		panic(5);
		NOTREACHED;
	}
	if (nllflag || (f=clone()) == 0) {
		if (redirect (niovp, NULL) < 0) {
			slret = 1;
		} else if (nargc) {
			dflttrp(ICMD);
			nenvp = envlvar(nenvp);
			flexec();
			slret = 1;
		} else if (nctlp) {
			command(nctlp->n_auxp);
		}
		exit(slret);
		NOTREACHED;
	}
	return (f);
}

puta(n, p)
char *p;
{
	if (n)
		prints(" ");
#ifdef VERBOSE
	else
		prints("<%d> ", getpid());
#endif
	prints("%s", p);
	return (1);
}

/*
 * Execute a pipe command.
 * Fork, if necessary, a subshell to execute the pipe.
 * Fork each segment off.
 * Wait for last, save slret, wait for all, return slret from last.
 */
pipecoms(np)
register NODE *np;
{
	register int f;
	register int p1st = 0;
	int pipev[2];

	if ( ! nllflag && (f = clone()) != 0)
		return (f);
	while (np->n_type == NPIPE) {
		if (f = pipeline(pipev)) {
			/* Parent takes right hand side */
			np = np->n_next;
			if (p1st == 0)
				p1st = f;
			dup2(pipev[0], 0);
			if (pipev[0] != 0)
				close(pipev[0]);
			close(pipev[1]);
		} else {
			/* Child takes left hand side */
			np = np->n_auxp;
			dup2(pipev[1], 1);
			close(pipev[0]);
			if (pipev[1] != 1)
				close(pipev[1]);
			exit(command(np));
			NOTREACHED;
		}
	}
	if (f = clone()) {
		/* Parent waits out pipe line */
		spipe = p1st;
		close(0);
		if (f = waitc(f))
			waitc(p1st);
		exit(f);
		NOTREACHED;
	} else {
		/* Child takes the pipe tail */
		exit(command(np));
		NOTREACHED;
	}
}

/*
 * Skip a redirection arg, return pointer to following nonspace.
 */
char *
skipredir(s) register char *s;
{
	if (*s >= '1' && *s <= '9')
		++s;
	if (*s == '>' || *s == '<')
		++s;
	if (*s == '>' || *s == '<')
		++s;
	if (*s == '&')
		++s;
	while (*s == ' ' || *s == '\t')
		++s;
	return s;
}

/* end of sh/exec1.c */
