/*
 * AWK
 * Part 3 - execution of the compiled code.
 */

#include "awk.h"
#include "y.tab.h"

static	CHAR	*sb;		/* String beginning -- evalre */
static	CHAR	toodeep[] = "For/while nesting too deep";
CHAR	*reexec();

/*
 * Called for each input file
 * Also, called before the first file
 * and after the last file.
 * This routine executes the code for
 * each input line.
 */
awk(np, fp, fn)
register NODE *np;
register FILE *fp;
CHAR *fn;
{
	CHAR ibuf[MAXRECORD];

	sassign(FILENAMEp, fn);
	if (fp == NULL) {
		lineno = 0;
		execute(np, NULL);
	} else {
		if (fp != stdin)
			setbuf(fp, inbuf);
		lineno = 1;
		setjmp(nextenv);
		while (awkinput(ibuf, MAXRECORD, fp) != NULL) {
			iassign(NRp, NRp->t_INT+1);
			execute(np, ibuf);
		}
		if (fp != stdin)
			fclose(fp);
	}
}

/*
 * Read a record in for
 * awk.
 */
STRING
awkinput(as, lim, fp)
STRING as;
unsigned lim;
register FILE *fp;
{
	register int c;
	register CHAR *s;
	register int rs;
	register STRING ret;
	register int nf = 0;
	register int spcflag = 0;

	if ((rs = RS[0]) == '\0') {
		rs = '\n';
		spcflag++;
	}
	s = as;
	while (--lim>0 && (c = getc(fp))!=EOF) {
		if (c == '\n')
			lineno++;
		if (c == rs)
			if (!spcflag || s==as || s[-1]=='\n')
				break;
		*s++ = c;
	}
	*s = '\0';
	ret = c==EOF && s==as ? NULL : as;
	s = as;
	if (whitesw) {
		for (;;) {
			while (FSMAP[c = *s])	/* pass spaces */
				s++;
			if (c == '\0')		/* if end of line no field */
				break;
			nf++;
			while ((c = *s) != '\0' && !FSMAP[c]) /* pass field */
				s++;
			if (c == '\0')
				break;
		}
	}
	else if (*s) {
		for (;;) {
			nf++;
			while ((c = *s) != '\0' && !FSMAP[c])
				s++;
			if (c == '\0')
				break;
			if (FSMAP[c = *s])	/* pass delimeter */
				s++;
			if (c == '\0')
				break;
		}
	}
	iassign(NFp, (INT)nf);
	return (ret);
}

/*
 * This is the root of the execution tree
 * that handles patterns and actions
 * and lists of the above.
 * `s' is the input string.
 * Free all outstanding nodes accumulated
 * during execution at the end of this
 * routine (each input line).
 */
execute(np, s)
register NODE *np;
register STRING s;
{
	register NODE *xp;

	inline = s;
	while (np != NULL) {
		if (np->n_op == ALIST) {
			xp = np->n_O1;
			np = np->n_O2;
		} else {
			xp = np;
			np = NULL;
		}
		if (xp->n_op != AROOT)
			awkerr("Bad op in execute %d", xp->n_op);
		if (xp->n_O1 != NULL) {
			if (evalpat(xp->n_O1))
				if (xp->n_O2 == NULL) {
					if (!beginflag && !endflag)
						xprint(&xfield0, NULL);
				} else
					evalact(xp->n_O2);
		} else if (!beginflag && !endflag)
			evalact(xp->n_O2);
	}
	while ((np = tempnodes) != NULL) {
		tempnodes = np->t_next;
		freenode(np);
	}
	if (inline != s)
		free(inline);
}

/*
 * Evaluate the pattern half
 * of the expression.
 */
evalpat(np)
register NODE *np;
{
	register int ret = 0;

	if ((beginflag && np->n_op==ABEGIN) || (endflag && np->n_op==AEND))
		return (1);
	if (beginflag || endflag)
		return (0);
	switch (np->n_op) {
	case ABEGIN:
	case AEND:
		return (0);

	case ARANGE:
		if (np->n_flag==0 && evalpat(np->n_O1)) {
			np->n_flag++;
			ret = 1;
		} else if (np->n_flag == 1) {
			if (evalpat(np->n_O2))
				np->n_flag = 0;
			ret = 1;
		}
		break;

	default:
		ret = evalint(np);
		break;
	}
	return (ret);
}

/*
 * Evaluate the action part of the
 * statement.
 */
