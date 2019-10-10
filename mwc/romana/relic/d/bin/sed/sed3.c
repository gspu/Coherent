/*
 * A stream editor.
 * Execution.
 */
#include <stdio.h>
#include <ctype.h>
#include "sed.h"

char	*match();

/*
 * Execute the compiled commands on each line of the input file.
 */
process()
{
	FILE *fp;
	QCL **qcrpp, *qcrp, *lqp, *qp;
	int n, subflag;
	char com;
	register COM *cp;
	register char *bp1, *bp2;

	subflag = 0;
	cp = NULL;
	com = '\0';
	lno = 0;
	qcrpp = &qcrp;
	dolflag = 0;
	pattlen = 0;
	holdlen = 0;
	pattbuf[0] = '\0';
	holdbuf[0] = '\0';
	for (;;) {
		while (cp == NULL) {
			if (com != 'D') {
				if (nflag==0 && pattlen!=0)
					printf("%s\n", pattbuf);
				pattlen = 0;
			}
			*qcrpp = NULL;
			qp = qcrp;
			while (qp != NULL) {
				cp = qp->q_comp;
				lqp = qp;
				qp = qp->q_next;
				free(lqp);
				switch (cp->c_name) {
				case 'a':
					printf("%s\n", cp->c_p.p_buf);
					break;
				case 'r':
					bp1 = cp->c_p.p_buf;
					if ((fp=fopen(bp1, "r")) == NULL) {
						printe("Cannot open %s", bp1);
						continue;
					}
					while (fgets(linebuf, LHPSIZE, fp))
						printf("%s", linebuf);
					fclose(fp);
				}
			}
			if (com == 'q')
				exit(0);
			if (com != 'D') {
				if (readpat() == 0)
					exit(0);
			}
			cp = comp;
			qcrpp = &qcrp;
			com = '\0';
		}
		if (admitr(cp) == 0) {
			cp = cp->c_next;
			continue;
		}
		switch (com=cp->c_name) {
		case '=':
			printf("%d\n", lno);
			break;
		case '{':
			cp = cp->c_p.p_com;
			continue;
		case '}':
			break;
		case 'a':
			*qcrpp = qp = (QCL *)salloc(sizeof (QCL));
			qp->q_comp = cp;
			qcrpp = &qp->q_next;
			break;
		case 'b':
			cp = cp->c_p.p_lab->l_comp;
			continue;
		case 'c':
			if (cp->c_iran == 0)
				printf("%s\n", cp->c_p.p_buf);
		case 'd':
			pattlen = 0;
			cp = NULL;
			continue;
		case 'D':
			bp1 = bp2 = pattbuf;
			pattlen = 0;
			while (*bp2 != '\0') {
				if (*bp2++ == '\n') {
					pattlen = 1;
					break;
				}
			}
			while (*bp1++=*bp2++)
				pattlen++;
			if (pattlen == 0)
				com = 'd';
			cp = NULL;
			continue;
		case 'g':
			n = pattlen = holdlen;
			bp1 = pattbuf;
			bp2 = holdbuf;
			while (n--)
				*bp1++ = *bp2++;
			break;
		case 'G':
			if (pattlen+holdlen > LHPSIZE) {
				printe("Pattern buffer overflow");
				break;
			}
			bp1 = &pattbuf[pattlen];
			bp2 = &holdbuf[0];
			if (pattlen != 0)
				bp1[-1] = '\n';
			pattlen += n = holdlen;
			while (n--)
				*bp1++ = *bp2++;
			break;
		case 'h':
			n = holdlen = pattlen;
			bp1 = holdbuf;
			bp2 = pattbuf;
			while (n--)
				*bp1++ = *bp2++;
			break;
		case 'H':
			if (holdlen+pattlen > LHPSIZE) {
				printe("Hold buffer overflow");
				break;
			}
			bp1 = &holdbuf[holdlen];
			bp2 = &pattbuf[0];
			if (holdlen != 0)
				bp1[-1] = '\n';
			holdlen += n = pattlen;
			while (n--)
				*bp1++ = *bp2++;
			break;
		case 'i':
			printf("%s\n", cp->c_p.p_buf);
			break;
		case 'l':
			execlis();
			break;
		case 'n':
			if (pattlen != 0)
				printf("%s\n", pattbuf);
			pattlen = 0;
		case 'N':
			if (readpat() == 0) {
				com = 'q';
				cp = NULL;
				continue;
			}
			break;
		case 'p':
			if (pattlen != 0)
				printf("%s\n", pattbuf);
			break;
		case 'P':
			bp1 = pattbuf;
			while (*bp1!='\0' && *bp1!='\n')
				putchar(*bp1++);
			putchar('\n');
			break;
		case 'q':
			cp = NULL;
			continue;
		case 'r':
			*qcrpp = qp = (QCL *)salloc(sizeof (QCL));
			qp->q_comp = cp;
			qcrpp = &qp->q_next;
			break;
		case 's':
			subflag = execsub(cp->c_p.p_sub);
			break;
		case 't':
			if (subflag) {
				cp = cp->c_p.p_lab->l_comp;
				continue;
			}
			break;
		case 'w':
			fprintf(cp->c_p.p_fil->f_filp, "%s\n", pattbuf);
			break;
		case 'x':
			n = pattlen;
			bp1 = linebuf;
			bp2 = pattbuf;
			while (n--)
				*bp1++ = *bp2++;
			n = holdlen;
			bp1 = pattbuf;
			bp2 = holdbuf;
			while (n--)
				*bp1++ = *bp2++;
			n = pattlen;
			bp1 = holdbuf;
			bp2 = linebuf;
			while (n--)
				*bp1++ = *bp2++;
			n = pattlen;
			pattlen = holdlen;
			holdlen = n;
			break;
		case 'y':
			bp2 = cp->c_p.p_buf;
			for (bp1=pattbuf; *bp1; bp1++)
				*bp1 = bp2[*bp1];
			break;
		}
		cp = cp->c_next;
	}
}

