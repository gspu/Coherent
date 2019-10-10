/*
 * getf.c
 * Nroff/Troff.
 * Input handling.
 */

#include <ctype.h>
#include "roff.h"

#define	ok_name(name) ((name[0]>0 && name[0]<128) && (name[1]>=0 && name[1]<128))

/*
 * Get a character handling escapes.
 * Flag bit 0: 0 means die if EOF seen, 1 means return EOF.
 * Flag bit 1: 2 means copy mode.
 * If character is escaped but not recognized as a special escape sequence,
 * set escflag and return character unchanged.
 */
getf(flags) int flags;
{
	register REG *rp;
	register int c, n;
	SPECIAL *sp;
	char charbuf[CBFSIZE], name[2];

	escflag = 0;
	for (;;) {
		if ((c = getl(flags&1)) == esc) {
			if ((c = getl(flags&1)) == EOF)
				return c;
			if (c >= ASCSIZE || esctab[c] == ENUL) {
				escflag = 1;
				return c;
			}
			c = esctab[c] & 0377;
		}
		switch (c) {
		case EIGN:
			continue;
		case ECOM:
			while ((c=getl(0)) != '\n')
				;
			return c;
		case EARG:
			c = getf(0);
			if (!isdigit(c)) {
				printe("bad argument reference");
				break;
			}
			if (strp->x1.s_argp[c-='0'])
				adscore(strp->x1.s_argp[c]);
			continue;
		case ESTR:
			if ((c=getf(0)) != '(') {
				name[0] = c;
				name[1] = '\0';
			} else {
				name[0] = getf(0);
				name[1] = getf(0);
			}
			if (!ok_name(name)) {
				printe("illegal string name");
				break;
			} else if ((rp=findreg(name, RTEXT)))
				adstreg(rp);
			continue;
		case ECHR:
			name[0] = getf(0);
			name[1] = getf(0);
			if (!ok_name(name)) {
				printe("illegal special character name");
				break;
			} else if ((sp = spc_find(name)) != NULL)
				adscore(sp->spc_val);
			continue;
		case ENUM:
			n = 0;
			if ((c=getf(0)) == '+') {
				n = 1;
				c = getf(0);
			} else if (c == '-') {
				n = -1;
				c = getf(0);
			}
			if (c != '(') {
				name[0] = c;
				name[1] = '\0';
			} else {
				name[0] = getf(0);
				name[1] = getf(0);
			}
			if (!ok_name(name)) {
				printe("illegal register name");
				break;
			} else if ((rp=findreg(name, RNUMR)) == NULL)
				spcnreg(name);
			else {
				rp->n_reg.r_nval += n*rp->n_reg.r_incr;
				adsnval(rp->n_reg.r_nval, rp->n_reg.r_form);
			}
			continue;
		case EWID:
			if (flags&2)		/* Copy mode */
				return c;
			if (scandel(charbuf, CBFSIZE) == 0)
				adsnval(0, '1');
			else
				adsnval(getwidth(charbuf), '1');
			continue;
		case EBEG:
			if (flags&2)
				return c;
			++bracelevel;
			return c;
		case EEND:
			if (flags&2)
				return c;
			if (bracelevel == 0)
				printe("\\} without matching \\{");
			else
				--bracelevel;
			return c;
		case EHEX:
			n = hexdigit(getf(0)) * 0x10;
			n += hexdigit(getf(0));
			return n;
		default:
			return c;
		}
	}
}

/*
 * Get width of string by processing into temporary environment.
 */
int
getwidth(cp) char *cp;
{
	int n;

	envsave(ENVWIDTH);
	setline();
	ccc = ind = tif = fill = 0;
	adscore(cp);
	strp->x1.s_eoff = 1;
	process();
	wordbreak(DNULL);
	n = nlinsiz;
	envload(ENVWIDTH);
	return n;
}

/*
 * Like scandel(), except the delimiter is optional.
 * Accepts digits with optional sign without delimiter.
 * Currently called only for pointsize changes, e.g. "\s10" or "\s+1".
 */
