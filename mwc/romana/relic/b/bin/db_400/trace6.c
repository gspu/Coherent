/*
 *	trace6.c
 *
 *	The information contained herein is a trade secret of Mark Williams
 *	Company, and  is confidential information.  It is provided  under a
 *	license agreement,  and may be  copied or disclosed  only under the
 *	terms of  that agreement.  Any  reproduction or disclosure  of this
 *	material without the express written authorization of Mark Williams
 *	Company or persuant to the license agreement is unlawful.
 *
 *	COHERENT Version 2.3.35
 *	Copyright (c) 1982, 1983, 1984.
 *	An unpublished work by Mark Williams Company, Chicago.
 *	All rights reserved.
 *
 *	Miscellaneous routines.
 */
#include <stdio.h>
#include <ctype.h>
#include <canon.h>
#include <l.out.h>
#include "trace.h"

#define MYPRINTF(x)	printf(x)
#define DEBUG		0

unsigned short	getMagic();
caddr_t		initn();
off_t		initb();
off_t		incb();
off_t		incb();
off_t		getseekoff();

/*
 * Save the rest of the input line in `miscbuf'.  If line is
 * not given, save the default, `cp'.
 */
getline(cp)
register char *cp;
{
	register int c;
	register char *bp;

	bp = miscbuf;
	if ((c=getn()) != '\n') {
		while (c != '\n') {
			if (c == '\\')
				if ((c=getn())!='\n' && c!='\\')
					*bp++ = '\\';
			*bp++ = c;
			c = getn();
		}
	} else {
		while (*cp != '\0')
			*bp++ = *cp++;
	}
	*bp++ = '\n';
	*bp = '\0';
}

/*
 * Add a string onto the input stream.
 * Make a node for string "cp" and insert it at the front of "inpp".
 */
addstrp(cp)
char *cp;
{
	register INP *ip;

	ip = nalloc(sizeof (INP));
	ip->i_st2.i_next = inpp;
	ip->i_st2.i_type = ICORE;
	ip->i_st2.i_strp = cp;
	inpp = ip;
}

/*
 * Add a file pointer onto the input stack.
 */
addfilp(fp)
FILE *fp;
{
	register INP *ip;

	ip = nalloc(sizeof (INP));
	ip->i_st1.i_next = inpp;
	ip->i_st1.i_type = IFILE;
	ip->i_st1.i_filp = fp;
	inpp = ip;
}

/*
 * Get the next character from the input stream.
 */
getn()
{
	register INP *ip;
	register int c;

	if (lastc != '\0') {
		c = lastc;
		lastc = '\0';
		return (c);
	}
	while ((ip=inpp) != NULL) {
		switch (ip->i_st1.i_type) {
		case ICORE:
			if ((c=*ip->i_st2.i_strp++) != '\0')
				return (c);
			break;
		case IFILE:
			if ((c=getc(ip->i_st1.i_filp)) != EOF)
				return (c);
			if (ip->i_st1.i_filp == stdin) {
				if (testint())
					continue;
				leave();
			}
			fclose(ip->i_st1.i_filp);
			break;
		}
		inpp = inpp->i_st1.i_next;
		nfree(ip);
	}
	return (EOF);
}

/*
 * Put back a character onto the current input stream.
 */
ungetn(c)
{
	lastc = c;
}

/*
 * Put out an address.  `t' is the tolerance allowed.
 */
putaddr(s, l, t)
long l;
{
	register char *sp;

	sp = conaddr(miscbuf, s, l, t);
	*sp = '\0';
	printx("%s", miscbuf);
}

/*
 * Read `n' bytes from the current position in segment `seg'
 * and store it in the buffer `bp'.
 */
getb(segn, bp, n)
char *bp;
unsigned n;
{
	if (getputb(segn, bp, n, 0) == 0)
		return (0);
	canon(bp, n);

#if DEBUG
{
	int i;
	for (i=0; i<n; i++) printf("bp = %x\n", (unsigned char)bp[i]);
}
#endif
	return (1);
}

/*
 * Put `n' bytes from the buffer `bp' into segment `seg'.
 */
putb(segn, bp, n)
char *bp;
unsigned n;
{
	canon(bp, n);
	return (getputb(segn, bp, n, 1));
}

/*
 * Canonize the buffer `bp' of length `n' according to `cantype'.
 */
canon(bp, n)
register char *bp;
unsigned n;
{
	register int i;
	register int t;

	if (cantype == 1) {
		if ((n&1) != 0)
			return;
		for (i=0; i<n; i+=2) {
			t = bp[i];
			bp[i] = bp[i+1];
			bp[i+1] = t;
		}
	}
}