/*
 * See if a the current line is within a given line range.
 */
admitr(cp)
register COM *cp;
{
	register int n;

	switch (cp->c_nadd) {
	case 0:
		cp->c_iran = 0;
		n = 1;
		break;
	case 1:
		cp->c_iran = 0;
		n = admits(&cp->c_a[0]);
		break;
	case 2:
		if (cp->c_iran == 0) {
			n = cp->c_iran = admits(&cp->c_a[0]);
		} else {
			if (admits(&cp->c_a[1]) != 0)
				cp->c_iran = 0;
			n = 1;
		}
	}
	return (cp->c_notf!=n);
}

/*
 * See if the current line matches the given line address.
 */
admits(ap)
register ADD *ap;
{
	if (ap->a_lno == 0)
		return (execute(ap->a_pat));
	if (ap->a_lno == HUGE)
		return (dolflag);
	return (ap->a_lno-1 == lno);
}

/*
 * Append a line into the pattern buffer.  The current line number
 * is incremented.  If the line overflows, an error message is
 * printed out.
 */
readpat()
{
	static int lastc = '\0';
	register int c, n;
	register char *cp;

	if (dolflag)
		return (0);
	cp = &pattbuf[pattlen];
	if (pattlen != 0)
		cp[-1] = '\n';
	n = &pattbuf[LHPSIZE-1] - cp;
	if (lastc == '\0')
		c = getp();
	else {
		c = lastc;
		lastc = '\0';
	}
	while (c!='\n' && c!=EOF) {
		if (n-- <= 0) {
			fprintf(stderr, "%d: Line too long\n", lno);
			exit(1);
		}
		*cp++ = c;
		c = getp();
	}
	*cp++ = '\0';
	pattlen = cp - pattbuf;
	lno++;
	if (c==EOF || (c=getp())==EOF)
		dolflag++;
	else
		lastc = c;
	return (1);
}

