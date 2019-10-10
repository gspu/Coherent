/*
 * /usr/src/cmd/prof.c
 * 6/23/93
 * prof interprets the mon.out files produced by the runtime profiling option,
 * i.e. by programs compiled with the cc option -p (a.k.a. -VPROF).
 * This version understands both COH286 l.out and COH386 COFF executables,
 * which have different mon.out sizes in addition to different symbol formats.
 * Usage:
 *	prof [ -abcs ] [ l.out [ mon.out ] ]
 * Options:
 * 	-a	use all symbols (as opposed to only external symbols)
 *	-b	print all bin information (to detect hot spots)
 *	-c	print all call information
 *	-s	print stack depth information
 */

#include <stdio.h>
#include <l.out.h>
#include <coff.h>
#include <sys/const.h>
#include <canon.h>
#include <mon.h>
#if	_I386
#include <oldmon.h>
#endif

#if	DEBUG
#define	dbprintf(args)	printf args
#else
#define	dbprintf(args)
#endif

/*
 * Note that in setting PSCALE, one must guard against overflow.
 * Examine putdata() carefully before changing this constant.
 * Also note that certain divisibility properties are assumed
 * there reguarding PSCALE and HZ.
 */
#define PSCALE	((long)100)		/* pc count scale factor	*/
#define	CSYMLEN	255			/* maximum COFF symbol length	*/
#define	SYMWMAX	32			/* maximum symbol printf width	*/
#define	TRUE	(0 == 0)
#define	FALSE	(0 != 0)

typedef struct	symbol {
	off_t		addr;		/* address			*/
	long		pcount;		/* pc count, scaled by PSCALE	*/
	long		ccount;		/* number of times routine called */
	char		name[];		/* name				*/
}	SYMBOL;

/* Forward. */
char	*alloc();
void	centi();
int	cmpdata();
int	cmpsym();
SYMBOL	**credit();
void	fatal();
void	getcdata();
void	getdata();
void	getpdata();
void	getstring();
void	getsyms();
void	putdata();
void	readcoffsyms();
void	readsyms();
void	usage();
void	warning();

/* Globals. */
int		aflag	= FALSE;	/* use all symbols		*/
int		bflag	= FALSE;	/* dump bin info		*/
int		cflag	= FALSE;	/* dump call info		*/
SYMBOL		**dict;			/* NULL-terminated SYMBOL list	*/
int		dsize;			/* number of symbols		*/
int		iscoff	= FALSE;	/* COFF executable (not l.out)	*/
#if	_I386
char		*lout	= "a.out";	/* executable file name		*/
#else
char		*lout	= "l.out";	/* executable file name		*/
#endif
caddr_t		lowpc;			/* lowest pc profiled		*/
char		*monout	= "mon.out";	/* monitor file name		*/
char		name[CSYMLEN+1];	/* symbol name buffer		*/
unsigned int	scaler;			/* scale factor			*/
int		sflag	= FALSE;	/* dump low stack mark		*/
off_t		stksz;			/* stack size			*/
long		strtable;		/* COFF string table offset	*/
int		symwidth = NCPLN;	/* printf symbol field width	*/
long		tcalls;			/* total number of calls	*/
long		tticks;			/* total number of clock ticks	*/

main(argc, argv) int argc; register char *argv[];
{
	register char *cp, ch;

	for (cp=*++argv; cp != NULL  &&  *cp++ == '-'; cp=*++argv)
		while ((ch=*cp++) != '\0')
			switch (ch) {
			case 'a':	aflag = TRUE;		break;
			case 'b':	bflag = TRUE;		break;
			case 'c':	cflag = TRUE;		break;
			case 's':	sflag = TRUE;		break;
			default:	usage();		break;
			}
	if (*argv != NULL)
		lout = *argv++;
	if (*argv != NULL)
		monout = *argv++;
	if (*argv != NULL)
		usage();
	getsyms();
	getdata();
	if (sflag)
		printf("%u bytes stack used\n", stksz);
	if (!bflag && !cflag)
		putdata();
	exit(0);
}