evalact(anp)
register NODE *anp;
{
	register NODE *np;
	register int i;

again:
	if (anp == NULL)
		return;
	if (anp->n_op == ALIST) {
		np = anp->n_O1;
		anp = anp->n_O2;
	} else {
		np = anp;
		anp = NULL;
	}
	switch (np->n_op) {
	case AIF:
		if (evalint(np->n_O1) != 0)
			evalact(np->n_O2);
		else if (np->n_O3 != NULL)
			evalact(np->n_O3);
		break;

	case AWHILE:
		if (++fwlevel >= NNEST)
			awkerr(toodeep);
		while (evalint(np->n_O1) != 0) {
			if ((i = setjmp(fwenv[fwlevel])) == ABREAK)
				break;
			else if (i == ACONTIN)
				continue;
			evalact(np->n_O2);
		}
		fwlevel--;
		break;

	case AFOR:
		if (++fwlevel >= NNEST)
			awkerr(toodeep);
		if (np->n_O1 != NULL)
			evalact(np->n_O1);
		for ( ; evalint(np->n_O2); evalact(np->n_O3)) {
			if ((i = setjmp(fwenv[fwlevel])) == ABREAK)
				break;
			else if (i == ACONTIN)
				continue;
			evalact(np->n_O4);
		}
		fwlevel--;
		break;

	case AFORIN:
		if (++fwlevel >= NNEST)
			awkerr(toodeep);
		xforin(np->n_O1, np->n_O2, np->n_O3);
		fwlevel--;
		break;

	case ABREAK:
	case ACONTIN:
		i = np->n_op;
		if (fwlevel < 0)
			awkwarn("No for or while for %s",
			    i==ABREAK ? "break" : "continue");
		else
			longjmp(fwenv[fwlevel], i);
		break;

	case ANEXT:
		longjmp(nextenv, 1);
		break;

	case AEXIT:
		if (!exitflag)
			awkexit(0);
		break;

	case APRINT:
		xprint(np->n_O1, np->n_O2);
		break;

	case APRINTF:
		xprintf(np->n_O1, np->n_O2, NULL);
		break;

	default:
		evalexpr(np);
	}
	while ((np = tempnodes) != NULL) {
		tempnodes = np->t_next;
		freenode(np);
	}
	goto again;
}

/*
 * Evaluate an expression.
 */
NODE *
evalexpr(np)
register NODE *np;
{
	register NODE *tnp;
	register int i;

	switch (np->n_op) {
	case ATERM:
		break;

	case AARRAY:
		np = xarray(np->n_O1, np->n_O2);
		break;

	case AFIELD:
		np = xfield((int)evalint(np->n_O1), NULL);
		break;

	case AFUNC:
		i = fnargs(tnp = np->n_O2);
		np = np->n_O1;
		if (i<np->t_MINARG || (np->t_MAXARG!=-1 && i>np->t_MAXARG))
			awkerr("Too %s arguments: %s",
			    i<np->t_MINARG ? "few" : "many", np->t_name);
		np = (*np->t_FUNC)(tnp, i);
		break;

	case AREMAT:
	case ARENMAT:
		i = evalre(np->n_O2, evalstring(np->n_O1));
		if (np->n_op == ARENMAT)
			i = !i;
		np = inode((INT)i);
		break;

	case ARE:
		np = inode((INT)evalre(np->n_O1, inline));
		break;

	case ACONC:
		np = xconc(np->n_O1, np->n_O2);
		break;

	case AADD:
		np = xadd(evalexpr(np->n_O1), evalexpr(np->n_O2));
		break;

	case ASUB:
		np = xsub(evalexpr(np->n_O1), evalexpr(np->n_O2));
		break;

	case AMUL:
		np = xmul(evalexpr(np->n_O1), evalexpr(np->n_O2));
		break;

	case ANEG:
		np = xneg(evalexpr(np->n_O1));
		break;

	case ADIV:
		np = xdiv(evalexpr(np->n_O1), evalexpr(np->n_O2));
		break;

	case AMOD:
		np = xmod(evalexpr(np->n_O1), evalexpr(np->n_O2));
		break;

	case AEQ:
	case ANE:
	case AGT:
	case AGE:
	case ALT:
	case ALE:
		np = xcmp(np->n_O1, np->n_O2, np->n_op);
		break;

	case AOROR:
		np = inode((INT)(evalint(np->n_O1) || evalint(np->n_O2)));
		break;

	case AANDAND:
		np = inode((INT)(evalint(np->n_O1) && evalint(np->n_O2)));
		break;

	case ANOT:
		np = inode((INT)(!evalint(np->n_O1)));
		break;

	case AASGN:
		np = xassign(np->n_O1, evalexpr(np->n_O2));
		break;

	case AINCA:
		np = xinca(np->n_O1);
		break;

	case ADECA:
		np = xdeca(np->n_O1);
		break;

	default:
		awkerr("Bad op in evalexpr %d", np->n_op);
	}
	return (np);
}