scanoptdel(cp, n) register char *cp; int n;
{
	register int c, retval;
	static char buf[2];

	c = getf(0);
	if (c != '-' && c != '+' && !isdigit(c)) {
		buf[0] = c;
		adscore(buf);			/* push back c */
		return scandel(cp, n);		/* and use scandel() */
	}
	retval = 1;
	do {
		if (--n == 0) {
			printe("delimiter argument too large");
			retval = 0;
		} else
			*cp++ = c;
		c = getf(0);
	} while (isdigit(c));
	buf[0] = c;
	adscore(buf);				/* push back nondigit */
	return retval;
}

/*
 * Given a buffer pointer, 'bp', and the size of the buffer, 'n',
 * get an argument surrounded by delimiters and store it in the buffer.
 */
scandel(cp, n) register char *cp; int n;
{
	int retval;
	register int c;
	register int endc;

	retval = 1;
	endc = getf(0);
	while ((c=getf(0)) != endc) {
		if (c == '\n') {
			printe("syntax error");
			return 0;
		}
		if (--n == 0) {
			printe("delimiter argument too large");
			retval = 0;
		} else if (n > 0)
			*cp++ = c;
	}
	*cp++ = '\0';
	return retval;
}

/*
 * Get a character from the current input source.
 * If EOF is seen: return EOF if eofflag, die if !eofflag.
 */
getl(eofflag) int eofflag;
{
	register STR *sp;
	int n;
	register int c;
	register char *cp;

	sp = strp;
	if (sp != NULL)
		sp->x1.s_clnc = sp->x1.s_nlnc;
	for (;;) {
		if (sp == NULL) {
			if (eofflag == 0)
				panic("unexpected end of file");
			return EOF;
		}
		switch (sp->x1.s_type) {
		case SSINP:
			if ((c=getc(stdin)) == EOF)
				break;
			if (c == '\0')
				break;
			if (c == '\n') {
				if ((c=getc(stdin)) == '\n')
					c = '\0';
				ungetc(c, stdin);
				c = '\n';
			}
			goto ret;
		case SFILE:
			if ((c=getc(sp->x2.s_fp)) == EOF) {
				fclose(sp->x2.s_fp);
				nfree(sp->x2.s_fname);
				break;
			}
			goto ret;
		case SCORE:
			if ((c = *sp->x3.s_cp++) == '\0') {
				if (sp->x3.s_srel)
					nfree(sp->x3.s_srel);
				break;
			}
			goto ret;
		case SCTEX:
			if (sp->x4.s_disk  &&  sp->x4.s_sp >= sp->x4.s_bufend) {
				nread((long)sp->x4.s_seek,
					(char *)sp->x4.s_bufp);
				sp->x4.s_seek += DBFSIZE;
				sp->x4.s_sp = sp->x4.s_bufp;
			}
			if ((c = *sp->x4.s_sp++) == '\0') {
				if (strnext())
					continue;
				nfree(sp->x4.s_bufp);
				break;
			}
			goto ret;
		case SCDIV:
			if (sp->x4.s_n-- == 0) {
				if (strnext())
					continue;
				nfree(sp->x4.s_bufp);
				break;
			}
			n = sizeof (CODE);
			cp = (char *) &codeval;
			while (n--)
				*cp++ = geth();
			return ECOD;
		}
		if (sp->x4.s_abuf)
			nfree(sp->x4.s_abuf);
		n = sp->x4.s_eoff;
		strp = strp->x4.s_next;
		nfree((char *)sp);
		sp = strp;
		if (n) {
			if (eofflag == 0)
				panic("incomplete macro in trap");
			return EOF;
		}
	}
ret:
	if ((c&=0377) == '\n')
		sp->x4.s_nlnc++;
	return c;
}

/*
 * In a chained macro, set up input stack to point to next element in chain.
 */