/*
 * alloc() is an interface to malloc() which exits if there is no room.
 */
char *
alloc(size) unsigned size;
{
	char	*result;

	result = (char *)malloc(size);
	if (result == NULL)
		fatal("out of space");
	return result;
}

/*
 * Print on standard output 'num' / 'den' to two places, with
 * at least 'width' places to the left of the decimal point.
 */
void
centi(num, den, width) long num, den; int width;
{
	long	cv;

	cv = (num*100 + den/2) / den;
	printf("%*ld.%02d", width, cv/100, (int)(cv%100));
}

/*
 * Compare the data in the dictionary entries 'sp1' and 'sp2'.
 * Return an int which reflects which entry should be listed first.
 * If the value is positive, 'sp2' should occur first.
 * If it is zero, it makes no difference. 
 * If it is negative, 'sp1' should occur first.
 */
int
cmpdata(sp1, sp2) SYMBOL **sp1, **sp2;
{
	register SYMBOL *adr1, *adr2;
	long rel;

	adr1 = *sp1;
	adr2 = *sp2;
	rel = adr2->pcount - adr1->pcount;
	if (rel == 0)
		rel = adr2->ccount - adr1->ccount;
	if (rel > 0)
		return 1;
	else if (rel < 0)
		return -1;
	else
		return strcmp(adr1->name, adr2->name);
}

/*
 * Compare the two SYMBOLs 'sp1' and 'sp2' and return an
 * int corresponding to the relative order of the address fields.
 */
int
cmpsym(sp1, sp2) SYMBOL **sp1, **sp2;
{
	register off_t adr1, adr2;

	adr1 = (*sp1)->addr;
	adr2 = (*sp2)->addr;
	if (adr1 > adr2)
		return 1;
	else if (adr1 == adr2)
		return 0;
	else
		return -1;
}

/*
 * Account for tick information.
 */
SYMBOL	**
credit(tick, low, high, dpp) int tick; off_t low, high; SYMBOL **dpp;
{
	register unsigned	overlap;
	register SYMBOL		*cur, *nxt;
	unsigned		binlen;

	dbprintf(("credit(%d, %x, %x, %s)\n", tick, low, high, (*dpp)->name));
	binlen = high - low;
	if (binlen == 0)
		binlen = 1;		/* avoid 0-divide below */

	nxt = *dpp;
	if (nxt == NULL  ||  nxt->addr >= high) {
		if (bflag)
			printf("%3d %06o %06o\n", tick, low, high-1);
		return dpp;
	}
	do {
		cur = nxt;
		nxt = *++dpp;
	} while (nxt != NULL  &&  nxt->addr <= low);
	if (bflag)
		printf("%3d %*s+%-4u ", tick, symwidth, cur->name,
			low - cur->addr);
	do {
		if (nxt != NULL  &&  nxt->addr < high)
			overlap = nxt->addr;
		else
			overlap = high;
		if (cur->addr > low)
			overlap -= cur->addr;
		else
			overlap -= low;
		cur->pcount += (PSCALE*overlap*tick + binlen/2) / binlen;
		cur = nxt;
		nxt = *++dpp;
	} while (cur != NULL && cur->addr < high);
	dpp -= 2;
	if (bflag)
		printf("%*s+%u\n", symwidth, dpp[0]->name,
			high - 1 - dpp[0]->addr);
	return dpp;
}

/*
 * Print fatal error message and die.
 */
void
fatal(str) char *str;
{
	fprintf(stderr, "prof: %r\n", &str);
	exit(1);
}

/*
 * Read function call information from the mon.out file.
 */
