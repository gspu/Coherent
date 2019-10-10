/*
 * sh/eval.c
 * Bourne shell.
 * Evaluation of parameter substitution, command substitution,
 * blank interpretation, and file name generation.
 */

#include "sh.h"

char	*arcp;		/* character position in argument */
int	argf = 1;	/* First character of argument flag */
int	argg = 0;	/* Glob seek, escape quoted glob chars */
int	argq = 0;	/* Quotation flag, no blanks or glob */

/*
 * Evaluate a string.
 */
eval(cp, f)
char *cp;
{
	register int m, c;

	strp = strt;
	arcp = cp;
	argf = 1;
	if (f==EHERE) {
		m = MHERE;
		argq = 2;
	} else if (f==EWORD) {
		m = MNQUO;
		argq = 2;
	} else {
		m = MNQUO;
		argq = 0;
	}
	argg = (f==EARGS || f==EPATT);

	while ((c = *arcp++) != '\0') {
		if (!class(c, m)) {
			add_char(c);
			continue;
		}
		switch (c) {
		case '"':	/* m == MNQUO || m == MDQUO */
			m = ((argq ^= 1) & 1) ? MDQUO : MNQUO;
			/*
			 * POSIX.2 says: If there are no positional
			 * parameters, the expansion of $@ shall generate
			 * zero fields, even when double-quoted.
			 */
#if 1
			if (m == MDQUO && strcmp (arcp, "\"") == 0)
				argf = 0;
#else
			if (m==MDQUO && strcmp(arcp, "$@\"")!=0)
				argf = 0;
#endif
			continue;
		case '\'':	/* m == MNQUO */
			while ((c = *arcp++) != '\'')
				add_quoted(c);
			argf = 0;
			continue;
		case '\\':	/* m == MDQUO || m == MNQUO */
			c = *arcp++;
			if (m != MNQUO && ! class(c, m)) {
				add_char('\\');
				add_char(c);
			} else
				add_quoted(c);
			argf = 0;
			continue;
		case '$':	/* m == MNQUO || m = MDQUO */
			variable();
			continue;
		case '`':	/* m == MNQUO || m = MDQUO */
			graves (m);
			continue;
		default:
			add_char(c);
			continue;
		}
	}
	if (f==EARGS)
		end_arg();
	else
		*strp++ = '\0';
}

/*
 * Read the name of a shell variable and perform the appropriate substitution.
 * Doesn't check for end of buffer.
 */
variable()
{
	VAR *vp;
	int s;
	char *wp, *sav;
	register int c, count, quote;
	register char *cp, *pp;

	cp = strp;
	s = '\0';
	c = *arcp++;
	if (class(c, MSVAR)) {
		special(c);
		return;
	} else if (class(c, MRVAR)) {
		while (class(c, MRVAR)) {
			*cp++ = c;
			c = *arcp++;
		}
		--arcp;
	} else if (c != '{') {
		/* Not a legal variable name, put it back. */
		add_char('$');
		add_char(c);
		return;
	} else {
		/* c == '{' */
		if (index("#?$!-@*0123456789", arcp[0]) != NULL && arcp[1] == '}') {
			/* Allow specials of the form "${?}" etc. */
			special(arcp[0]);
			arcp += 2;
			return;
		}
		while (index("}-=?+", c = *arcp++) == NULL)
			*cp++ = c;
		if (c != '}') {
			/* ${VAR [-=?+] token} */
			s = c;
			if (cp[-1] == ':')
				--cp;		/* allow e.g. ${VAR:=foo} */
			*cp++ = '=';
			wp = cp;
			if ((quote = *arcp) == '"' || quote =='\'')
				++arcp;
			else
				quote = 0;
			for (count = 1; ; ) {
				c = *arcp++;
				if (c == '}' && count-- == 1)
					break;
				else if (c == '$' && quote != '\'') {
/*
 * steve 6/24/92
 * This truly sleazy hack handles e.g. "${V1-$V2}", oy.
 * It doesn't do it very well, paying no attention to quotes (for example).
 * The recursive call to variable() should be straightforward but is not,
 * the hacky way this module uses globals like strp requires the save/restore.
 */
					sav = strp;
					strp = cp;
					variable();
					cp = strp;
					strp = sav;
					continue;
				} else if (c == '{')
					++count;
				else if (quote != 0 && c == quote) {
					quote = 0;
					continue;
				}
				*cp++ = c;
			}
		}
	}
	*cp++ = '\0';
	if (class((c = *strp), MDIGI)) {
		if ((c -= '1') >= sargc)
			pp = NULL;
		else
			pp = sargp[c];
	} else if (namevar(strp) == 0) {
		eillvar(strp);
		return;
	} else {
		pp = NULL;
		if ((vp=findvar(strp)) != NULL) {
			pp = convvar(vp);
			if (*pp == '\0')
				pp = NULL;	/* regard value "" as not set */
		}
	}
	switch (s) {
	case '\0':
		if (uflag!=0 && pp==NULL)
			enotdef(strp);
		break;
	case '-':
		if (pp == NULL)
			pp = wp;
		break;
	case '=':
		if (pp == NULL) {
			pp = wp;
			if (class(*strp, MDIGI)) {
				printe("Illegal substitution");
				return;
			}
			setsvar(strp);
		}
		break;
	case '?':
		if (pp != NULL)
			break;
		if (*wp != '\0')
			prints("%s\n", wp);
		else {
			*--wp = '\0';
			enotdef(strp);
		}
		reset(RUABORT);
		NOTREACHED;
	case '+':
		if (pp != NULL)
			pp = wp;
		break;
	}
	if (pp != NULL)
		while ((c = *pp++) != '\0')
			add_char(c);
	argf &= ~ argq;
}

