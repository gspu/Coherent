/*
 * db/db7.c
 * A debugger.
 * Miscellaneous routines.
 */

#include "db.h"
#ifndef	NOCANON
#include <canon.h>
#endif

/*
 * Add a string or a FILE * to the input stream.
 */
void
add_input(type, cp) int type; char *cp;
{
	register INP *ip;

	ip = (INP *)nalloc(sizeof(INP), "input stack");
	ip->i_next = inpp;
	inpp = ip;
	ip->i_type = type;
	if (type == ICORE)
		ip->i_u.i_strp = cp;
	else
		ip->i_u.i_filp = (FILE *)cp;
}

#ifndef	NOCANON
/*
 * Canonize the buffer 'bp' of length 'n' according to 'cantype'.
 */
void
canon(bp, n) register char *bp; unsigned int n;
{
	register int i;
	register int t;

	if ((n&1) != 0)
		return;
	for (i=0; i<n; i+=2) {
		t = bp[i];
		bp[i] = bp[i+1];
		bp[i+1] = t;
	}
}
#endif

/*
 * Convert value 'l' in segment 's' to an address.
 * Store the result in 'buf' and return a pointer past the end.
 * Conversion can produce a constant or a symbol with optional displacement.
 */
char *
cvt_addr(buf, s, addr) register char *buf; int s; register ADDR_T addr;
{
	register SYM *sp;
	register ADDR_T d;

	if ((sp = findsym(s, addr)) == (SYM *)NULL)	/* name not found */
		print_const(buf, addr);			/* so print as constant */
	else if ((d = addr - sp->s_addr) == 0)		/* no displacement */
		sprintf(buf, "%s", sp->s_id);
	else						/* with displacement */
		sprintf(buf, "%s+%lu", sp->s_id, (long)d);
	/* FIX_ME Watch out for buf overflow on id printf's above. */
	return strchr(buf, '\0');
}

#ifndef	NOCANON		/* If NOCANON, this is a macro in db.h. */
/*
 * Read 'n' bytes from the current position in segment 'segn'
 * and store it in the buffer 'bp'.
 */
int
getb(segn, bp, n) int segn; char *bp; unsigned int n;
{
	if (getputb(segn, bp, n, 0) == 0)
		return 0;
#ifndef	NOCANON
	if (cantype != 0)
		canon(bp, n);
#endif
#if	0
{
	register int i;
	printf("getb(%d, ..., %d):\n", segn, n);
	for (i = 0; i < n; i++)
		printf("bp = %X\n", (unsigned char)bp[i]);
}
#endif
	return 1;
}
#endif

/*
 * Read 'n' bytes to 'bp' from the file specified by 'fp'.
 */
int
getf(fp, bp, n) register FILE *fp; char *bp; int n;
{
	return fread(bp, n, 1, fp);
}

/*
 * Save the rest of the input line in 'miscbuf'.
 * If line is not given, save the default, 'cp'.
 */
