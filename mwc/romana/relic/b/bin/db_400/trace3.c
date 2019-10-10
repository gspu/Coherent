/* $Header: /src386/bin/db/RCS/trace3.c,v 1.2 93/03/11 07:46:06 bin Exp Locker: bin $
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
 */
/*
 * A debugger.
 * Display.
 *
 * $Log:	trace3.c,v $
 * Revision 1.2  93/03/11  07:46:06  bin
 * Hal: post NDP update that at least
 * can st breakpoints.
 * 
 * Revision 1.1  92/06/10  14:36:59  bin
 * Initial revision
 * 
 * Revision 1.1	88/10/17  04:05:31 	src
 * Initial revision
 * 
 *
 * Revision: 386 version 92/05/01 
 * Bernard Wald, Wald Software Consulting, Germany
 *
 */
#include <stdio.h>
#include <ctype.h>
#include <l.out.h>
#include "trace.h"

/*
 * Execute a command string.
 */
execute(cp)
char *cp;
{
	register INP *ip;

	/* Delete list starting at "inpp" */
	while (ip=inpp) {
		inpp = ip->i_st1.i_next;
		nfree(ip);
	}

	/* muck with some globals */
	modsize = sizeof(int);
	sincmod = SNULL;
	dot = getpc();
	lastc = '\0';

	/* put string "cp" in first node at "inpp" list */
	if (cp) {
		addstrp(cp);
		request();
	}

	/* print if interrupt occurred - mess with more globals */
	testint();
}

/*
 * Parse requests and execute them.
 */
request(arg)
{
	register int c;
	register char *cp;
	register unsigned segn;
	register long l;
	register long d;
	VAL val[VALSIZE];

	while ((c=getn()) != EOF) {
		if (c == '!') {
			syscall();
			continue;
		}
		if (c == '?') {
			if ((c=getn()) != '\n') {
				printe("Syntax error");
				goto next;
			}
			if (errrstr == NULL)
				printx("No error\n");
			else
				printx("%s\n", errrstr);
			continue;
		}
		ungetn(c);
		if (explist(val) == 0)
			goto next;
		switch (c=getn()) {
		case ':':
			sincmod = SNULL;
			if (command(val) == 0)
				return;
			continue;
		case '\n':
			if (nvalue(&val[0]) && sincmod!=SNULL) {
				if (excflag == 0) {
					sincmod = SNULL;
					printe("Cannot single step");
					continue;
				}
				sinmode = sincmod;
				sindecr = rvalue(&val[1], (long)1);
				return;
			}
		case '=':
			ungetn(c);
		case '?':
			sincmod = SNULL;
			segn = vsegno(&val[0], cseg);
			if ((c=getn()) == '=') {
				l = lvalue(&val[0], (long)dot);
				if ((c=getn()) == '\n') {
					printx(DAFMT, l);
					printx("\n");
					continue;
				}
				ungetn(c);
				if (setdata(segn, l) == 0)
					break;
				continue;
			}
			if (c != '\n') {
				cp = &segform[segn][0];
				while (c != '\n') {
					*cp++ = c;
					c = getn();
				}
				*cp++ = '\0';
			}
			d = dot;
			l = lad;
			dot = lvalue(&val[0], (long)lad);
			dispfmt(segn, segform[segn],
				(int)rvalue(&val[1], (long)1));
			if (segn != 2)
				cseg = segn;
			else {
				dot = d;
				lad = l;
			}
			continue;
		default:
			sincmod = SNULL;
			printe("Syntax error");
			break;
		}
	next:
		while ((c=getn()) != '\n')
			;
	}
}

/*
 * Change the value of a location.
 */
setdata(segn, a)
long a;
{
	register char *cp;
	register int n;
	register int c;
	char b[1];
	char l3[3];
	int i[1];
	long l[1];
	VAL val[VALSIZE];

	if (explist(val) == 0)
		return (0);
	if ((c=getn()) != '\n')
		return (0);
	for (n=0; n<VALSIZE; n++, a+=modsize) {
		if (nvalue(&val[n]))
			continue;
		l[0] = rvalue(&val[n], (long)0);
		switch (modsize) {
		case (sizeof(char)):
			b[0] = l[0];
			cp = b;
			break;
		case (sizeof(short)):
			i[0] = l[0];
			cp = i;
			break;
		case (sizeof(l3)):
			ltol3(l3, l, 1);
			cp = l3;
			break;
		case (sizeof(long)):
			cp = l;
			break;
		default:
			printe("Bad change type");
			return (1);
		}
		add = a;
		if (putb(segn, cp, modsize) == 0) {
			printe("Cannot change value");
			return (1);
		}
	}
	if (segn == 2)
		setregs();
	return (1);
}

