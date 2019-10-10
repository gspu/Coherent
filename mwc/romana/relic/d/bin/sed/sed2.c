/*
 * sed/sed2.c
 * A stream editor.
 * Compiler.
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "sed.h"

FIL	*codefil();
char	*codetrn();
LAB	*codelab();
char	*duplstr();
SUB	*codesub();

/*
 * Compile a file of commands.
 */
compfil(name)
char *name;
{
	register FILE *fp;
	register int c;
	register char *cp;

	if ((fp=fopen(name, "r")) == NULL) {
		fprintf(stderr, "Cannot open %s\n", name);
		exit(1);
	}
	cp = linebuf;
	while ((c=getc(fp)) != EOF) {
		if (cp >= &linebuf[LHPSIZE-2]) {
			printc("Line too long");
			cp = linebuf;
			while ((c=getc(fp))!='\n' && c!=EOF)
				;
			if (c == EOF)
				break;
		}
		if (c == '\n' || c == ';') {
			*cp = '\0';
			ncp = linebuf;
			compcom();
			cp = linebuf;
			if (c == ';')
				--lno;
			continue;
		}
		if (c == '\\') {
			if ((c=getc(fp))!='\n' && c!='\\' && c!=';') {
				ungetc(c, fp);
				c = '\\';
			}
		}
		*cp++ = c;
	}
	if (cp != linebuf) {
		*cp++ = '\0';
		ncp = linebuf;
		compcom();
	}
}

/*
 * Compile a string.
 */
compstr(cp) register char *cp;
{
	register int c;
	register char *lp;

	do {
		lp = linebuf;
		while ((c = *cp++) != '\0' && c != '\n' && c != ';') {
			if (c == '\\' && (*cp == ';' || *cp == '\n'))
				c = *cp++;	/* "\;" means ';' */
			*lp++ = c;
		}
		*lp = '\0';
		ncp = linebuf;
		compcom();
	} while (c != '\0');
}

/*
 * Compile a command and return a pointer to the compiled
 * structure.
 */
compcom()
{
	QCL *qp;
	int nadd;
	register COM *cp;
	register LAB *lp;
	register int c;

	lno++;
	while (isascii(c=getn()) && isspace(c))
		;
	if (c == '\0')
		return;
	ungetn(c);
	cp = (COM *)salloc(sizeof (COM));
	if (getaddr(&cp->c_a[0]) == 0)
		goto err;
	nadd = addnone ? 0 : 1;
	if ((c=getn()) == ',') {
		if (addnone!=0 || getaddr(&cp->c_a[1])==0 || addnone!=0)
			goto err;
		c = getn();
		nadd++;
	}
	while (isascii(c) && isspace(c))
		c = getn();
	if (c == '!') {
		cp->c_notf = 1;
		c = getn();
	} else
		cp->c_notf = 0;
	cp->c_name = c;
	cp->c_nadd = nadd;
	switch (c) {
	case '{':
		qp = (QCL *)salloc(sizeof (QCL));
		qp->q_next = qcbp;
		qcbp = qp;
		qp->q_comp = cp;
		if ((c=getn()) != '\0') {
			*compp = cp;
			compp = &cp->c_next;
			ungetn(c);
			compcom();
			return;
		}
		break;
	case '}':
		if ((qp=qcbp) == NULL) {
			printc("Too many `}'");
			goto err;
		}
		qcbp = qcbp->q_next;
		if (codenwl() == 0)
			goto err;
		qp->q_comp->c_p.p_com = qp->q_comp->c_next;
		qp->q_comp->c_next = cp;
		free(qp);
		break;
	case '=':
		if (codenwl() == 0)
			goto err;
		break;
	case ':':
		if ((lp=codelab()) == NULL)
			goto err;
		if (lp->l_comp != NULL) {
			printc("Label %s multiply defined", pattbuf);
			goto err;
		}
		lp->l_comp = cp;
		break;
	case 'a':
		if (nadd > 1)
			goto adr;
		if (getn() != '\n')
			goto syn;
		if (codelin() == 0)
			goto err;
		cp->c_p.p_buf = duplstr(pattbuf);
		if (cp->c_a[0].a_lno == 0+1) {
			cp->c_name = 'i';
			cp->c_a[0].a_lno = 1+1;
		}
		break;
	case 'b':
		if ((lp=codelab()) == NULL)
			goto err;
		cp->c_p.p_lab = lp;
		lp->l_refc++;
		break;
	case 'c':
		if (getn() != '\n')
			goto syn;
		if (codelin() == 0)
			goto err;
		cp->c_p.p_buf = duplstr(pattbuf);
		break;
	case 'd':
	case 'D':
	case 'g':
	case 'G':
	case 'h':
	case 'H':
		if (codenwl() == 0)
			goto err;
		break;
	case 'i':
		if (nadd > 1)
			goto adr;
		if (getn() != '\n')
			goto syn;
		if (codelin() == 0)
			goto err;
		cp->c_p.p_buf = duplstr(pattbuf);
		break;
	case 'l':
	case 'n':
	case 'N':
	case 'p':
	case 'P':
		if (codenwl() == 0)
			goto err;
		break;
	case 'q':
		if (nadd > 1)
			goto adr;
		if (codenwl() == 0)
			goto err;
		break;
	case 'r':
		if (nadd > 1)
			goto adr;
		if (codelin() == 0)
			goto err;
		if (pattbuf[0] != ' ')
			goto err;
		cp->c_p.p_buf = duplstr(&pattbuf[1]);
		break;
	case 's':
		if ((cp->c_p.p_sub=codesub()) == NULL)
			goto err;
		break;
	case 't':
		if ((lp=codelab()) == NULL)
			goto err;
		cp->c_p.p_lab = lp;
		lp->l_refc++;
		break;
	case 'w':
		if ((cp->c_p.p_fil=codefil()) == NULL)
			goto err;
		break;
	case 'x':
		if (codenwl() == 0)
			goto err;
		break;
	case 'y':
		if ((cp->c_p.p_buf=codetrn()) == NULL)
			goto err;
		break;
	default:
		printc("Bad command");
		goto err;
	}
	*compp = cp;
	compp = &cp->c_next;
	return;

syn:
	printc("Syntax error");
	free(cp);
	return;

adr:
	printc("Bad address");
err:
	free(cp);
}