void
getcdata(fp, nfnc) FILE *fp; register unsigned nfnc;
{
	register SYMBOL	**dpp, *dp;
	struct m_func	buf;
#if	_I386
	struct	old_m_func obuf;
#endif

	dbprintf(("getcdata(): nfnc=%d\n", nfnc));
	while (nfnc-- != 0) {
#if	_I386
		if (!iscoff) {
			if (fread(&obuf, sizeof obuf, 1, fp) != 1)
				fatal("unexpected end of file on \"%s\"", monout);
			buf.m_addr = (caddr_t)obuf.m_addr;
			buf.m_ncalls = obuf.m_ncalls;
		} else
#endif
		if (fread(&buf, sizeof buf, 1, fp) != 1)
			fatal("unexpected end of file on \"%s\"", monout);
		for (dpp=dict; (dp=*++dpp) != NULL && dp->addr <= (off_t)buf.m_addr;)
			;
		dp = dpp[-1];
		if (cflag)
			printf("%4ld %*s+%u\n", buf.m_ncalls, symwidth,
				dp->name, buf.m_addr - dp->addr);
		tcalls += buf.m_ncalls;
		dp->ccount += buf.m_ncalls;
	}
	dbprintf((" tcalls=%ld\n", tcalls));
}

/*
 * Read the mon.out file and put the information into the dictionary.
 */
void
getdata()
{
	FILE		*fp;
	struct m_hdr	hdr;
#if	_I386
	struct	old_m_hdr ohdr;
#endif

	dbprintf(("getdata():\n"));
	fp = fopen(monout, "r");
	if (fp == NULL)
		fatal("cannot open \"%s\"", monout);
#if	_I386
	if (!iscoff) {
		/* Read COH286 mon.out and massage accordingly. */
		if (fread(&ohdr, sizeof ohdr, 1, fp) != 1)
			fatal("\"%s\" is not a 286 mon.out file", monout);
		hdr.m_nbins = ohdr.m_nbins;
		hdr.m_scale = ohdr.m_scale;
		hdr.m_nfuncs = ohdr.m_nfuncs;
		hdr.m_lowpc = (caddr_t)ohdr.m_lowpc;
		hdr.m_lowsp = (caddr_t)ohdr.m_lowsp;
		hdr.m_hisp = (caddr_t)ohdr.m_hisp;
	} else
#endif
	if (fread(&hdr, sizeof hdr, 1, fp) != 1)
		fatal("\"%s\" is not a mon.out file", monout);
	dbprintf((" nbins=%d scale=%d nfuncs=%d\n", hdr.m_nbins, hdr.m_scale, hdr.m_nfuncs));
	scaler = hdr.m_scale & 0xffff;
	if ((scaler & 0xfff) == 0xfff)
		scaler++;
	lowpc = hdr.m_lowpc;
	stksz = hdr.m_hisp - hdr.m_lowsp;
	dbprintf((" lowpc=%x hisp=%x lowsp=%x\n", lowpc, hdr.m_hisp, hdr.m_lowsp));
	dbprintf((" scaler=%d stksz=%x\n", scaler, stksz));

	/* Read call data or skip over it. */
	if (cflag || !bflag)
		getcdata(fp, hdr.m_nfuncs);
	else if (iscoff)
		fseek(fp, hdr.m_nfuncs * (long)sizeof (struct m_func), SEEK_CUR);
	else
		fseek(fp, hdr.m_nfuncs * (long)sizeof (struct old_m_func), SEEK_CUR);

	/* Read clock tick profil data. */
	if (bflag || !cflag)
		getpdata(fp, hdr.m_nbins);
	fclose(fp);
}

/*
 * Reads in the profiling data and increment the corresponding
 * symbols' pcount fields.
 * N.B. the global scale must contain the mon.out scale divided by 2.
 *
 *	scale		.text bytes per bin
 *	0x10000		2
 *	0xFFFF		2	(for historical reasons)
 *	0x8000		4
 *	0x7FFF		4	(for historical reasons)
 *	0x4000		8
 *	...		...
 *	0x0002		65536
 *
 */