/*
 * Return the value of the special shell variables.
 * No check for end of buffer.
 */
special(n)
register int n;
{
	register char *sp;
	register int flag;

	sp = strp;
	switch (n) {
	case '#':
		n = sargc;
		goto maked;
	case '?':
		n = slret;
		goto maked;
	case '$':
		n = shpid;
		goto maked;
	case '!':
		n = sback;
		goto maked;
	maked:
		sprintf(sp, "%d", n);
		break;
	case '-':
		for (sp = &eflag; sp <= &xflag; sp += 1)
			if (*sp)
				add_char(*sp);
		argf &= ~ argq;
		return;
	case '@':
	case '*':
		flag = (argq == 1 && n == '@');
		for (n = 0; n < sargc; n++) {
			if (n) {
				argq ^= flag;
				add_char(' ');
				argq ^= flag;
			}
			sp = sargp[n];
			while (*sp)
				add_char(*sp++);
			/*
			 * Make sure that arguments like "" get handled
			 * properly when expanding "$@"
			 */
			if (flag)
				argf &= ~ argq;
		}
		return;
	case '0':
		sp = sarg0;
		break;
	default:
		if ((n-='1') >= sargc) {
			if (uflag)
				printe("Unset parameter: %c", n+'1');
			argf &= ~ argq;
			return;
		}
		sp = sargp[n];
		break;
	}
	while (*sp)
		add_char(*sp++);
	argf &= ~ argq;
}

/*
 * Read and evaluate a command found between graves.
 *
 * NB : The code below relating to saving/restoring the global variable
 * "argf" has been removed; it was not clear what the intended effect was,
 * but it caused expansions into single words (eg, "echo `pwd`") to vanish
 * and caused the word-break algorithm to perform in an unintuitive (and
 * incorrect according to POSIX.2) manner.
 *
 * NB : Backslash-quoting inside graves was not supported properly before,
 * and I'm not sure that I've got it right. The idea is that before passing
 * things off to session (), we process the appropriate backslash-escapes.
 * What consists of an "appropriate" escape depends on whether the graves
 * appeared within a double-quoted section (in which case we recognise the
 * specials appropriate to that) or not (in which case we recognised the
 * characters $`\ as specials. We also process '$'-expansions in graves now,
 * but not globs.
 *
 * Since we are removing some bashslashes here, we should be building into
 * a temporary buffer. We append our work onto the global "strp" buffer and
 * cut it back once we have finished.
 */