/*
 * Evaluate an expression and
 * return the resultant string.
 */
CHAR *
evalstring(np)
register NODE *np;
{
	static CHAR numbuf[100];

	np = evalexpr(np);
	if (np->n_flag & T_NUM) {
		if (np->n_flag & T_INT)
			sprintf(numbuf, "%ld", np->t_INT);
		else
			sprintf(numbuf, "%.6g", np->t_FLOAT);
		return (numbuf);
	} else
		return (np->t_STRING);
}

/*
 * Evaluate an expression and
 * return the resultant INT.
 */
INT
evalint(np)
register NODE *np;
{
	np = evalexpr(np);
	if (np->n_flag & T_NUM) {
		if (np->n_flag & T_INT)
			return (np->t_INT); else
			return (np->t_FLOAT);
	} else
		return (stoi(np->t_STRING));
}

/*
 * Evaluate an expression and
 * return the resultant FLOAT.
 */
FLOAT
evalfloat(np)
register NODE *np;
{
	np = evalexpr(np);
	if (np->n_flag & T_NUM) {
		if (np->n_flag & T_INT)
			return (np->t_INT); else
			return (np->t_FLOAT);
	} else
		return (stof(np->t_STRING));
}

/*
 * Called to evaluate a regular expression
 * with the given string.
 */
evalre(np, s)
register NODE *np;
register CHAR *s;
{
	sb = s;
	if (np!=NULL && np->n_op==ARBOL)
		return (reexec(np, s) != NULL);
	s--;
	do {
		if (reexec(np, ++s) != NULL)
			return (1);
	} while (*s!='\n' && *s!='\0');
	return (0);
}

/*
 * Internal regular expression
 * execution routines
 */
CHAR *
reexec(np, s)
register NODE *np;
register CHAR *s;
{
	register c;
	register CHAR *ss, *es;

	for ( ; np != NULL; np = np->n_O3)
		switch (np->n_op) {
		case ARBOL:
			if (s != sb)
				return (NULL);
			break;
	
		case AREOL:
			if (*s!='\n' && *s!='\0')
				return (NULL);
			break;
	
		case ARANY:
			c = *s++;
			if (c=='\n' || c=='\0')
				return (NULL);
			break;
	
		case ARDCHAR:
			if (islower(np->n_o1.n_char)) {
				if (isupper(c = *s++))
					c = tolower(c);
				if (c != np->n_o1.n_char)
					return (NULL);
				break;
			}
		case ARCHAR:
			c = *s++;
			if (c != np->n_o1.n_char)
				return (NULL);
			break;
	
		case ARDCLASS:
			if (isupper(c = *s++))
				c = tolower(c);
			if ((np->n_o1.n_charp[c/NBPC] & (1<<(c%NBPC))) == 0)
				return (NULL);
			break;

		case ARCLASS:
			c = *s++;
			if ((np->n_o1.n_charp[c/NBPC] & (1<<(c%NBPC))) == 0)
				return (NULL);
			break;
	
		case AROR:
			ss = s;
			if ((s = reexec(np->n_O1, s)) == NULL)
				if ((s = reexec(np->n_O2, ss))==NULL)
					return (NULL);
			break;
	
		case ARCLOS:
			ss = s;
			while ((es = reexec(np->n_O1, s)) != NULL)
				s = es;
			while (s >= ss)
				if ((es = reexec(np->n_O3, s--))!=NULL)
					return (es);
			return (NULL);
	
		case ARNECL:
			ss = s;
			while ((es = reexec(np->n_O1, s)) != NULL)
				s = es;
			while (s > ss)
				if ((es = reexec(np->n_O3, s--))!=NULL)
					return (es);
			return (NULL);
	
		case ARZOCL:
			ss = s;
			if ((es = reexec(np->n_O1, s)) != NULL)
				s = es;
			while (s >= ss)
				if ((es = reexec(np->n_O3, s--))!=NULL)
					return (es);
			return (NULL);
	
		default:
			awkerr("RE bad op %d", np->n_op);
		}
	return (s);
}