/*
 * Send a command to the shell.
 */
syscall()
{
	register int c;
	register char *cp;

	cp = miscbuf;
	while ((c=getn()) != '\n')
		*cp++ = c;
	*cp++ = '\0';
	system(miscbuf);
	testint();
	printf("!\n");
}

/*
 * Print out data according to a format string.  `n' is the number
 * of times this is done.  Segment number is 's'.
 */
dispfmt(s, fs, n)
char *fs;
{
	register char *sp1;
	register char *sp2;
	register int c;
	register int r;
	register int t1;
	register int t2;
	register char *sp3;
	register char *fp;
	register long nad;
	register long pad;
	register long uad;

	r = 1;
	t2 = '\0';
	add = dot;
	nad = dot;
	pad = dot;
	uad = dot;
	sp1 = miscbuf;
	sp3 = &sp1[DISSIZE];
	while (n--) {
		fp = fs;
		for (;;) {
			c = *fp++;
			if (isascii(c) && isdigit(c)) {
				r = 0;
				do {
					r = r*10 + c-'0';
					c = *fp++;
				} while (isascii(c) && isdigit(c));
				--fp;
				continue;
			}
			switch (c) {
			case '^':
				add = uad;
				continue;
			case '+':
				add += r;
				r = 1;
				continue;
			case '-':
				add -= r;
				r = 1;
				continue;
			case 'n':
				*sp1 = '\0';
				flushb((long)nad);
				sp1 = miscbuf;
				nad = add;
				continue;
			case 'd':
			case 'o':
			case 'u':
			case 'x':
				t2 = c;
				continue;
			default:
				t1 = c;
				if (t1=='\0' && t2=='\0')
					break;
				if (t1 == '\0')
					t1 = 'w';
				if (t2 == '\0')
					t2 = DDCHR;
				uad = add;
				while (r--) {
					if (testint())
						return (1);
					*sp1++ = ' ';
					pad = add;
					sp2 = conform(sp1, s, t1, t2, getb);
					if (sp2 == NULL) {
						*--sp1 = '\0';
						if (sp1 != miscbuf)
							flushb((long)nad);
						printe("Addressing error");
						return (0);
					}
					if (sp2 <= sp3)
						sp1 = sp2;
					else {
						*--sp1 = '\0';
						flushb((long)nad);
						*sp2 = '\0';
						*sp1 = ' ';
						sp2 = sp1;
						sp1 = miscbuf;
						while (*sp2)
							*sp1++ = *sp2++;
						nad = pad;
					}
					if (c == 'i') {
						*sp1++ = '\0';
						flushb((long)nad);
						sp1 = miscbuf;
						nad = add;
					}
				}
				r = 1;
				t2 = '\0';
				if (c != '\0')
					continue;
			}
			break;
		}
		lad = add;
	}
	if (sp1 != miscbuf) {
		*sp1 = '\0';
		flushb((long)nad);
	}
	return (1);
}

/*
 * Flush the output buffer.  The given address `a' is printed before the
 * contents of the buffer and dot is set to it.
 */
flushb(a)
long a;
{
	dot = a;
	printx(DAFMT, (long)dot);
	printx("  %s\n", miscbuf);
}

/*
 * Given a format character `f' and a function `func', print out
 * data according to the given format.  The function `get' is
 * called to read characters.  Segment number is 's'.
 */
