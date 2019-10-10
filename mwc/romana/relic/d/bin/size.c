/*
 * size.c
 */

/*
 * Object and archive file services.
 * This section is common to nm and size.
 * And should be common to ndis.
 */
#include <stdio.h>
#include <ar.h>
#include <canon.h>

#if COHERENT
#include <n.out.h>
#define DEFFILE	"l.out"
#endif
#if MSDOS
#include <nout.h>
#define DEFFILE	"l.exe"
#define	DOEXE	1
#endif
#if GEMDOS
#include <nout.h>
#define DEFFILE	"l.prg"
#define	DOPRG	1
#endif

char	*argv0;			/* Command name */
char	*fn;			/* File name */
FILE	*fp;			/* Input file */
struct	ldheader	ldh;	/* The l.out.h header */
struct	ldheader	skh;	/* Seek positions for segments */
struct	ldsym		lds;	/* Symbol buffer */
struct	ar_hdr		ahb;	/* Archive header buffer */
int	title;			/* Title each line */
int	amemb;			/* Archive member */
long	offset;			/* Into archive for start of l.out */
int	status;			/* Exit status */

/*
 * Open and close files.
 * If an archive arrange that (*ffp)()
 * is called on each member.
 * Otherwise call (*ffp)() on the file itself.
 */
ar(ffp)	int (*ffp)();
{
	int magic;

	amemb = 0;
	offset = 0;
	if ((fp = fopen(fn, "rb")) == NULL) {
		error("cannot open");
		return;
	}
	magic = getw(fp);
	canshort(magic);
	if (magic == ARMAG) {
		title = amemb = 1;
		while (fread(&ahb, sizeof(ahb), 1, fp) == 1) {
			offset = ftell(fp);
			if (strncmp(ahb.ar_name, HDRNAME, DIRSIZ) != 0)
				(*ffp)();
			cansize(ahb.ar_size);
			fseek(fp, offset+ahb.ar_size, 0);
		}
	} else {
		fseek(fp, 0L, 0);
		(*ffp)();
	}
	if (ferror(fp))
		error("read error");
	fclose(fp);
}

/*
 *	Read in the l.out header.
 */
gethdr()
{
	register int i;
	fread((char *)&ldh, 1, sizeof(ldh), fp);
	canshort(ldh.l_magic);
	canshort(ldh.l_flag);
	canshort(ldh.l_machine);
	if (ldh.l_magic != L_MAGIC)
		return 1;
	if ((ldh.l_flag&LF_32) == 0) {
		canshort(ldh.l_tbase);
		ldh.l_entry = ldh.l_tbase;
		ldh.l_tbase = sizeof(ldh) - 2*sizeof(short);
	} else {
		canshort(ldh.l_tbase);
		canlong(ldh.l_entry);
	}
	skh.l_ssize[L_SYM] = ldh.l_tbase;
	for (i=0; i<NLSEG; i++) {
		cansize(ldh.l_ssize[i]);
		if (i < L_SYM && i != L_BSSI && i != L_BSSD)
			skh.l_ssize[L_SYM] += ldh.l_ssize[i];
	}
	return (0);
}

/*
 * Read the exe header.
 */
#ifndef DOEXE
getexe() { return 1; }
#else
#include "exe.h"

execani(ip)	/* convert from MSDOS exe header byte order to host */
register short *ip;
{
	/* First convert to standard pdp-11 byte order */
	/* Notice, nothing to do to accomplish this    */

	/* Now convert from pdp-11 byte order to host */

	canshort(*ip);
}

getexe()
{
	long daddr;
	exehdr_t exehdr;

	fseek(fp, offset, 0);
	fread((char *)&exehdr, 1, sizeof(exehdr), fp);
	execani(&exehdr.x_magic);
	if (exehdr.x_magic != EXEMAGIC)
		return (1);
	execani(&exehdr.x_sectors);
	execani(&exehdr.x_bytes);
	daddr = (long) exehdr.x_sectors * 512;
	if ( exehdr.x_bytes != 0 )
		daddr += exehdr.x_bytes - 512;
	fseek(fp, offset+daddr, 0);
	if (gethdr() != 0)
		return 1;
	daddr += ldh.l_tbase + ldh.l_ssize[L_DEBUG];
	skh.l_ssize[L_SYM] = daddr;
	return 0;
}
#endif

/*
 *	Read a gemdos .prg header.
 */
#ifndef DOPRG
getprg() { return 1; }
#else
#include "gemout.h"
/* convert 68000 byte order to pdp11 byte order and vice versa */
#if PDP11
gcani(ip) register unsigned char *ip;
{
	register t;
	t = ip[0]; ip[0] = ip[1]; ip[1] = t;
}
gcanl(lp) register unsigned char *lp;
{
	register t;
	t = lp[0]; lp[0] = lp[1]; lp[1] = t;
	t = lp[2]; lp[2] = lp[3]; lp[3] = t;
}
#endif
#if M68000
#define gcani(i)	/* Nil */
#define gcanl(l)	/* Nil */
#endif

