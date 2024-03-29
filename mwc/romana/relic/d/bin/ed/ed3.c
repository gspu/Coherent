/*
 * An editor.
 * Regular expressions and the substitute and global commands.
 */
#include <stdio.h>
#include <ctype.h>
#include "ed.h"

/*
 * Global command.
 */
global(a1, a2, not)
{
	register int a, c;
	register char *gp;

	if (a1>a2 || a1<1 || a2>doladd) {
		derror("Address out of range");
		return (0);
	}
	if (gcp != NULL) {
		derror("Global inside global not allowed");
		return (0);
	}
	if ((c=getx()) == '\n') {
		ungetx(c);
		derror("Syntax error");
		return (0);
	}
	if (compile(c) == 0)
		return;
	gp = globbuf;
	while ((c=getx()) != '\n') {
		if (gp >= &globbuf[GBSIZE-3]) {
			derror("Global buffer overflow");
			return (0);
		}
		if (c == '\\')
			if ((c=getx())!='\n' && c!='\\')
				*gp++ = '\\';
		*gp++ = c;
	}
	if (gp == globbuf)
		goto out;
	*gp++ = '\n';
	*gp = '\0';
	for (a=a1; a<=a2; a++) {
		if (intflag)
			goto out;
		if (execute(a) != not)
			line[a] |= 1;
	}
	a = 1;
	while (a <= doladd) {
		if (intflag)
			goto out;
		if ((line[a]&1) == 0) {
			a++;
			continue;
		}
		--line[a];
		dotadd = a;
		gcp = globbuf;
		while (*gcp != '\0')
			if (command() == 0)
				goto out;
		a = 1;
	}
out:
	for (a=1; a<=doladd; a++)
		line[a] &= ~1;
	gcp = NULL;
	ungetx('\n');
	return (1);
}

/*
 * Substitute part 1.
 * Check that address range is legal and parse substitute command.
 */
subs1(a1, a2)
{
	register int ec, c;
	register char *sp;

	if (a1>a2 || a1<1 || a2>doladd) {
		derror("Address out of range");
		return (0);
	}
	subnewl = 0;
	subnths = 1;
	if (isascii(c=getx()) && isdigit(c)) {
		subnths = 0;
		while (isascii(c) && isdigit(c)) {
			subnths = subnths*10 + c-'0';
			c = getx();
		}
	}
	if ((ec=c) == '\n') {
		ungetx(c);
		derror("Syntax error");
		return (0);
	}
	if (compile(ec) == 0)
		return (0);
	sp = subsbuf;
	while ((c=getx()) != ec) {
		if (sp >= &subsbuf[SBSIZE-4]) {
			derror("Temporary buffer overflow");
			return (0);
		}
		switch (c) {
		case '\n':
			ungetx(c);
			derror("Syntax error");
			return (0);
		case '&':
			*sp++ = '\\';
			*sp++ = '0';
			continue;
		case '\\':
			if ((c=getx()) == '\\') {
				*sp++ = '\\';
				*sp++ = '\\';
				continue;
			}
			if (c >= '1'  &&  c <= '9') {
				*sp++ = '\\';
				*sp++ = c;
				continue;
			}
			if (c == '\n')
				subnewl++;
		default:
			*sp++ = c;
			continue;
		}
	}
	*sp++ = '\0';
	if ((c=getx()) == 'g')
		subnths = 0;
	else
		ungetx(c);
	return (1);
}

/*
 * Substitute (part 2).
 * Execute substitute command.
 */