strnext()
{
	register STR *sp;
	register MAC *mp;

	sp = strp;
	if ((mp=sp->x4.s_macp) == NULL)
		return 0;
	sp->x4.s_macp = mp->t_div.m_next;
	switch (mp->t_div.m_type) {
	case MTEXT:
		sp->x1.s_type = SCTEX;
		break;
	case MDIVN:
		sp->x4.s_type = SCDIV;
		if ((sp->x4.s_n=mp->t_div.m_size) == 0)
			return strnext();
		break;
	}
	if (mp->t_div.m_core) {
		sp->x4.s_disk = 0;
		sp->x4.s_sp = mp->t_div.m_core;
	} else {
		sp->x4.s_disk = 1;
		sp->x4.s_seek = mp->t_div.m_seek/DBFSIZE * DBFSIZE;
		sp->x4.s_sp = &(sp->x4.s_bufp[mp->t_div.m_seek%DBFSIZE]);
		nread((long)sp->x4.s_seek,(char *)sp->x4.s_bufp);
		sp->x4.s_seek += DBFSIZE;
	}
	return 1;
}

/*
 * Get a single character assuming the stack has been set up
 * for chained I/O.
 */
geth()
{
	if (strp->x4.s_disk  &&  strp->x4.s_sp >= strp->x4.s_bufend) {
		nread((long)strp->x4.s_seek,(char *)strp->x4.s_bufp);
		strp->x4.s_seek += DBFSIZE;
		strp->x4.s_sp = strp->x4.s_bufp;
	}
	return (*strp->x4.s_sp++);
}

/*
 * Push down input stack and add the given file on top.
 */
adsfile(file)
char *file;
{
	register STR *sp;
	register FILE *fp;

	if ((fp = fopen(file, "r")) == NULL) {
		printe("cannot open file \"%s\"", file);
		return 0;
	}
	sp = allstr(SFILE);
	sp->x2.s_fname = duplstr(file);
	sp->x2.s_fp = fp;
	return 1;
}

/*
 * Push down input stack and add the given FILE.
 */
adsunit(fp) FILE *fp;
{
	register STR *sp;

	sp = allstr(SFILE);
	sp->x2.s_fname = duplstr("<stdin>");
	sp->x2.s_fp = fp;
}

/*
 * Push down input stack and add the given register.
 */
adstreg(rp)
register REG *rp;
{
	register STR *sp;

	if (rp->t_reg.r_macd.t_div.m_type == MREQS)
		return 1;	/* return had been random, 1 is guess by cef */
	sp = allstr(0);
	sp->x4.s_bufp = nalloc(DBFSIZE);
	sp->x4.s_bufend = &sp->x4.s_bufp[DBFSIZE];
	sp->x4.s_macp = &rp->t_reg.r_macd;
	if (strnext() == 0) {
		strp = strp->x4.s_next;
		nfree(sp->x4.s_bufp);
		nfree((char *)sp);
		return 0;
	}
	return 1;
}

/*
 * Given the name of a special number register, expand it and
 * add it onto the input stack.
 */
spcnreg(name)
char name[2];
{
	register STR *sp;
	register int n;
	register char *cp;

	n = 0;
	if (name[0] == '.') {
		switch (name[1]) {
		case '$':
			if ((n=strp->x1.s_argc) > 0)
				--n;
			break;
		case 'A':	n = A_reg;			break;
		case 'F':			/* Filename, not in V7. */
			for (sp=strp; sp; sp=sp->x1.s_next) {
				if (sp->x1.s_type == SFILE) {
					adscore(sp->x2.s_fname);
					return;
				}
			}
			return;
		case 'H':	n = unit(SMHRES, SDHRES);	break;
		/* case 'L': TO_DO */
		/* case 'P': TO_DO */
		case 'T':	n = T_reg;			break;
		case 'V':	n = unit(SMVRES, SDVRES);	break;
		case 'a':	n = a_reg;			break;
		case 'c':
			n = 0;
			for (sp=strp; sp; sp=sp->x1.s_next) {
				if (sp->x1.s_type == SFILE) {
					n = sp->x1.s_clnc;
					break;
				}
			}
			break;
		case 'd':	n = cdivp->d_rpos;		break;
		case 'f':	n = 0;				break;
		case 'h':	n = cdivp->d_maxh;		break;
		case 'i':	n = ind;			break;
		/* case 'j': TO_DO */
		/* case 'k': TO_DO */
		case 'l':	n = lln;			break;
		case 'n':	n = n_reg;			break;
		case 'o':	n = pof;			break;
		case 'p':	n = pgl;			break;
		case 's':	n = unit(psz*SDPOIN, SMPOIN);	break;
		case 't':
			n = cdivp->d_ctpp ? cdivp->d_ctpp->t_apos : pgl;
			if (n > pgl)
				n = pgl;
			n -= cdivp->d_rpos;
			break;
		case 'u':	n = fill;			break;
		case 'v':	n = vls;			break;
		case 'w':	n = 0;				break;
		case 'x':	n = 0;				break;
		case 'y':	n = 0;				break;
		case 'z':
			cp = nalloc(3);
			cp[0] = cdivp->d_name[0];
			cp[1] = cdivp->d_name[1];
			cp[2] = '\0';
			adscore(cp);
			strp->x3.s_srel = cp;
			return;
		}
	}
	adsnval(n, '1');
}

