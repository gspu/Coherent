/*
 * A debugger.
 * Command analyser.
 */
#include <stdio.h>
#include "trace.h"


/*
 * Execute a command.
 * Return true if more commands may be read.
 */
command(vp)
register VAL *vp;
{
	register int c;
	register int n;

	switch (c=getn()) {
	case '=':
		if (newline() == 0)
			break;
		printx("%lo\n", rvalue(&vp[0], (long)dot));
		return (1);
	case '?':
		if (newline() == 0)
			break;
		if (errrstr != NULL)
			printx("%s\n", errrstr);
		return (1);
	case 'a':
		if (newline() == 0)
			break;
		putaddr(vsegno(&vp[0], -1), lvalue(&vp[0], (long)dot), I);
		putx('\n');
		return (1);
	case 'b':
		if (objflag == 0)
			break;
		if ((c=getn()) != 'r') {
			ungetn(c);
			c = 'b';
		}
		getline("i+.:a\ni+.?i\n:x");
		n = setubpt(c, (vaddr_t)lvalue(&vp[0], (long)dot), miscbuf);
		if (n == 0)
			printe("Cannot set breakpoint");
		return (1);
	case 'c':
		if (newline() == 0)
			break;
		else
			c = '\n';
		if (excflag == 0)
			break;
		if (nvalue(&vp[0]) == 0)
			setpc((vaddr_t)lvalue(&vp[0], (long)0));
		sinmode = SNULL;
		return (0);
	case 'd':
		if (objflag == 0)
			break;
		if ((c=getn())!='r' && c!='s' && c!='a') {
			ungetn(c);
			c = 'b';
		}
		if (newline() == 0)
			break;
		if( c == 'a') {
			bptinit();
			return( 1);
		}
		if (delubpt(c, (vaddr_t)lvalue(&vp[0], (long)dot)) == 0)
			printe("Cannot delete breakpoint");
		return (1);
	case 'e':
		if (nvalue(&vp[0]) == 0)
			setpc((vaddr_t)lvalue(&vp[0], (long)0));
		getline("");
		return(runfile());
	case 'f':
		if (newline() == 0)
			break;
		printx("%s\n", trapstr!=NULL ? trapstr : "No fault");
		return (1);
	case 'k':
		if (excflag == 0)
			break;
		if (newline() == 0)
			break;
		if (nvalue(&vp[0]) != 0) {
			printe("No signal specified");
			return (1);
		}
		printr("Cannot send %d", (int)rvalue(&vp[0], (long)0));
		return (1);
	case 'm':
		if (newline() == 0)
			break;
		dispmap();
		return (1);
	case 'n':
		if( ( c = getn()) == 'o')
			dbase = 8;
		else if( c == 'd')
			dbase = 10;
		else if( c == 'x')
			dbase = 16;
		else
			break;
		if( newline() == 0)
			break;
		return( 1);
	case 'o':
		if (newline() == 0)
			break;
		cantype = rvalue(&vp[0], (long)0);
		return (1);
	case 'p':
		if (newline() == 0)
			break;
		dispbpt();
		return (1);
	case 'q':
		if (newline() == 0)
			break;
		if (rvalue(&vp[0], (long)1))
			leave();
		return (1);
	case 'r':
		if (newline() == 0)
			break;
		else
			c = '\n';
		if (regflag == 0)
			break;
		dispreg();
		return (1);
	case 's':
		if (excflag == 0)
			break;
		if (nvalue(&vp[0]) == 0)
			setpc((vaddr_t)lvalue(&vp[0], (long)0));
		sindecr = rvalue(&vp[1], (long)1);
		sinmode = SCONT;
		if ((c=getn()) != 'c') {
			sinmode = SSTEP;
			ungetn(c);
		}
		getline("i+.?i");
		sinp = savecom(sinp, miscbuf);
		return (0);
	case 't':
		if (newline() == 0)
			break;
		else
			c = '\n';
		if (regflag == 0)
			break;
		dispsbt((int)rvalue(&vp[0], (long)0));
		return (1);
	case 'x':
		if (newline() == 0)
			break;
		if (rvalue(&vp[0], (long)1) != 0) {
			sincmod = sinmode;
			sinmode = SNULL;
			addfilp(stdin);
		}
		return (1);
	}
	printe("Illegal command");
	while (c != '\n')
		c = getn();
	return (1);
}

/*
 * If the next character is a newline, return 1, else 0.
 */
newline()
{
	return (getn()=='\n');
}

/*
 * Given a pointer to an allocated buffer and a pointer to a
 * command string, copy the command string in the allocated
 * buffer.  If the allocated buffer hasn't been allocated,
 * allocate it.
 */
char *
savecom(bp, sp)
register char *bp;
register char *sp;
{
	register char *cp;

	if (bp == NULL)
		bp = nalloc(COMSIZE);
	cp = bp;
	while (*sp) {
		if (cp >= &bp[COMSIZE-1])
			break;
		*cp++ = *sp++;
	}
	*cp++ = '\0';
	return (bp);
}