/*
 * Move `n' bytes between the buffer `bp' and the segment `segn'.
 * The flag `d' controls the direction.
 */
getputb(segn, bp, n, d)
char *bp;
unsigned n;
{
	register MAP *mp;
	register unsigned l;
	register unsigned s;
	register int (*f)();
	off_t a;

	s = n;
	a = add;
	while (s) {
		if ((mp=mapaddr(segn, a)) == NULL) {
			return 0;
		}
		l = s;
		if (a+l > mp->m_bend)
			l = mp->m_bend-a+1;
		f = d ? mp->m_putf : mp->m_getf;
		if ((*f)(mp->m_segi, mp->m_offt+(a-mp->m_base), bp, l) == 0) {
			return 0;
		}
		bp += l;
		a += l;
		s -= l;
	}
	add += n;
	return (1);
}

/*
 * Given a segment number and an address, return the segment map
 * in which the address falls.
 */
MAP *
mapaddr(n, a)
off_t a;
{
	register MAP *mp;

	for (mp=segmapl[n]; mp; mp=mp->m_next) {
		if (mp->m_base<=a && a<mp->m_bend)
			break;
	}
	return (mp);
}

/*
 * Read a symbol and return its value in the given `VAL' structure.
 */
getsval(vp)
VAL *vp;
{
	register int n;
	register int c;
	struct LDSYM lds;

	n = 0;
	c = getn();
	while (isascii(c) && (isalnum(c)||c=='_')) {
		if (n < NCPLN)
			lds.ls_id[n++] = c;
		c = getn();
	}
	ungetn(c);
	while (n < NCPLN)
		lds.ls_id[n++] = '\0';
	if (specsym(&lds)==0 && regaddr(&lds)==0 && nameval(&lds)==0) {
		printe("Symbol not found");
		return (0);
	}
	switch (lds.ls_type&LR_SEG) {
	case L_SHRI:
	case L_PRVI:
	case L_BSSI:
		vp->v_segn = 1;
		vp->v_flag = VLVAL;
		break;
	case L_SHRD:
	case L_PRVD:
	case L_BSSD:
	case L_ABS:
		vp->v_segn = 0;
		vp->v_flag = VLVAL;
		break;

	case L_REF:
		printe("Symbol not defined");
		return (0);
	case L_REG:
		vp->v_segn = 2;
		vp->v_flag = VLVAL;
		break;
	}
	vp->v_nval = lds.ls_addr;
	return (1);
}

/*
 * See if the given symbol is a special symbol.
 */
specsym(ldp)
register struct LDSYM *ldp;
{
	if (ldp->ls_id[1] != '\0')
		return (0);
	switch (ldp->ls_id[0]) {
	case 'd':
		ldp->ls_type = L_SHRD;
		break;
	case 'i':
		ldp->ls_type = L_SHRI;
		break;
	case 'u':
		ldp->ls_type = L_REG;
		break;
	default:
		return (0);
	}
	ldp->ls_addr = 0;
	return (1);
}

/*
 * Convert a value to an address.  `t' is the tolerance
 * allowed.
 */
char *
conaddr(sp, s, l, t)
register char *sp;
unsigned long l;
{
/* Bug in PDP11 compiler
	register unsigned d;
*/
	unsigned d;
	struct LDSYM ldsym;


/* Bug in 8086 compiler
	if (t<0 || valname(s, l, &ldsym)==0 || (d=l-ldsym.ls_addr)>t)
*/
	if (t<0 || valname(s, l, &ldsym)==0 ||
	    (d=(unsigned)l-ldsym.ls_addr)>t)
		concons(sp, l);
	else {
		sprintf(sp, "%.*s", NCPLN, ldsym.ls_id);
		while (*sp)
			sp++;
		if (d != 0) {
			*sp++ = '+';
			sprintf(sp, "%u", d);
		}
	}
	while (*sp)
		sp++;
	return (sp);
}

/*
 * Find the value of a symbol.
 */
nameval(ldp)
struct LDSYM *ldp;
{
	register int u;
	register caddr_t n;
	register SYM *sp;
	register int r;
	register int h;
	register off_t b;
	struct LDSYM lds;
	struct LDSYM ldu;
	