getprg()
{
	struct gemohdr ghd;
	register long daddr;
	register int c;
	fseek(fp, offset, 0);
	fread((char *)&ghd, 1, sizeof(ghd), fp);
	gcani(&ghd.g_magic);
	if (ghd.g_magic != GEMOMAGIC)
		return 1;
	gcanl(&ghd.g_ssize[0]);
	gcanl(&ghd.g_ssize[1]);
	gcanl(&ghd.g_ssize[2]);
	gcanl(&ghd.g_ssize[3]);
	daddr = sizeof(ghd) + ghd.g_ssize[0] + ghd.g_ssize[1] + ghd.g_ssize[3];
	fseek(fp, daddr+offset, 0);
	if (getw(fp) | getw(fp)) {
		while (c = getc(fp))
			if (c == EOF)
				return 1;
	}
	daddr = ftell(fp) - offset;
	if (gethdr() != 0)
		return 1;
	daddr += ldh.l_tbase + ldh.l_ssize[L_DEBUG];
	skh.l_ssize[L_SYM] = daddr;
	return 0;
}
#endif

/*
 * Print a title.
 */
dotitle(fp, tail) FILE *fp; char *tail;
{
	if (amemb)
		fprintf(fp, "%s(%.*s)", fn, DIRSIZ, ahb.ar_name);
	else
		fprintf(fp, "%s", fn);
	if (tail)
		fprintf(fp, "%s", tail);
}
/*
 * Give up.
 * Tag the line with the file
 * name.
 */
error(a)
{
	fprintf(stderr, "%s: ", argv0);
	dotitle(stderr, ": ");
	fprintf(stderr, "%r\n", &a);
	status = 1;
}

/*
 * Size specific code.
 */
int	cflag;		/* include common space indication */
int	tflag;		/* print totals for all files */
int	aflag;		/* print all segments */

long	tsize[NLSEG];
long	tcomm;
long	total;

char	octf[]	= "%06lo";
char	hexf[]	= "%04lx";
char	lngf[]	= "%08lx";
char	segf[]	= "%06lx";

char *afmt[] = {		/* Purloined from nm.c */
	octf,			/* Unused or unknown */
	octf,			/* 11 */
	lngf,			/* VAX */
	segf,			/* 360 */
	lngf,			/* Z-8001 */
	hexf,			/* Z-8002 */
	hexf,			/* 8086 */
	hexf,			/* 8080 and 8085 */
	hexf,			/* 6800 */
	hexf,			/* 6809 */
	lngf,			/* 68000 */
	lngf,			/* NS16000 */
	lngf			/* Large Model 8086 */
};

char	*format = octf;

#define	MTYPES	(sizeof(afmt)/sizeof(char *))

main(argc, argv) int argc; char *argv[];
{
	register char *p;
	register c, i;
	int nf;
	extern size();

	argv0 = argv[0];
	for (i = 1; i<argc && argv[i][0]=='-'; ++i) {
		p = &argv[i][1];
		while (c = *p++) {
			switch (c) {
			case 'a':	++aflag;	break;
			case 'c':	++cflag;	break;
			case 't':	++tflag;	break;
			default:	usage();	break;
			}
		}
	}
	if ((nf = argc-i) != 0) {
		if (nf > 1)
			title = 1;
		while (i < argc) {
			fn = argv[i++];
			ar(size);
		}
		if (title && tflag) {
			printf("Total: ");
			for (i=L_SHRI; i<NLSEG; ++i) {
				if (aflag == 0 && i == L_DEBUG)
					break;
				if (i != L_SHRI)
					putchar('+');
				printf("%ld", tsize[i]);
			}
			if (cflag && tcomm!=0)
				printf("+%ld", tcomm);
			printf(" %ld (", total);
			printf(format, total);
			printf(")\n");
		}
	} else {
		fn = DEFFILE;
		ar(size);
	}
	exit(status);
}

/*
 * Size.
 * You are seeked to the start of a file
 * which is not an archive.
 */
size()
{
	register int i;
	int machine;
	long common;
	long size;
	long tot;

	if (gethdr() != 0
	 && getexe() != 0
	 && getprg() != 0) {
		error("not an object file");
		return;
	}
	machine = ldh.l_machine;

	if ( machine >= MTYPES )
		machine = 0;
	format = afmt[machine];

	if (title)
		dotitle(stdout, ": ");
	tot = 0;
	for (i=L_SHRI; i<NLSEG; ++i) {
		size = ldh.l_ssize[i];
		tsize[i] += size;
		if (aflag != 0 || i < L_DEBUG) {
			if (i != L_SHRI)
				putchar('+');
			printf("%ld", size);
			tot += size;
		}
	}
	if (cflag) {
		common = 0;
		fseek(fp, skh.l_ssize[L_SYM]+offset, 0);
		if (ldh.l_flag & LF_32) {
			while (ldh.l_ssize[L_SYM]) {
				fread(&lds, sizeof(lds), 1, fp);
				canshort(lds.ls_type);
				canlong(lds.ls_addr);
				if (lds.ls_type == (L_GLOBAL|L_REF))
					common += (long)lds.ls_addr;
				ldh.l_ssize[L_SYM] -= sizeof(lds);
			}
		} else {
			while (ldh.l_ssize[L_SYM]) {
				fread(&lds, sizeof(lds)-sizeof(short), 1, fp);
				canshort(lds.ls_type);
				i = *(unsigned *)(&lds.ls_addr);
				canshort(i);
				if (lds.ls_type == (L_GLOBAL|L_REF))
					common += (unsigned)i;
				ldh.l_ssize[L_SYM] -= sizeof(lds)-sizeof(short);
			}
		}
		if (common != 0) {
			printf("+%ld", common);
			tot += common;
			tcomm += common;
		}
	}
	printf(" %ld (", tot);
	printf(format, tot);
	printf(")\n");
	total += tot;
}

usage()
{
	fprintf(stderr, "Usage: size [-act] [file ...]\n");
	exit(1);
}

/* end of size.c */