subs2(a1, a2)
{
	long seek, mark;
	int len, nth, err, a;
	char *mp, *sp;
	register int n;
	register char *lp, *rp;

	err = 1;
	for (a=a1; a<=a2; a++) {
		if (intflag)
			return (1);
		if (getline(a, tempbuf) == 0)
			return (0);
		for (n=0; n<1+BRSIZE; n++) {
			brace[n].b_bp = NULL;
			brace[n].b_ep = NULL;
		}
		brace[0].b_ep = tempbuf;
		mp = tempbuf;
		lp = linebuf;
		nth = 0;
		for (;;) {
			rp = codebuf;
			if (*rp == CSSOL) {
				if (mp != tempbuf)
					break;
				rp++;
			}
			if ((rp=match(mp, rp)) == NULL) {
				if (*mp++ == '\0')
					break;
				continue;
			}
			nth++;
			if (subnths) {
				if (nth < subnths) {
					mp = rp;
					continue;
				}
				if (nth > subnths)
					goto done;
			}
			err = 0;
			saved = 0;
			dotadd = a;
			brace[0].b_bp = mp;
			mp = rp;
			rp = brace[0].b_ep;
			brace[0].b_ep = mp;
			len = mp - brace[0].b_bp;
			n = brace[0].b_bp - rp;
			if (lp+n >= &linebuf[LBSIZE-1])
				goto ovf;
			while (n--)
				*lp++ = *rp++;
			sp = subsbuf;
			while (*sp) {
				if (lp >= &linebuf[LBSIZE-4])
					goto ovf;
				if (*sp != '\\') {
					*lp++ = *sp++;
					continue;
				}
				if (*++sp == '\\') {
					*lp++ = *sp++;
					continue;
				}
				n = *sp++ - '0';
				if ((rp=brace[n].b_bp) == NULL)
					continue;
				n = brace[n].b_ep-rp;
				if (lp+n >= &linebuf[LBSIZE-4])
					goto ovf;
				while (n--)
					*lp++ = *rp++;
			}
			if (*mp == '\0')
				break;
			if (len == 0)
				mp++;
			if (subnths!=0 && nth==subnths)
				break;
		}
	done:
		if (dotadd != a)
			continue;
		rp = brace[0].b_ep;
		while (*rp) {
			if (lp >= &linebuf[LBSIZE-1])
				goto ovf;
			*lp++ = *rp++;
		}
		*lp++ = '\0';
		seek = linead();
		if (subnewl) {
			lp = linebuf;
			while (*lp != '\n')
				lp++;
			*lp++ = '\0';
		}
		if (putline(linebuf, lp-linebuf) == 0)
			return (0);
		suborig = line[a];
		if (suborig&1)
			--suborig;
		subseek = seek|1;
		line[a] = seek;
		mark = suborig|1;
		for (n=0; n<MKSIZE; n++)
			if (marklin[n] == mark)
				marklin[n] = subseek;
		if (subnewl != 0) {
			n = 1;
			do {
				mp = lp;
				while (*lp!='\n' && *lp!='\0')
					lp++;
				if (*lp == '\0')
					n = 0;
				*lp++ = '\0';
				seek = linead();
				if (putline(mp, lp-mp) == 0)
					return (0);
				if (expand(a) == 0)
					return (0);
				line[++a] = seek;
				dotadd = a;
				a2++;
			} while (n);
		}
	}
	if (err!=0 && gcp==NULL) {
		derror("Pattern not matched");
		return (0);
	}
	return (1);

ovf:
	derror("Temporary buffer overflow");
	return (0);
}

/*
 * Compile a regular expression.  `ec' is the character upon which
 * the regular expression ends.  If an error is encountered, the
 * pattern is restored and input characters are thrown away until
 * a new line is found.
 */