	u = 0;
	h = hash(ldp);
	evalhdrinfo(lfp);
	for (n=initn(), sp=ssymp, b=initb(); n--
				; sp++, b=incb(b, ((caddr_t)sngblsym-n))) {
		if (ssymp!=NULL && sp->s_hash!=h) {
			continue;
		}

		if (hdrinfo.magic == L_MAGIC) {
			if (!rdldSym(sfp, (long)b, &lds))
				return(0);
		}
		else 
			if (!rdcoff_to_loutSym(sfp, (long)b, &lds))
				return(0);

		if ((r=namecmp(ldp, &lds)) > 0) {
			*ldp = lds;
			return (1);
		}
		if (r < 0) {
			ldu = lds;
			u = 1;
		}
	}
	if (u) {
		*ldp = ldu;
		return (1);
	}
	return (0);
}

/*
 * Compare the names of two symbol table entries.  If the compare
 * fails and the name of the first entry doesn't already end with
 * an '_', the compare is effectively tried again after appending
 * an '_', to the end of the first symbol name.
 */
namecmp(ldp1, ldp2)
struct LDSYM *ldp1;
struct LDSYM *ldp2;
{
	register char *cp1;
	register char *cp2;
	register int n;

	n = CCSSIZE;
	cp1 = ldp1->ls_id;
	cp2 = ldp2->ls_id;
	do {
		if (*cp1 != *cp2++) {
			if (*cp2!='\0' || *--cp2!='_')
				return (0);
			if (*cp1!='\0' || *--cp1=='_')
				return (0);
			return (-1);
		}
		if (*cp1++ == '\0')
			return (1);
	} while (--n);
	return (1);
}

#define	NEW	1

/*
 * Given a value, find the closest symbol below.
 */
valname(s, m, ldp)
off_t m;
struct LDSYM *ldp;
{
	register int	n;
	register off_t	a;
	register SYM	*sp;
	register int	t;
	register int	n1;
	register off_t	a1;
	struct LDSYM	lds;
	off_t		b, seekoff;

	strncpy(ldp->ls_id, "", NCPLN);
	if (sfp == NULL)
		return (0);
	a1 = 0;
	n1 = -1;
	if (ssymp == NULL)
		fseek(sfp, (long)sbase, 0);
	hdrinfo.magic = getMagic(lfp);
	for (n=0, sp=ssymp, b=initb(); n<initn()
				; n++, sp++, b=incb(b, ((caddr_t)n))) {
		if (ssymp != NULL) {
			a = sp->s_sval;
			t = sp->s_type;
		}
		else {
			if (hdrinfo.magic == L_MAGIC) {
				if (!rdldSym(sfp, (long)b, &lds))
					return(0);
			}
			else { 
				if (!rdcoff_to_loutSym(sfp, (long)b, &lds))
					return(0);
			}
			a = (off_t)lds.ls_addr;
			t = lds.ls_type;
		}
		if (hdrinfo.magic == L_MAGIC) {
			switch (t&LR_SEG) {
			case L_SHRI:
			case L_PRVI:
			case L_BSSI:
				if (s == 0)
					continue;
				break;
			case L_SHRD:
			case L_PRVD:
			case L_BSSD:
				if (s == 1)
					continue;
				break;
			default:
				continue;
			}
		}
		if ( ((unsigned long)a <= (unsigned long)m) ) {
			if (a1==0 || (unsigned long)a > (unsigned long)a1) {
				a1 = a;
				n1 = n;
			}
		}
	}

	if (n1 >= 0) {
		seekoff = sbase + getseekoff(n1);
		if (hdrinfo.magic == L_MAGIC) {
			if (!rdldSym(sfp, seekoff, ldp))
				return(0);
		}
		else {
			if (!rdcoff_to_loutSym(sfp, seekoff, ldp))
				return(0);
		}
		return (1);
	}
	return (0);
}

rdldSym(fp, seekoff, LDSP)
FILE		*fp;
long		seekoff;
struct	LDSYM	*LDSP;
{
	register int	i;
	struct	ldsym	lds;

	fseek(fp, seekoff, 0);
	if (fread(&lds, sizeof(struct ldsym), 1, fp) != 1)
		return (0);
	canint(lds.ls_type);
	canvaddr(lds.ls_addr);
	for (i=0; i<NCPLN; i++)
		LDSP->ls_id[i] = lds.ls_id[i];
	LDSP->ls_type = lds.ls_type;
	LDSP->ls_addr = (long)lds.ls_addr;
	return(1);
}

/*
 * Read `n' bytes from the file specified by `f' starting at
 * address `seek'.
 */
getf(f, seek, bp, n)
long seek;
char *bp;
{
	register FILE *fp;

	fp = f ? cfp : lfp;
	fseek(fp, (long)seek, 0);
	return (fread(bp, n, 1, fp));
}

/*
 * Write `n' bytes into the files specified by `f' starting at
 * address `seek'.
 */