void
getpdata(fp, nbins) FILE *fp; unsigned nbins;
{
	register SYMBOL	**dpp;
	off_t		high, low;
	int		highr, inc, incr;
	short		tick;

	dbprintf(("getpdata(): nbins=%d\n", nbins));
	high = (off_t)lowpc;
	highr = 0;
#if	1
	inc = ((long)1<<17) / scaler;
	incr = ((long)1<<17) % scaler;
	if (incr) {
		++inc;
		incr -= scaler;
	}
#else
	inc = 131072L/scaler;
#endif
	dbprintf((" inc=%d incr=%d scale=%d\n", inc, incr, scaler));
	for (dpp=dict; nbins > 0; --nbins) {
		low = high;
		high += inc;
		highr += incr;
		if (-highr >= scaler) {
			--high;
			highr += scaler;
		}
		if (fread(&tick, sizeof tick, 1, fp) != 1)
			fatal("unexpected end of file on \"%s\"", monout);
		if (tick == 0)
			continue;
		tticks += tick;
		dpp = credit(tick, low, high, dpp);
	}
	if (fgetc(fp) != EOF)
		warning("excess data in \"%s\"", monout);
}

/*
 * Read a NUL-terminated string from offset 'loc' (in COFF string table)
 * in fp into name[].
 * Symbols longer than CSYMLEN are truncated.
 */
void
getstring(fp, loc) FILE *fp; long loc;
{
	register long sav;
	register char *cp;
	register int c;

	sav = ftell(fp);
	if (fseek(fp, strtable+loc, SEEK_SET) == -1L)
		fatal("seek failed");
	for (cp = name; cp < &name[CSYMLEN]; *cp++ = c)
		if ((c = fgetc(fp)) == '\0' || c == EOF)
			break;
	*cp = '\0';
	if (c != '\0' && fgetc(fp) != '\0')
		warning("symbol truncated to %s", name);
	if (fseek(fp, sav, SEEK_SET) == -1L)
		fatal("seek failed");
}

/*
 * Read the symbols from an l.out file.
 * Set dict to an array of them, in sorted order.
 */
void
getsyms()
{
	FILE		*fp;
	long		skip;
	struct ldheader	hdr;
	FILEHDR		chdr;

	dbprintf(("getsyms():\n"));
	fp = fopen(lout, "r");
	if (fp == NULL)
		fatal("cannot open \"%s\"", lout);
	if (fread(&hdr, sizeof hdr, 1, fp) != 1 || hdr.l_magic != L_MAGIC) {
		/* File is not l.out, see if it is COFF. */
		rewind(fp);
		if (fread(&chdr, sizeof chdr, 1, fp) != 1
		 || !ISCOFF(chdr.f_magic))
			fatal("\"%s\" is neither l.out nor COFF executable", lout);
		iscoff = TRUE;
		dbprintf(("386 COFF executable\n"));
		if (fseek(fp, chdr.f_symptr, SEEK_SET) == -1L)
			fatal("seek to symbol table failed");
		strtable = chdr.f_symptr + chdr.f_nsyms * sizeof(SYMENT);
		readcoffsyms(chdr.f_nsyms, fp);
	} else {
		/* File is l.out. */
		dbprintf(("286 l.out executable\n"));
		cansize(hdr.l_ssize[L_SHRI]);
		cansize(hdr.l_ssize[L_PRVI]);
		cansize(hdr.l_ssize[L_SHRD]);
		cansize(hdr.l_ssize[L_PRVD]);
		cansize(hdr.l_ssize[L_DEBUG]);
		cansize(hdr.l_ssize[L_SYM]);
		skip = hdr.l_ssize[L_SHRI] + hdr.l_ssize[L_PRVI]
			+ hdr.l_ssize[L_SHRD] + hdr.l_ssize[L_PRVD]
			+ hdr.l_ssize[L_DEBUG];
		fseek(fp, skip, SEEK_CUR);
		readsyms((int)(hdr.l_ssize[L_SYM]/sizeof (struct ldsym)), fp);
	}
	if (dsize == 0)
		fatal("no symbols found in \"%s\"", lout);
	dict = (SYMBOL *)realloc(dict, (dsize + 1) * sizeof *dict);
	qsort(dict, dsize, sizeof *dict, cmpsym);
	fclose(fp);
}

/*
 * Print out the results which have been tabulated in the dictionary.
 */