char *
conform(sp, s, t1, t2, get)
register char *sp;
int (*get)();
{
	register char *lp;
	unsigned char c[1];
	unsigned char v[3];
	unsigned short h[1];
	char *p[1];
	int  i[1];
	long l[1];
#ifndef	NFP
	float f[1];
	double d[1];
#endif
	switch (t1) {
	case 'b':
		if ((*get)(s, c, sizeof(c)) == 0)
			return (NULL);
		modsize = sizeof(c);

		/*
		 * There should be a better way to do this.
		 */
		if (t2 != 'd')
			i[0] = c[0];
		else {
			char x[1];

			x[0] = c[0];
			i[0] = x[0];
		}
		sprintf(sp, getform(t1, t2), i[0]);
		break;
	case 'c':
		if ((*get)(s, c, sizeof(c)) == 0)
			return (NULL);
		modsize = sizeof(c);
		sp = conescs(sp, c[0]);
		return (sp);
	case 'C':
		if ((*get)(s, c, sizeof(c)) == 0)
			return (NULL);
		modsize = sizeof(c);
		*sp++ = (c[0]>=040&&c[0]<0177) ? c[0] : '.';
		return (sp);
#ifndef	NFP
	case 'f':
		if ((*get)(s, (char *)f, sizeof(f)) == 0)
			return (NULL);
		modsize = sizeof(f);
		sprintf(sp, getform(t1, t2), f[0]);
		break;
	case 'F':
		if ((*get)(s, (char *)d, sizeof(d)) == 0)
			return (NULL);
		modsize = sizeof(d);
		sprintf(sp, getform(t1, t2), d[0]);
		break;
#endif
	case 'h':
		if ((*get)(s, h, sizeof(h)) == 0)
			return (NULL);
		modsize = sizeof(h);

		/*
		 * Oh well.
		 */
		if (t2 != 'd')
			i[0] = h[0];
		else {
			short x[1];

			x[0] = h[0];
			i[0] = x[0];
		}
		sprintf(sp, getform(t1, t2), i[0]);
		break;
	case 'i':
		sp = newconinst(sp, s);
		modsize = INLEN;
		return (sp);
	case 'l':
		if ((*get)(s, (char *)l, sizeof(l)) == 0)
			return (NULL);
		modsize = sizeof(l);
		sprintf(sp, getform(t1, t2), l[0]);
		break;
	case 'p':
		if ((*get)(s, (char *)p, sizeof(p)) == 0)
			return (NULL);
		modsize = sizeof(p);
		sp = conaddr(sp, 0, (long)p[0], I);
		return (sp);
	case 's':
	case 'S':
		lp = &sp[DISSIZE];
		c[0] = '\0';
		for (;;) {
			if ((*get)(s, c, sizeof(c)) == 0)
				return (NULL);
			if (c[0] == '\0')
				break;
			if (t1 == 's')
				sp = conescs(sp, c[0]);
			else
				*sp++ = c[0];
			if (sp > lp)
				return (NULL);
		}
		modsize = sizeof(i);
		return (sp);
	case 'v':
		if ((*get)(s, (char *)v, sizeof(v)) == 0)
			return (NULL);
		modsize = sizeof(v);
		l3tol(l, v, 1);
		sprintf(sp, getform(t1, t2), l[0]);
		break;
	case 'w':
		if ((*get)(s, (char *)i, hdrinfo.magic == L_MAGIC 
					? sizeof(short) : sizeof(int)) == 0)
			return (NULL);
		modsize = hdrinfo.magic == L_MAGIC 
					? sizeof(short) : sizeof(int);
		sprintf(sp, getform(t1, t2), hdrinfo.magic == L_MAGIC 
					? (unsigned short)i[0] : i[0]);
		break;
	case 'Y':
		sp = gettime(sp, cseg, get);
		modsize = sizeof(i);
		return (sp);
	default:
		return (NULL);
	}
	while (*sp)
		sp++;
	return (sp);
}

/*
 * If the given character is unprintable, convert it into
 * an escape sequence.
 */
conescs(sp, c)
register char *sp;
register int c;
{
	*sp++ = '\\';
	switch (c) {
	case '\0':
		*sp++ = '0';
		break;
	case '\b':
		*sp++ = 'b';
		break;
	case '\n':
		*sp++ = 'n';
		break;
	case '\f':
		*sp++ = 'f';
		break;
	case '\r':
		*sp++ = 'r';
		break;
	case '\\':
		*sp++ = '\\';
		break;
	default:
		if (c<040 || c>=0177) {
			sprintf(sp, "%03o", c);
			while (*sp)
				sp++;
			break;
		}
		sp[-1] = c;
	}
	return (sp);
}

/*
 * Get the current time.
 */
char *
gettime(sp, s, get)
register char *sp;
int (*get)();
{
	long l;
	register int n;
	register char *cp;

	if ((*get)(s, (char *) &l, sizeof(l)) == 0)
		return (NULL);
	cp = ctime(&l);
	n = 24;
	do {
		*sp++ = *cp++;
	} while (--n);
	return (sp);
}