/*
 * Read a line address and compile it into the given address
 * pointer.
 */
getaddr(ap)
register ADD *ap;
{
	register int c, n;

	addnone = 0;
	if ((c=getn()) == '/') {
		if (compile('/') == 0)
			return 0;
		ap->a_lno = 0;
		ap->a_pat = duplstr(pattbuf);
		return 1;
	}
	if (c == '$') {
		ap->a_lno = HUGE;
		return 1;
	}
	if (isascii(c) && isdigit(c)) {
		n = 0;
		do {
			n = n*10 + c-'0';
		} while (isascii(c=getn()) && isdigit(c));
		ungetn(c);
		ap->a_lno = n+1;
		return 1;
	}
	ungetn(c);
	addnone = 1;
	return 1;
}

/*
 * There shouldn't be any more input left.
 */
codenwl()
{
	if (getn() != '\0') {
		printc("Syntax error");
		return 0;
	}
	return 1;
}

/*
 * Parse a label, put it into the label table and return
 * a pointer to the entry.
 */
LAB *
codelab()
{
	char name[LNMSIZE];
	register LAB *lp;
	register int c;
	register char *np;

	while (isascii(c=getn()) && isspace(c))
		;
	np = name;
	while (c != '\0') {
		if (np < &name[LNMSIZE-1])
			*np++ = c;
		c = getn();
	}
	*np = '\0';
	for (lp=labp; lp; lp=lp->l_next) {
		if (strcmp(lp->l_name, name) == 0)
			return lp;
	}
	lp = (LAB *)salloc(sizeof (LAB));
	lp->l_next = labp;
	labp = lp;
	strcpy(lp->l_name, name);
	lp->l_comp = NULL;
	lp->l_refc = 0;
	return lp;
}

/*
 * Parse a file name that will be used to write on and set it
 * up in our write table.
 */
FIL *
codefil()
{
	char name[FNMSIZE];
	register FIL *fp;
	register int c;
	register char *np;

	while (isascii(c=getn()) && isspace(c))
		;
	np = name;
	while (c != '\0') {
		if (np >= &name[FNMSIZE-1]) {
			printc("Line buffer overflow");
			return NULL;
		}
		*np++ = c;
		c = getn();
	}
	*np = '\0';
	for (fp=filp; fp; fp=fp->f_next) {
		if (strcmp(fp->f_name, name) == 0)
			return fp;
	}
	fp = (FIL *)salloc(sizeof (FIL));
	fp->f_next = filp;
	filp = fp;
	strcpy(fp->f_name, name);
	fp->f_filp = NULL;
	return fp;
}