graves (quotemode)
int		quotemode;
{
	int pipev[2], f, oslret;
	int oargf;
	char	      *	ostrp;
	register FILE *fp;
	register int c;
	register int nnl;
	char *cmdp;

	oargf = argf;
	ostrp = strp;
	oslret = slret;

	cmdp = strp;
	while ((c = *arcp++) != '`') {
		if (c != '\\') {
			add_arg (c);
			continue;
		}
		c = * arcp ++;

		if (! (quotemode == MDQUO && class (c, MDQUO)) &&
		    (c != '$' && c != '\\' && c != '`'))
			add_arg ('\\');
		add_arg (c);
	}
	* strp = 0;

	if ((f = pipeline(pipev)) == 0) {
		slret = oslret;		/* in case grave command uses $? */
		dup2(pipev[1], 1);
		close(pipev[0]);
		close(pipev[1]);
		exit(session(SARGS, cmdp));
		NOTREACHED;
	}

	close(pipev[1]);
	if ((fp=fdopen(pipev[0], "r")) == NULL) {
		close(pipev[0]);
		ecantfdop();
		return;
	}
	strp = ostrp;
	argf = oargf;
	nnl = 0;
	while ((c=getc(fp)) != EOF) {
		if ( ! recover(IEVAL)) {
#ifdef VERBOSE
			if (xflag) prints("Interrupt in eval\n");
#endif
			errflag++;
			break;
		}
		if (c=='\n')
			++nnl;
		else {
			while (nnl) {
				nnl--;
				add_char('\n');
			}
			add_char(c);
		}
	}

	/*
	 * If we expanded to something, we have an arg. If we are in double-
	 * quotes, we have an arg. Otherwise, we have an arg if we had an arg
	 * before.
	 */

	if (argf)
		argf = argq ? 0 : oargf;

	fclose(fp);
	waitc(f);
}

/*
 * Add a character to the current argument.
 * If no quotation is set, pick off blanks and globs.
 */
add_char(c)
register int c;
{
	if (argq==0) {
		if (index(vifs, c) != NULL) {
			end_arg();
			return;
		}
		if (argg && class(c, MGLOB)) {
			add_arg(c);
			return;
		}
	}
	add_quoted(c);
}

/*
 * Add a quoted character to the current argument.
 * if argg is set, then glob characters are quoted with a \,
 * as well as \ itself.
 */
add_quoted(c) register int c;
{
	if (argg && (class(c, MGLOB) || c == '\\'))
		add_arg('\\');
	add_arg(c);
}

/*
 * Add a character to the current argument
 * and check for end of buffer.
 */
add_arg(c) register int c;
{
	if (strp >= &strt[STRSIZE])	/* Should do more */
		etoolong();
	else
		*strp++ = c;
	argf = 0;
}

/*
 * Terminate the current argument if it is non-empty.
 * If argg is set, then glob the argument to expand globs
 * or to simply remove any quotes.
 */
end_arg()
{
	if (argf != 0)
		return;
	*strp++ = '\0';
	if (argg)
		glob1(duplstr(strt, 0));
	else {
		nargv = addargl(nargv, duplstr(strt, 0));
		nargc += 1;
	}
	strp = strt;
	argf = 1;
	return;
}

/*
 * Evaluate a here document.
 * Unevaluated document is on u2, put the evaluated document there, too.
 */
evalhere(u2)
{
	register int u1;
	register FILE *f2;
	char buf[128];
	char *tmp;

	tmp = shtmp();
	if ((u1=creat(tmp, 0666))<0) {
		ecantmake(tmp);
		return -1;
	}
	if ((f2=fdopen(u2, "r"))==NULL) {
		ecantfdop();
		close(u1);
		close(u2);
		return -1;
	}
	while (fgets(buf, 128, f2) != NULL) {
		eval(buf, EHERE);
		write(u1, strt, strp-1-strt);
	}
	close(u1);
	fclose(f2);
	if ((u2 = open(tmp, 0))<0) {
		ecantopen(tmp);
		u2 = -1;
	}
	unlink(tmp);
	return u2;
}

/* end of sh/eval.c */
