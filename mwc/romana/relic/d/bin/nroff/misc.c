/*
 * misc.c
 * Nroff/Troff.
 * Miscellaneous routines.
 */

#include "roff.h"
#include <access.h>
#include <ctype.h>
#include <string.h>

/*
 * Define a macro.
 * 'mp' is a pointer to a macro descriptor which is
 * filled in with the appropriate information and name is the name
 * of the macro upon which the definition ends.
 */
deftext(mp, name) register MAC *mp; char name[2];
{
	static char residue[4];
	register STR *sp;
	register char *bp;
	int nlflag;

	if (mp != NULL) {
		mp->t_div.m_next = NULL;
		mp->t_div.m_type = MTEXT;
		mp->t_div.m_core = NULL;
		mp->t_div.m_seek = tmpseek;
	}
#if	0
	/* Conditionalized out 4/11/91 by steve, seems wrong. */
	infalse = 1;
#endif
	bp = miscbuf;
	for (nlflag = 1;;) {

		/* nlflag indicates if last character was \n. */
		if (bp > miscbuf)
			nlflag = (*(bp-1) == '\n');

		/* Flush if necessary, leaving room for "..\n" or ".XX". */
		if (bp >= &miscbuf[MSCSIZE-3]) {
			if (mp != NULL)
				nwrite(miscbuf, 1, bp-miscbuf);
			bp = miscbuf;
		}

		/* Ignore remainder of line not starting with endmark. */
		if (!nlflag) {
			*bp++ = getf(2);
			continue;
		}

		/* Last character was \n, look for endmark on next line. */
		if (name[0]=='\0') {
			if ((*bp++ = getf(2))!='.' || escflag!=0)
				continue;
			if ((*bp++ = getf(2))!='.' || escflag!=0)
				continue;
			if ((*bp++ = getf(2)) != '\n')
				continue;
			bp -= 3;
			break;
		} else {
			if ((*bp++ = getf(2))!=ccc || escflag!=0)
				continue;
			if ((*bp++ = getf(2))!=name[0] || escflag!=0)
				continue;
			if ((*bp++ = getf(2))!=name[1] || escflag!=0)
				continue;
			bp -= 3;
			residue[0] = bp[0];
			residue[1] = bp[1];
			residue[2] = bp[2];
			residue[3] = '\0';
			sp = allstr(SCORE);
			sp->x3.s_cp = residue;
			sp->x3.s_srel = NULL;
			break;
		}
	}
	*bp++ = '\0';
	if (mp != NULL)
		nwrite(miscbuf, 1, bp-miscbuf);
#if	0
	/* Conditionalized out 4/11/91 by steve, as above. */
	infalse = 0;
#endif
}

/*
 * Define a special character.
 */
spc_def(name, value) char name[2]; char *value;
{
	register SPECIAL **spp;
	register int n;
	int len;
	SPECIAL *sp;
	char *cp;

	len = strlen(value) + 1;
	for (spp = &spc_list; *spp != NULL; spp = &((*spp)->spc_link)) {
		sp = *spp;
		n = strncmp(sp->spc_name, name, 2);
		if (n < 0)
			continue;		/* keep looking */
		else if (n > 0)
			break;			/* not found, add it */
		/* Gotcha, redefine it. */
		nfree(sp->spc_val);		/* free old value */
		sp->spc_val = nalloc(len);	/* allocate new space */
		strcpy(sp->spc_val, value);	/* and copy in value */
		return;
	}
	/* Add new entry. */
	sp = (SPECIAL *)nalloc(sizeof(*sp));	/* allocate new entry */
	cp = nalloc(len);			/* allocate new value space */
	sp->spc_link = *spp;			/* link into SPECIAL list */
	*spp = sp;
	sp->spc_name[0] = name[0];
	sp->spc_name[1] = name[1];
	sp->spc_val = cp;
	strcpy(cp, value);
}

/*
 * Find a special character entry in the special character list.
 * Print error message and return NULL if not found.
 */
SPECIAL *
spc_find(name) char name[2];
{
	register SPECIAL *sp;
	register int n;

	for (sp = spc_list; sp != NULL; sp = sp->spc_link) {
		n = strncmp(sp->spc_name, name, 2);
		if (n < 0)
			continue;
		else if (n > 0)
			break;
		else
			return sp;
	}
	printe("special character %c%c not found", name[0], name[1]);
	return NULL;
}

/*
 * Process requests from a library file if it exists.
 * The flag is 1 if file contains requests, 0 if binary data.
 * Return 1 if it exists, 0 if not.
 */
lib_file(s, flag) char *s; int flag;
{
	register char file[40];

	/* Look file, process it if found. */
	sprintf(file, "%s%s%s", LIBDIR,
		(ntroff == NROFF) ? NRDIR : (pflag) ? TPSDIR : TPCLDIR, s);
	if (access(file, AREAD) != 0)
		return 0;
	if (flag) {
		adsfile(file);
		process();
		return 1;
	}
	return copy_file(file);
}

/*
 * Copy a file verbatim.
 * Return 0 if not found or not readable, 1 on success.
 */