/*
 * Get a line of text for commands such as the append.
 */
codelin()
{
	register int c;
	register char *pp;

	pp = pattbuf;
	while ((c=getn()) != '\0') {
		if (pp >= &pattbuf[LHPSIZE-1]) {
			printc("Line buffer overflow");
			return 0;
		}
		*pp++ = c;
	}
	*pp = '\0';
	return 1;
}

/*
 * Compile the substitute command.
 */
SUB *
codesub()
{
	int nth, ec;
	register SUB *sp;
	register int c;
	register char *pp;

	nth = 1;
	if (isascii(c=getn()) && isdigit(c)) {
		nth = 0;
		while (isascii(c) && isdigit(c)) {
			nth = nth*10 + c-'0';
			c = getn();
		}
	}
	if ((ec=c) == '\n') {
		ungetn(c);
		printc("Syntax error");
		return NULL;
	}
	if (compile(ec) == 0)
		return NULL;
	pp = holdbuf;
	while ((c=getn()) != ec) {
		if (pp >= &holdbuf[LHPSIZE-4]) {
			printc("Pattern buffer overflow");
			return NULL;
		}
		switch (c) {
		case '\0':
			ungetn(c);
			printc("Syntax error");
			return NULL;
		case '&':
			*pp++ = '\\';
			*pp++ = '0';
			continue;
		case '\\':
			if ((c=getn()) == '\\') {
				*pp++ = '\\';
				*pp++ = '\\';
				continue;
			}
			if (c >= '1'  &&  c <= '9') {
				*pp++ = '\\';
				*pp++ = c;
				continue;
			}
		default:
			*pp++ = c;
			continue;
		}
	}
	*pp++ = '\0';
	sp = (SUB *)salloc(sizeof (SUB));
	sp->s_pat = duplstr(pattbuf);
	sp->s_rep = duplstr(holdbuf);
	sp->s_nth = nth;
	sp->s_cop = 0;
	sp->s_fil = NULL;
	while ((c=getn()) != '\0') {
		switch (c) {
		case 'g':
			sp->s_nth = 0;
			continue;
		case 'p':
			sp->s_cop = 1;
			continue;
		case 'w':
			sp->s_fil = codefil();
			continue;
		default:
			printc("Bad flag in substitute");
			free(sp->s_pat);
			free(sp->s_rep);
			free(sp);
			return NULL;
		}
	}
	return sp;
}

/*
 * Compile a regular expression.  `ec' is the character upon which
 * the regular expression ends.  If an error is encountered, the
 * pattern is cleared and input characters are thrown away until
 * a new line is found.
 */