compile(ec)
{
	int bstack[BRSIZE], bcount, blevel, n;
	register int c;
	register char *cp, *lcp;

	bcopy(tempbuf, codebuf);
	bcount = 1;
	blevel = 0;
	cp = &codebuf[0];
	if ((c=getx()) == ec) {
		if (*cp == CSNUL) {
			derror("No saved pattern");
			goto err;
		}
		return (1);
	}
	if (c == '^') {
		*cp++ = CSSOL;
		c = getx();
	}
	while (c != ec) {
		if (c == '\n') {
			goto nwl;
		}
		if (cp > &codebuf[CBSIZE-4])
			goto ovf;
		switch (c) {
		case '*':
			goto syn;
		case '.':
			if ((c=getx()) != '*') {
				*cp++ = CSDOT;
				continue;
			}
			*cp++ = CMDOT;
			c = getx();
			continue;
		case '$':
			if ((c=getx()) != ec) {
				ungetx(c);
				c = '$';
				goto character;
			}
			*cp++ = CSEOL;
			continue;
		case '[':
			lcp = cp;
			if ((c=getx()) == '^')
				*cp++ = CSNCL;
			else {
				ungetx(c);
				*cp++ = CSCCL;
			}
			*cp++ = 0;
			if ((c=getx()) == ']')
				*cp++ = c;
			else
				ungetx(c);
			while ((c=getx()) != ']') {
				if (c == '\n')
					goto nwl;
				if (c!='-' || cp==lcp+2) {
					if (cp >= &codebuf[CBSIZE-4])
						goto ovf;
					*cp++ = c;
					if (sflag && isascii(c) && isallet(c))
						*cp++ = toother(c);
					continue;
				}
				if ((c=getx()) == '\n')
					goto nwl;
				if (c == ']') {
					*cp++ = '-';
					ungetx(c);
					continue;
				}
				if ((n=cp[-1]) > c)
					goto syn;
				while (++n <= c) {
					if (cp >= &codebuf[CBSIZE-4])
						goto ovf;
					*cp++ = n;
					if (sflag && isascii(c) && isallet(c))
						*cp++ = toother(c);
				}
			}
			if ((c=getx()) == '*') {
				(*lcp)++;
				c = getx();
			}
			if ((n=cp-(lcp+2)) > 255) {
				derror("Character class too large");
				goto err;
			}
			*++lcp = n;
			continue;
		case '\\':
			switch (c=getx()) {
			case '\n':
				goto nwl;
			case '(':
				if (bcount > BRSIZE) {
					derror("Too many \\(");
					goto err;
				}
				*cp++ = CSOPR;
				*cp++ = bstack[blevel++] = bcount++;
				c = getx();
				continue;
			case ')':
				if (blevel == 0)
					goto syn;
				*cp++ = CSCPR;
				*cp++ = bstack[--blevel];
				c = getx();
				continue;
			default:
				if (isascii(c) && isdigit(c)) {
					*cp++ = CSBRN;
					*cp++ = c-'0';
					c = getx();
					continue;
				}
			}
		default:
		character:
			if (sflag && isascii(c) && isallet(c)) {
				*cp++ = CSSCC;
				if (isupper(c))
					c = tolower(c);
			} else
				*cp++ = CSCHR;
			*cp++ = c;
			if ((c=getx()) == '*') {
				cp[-2]++;
				c = getx();
			}
		}
	}
	*cp++ = CSNUL;
	return (1);
ovf:
	derror("Code buffer overflow");
	bcopy(codebuf, tempbuf);
	return (0);
nwl:
	ungetx(c);
syn:
	derror("Syntax error");
err:
	bcopy(codebuf, tempbuf);
	return (0);
}

/*
 * Copy BSIZE buffer into dest from source
 */
bcopy(dest, source)
register char *dest;
register char *source;
{
	register int n;

	n = CBSIZE;
	do
		*dest++ = *source++;
	while (--n > 0);
}

/*
 * Return 1 if the compiled expression in `codebuf' matches the line in
 * `linebuf', else 0.
 */
execute(a)
{
	register int i;
	register char *lp, *ep;

	if (getline(a, linebuf) == 0)
		return (0);
	for (i=0; i<1+BRSIZE; i++) {
		brace[i].b_bp = NULL;
		brace[i].b_ep = NULL;
	}
	if (codebuf[0] == CSSOL)
		ep = match(lp=linebuf, &codebuf[1]);
	else {
		ep = NULL;
		lp = linebuf;
		do {
			if (ep=match(lp, codebuf))
				break;
		} while (*lp++);
	}
	if (ep) {
		brace[0].b_bp = lp;
		brace[0].b_ep = ep;
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
			brace[*cp++].b_bp = lp;
			continue;
		case CSCPR:
			brace[*cp++].b_ep = lp;
			continue;
		case CSBRN:
			n = *cp++;
			lcp = cp;
			cp = brace[n].b_bp;
			n = brace[n].b_ep - cp;
			if (n > LBSIZE)
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