/*
 * Initialise breakpoint table.
 */
bptinit()
{
	register BPT *bp;
	register int n;

	for (n=NBPT, bp=&bpt[0]; --n; bp++) {
		bp->b_flag = 0;
		bp->b_bcom = NULL;
		bp->b_rcom = NULL;
	}
}

/*
 * Set a breakpoint (user callable).  If the type is 'b', a regular
 * breakpoint.  If it is 'r', a return breakpoint.
 */
setubpt(t, a, cp)
vaddr_t a;
char *cp;
{
	register unsigned n;
	vaddr_t pc;
	vaddr_t fp;

	if (t == 'b')
		n = setibpt(BBPT, a, (vaddr_t)0, cp);
	else {
		setretf(&pc, &fp);
		n = setibpt(BRET, pc, fp, cp);
	}
	return (n);
}

/*
 * Set a breakpoint (internal routine).
 */
setibpt(f, a, fp, cp)
register vaddr_t a;
vaddr_t fp;
char *cp;
{
	register BPT *bp;
	register int n;
	int w;

#if FORZ8001
	a &= 0x7F00FFFFL;
#endif
	add = (long)a;
	if (getb(1, (char *)&w, sizeof(w)) == 0)
		return (0);
	for (n=NBPT, bp=&bpt[0]; --n; bp++) {
		if (bp->b_flag!=0 && bp->b_badd==a)
			goto out;
	}
	for (n=NBPT, bp=&bpt[0]; --n; bp++) {
		if (bp->b_flag == 0)
			goto out;
	}
	return (0);
out:
	if (f!=BBPT && (bp->b_flag&f)!=0)
		return (0);
	bp->b_flag |= f;
	bp->b_badd = a;
	switch (f) {
	case BBPT:
		bp->b_bcom = savecom(bp->b_bcom, cp);
		break;
	case BRET:
		bp->b_rfpt = fp;
		bp->b_rcom = savecom(bp->b_rcom, cp);
		break;
	case BSIN:
		bp->b_sfpt = fp;
		break;
	}
	return (1);
}

/*
 * Delete a breakpoint.  (user callable)
 * If `r' is set, it is a return breakpoint.
 */
delubpt(t, a)
vaddr_t a;
{
	register int f;

	switch (t) {
	case 'b':
		f = BBPT;
		break;
	case 'r':
		f = BRET;
		break;
	case 's':
		f = BSIN;
		break;
	}
	return (delibpt(f, a));
}

/*
 * Delete a breakpoint.  (internal routine)
 */
delibpt(f, a)
register int f;
vaddr_t a;
{
	register BPT *bp;
	register int n;

	for (n=NBPT, bp=&bpt[0]; --n; bp++) {
		if ((bp->b_flag&f)==0 || bp->b_badd!=a)
			continue;
		bp->b_flag &= ~f;
		return (1);
	}
	return (0);
}

/*
 * Display breakpoints.
 */
dispbpt()
{
	register BPT *bp;
	register int n;

	for (n=NBPT, bp=&bpt[0]; --n; bp++) {
		if (testint())
			return;
		if ((bp->b_flag&BBPT) != 0)
			putlbpt(' ', bp->b_badd, bp->b_bcom);
		if (testint())
			return;
		if ((bp->b_flag&BRET) != 0)
			putlbpt('r', bp->b_badd, bp->b_rcom);
		if (testint())
			return;
		if ((bp->b_flag&BSIN) != 0)
			putlbpt('s', bp->b_badd, "");
	}
}

/*
 * Print out a breakpoint address, type and command.
 */
putlbpt(t, a, cp)
vaddr_t	a;
register char *cp;
{
	register int c;

	printx(DAFMT, (long)a);
	printx("%c (", t);
	putaddr(1, (long)a, I);
	printx(") ");
	while ((c=*cp++) != '\0') {
		if (c == '\n') {
			printx("\\n");
			continue;
		}
		if (c == '\\') {
			printx("\\\\");
			continue;
		}
		putx(c);
	}
	putx('\n');
}

/*
 * Print out segmentation map.
 */
dispmap()
{
	register MAP *mp;
	register int n;

	for (n=0; n<NSEGM; n++) {
		if ((mp=segmapl[n]) == NULL)
			continue;
		printx("%s", segname[n]);
		while (n<NSEGM-1 && segmapl[n+1]==mp)
			printx(" == %s", segname[++n]);
		printx("\n");
		for (; mp!=NULL; mp=mp->m_next) {
			printx("[");
			putmadd(mp->m_base);
			printx(", ");
			putmadd(mp->m_bend);
			printx("] => ");
			putmadd(mp->m_offt);
			printx(" (%d)\n", mp->m_segi);
			if (testint())
				return;
		}
	}
}

/*
 * Print out a long address for segmentation map.
 */
putmadd(l)
fsize_t l;
{
	register int c, n;

	if (l!=MLI && l!=LI) {
		printx(VAFMT, l);
		return;
	}
	c = l==MLI ? '-' : '+';
	n = VAWID;
	while (n--)
		putchar(c);
}