compile(ec)
{
	int bstack[NBRC], bcount, blevel, n;
	register int c;
	register char *pp, *lpp;

	bcount = 1;
	blevel = 0;
	pp = &pattbuf[0];
	if ((c=getn()) == ec) {
		if (*pp == CSNUL) {
			printc("No saved pattern");
			goto err;
		}
		return 1;
	}
	if (c == '^') {
		*pp++ = CSSOL;
		c = getn();
	}
	while (c != ec) {
		if (pp > &pattbuf[LHPSIZE-4])
			goto ovf;
		switch (c) {
		case '\0':
		case '.':
			if ((c=getn()) != '*') {
				*pp++ = CSDOT;
				continue;
			}
			*pp++ = CMDOT;
			c = getn();
			continue;
		case '$':
			if ((c=getn()) != ec) {
				ungetn(c);
				c = '$';
				goto character;
			}
			*pp++ = CSEOL;
			continue;
		case '[':
			lpp = pp;
			if ((c=getn()) == '^')
				*pp++ = CSNCL;
			else {
				ungetn(c);
				*pp++ = CSCCL;
			}
			*pp++ = 0;
			if ((c=getn()) == ']')
				*pp++ = c;
			else
				ungetn(c);
			while ((c=getn()) != ']') {
				if (c == '\n')
					goto nwl;
				if (c!='-' || pp==lpp+2) {
					if (pp >= &pattbuf[LHPSIZE-4])
						goto ovf;
					*pp++ = c;
					if (sflag && isascii(c) && isallet(c))
						*pp++ = toother(c);
					continue;
				}
				if ((c=getn()) == '\n')
					goto nwl;
				if (c == ']') {
					*pp++ = '-';
					ungetn(c);
					continue;
				}
				if ((n=pp[-1]) > c)
					goto syn;
				while (++n <= c) {
					if (pp >= &pattbuf[LHPSIZE-4])
						goto ovf;
					*pp++ = n;
					if (sflag && isascii(c) && isallet(c))
						*pp++ = toother(c);
				}
			}
			if ((c=getn()) == '*') {
				(*lpp)++;
				c = getn();
			}
			if ((n=pp-(lpp+2)) > 255) {
				printc("Character class too large");
				goto err;
			}
			*++lpp = n;
			continue;
		case '\\':
			switch (c=getn()) {
			case '\n':
				goto nwl;
			case '(':
				if (bcount > NBRC) {
					printc("Too many \\(");
					goto err;
				}
				*pp++ = CSOPR;
				*pp++ = bstack[blevel++] = bcount++;
				c = getn();
				continue;
			case ')':
				if (blevel == 0)
					goto syn;
				*pp++ = CSCPR;
				*pp++ = bstack[--blevel];
				c = getn();
				continue;
			case 'n':
				c = '\n';
				goto character;
			default:
				if (isascii(c) && isdigit(c)) {
					*pp++ = CSBRN;
					*pp++ = c-'0';
					c = getn();
					continue;
				}
			}
		case '*':
		default:
		character:
			if (sflag && isascii(c) && isallet(c)) {
				*pp++ = CSSCC;
				if (isupper(c))
					c = tolower(c);
			} else
				*pp++ = CSCHR;
			*pp++ = c;
			if ((c=getn()) == '*') {
				pp[-2]++;
				c = getn();
			}
		}
	}
	*pp++ = CSNUL;
	return 1;
ovf:
	printc("Pattern buffer overflow");
	pattbuf[0] = CSNUL;
	return 0;
nwl:
	ungetn(c);
syn:
	printc("Syntax error");
err:
	pattbuf[0] = CSNUL;
	return 0;
}

/*
 * Compile the translate command.
 */
char *
codetrn()
{
	int ec;
	char *trnp;
	register int c, n;
	register char *bp;

	trnp = salloc(256);
	bp = trnp;
	for (n=0; n<256; n++)
		*bp++ = n;
	if ((ec=getn()) == '\0') {
		printc("Syntax error");
		return NULL;
	}
	bp = pattbuf;
	while ((c=getn()) != ec) {
		if (c == '\0') {
			printc("Syntax error");
			return NULL;
		}
		if (bp >= &pattbuf[LHPSIZE-1]) {
			printc("Code buffer overflow");
			return NULL;
		}
		*bp++ = c&0377;
	}
	bp = pattbuf;
	while ((c=getn()) != ec) {
		if (c == '\0') {
			printc("Syntax error");
			return NULL;
		}
		if (*bp == '\0') {
			printc("Right part of translate too long");
			return NULL;
		}
		trnp[*bp++] = c;
	}
	return trnp;
}

/*
 * Given a string, return a pointer to a copy of it.
 */
char *
duplstr(s) register char *s;
{
	return strcpy(salloc(strlen(s)+1), s);
}

/*
 * Get a character.
 */
getn()
{
	register int c;

	if ((c = *ncp++) == '\0')
		--ncp;
	return c;
}

/*
 * Unget a character.
 */
ungetn(c)
{
	if (*ncp!='\0' || c!='\0')
		*--ncp = c;
}

/*
 * Go through the command list fixing up labels and files.
 */
load()
{
	register LAB *lp;
	register FIL *fp;

	*compp = NULL;
	if (qcbp != NULL)
		printc("Missing `}'");
	for (lp=labp; lp; lp=lp->l_next) {
		if (lp->l_comp == NULL)
			printc("Undefined label %s", lp->l_name);
	}
	for (fp=filp; fp; fp=fp->f_next) {
		if (fp->f_filp == NULL)
			if ((fp->f_filp=fopen(fp->f_name, "w")) == NULL)
				printc("Cannot open %s", fp->f_name);
	}
}

/*
 * Print out a compile error message.
 */
printc(s)
{
	nerr++;
	fprintf(stderr, "%d: %r", lno, &s);
	putc('\n', stderr);
}

/* end of sed/sed2.c */
