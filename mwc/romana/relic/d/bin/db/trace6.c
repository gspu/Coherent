/*
 * A debugger.
 * Miscellaneous routines.
 */
#include <stdio.h>
#include <ctype.h>
#include <canon.h>
#include "trace.h"
#if NOUT
#include <n.out.h>
#else
#include <l.out.h>
#endif

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
 */
addstrp(cp)
char *cp;
{
	register INP *ip;

	ip = nalloc(sizeof (INP));
	ip->i_next = inpp;
	ip->i_type = ICORE;
	ip->i_strp = cp;
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
	ip->i_next = inpp;
	ip->i_type = IFILE;
	ip->i_filp = fp;
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
		switch (ip->i_type) {
		case ICORE:
			if ((c=*ip->i_strp++) != '\0')
				return (c);
			break;
		case IFILE:
			if ((c=getc(ip->i_filp)) != EOF)
				return (c);
			if (ip->i_filp == stdin) {
				if (testint())
					continue;
				leave();
			}
			fclose(ip->i_filp);
			break;
		}
		inpp = inpp->i_next;
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
	fsize_t a;

	s = n;
	a = add;
	while (s) {
		if ((mp=mapaddr(segn, a)) == NULL)
			return (0);
		l = s;
		if (a+l > mp->m_bend)
			l = mp->m_bend-a+1;
		f = d ? mp->m_putf : mp->m_getf;
		if( (*f)( mp->m_segi, mp->m_offt+
			  ( fflag ? a - mp->m_base : size( a - mp->m_base))
			  , bp, l) == 0)
			return (0);
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
fsize_t a;
{
	register MAP *mp;

	for (mp=segmapl[n]; mp != NULL; mp=mp->m_next)
		if (mp->m_base<=a && a<mp->m_bend)
			break;
	return (mp);
}

/*
 * Read a symbol and return it's value in the given `VAL' structure.
 */
getsval(vp)
VAL *vp;
{
	register int n;
	register int c;
	struct ldsym lds;

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
register struct ldsym *ldp;
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
	struct ldsym ldsym;

/* Bug in 8086 compiler
	if (t<0 || valname(s, (tvaddr)l, &ldsym)==0 || (d=l-ldsym.ls_addr)>t)
*/
	if (t<0 || valname(s, (tvaddr)l, &ldsym)==0 ||
	    (d=(unsigned)l-ldsym.ls_addr)>t)
#if FORZ8001
		conaddnum( sp, l);
#else
		concons(sp, l);
#endif
	else {
		sprintf(sp, "%.*s", NCPLN, ldsym.ls_id);
		while (*sp)
			sp++;
		if (d != 0) {
			*sp++ = '+';
			concons( sp, (long)d);
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
struct ldsym *ldp;
{
	register int u;
	register fsize_t n;
	register SYM *sp;
	register int r;
	register int h;
	register fsize_t b;
	struct ldsym lds;
	struct ldsym ldu;

	u = 0;
	h = hash(ldp);
	for (n=snsym, sp=ssymp, b=sbase; n--; sp++, b+=sizeof(lds)) {
		if (ssymp!=NULL && sp->s_hash!=h)
			continue;
		fseek(sfp, (long)b, 0);
		if (fread(&lds, sizeof(lds), 1, sfp) != 1)
			return (0);
#if NOUT
		canshort( lds.ls_type);
		canlong( lds.ls_addr);
#else
		canint(lds.ls_type);
		canvaddr(lds.ls_addr);
#endif
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
struct ldsym *ldp1;
struct ldsym *ldp2;
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

/*
 * Given a value, find the closest symbol below.
 * Searches in segment s; if s = -1 looks in all segments.
 */
valname(s, m, ldp)
tvaddr m;
struct ldsym *ldp;
{
	register int n;
	register tvaddr a;
	register SYM *sp;
	register int t;
	register int n1;
	register tvaddr a1;
	struct ldsym lds;

	strncpy(ldp->ls_id, "", NCPLN);
	if (sfp == NULL)
		return (0);
	a1 = 0;
	n1 = -1;
	if (ssymp == NULL)
		fseek(sfp, (long)sbase, 0);
	for (n=0, sp=ssymp; n<snsym; n++, sp++) {
		if (ssymp != NULL) {
			a = sp->s_sval;
			t = sp->s_type;
		} else {
			if (fread(&lds, sizeof(lds), 1, sfp) != 1)
				return (0);
#if NOUT
			canshort( lds.ls_type);
			canlong( lds.ls_addr);
#else
			canint(lds.ls_type);
			canvaddr(lds.ls_addr);
#endif
			a = lds.ls_addr;
			t = lds.ls_type;
		}
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
		if (a <= m) {
			if (a1==0 || a>a1) {
				a1 = a;
				n1 = n;
			}
		}
	}
	if (n1 >= 0) {
		fseek(sfp, (long)sbase + n1*sizeof(struct ldsym), 0);
		if (fread(ldp, sizeof(*ldp), 1, sfp) != 1)
			return (0);
#if NOUT
		canshort( lds.ls_type);
		canlong( lds.ls_addr);
#else
		canint(ldp->ls_type);
		canvaddr(ldp->ls_addr);
#endif
		return (1);
	}
	return (0);
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

	fp = f ? cfp : lfp;
	fseek(fp, (long)seek, 0);
	return (fwrite(bp, n, 1, fp) && fflush(fp)!=EOF);
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
vaddr_t a;
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
	fprintf(stderr, "?\n");
}

/*
 * Print formatted.
 */
printx(a1)
char *a1;
{
	printf("%r", &a1);
}

putaddnum( a)
long	a;
{
	char	buf[40];

	conaddnum( buf, a);
	printx( "%s", buf);
}

putnum( a)
long( a);
{
	char	buf[20];

	concons( buf, a);
	printx( "%s", buf);
}

char *
concons( sp, a)
char *sp;
long a;
{
	register char *cp = sp;

	switch( dbase) {
		case 8: sprintf( cp, "%lo", a); break;
		case 10: sprintf( cp, "%ld", a); break;
		case 16: sprintf( cp, "%lx", a); break;
	}
	while( *++cp) ;
	return( cp);
}