copy_file(s) char *s;
{
	register FILE *fp;
	register int c;

	if ((fp = fopen(s, "rb")) == NULL)
		return 0;
	while ((c = fgetc(fp)) != EOF)
		putchar(c);
	fclose(fp);
	return 1;
}

/*
 * Read a block into the buffer 'bp' starting at seek position
 * 'l' in the temp file.
 */
nread(l, bp)
long l;
char *bp;
{
	if (tmpseek-l <= DBFSIZE) {
		copystr(bp, diskbuf, DBFSIZE, 1);
		return;
	}
	lseek(fileno(tmp), (long) l, 0);
	if (read(fileno(tmp), bp, DBFSIZE) != DBFSIZE)
		panic("temporary file read error");
}

/*
 * Write the buffer 'bp' which contains 'n' elements of size 's'
 * onto the end of the temp file.
 */
nwrite(bp, s, n) register char *bp; unsigned s; register unsigned n;
{
	unsigned bno, off;
	register char *dp;

	bno = tmpseek / DBFSIZE;
	off = tmpseek % DBFSIZE;
	if (bno!=0 && off==0) {
		--bno;
		off += DBFSIZE;
	}
	dp = &diskbuf[off];
	s = n *= s;
	while (n--) {
		if (dp >= &diskbuf[DBFSIZE]) {
			lseek(fileno(tmp), (long) bno++*DBFSIZE, 0);
			if (write(fileno(tmp), diskbuf, DBFSIZE) != DBFSIZE)
				panic("temporary file write error");
			dp = &diskbuf[0];
		}
		*dp++ = *bp++;
	}
	tmpseek += s;
}

/*
 * Given a string, return a pointer to a copy of it.
 */
char *
duplstr(s) register char *s;
{
	return strcpy(nalloc(strlen(s) + 1), s);
}

/*
 * Copy the array of 'n' elements containing a structure of size
 * 'size' from 's2' to 's1'.
 */
copystr(s1, s2, size, n) char *s1, *s2; int size; register int n;
{
	if ((n *= size) == 0)
		return;
	memcpy(s1, s2, n);
}

/*
 * Allocate 'n' bytes.
 */
char	*
nalloc(n)
{
	extern char *calloc();
	register char *cp;

	if ((cp = calloc(n, 1)) == NULL)
#if	MSDOS
		panic("out of space");		/* no memok() in MS-DOS libc */
#else
		panic("out of space - memory %s", (memok() ? "good" : "bad"));
#endif
	return cp;
}

/*
 * Release the given storage.
 */
nfree(cp)
char *cp;
{
	free(cp);
}

/*
 * Execute conditional.
 * bp points to the remainder of the line.
 * Called from .el and .ie.
 */
do_cond(cond, bp) int cond; unsigned char *bp;
{
	unsigned char charbuf[CBFSIZE];
	register unsigned char *cp;
	unsigned char c;

	while (isascii(*bp) && isspace(*bp))
		bp++;				/* skip leading space */
	if (cond) {
		/* Execute true branch. */
		cp = charbuf;
		bracelevel = oldbracelevel;	/* restore original brace level */
		if (*bp == EBEG) {
			bp++;			/* ignore \{ */
			bracelevel++;
		}
		while (c = *bp++)
			if (cp < &charbuf[CBFSIZE-2])
				*cp++ = c;
		*cp++ = '\n';
		*cp = '\0';
		cp = duplstr(charbuf);
		adscore(cp);			/* execute remainder */
		strp->x3.s_srel = cp;
	} else {
		/* Skip false branch. */
		if (*bp != EBEG)
			return;			/* one-line conditional */
		for (; *bp; bp++) {
			if (*bp == EBEG)
				++infalse;
			else if (*bp == EEND)
				--infalse;
		}
		if (infalse == 0)
			return;			/* \{ ... \} on one line */
		while (infalse && (c = getf(0))) {
			if (c == EBEG)
				++infalse;
			else if (c == EEND)
				--infalse;	/* scan to matching \} */
		}
		while (getf(0) != '\n')
			;			/* then scan to newline */
	}
}

/*
 * Set a value, saving old value.
 * Called by various requests.
 */
setval(vp, op, arg, mul, div) int *vp, *op; char *arg; long mul, div;
{
	register int val;

	val = *vp;					/* save value */
	*vp = number(arg, mul, div, val, 0, *op);	/* set value */
	*op = val;					/* save old */
}

/*
 * Return the number of the named font.
 * Return -1 if not found.
 */
int
font_num(name) char name[2];
{
	register FTB *p;

	for (p = fontab; p < &fontab[NFNAMES]; p++) {
		if (p->f_name[0] == '\0')
			break;
		else if ((p->f_name[0] == name[0]) && (p->f_name[1] == name[1]))
			return p->f_font;
	}
	return -1;
}

/*
 * Return the number of the named font.
 * Print an error message and return -1 if not found.
 */
int
font_number(name, s) char name[2]; char *s;
{
	register int n;

	if ((n = font_num(name)) == -1)
		printe("%scannot find font %c%c", (s==NULL) ? "" : s, name[0], name[1]);
	return n;
}