/*
 * Get a character.
 */
getp()
{
	ECL *ep;
	register int c;

	while (ifp==NULL || (c=getc(ifp))==EOF) {
		if (ifp != NULL)
			fclose(ifp);
		if ((ep=eclp) == NULL)
			return (EOF);
		if ((ifp=fopen(ep->e_argp, "r")) == NULL) {
			fprintf(stderr, "Cannot open %s\n", ep->e_argp);
			exit(1);
		}
		eclp = ep->e_next;
		free(ep);
	}
	return (c);
}

/*
 * Print out the contents of the pattern buffer in an unambigous
 * form.  All non-printing characters are escaped.
 */
execlis()
{
	register int n;
	register char *cp;

	n = 0;
	for (cp=pattbuf; *cp; cp++) {
		if (n++ >= 72) {
			n = 0;
			printf("\\\n");
		}
		switch (*cp) {
		case '\b':
			printf("-\b<");
			continue;
		case '\t':
			printf("-\b>");
			continue;
		case '\\':
			printf("\\\\");
			continue;
		default:
			if (isascii(*cp) && !iscntrl(*cp)) {
				putchar(*cp);
				continue;
			}
			printf("\\%03o", *cp);
			n += 3;
		}
	}
	putchar('\n');
}

/*
 * Execute substitute command.
 */
execsub(sp)
SUB *sp;
{
	register char *np;
	register char *pp;
	register int n;
	register char *lp;
	register char *rp;
	register int nth;
	register int len;
	register int subflag;
	register int subnths;

	subflag = 0;
	subnths = sp->s_nth;
	for (n=0; n<1+NBRC; n++) {
		brcl[n].b_bp = NULL;
		brcl[n].b_ep = NULL;
	}
	brcl[0].b_ep = pattbuf;
	lp = pattbuf;
	np = linebuf;
	nth = 0;
	for (;;) {
		pp = sp->s_pat;
		if (*pp == CSSOL) {
			if (lp != pattbuf)
				break;
			pp++;
		}
		if ((pp=match(lp, pp)) == NULL) {
			if (*lp++ == '\0')
				break;
			continue;
		}
		nth++;
		if (subnths) {
			if (nth < subnths) {
				lp = pp;
				continue;
			}
			if (nth > subnths)
				goto done;
		}
		subflag = 1;
		len = pp - lp;
		brcl[0].b_bp = lp;
		lp = pp;
		pp = brcl[0].b_ep;
		brcl[0].b_ep = lp;
		n = brcl[0].b_bp - pp;
		if (np+n >= &linebuf[LHPSIZE-1])
			goto ovf;
		while (n--)
			*np++ = *pp++;
		rp = sp->s_rep;
		while (*rp) {
			if (np >= &linebuf[LHPSIZE-4])
				goto ovf;
			if (*rp != '\\') {
				*np++ = *rp++;
				continue;
			}
			if (*++rp == '\\') {
				*np++ = *rp++;
				continue;
			}
			n = *rp++ - '0';
			if ((pp=brcl[n].b_bp) == NULL)
				continue;
			n = brcl[n].b_ep-pp;
			if (np+n >= &linebuf[LHPSIZE-4])
				goto ovf;
			while (n--)
				*np++ = *pp++;
		}
		if (*lp == '\0')
			break;
		if (len == 0)
			lp++;
		if (subnths!=0 && nth==subnths)
			break;
	}
done:
	pp = brcl[0].b_ep;
	while (*pp) {
		if (np >= &linebuf[LHPSIZE-1])
			goto ovf;
		*np++ = *pp++;
	}
	*np++ = '\0';
	pattlen = n = np - linebuf;
	pp = pattbuf;
	np = linebuf;
	do {
		*pp++ = *np++;
	} while (--n);
	if (subflag) {
		if (sp->s_cop)
			printf("%s\n", pattbuf);
		if (sp->s_fil)
			fprintf(sp->s_fil->f_filp, "%s\n", pattbuf);
	}
	return (subflag);
ovf:
	printc("Line buffer overflow");
	return (0);
}