putf(f, seek, bp, n)
long seek;
char *bp;
{
	register FILE *fp;
	int ret = 1;

	fp = f ? cfp : lfp;
	fseek(fp, (long)seek, 0);
	if (fwrite(bp, n, 1, fp) != 1) {
		ret = 0;
	}
	if (ret) {
		if (fflush(fp)!=EOF) {
			ret = 0;
		}
	}
	return ret;
}

/*
 * Allocate `n' bytes.
 */
char *
nalloc(n)
{
	register char *cp;

	if ((cp=malloc(n)) == NULL)
		panic("No memory");
	return (cp);
}

/*
 * Free up storage.
 */
nfree(cp)
char *cp;
{
	free(cp);
}

/*
 * Print out an error message and exit.
 */
panic(a1)
char *a1;
{
	fprintf(stderr, "%r", &a1);
	putc('\n', stderr);
	exit(1);
}

/*
 * Print out a breakpoint error message.
 */
printb(a)
caddr_t a;
{
	fprintf(stderr, "Breakpoint error at ");
	fprintf(stderr, DAFMT, (long)a);
	putc('\n', stderr);
}

/*
 * Print out a fatal error message.
 */
printr(a1)
char *a1;
{
	fprintf(stderr, "%r", &a1);
	putc('\n', stderr);
}

/*
 * Print out an error message.
 */
printe(sp)
char *sp;
{
	errrstr = sp;
	fprintf(stderr, "%s?\n", errrstr);
}

/*
 * Print formatted.
 */
printx(a1)
char *a1;
{
	printf("%r", &a1);
}

/*
 * Put out a character.
 */
putx(c)
{
	putchar(c);
}


/******************************************************************************
 *
 * Note some #defines in these include files interferes with
 * items in preceeding include files. Hence the strange
 * program order.
 *
 ******************************************************************************/

#include <coff.h>

static	FILEHDR	coffh;

evalhdrinfo(fp)
FILE	*fp;
{
	hdrinfo.magic = getMagic(fp);
	if (hdrinfo.magic == C_386_MAGIC)
		hdrinfo.defsegatt = DSA32;
	else
		hdrinfo.defsegatt = DSA16;
}

unsigned short
getMagic(fp)
FILE	*fp;
{
	fseek(fp,0L,0);
	if ( fread(&coffh, sizeof(FILEHDR), 1, fp) != 1 )
		panic("Cannot read object file");
	if (coffh.f_magic == C_386_MAGIC)
		return(C_386_MAGIC);
	else {
		canint(coffh.f_magic);
		if ( coffh.f_magic == L_MAGIC )
			return(L_MAGIC);
		else
			panic("Bad object file");
	}
	return(0);
}

rdcoff_to_loutSym(fp, seekoff, ldsp)
FILE		*fp;
long		seekoff;
struct	LDSYM	*ldsp;
{
	SYMENT	coffsym;
	char	*str_tabp;
	SCNHDR	*scnhp;

	str_tabp = (char *)read_str_tab(fp, snsym, sbase);

	fseek(fp, seekoff, 0);
	if (fread(&coffsym, sizeof(SYMENT), 1, fp) != 1) {
		printf("Unable to read coff symbols\n");
		return(0);
	}

	scnhp = (SCNHDR *)read_scns(fp, coffh.f_nscns
				, sizeof(FILEHDR)+coffh.f_opthdr);
	if (scnhp == NULL) {
		printf("Unable to read section headers\n");
		return(0);
	}

	coff_to_lout(&coffsym, ldsp, str_tabp, scnhp);

	nfree(str_tabp);
	nfree(scnhp);
	return(1);
}


caddr_t
initn()
{
	
	return(hdrinfo.magic == C_386_MAGIC ? (caddr_t)sngblsym : (caddr_t)snsym);
}

off_t
initb()
{
	off_t	b;

	b = (hdrinfo.magic == C_386_MAGIC 
		? ( sbase+(gblsymMap[0]*(off_t)sizeof(SYMENT)) )
		  : sbase);
	return(b);
}

off_t
incb(b, i)
off_t	b;
int i;
{
	b += ( hdrinfo.magic == C_386_MAGIC 
		? ( (gblsymMap[i] - gblsymMap[i-1L]) * sizeof(SYMENT) )
		  : sizeof(struct ldsym) );
	return(b);
}

off_t
getseekoff(n1)
{
	off_t	b;

	b = ( hdrinfo.magic == C_386_MAGIC 
		? ( (gblsymMap[n1]) * sizeof(SYMENT) )
		  : (n1 * sizeof(struct ldsym)) );
	return(b);
}

/* end of trace6.c */