/*
 * Given a value and a format, expand out the value to the
 * particular format and add it onto the top of the stack.
 */
adsnval(val, f) int val, f;
{
	register ROM *op;
	int w;
	register int n;
	char charbuf[CBFSIZE], *str, *cp2;
	register char *cp1;

	n = val;
	if (n <= 0) {
		if ((n = -n) <= 0) {
			n = 0;
			if (!isdigit(f))
				f = '0';
		}
	}
	cp1 = charbuf;
	switch (f) {
	case 'a':
	case 'A':
		while (n) {
			n--;
			*cp1++ = 'a' + n%26;
			n /= 26;
		}
		break;
	case 'i':
	case 'I':
		for (cp2="ivxlcdm"; *cp2!='m'; cp2+=2) {
			op = &romtab[n%10];
			n /= 10;
			while (op != &romtab[0]) {
				*cp1++ = cp2[op->o_digit];
				op = &romtab[op->o_state];
			}
		}
		if (cp1+n >= &charbuf[CBFSIZE]) {
			*cp1++ = '*';
			break;
		}
		while (n--)
			*cp1++ = 'm';
		break;
	default:
		w = f - '0';
		while (n) {
			--w;
			*cp1++ = n%10 + '0';
			n /= 10;
		}
		while (w-- > 0)
			*cp1++ = '0';
		if (val < 0)
			*cp1++ = '-';
	}
	cp2 = str = nalloc(cp1-charbuf + 1);
	if (isupper(f)) {
		while (cp1 > charbuf)
			if (islower(*--cp1))
				*cp2++ = toupper(*cp1);
			else    *cp2++ = *cp1;
	} else {
		while (cp1 > charbuf)
			*cp2++ = *--cp1;
	}
	*cp2 = '\0';
	adscore(str);
	strp->x3.s_srel = str;
}

/*
 * Divert input to the given string.
 */
adscore(cp) char *cp;
{
	register STR *sp;

	sp = allstr(SCORE);
	sp->x3.s_cp = cp;
	sp->x3.s_srel = NULL;
}

/*
 * Allocate an entry and add it to the input stack.
 */
STR *
allstr(type)
{
	register STR *sp;
	register int i;

	sp = (STR *) nalloc(sizeof *sp);
	sp->x1.s_next = strp;
	strp = sp;
	sp->x1.s_type = type;
	sp->x1.s_eoff = 0;
	sp->x1.s_clnc = 1;
	sp->x1.s_nlnc = 1;
	sp->x1.s_argc = 0;
	for (i = 0; i < ARGSIZE; i++)
		sp->x1.s_argp[i] = "";
	sp->x1.s_abuf = NULL;
	return sp;
}

hexdigit(c) register int c;
{
	if (isdigit(c))
		return c - '0';
	else if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	else if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	printe("illegal hex digit");
	return 0;
}

		
/* end of getf.c */