/*
 * See if the compiled pattern `pat' matches the line in pattbuf.
 */
execute(pat)
char *pat;
{
	register int i;
	register char *lp, *ep;

	for (i=0; i<1+NBRC; i++) {
		brcl[i].b_bp = NULL;
		brcl[i].b_ep = NULL;
	}
	if (pat[0] == CSSOL)
		ep = match(lp=pattbuf, &pat[1]);
	else {
		ep = NULL;
		lp = pattbuf;
		do {
			if (ep=match(lp, pat))
				break;
		} while (*lp++);
	}
	if (ep) {
		brcl[0].b_bp = lp;
		brcl[0].b_ep = ep;
	}
	return (ep ? 1 : 0);
}

/*
 * Given a pointer to a compiled expression, `cp', and a pointer to
 * a line, `lp', return 1 if the expression matches, else 0.
 */
char *
match(lp, cp)
register char *lp, *cp;
{
	register int n;
	char *llp, *lcp;

	for (;;) {
		switch (*cp++) {
		case CSNUL:
			return (lp);
		case CSEOL:
			if (*lp)
				return (NULL);
			return (lp);
		case CSOPR:
			brcl[*cp++].b_bp = lp;
			continue;
		case CSCPR:
			brcl[*cp++].b_ep = lp;
			continue;
		case CSBRN:
			n = *cp++;
			lcp = cp;
			cp = brcl[n].b_bp;
			n = brcl[n].b_ep - cp;
			if (n > LHPSIZE)
				return (NULL);
			while (n-- > 0)
				if (*lp++ != *cp++)
					return (NULL);
			cp = lcp;
			continue;
		case CSDOT:
			if (*lp++ == '\0')
				return (NULL);
			continue;
		case CMDOT:
			llp = lp;
			while (*lp)
				lp++;
			goto star;
		case CSCHR:
			if (*cp++ != *lp++)
				return (NULL);
			continue;
		case CMCHR:
			llp = lp;
			while (*cp == *lp)
				lp++;
			cp++;
			goto star;
		case CSSCC:
			if (*cp++ == tolower(*lp++))
				continue;
			return (NULL);
		case CMSCC:
			llp = lp;
			while (*cp == tolower(*lp++))
				;
			cp++;
			goto star;
		case CSCCL:
			n = *cp++;
			while (*cp++ != *lp)
				if (--n == 0)
					return (NULL);
			lp++;
			cp += n-1;
			continue;
		case CMCCL:
			llp = lp;
			lcp = cp;
			while (*lp) {
				cp = lcp;
				n = *cp++;
				while (*cp++ != *lp)
					if (--n == 0)
						goto star;
				lp++;
			}
			cp = lcp + *lcp + 1;
			goto star;
		case CSNCL:
			if (*lp == '\0')
				return (NULL);
			n = *cp++;
			while (n--)
				if (*cp++ == *lp)
					return (NULL);
			lp++;
			continue;
		case CMNCL:
			llp = lp;
			lcp = cp;
			while (*lp) {
				cp = lcp;
				n = *cp++;
				while (n--) {
					if (*cp++ == *lp) {
						cp = lcp + *lcp + 1;
						goto star;
					}
				}
				lp++;
			}
			cp = lcp + *lcp + 1;
		star:
			do {
				if (lcp=match(lp, cp))
					return (lcp);
			} while (--lp >= llp);
			return (NULL);
		}
	}
}


/*
 * Print out an execution error message.
 */
printe(s)
{
	fprintf(stderr, "%r", &s);
	putc('\n', stderr);
}