void
getline(cp) register char *cp;
{
	register int c;
	register char *bp;

	bp = miscbuf;
	if ((c = getn()) != '\n') {
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
 * Get the next character from the input stream.
 */
int
getn()
{
	register INP *ip;
	register int c;

	if (ungotc != '\0') {
		c = ungotc;
		ungotc = '\0';
		return c;
	}
	while ((ip = inpp) != (INP *)NULL) {
		switch (ip->i_type) {
		case ICORE:
			if ((c = *ip->i_u.i_strp++) != '\0')
				return c;
			break;
		case IFILE:
			if ((c = getc(ip->i_u.i_filp)) != EOF)
				return c;
			if (ip->i_u.i_filp == stdin) {
				if (testint())
					continue;
				leave();
			}
			fclose(ip->i_u.i_filp);
			break;
		}
		inpp = inpp->i_next;
		nfree(ip);
	}
	return EOF;
}

/*
 * Move 'n' bytes between the buffer 'bp' and the segment 'segn'.
 * The flag 'd' controls the direction.
 */
int
getputb(segn, bp, n, d) int segn; char *bp; unsigned int n; int d;
{
	register MAP *mp;
	register FILE *fp;
	register unsigned int l;
	register unsigned int s;
	register int (*f)();
	ADDR_T a;

	dbprintf(("getputb(s=%d bp=%X n=%d d=%s) add=%X\n", segn, bp, n,(d) ? "put" : "get", add));
	s = n;
	a = add;
	while (s) {
		if ((mp = map_addr(segn, a)) == (MAP *)NULL)
			return 0;
		l = s;
		if (a+l > mp->m_bend)
			l = mp->m_bend-a+1;
		if (mp->m_flag == MAP_CHILD) {
			/* Map to child process. */
			f = (d) ? putp : getp;
			if ((*f)(mp->m_segi, ((ADDR_T)mp->m_offt)+(a-mp->m_base), bp, l) == 0)
				return 0;
		} else {
			/* Map to program or core file. */
			fp = mp->m_flag == MAP_PROG ? lfp : cfp;
			f = (d) ? putf : getf;
			if (fseek(fp, (long)(mp->m_offt+(a-mp->m_base)), SEEK_SET) == -1)
				return 0;
			if ((*f)(fp, bp, l) == 0) {
				clearerr(fp);
				return 0;
			}
		}
		bp += l;
		a += l;
		s -= l;
	}
	add += n;
	return 1;
}

/*
 * Allocate 'n' bytes.
 */
char *
nalloc(n, msg) int n; char *msg;
{
	register char *cp;

	if ((cp = malloc(n)) == NULL)
		panic("No memory for %s", msg);
	return cp;
}

#if	0
/*
 * Free storage allocated by nalloc().
 */
void
nfree(cp) char *cp;
{
	free(cp);
}
#endif

/*
 * Print a fatal error message and die.
 */
/* VARARGS */
void
panic(arg1)
char *arg1;
{
	fprintf(stderr, "%r\n", &arg1);
	exit(1);
}

/*
 * Print a breakpoint error message.
 */
void
printb(addr) ADDR_T addr;
{
	fprintf(stderr, "Breakpoint error at ");
	fprintf(stderr, addr_fmt, addr);
	fprintf(stderr, "\n");
}

/*
 * Print an error message and set lasterr.
 * Return a 0 for the convenience of the caller.
 */
int
printe(msg) register char *msg;
{
	lasterr = msg;
	fprintf(stderr, "%s\n", msg);
	return 0;
}

/*
 * Print an error message.
 * Return a 0 for the convenience of the caller.
 */
/* VARARGS */
int
printr(arg1) char *arg1;
{
	fprintf(stderr, "%r\n", &arg1);
	return 0;
}

#if	0				/* covered by macro in db.h */
/*
 * Formatted print.
 */
/* VARARGS */
void
printx(arg1) char *arg1;
{
	printf("%r", &arg1);
}
#endif

/*
 * Put out address 'addr' in segment 's'.
 */
void
putaddr(s, addr) int s; ADDR_T addr;
{
	register char *sp;

	sp = cvt_addr(miscbuf, s, addr);
	*sp = '\0';
	printx("%s", miscbuf);
}

#ifndef	NOCANON			/* If NOCANON, this is a macro in db.h. */
/*
 * Put 'n' bytes from the buffer 'bp' into segment 'seg'
 * at the current position.
 */
int
putb(segn, bp, n) int segn; char *bp; unsigned int n;
{
#ifndef	NOCANON
	if (cantype != 0)
		canon(bp, n);
#endif
	return getputb(segn, bp, n, 1);
}
#endif

/*
 * Write 'n' bytes from 'bp' to the file specified by 'fp'.
 */
int
putf(fp, bp, n) register FILE *fp; char *bp; int n;
{
	return (fwrite(bp, n, 1, fp) == 1 && fflush(fp)!=EOF);
}

#if	0				/* covered by macro in db.h */
/*
 * Put out a character.
 */
void
putx(c) int c;
{
	putchar(c);
}
#endif

#if	0				/* covered by macro in db.h */
/*
 * Put back a character onto the current input stream.
 */
void
ungetn(c)
{
	ungotc = c;
}
#endif

/* end of db/db7.c */