/*
 * Assign a font number to a name.
 * If there is no font of the given name, add one.
 * Return the previously assigned font number, or -1 if none.
 */
int
assign_font(name, c) char *name; int c;
{
	char a, b;
	register FTB *p;

	a = name[0];
	b = name[1];
	for (p = fontab; p < &fontab[NFNAMES]; p++) {
		if ((p->f_name[0] == a) && (p->f_name[1] == b)) {

			/* Replace existing entry */
			a = p->f_font;
			p->f_font = c;

			/* Watch for current, tab, underline fonts. */
			if (a == curfont)
				dev_font(c);
			if (a == tfn)
				tfn = c;
			if (a == ufn)
				ufn = c;

			return a;
		} else if (p->f_name[0] == '\0') {
			/* Add new entry */
			p->f_name[0] = a;
			p->f_name[1] = b;
			p->f_font    = c;
			return -1;
		}
	}
	printe("no room for new font name %c%c", a, ((b) ? b : ' '));
	return -1;
}

/*
 * Given a font number, change font.
 */
setfontnum(n, setflag) register int n; int setflag;
{
	register FTB *p;

	for (p = fontab; p < &fontab[NFNAMES]; p++) {
		if (p->f_font == n) {
			setfont(p->f_name, setflag);
			return;
		} else if (p->f_name[0] == '\0')
			break;
	}
	printe("cannot find font %d", n);
}

/*
 * Given a font name, change font.
 * Understands \fP and \fn, saves previous font in oldfon.
 * Return the new font number, or -1 if not found.
 * dev_font() does the real work.
 */
int
setfont(name, setflag) char name[2]; int setflag;
{
	register int n;

	if ((name[0] >= '0') && (name[0] <= '9')) {
		n = name[0] - '0';
		name[0] = mapfont[n][0];
		name[1] = mapfont[n][1];
	}
	if (name[0]=='P' && name[1]=='\0') {
		name[0] = oldfon[0];
		name[1] = oldfon[1];
	}
	if ((n = font_number(name, NULL)) < 0) {
		if (setflag) {
			/*
			 * Bogus font change sets oldfon so \fP gets back
			 * from the bogus font to the present font.
			 * The bogus font change is really ignored.
			 */
			oldfon[0] = fon[0];
			oldfon[1] = fon[1];
		}
		return -1;
	}
	dev_font(n);
	if (setflag) {
		oldfon[0] = fon[0];
		oldfon[1] = fon[1];
		fon[0] = name[0];
		fon[1] = name[1];
	}
	return n;
}

/*
 * Print out a warning.
 */
/*VARARGS*/
printe(a1) char *a1;
{
	register STR *sp;

	fprintf(stderr, "%s: ", argv0);
	for (sp=strp; sp; sp=sp->x1.s_next) {
		if (sp->x1.s_type == SFILE) {
			fprintf(stderr, "%s: %d: ", sp->x2.s_fname, sp->x2.s_clnc);
			break;
		}
	}
	fprintf(stderr, "%r\n", &a1);
	if (dflag)
		fprintf(stderr, "Request: %s\n", miscbuf);
}

/*
 * Print an unimplemented warning.
 */
printu(s) char *s;
{
	printe("%s not implemented yet", s);
}

/*
 * An irrecoverable error was found.
 * Print out an error message and leave.
 */
/*VARARGS*/
panic(s)
{
	fprintf(stderr, "%s: %r\n", argv0, &s);
	leave(1);
}

/* Debug stuff follows, used to be in codebug.c. */

#if	(DDEBUG != 0)
#if	(DDEBUG & DBGCODE)

static char *codtab[] = {
	"DNULL",
	"DHMOV (move horizontal)",
	"DVMOV (move vertical)",
	"DFONT (change font)",
	"DPSZE (change pointsize)",
	"DSPAR (space down and return)",
	"DPADC (Paddable character)",
	"DHYPH (Place to hyphenate)",
	"DHYPC (Hyphen character)"
	};

codebug(code, parm1)
{
	if (code <= 0) {
		printd(DBGCODE, "%s %u\n", codtab[-code], parm1);
	} else {
		printd(DBGCODE, "%c (width=%d)\n", code, parm1);
	}
}
#endif

static char *dbgtbl[] = {
	"CHECKpoints",
	"REGisterS",
	"REGister eXecution",
	"output CODEs",
	"DIVeRsions",
	"FILEs",
	"FUNCtions",
	"CHARacters",
	"PROCess trace",
	"MACro eXecution",
	"MISCelaneous",
	"MOVEment",
	"ENViRonment",
	"CALL tracing"
};

printd(lvl, fmt)
int lvl;
char *fmt;
{
	if (lvl & dbglvl)
		fprintf(stderr, "%r", &fmt);
}

void dbginit()
{
	register int t=dbglvl;
	register int j;
	register int m=1;

	if (dbglvl == 0)
		return;
	for (j=0; j<15; j++) {
		if (t & m)
			fprintf(stderr, "debugging %s\n", dbgtbl[j]);
		m <<= 1;
	}
}
#endif

/* end of misc.c */