void
putdata()
{
	register SYMBOL	**dpp, *dp;

	dbprintf(("putdata():\n"));
	qsort(dict, dsize, sizeof *dict, cmpdata);
	for (dpp=dict; (dp=*dpp++) != NULL;) {
		if (dp->pcount == 0 && dp->ccount == 0)
			continue;
		printf("%-*s", symwidth, dp->name);
		if (tticks != 0) {
			centi(dp->pcount, (PSCALE * tticks)/100, 2);
			putchar('%');
		}
		if (dp->ccount != 0) {
			printf(" %7ld ", dp->ccount);
			centi((1000*dp->pcount) / (PSCALE*10),
				(HZ*dp->ccount) / 10, 3);
		}
		putchar('\n');
	}
}

/*
 * Read in nsyms COFF symbols from FILE fp.
 * Set dict to an array of the resulting symbols.
 */
#define	SCNUM_TEXT	1		/* COFF .text section number */
void
readcoffsyms(nsyms, fp) register long nsyms; FILE *fp;
{
	register SYMBOL **dpp, *dp;
	SYMENT sym;
	int len, maxlen;

	dbprintf(("readcoffsyms(): nsyms=%ld\n", nsyms));
	dict = dpp = (SYMBOL **)alloc((nsyms + 1) * sizeof *dpp);
	maxlen = 0;
	while (nsyms-- > 0) {
		if (fread(&sym, sizeof sym, 1, fp) != 1)
			fatal("symbol read failed");
		if (sym.n_scnum != SCNUM_TEXT || sym.n_sclass != C_EXT)
			continue;		/* ignore all but .text */
		if (sym.n_zeroes == 0L)
			getstring(fp, sym.n_offset);
		else {
			strncpy(name, sym.n_name, 8);
			name[8] = '\0';
		}
		len = strlen(name);
		dp = (SYMBOL *)alloc(sizeof *dp + len + 1);
		strcpy(dp->name, name);
		dp->addr = sym.n_value;
		dp->pcount = dp->ccount = 0;
		*dpp++ = dp;
		if (len > maxlen)
			maxlen = len;
	}
	*dpp = NULL;
	dsize = dpp - dict;
	if (maxlen > NCPLN) {
		symwidth = maxlen;
		if (symwidth > SYMWMAX)
			symwidth = SYMWMAX;		/* for readability */
	}
}

/*
 * Read in nsyms l.out ldsyms from the FILE fp. 
 * Set dict to an array of the resulting symbols.
 */
void
readsyms(nsyms, fp) register int nsyms; FILE *fp;
{
	register SYMBOL	**dpp, *dp;
	struct ldsym	lsym;

	dbprintf(("readsyms(): nsyms=%d\n", nsyms));
	dict = dpp = (SYMBOL **)alloc((nsyms + 1) * sizeof *dpp);
	while (--nsyms >= 0) {
		if (fread(&lsym, sizeof lsym, 1, fp) != 1)
			fatal("unexpected end of file on \"%s\"", lout);
		if ((lsym.ls_type & ~L_GLOBAL) > L_BSSI)
			continue;
		if ((lsym.ls_type & L_GLOBAL) == 0 && ! aflag)
			continue;
		strncpy(name, lsym.ls_id, NCPLN);
		name[NCPLN] = '\0';
		dp = (SYMBOL *)alloc(sizeof *dp + strlen(name) + 1);
		strcpy(dp->name, lsym.ls_id);
		dp->addr = lsym.ls_addr;
		dp->pcount = dp->ccount = 0;
		*dpp++ = dp;
	}
	*dpp = NULL;
	dsize = dpp - dict;
}

/*
 * Print usage message and die.
 */
void
usage()
{
	fprintf(stderr, "Usage: prof [ -abcs ] [ l.out [ mon.out ] ]\n");
	exit(1);
}

/*
 * Print nonfatal warning message.
 */
void
warning(str) char *str;
{
	fprintf(stderr, "prof: Warning: %r\n", &str);
}

/* end of /usr/src/cmd/prof.c */
