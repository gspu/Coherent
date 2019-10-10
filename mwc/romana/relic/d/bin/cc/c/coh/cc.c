/*
 * cc.c
 * 11/3/90
 * CC command.
 * Compile, assemble and link edit C programs.
 * A lot of grunge.
 * Revised by rec may1987 to incorporate all
 * coherent and gemdos revisions to date.
 *
 * Defining VeryVflag will produce very verbose output under -VSTAT
 *	option
 *
 * C compiler/loader switch map.
 *	*7	marks a version seven documented option.
 *	*7d	marks a defunct version seven option.
 *	*7c	marks a changed version seven option.
 *	*u	marks an option unrecognized by cc, ie passed to linker
 *		with no interpretation or processing.
 *
 *7c	Bstring		use string to find compiler passes
 *	A		run in auto edit mode
 *7	Dname[=value]	preprocessor: #define
 *7	E		run preprocessor to stdout
 *7	Ipathname	preprocessor: #include search directory
 *	K		keep intermediate files in name.[P012so]
 *	L		loader: directive for >64K file sizes
 *	Mstring		use string as cross-compiler prefix
 *	N[01ab2sdlrt]string	rename pass with string
 *7	O		run object code optimiser
 *7d	P		put preprocessor output into name.i; use -Kqp
 *	Q		be quiet, make no messages
 *7	S		make assembly language output in name.s
 *7	Uname		preprocessor: #undef
 *	V		be verbose, report everything
 *	Vvariant	enable variant
 *	X		loader: remove c generated local symbols
 *	Z		(GEMDOS) floppy change prompts for phases
 *7	c		compile but do not load
 *	d		loader: define common space
 *7	e name		loader: entry point specification
 *7c	f		fake floating point operations
 *7u	i		loader: separate i and d spaces
 *	k[system]	loader: bind as kernel process
 *7c	lname		loader: library specification
 *7u	n		loader: shared instruction space
 *7	o name		loader: output file name
 *7	p		generate code to profile function calls
 *	q[p012s]	quit after specified pass
 *	r		loader: retain relocation in output
 *	s		loader: strip symbol table
 *7c	t[p012adlrt]	take specified passes from -Bdirectory
 *7	u name		loader: enter name into symbol table
 *	w		loader: watch
 *	x		loader: remove local symbols from symbol table
 */
#if GEMDOS
#ifndef VERS
#define VERS	"2.1"
#endif
#endif

#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <path.h>
#include <errno.h>
#include "mch.h"
#undef NONE
#include "var.h"
#include "varmch.h"

#ifndef VeryVflag
#define VeryVflag 0
#endif

#ifndef PREFIX
#define PREFIX	""
#endif

/*
** Pass information.
*/
#define NONE	-1
#define CPP	0	/* Pass index numbers */
#define CC0	1
#define CC1	2
#define CC2	3
#define CC3	4
#define CC4	5	/* Output writer postprocessor */
#define AS	6
#define LD	7
#define LD2	8	/* Loader postprocessor */
#define ED	9
#define LIB	10
#define CRT	11
#define TMP	12
#define ALL	13

#define P_TAKE	1	/* Take pass from backup directory */
#define P_BACK	2	/* Backup directory specified */
#define P_LIB	4	/* Take pass from LIBPATH */
#define P_BIN	8	/* Take pass from BIN */

#define PTMP	16	/* Pass name buffer size */

char	dnul[] = "";		/* Global null string */
char	dmch[PTMP] = PREFIX;	/* Cross compiler prefix */

struct pass {
	char p_flag;		/* Flags */
	char p_psn;		/* Pass short name */
	char p_pln[PTMP];	/* Pass long name */
	char p_pfs[4];		/* Pass output file suffix */
	char *p_ifn;		/* Input file name */
	char *p_ofn;		/* Output file name */
	char *p_sfn;		/* Scratch file name */
	char *p_dir;		/* Path lookup or backup string */
	char *p_mch;		/* Machine prefix name */
} pass[] = {
	{ P_LIB, 'p', "cpp",  "i",	NULL, NULL, NULL, NULL,	dmch },
	{ P_LIB, '0', "cc0",  "0",	NULL, NULL, NULL, NULL,	dmch },
	{ P_LIB, '1', "cc1",  "1",	NULL, NULL, NULL, NULL,	dmch },
	{ P_LIB, '2', "cc2",  "o",	NULL, NULL, NULL, NULL,	dmch },
	{ P_LIB, '3', "cc3",  "s",	NULL, NULL, NULL, NULL,	dmch },
	{ P_LIB, '4', "cc4",  "o",	NULL, NULL, NULL, NULL,	dmch },
	{ P_BIN, 's', "as",   "o",	NULL, NULL, NULL, NULL,	dmch },
	{ P_BIN, 'd', "ld",   "",	NULL, NULL, NULL, NULL,	dnul },
	{ P_BIN, 'x', "ld2",  "",	NULL, NULL, NULL, NULL,	dnul },
	{ P_BIN, 'e', "me",   "",	NULL, NULL, NULL, NULL,	dnul },
	{ P_LIB, 'l', "lib",  "a",	NULL, NULL, NULL, NULL,	dmch },
	{ P_LIB, 'r', "crts0.o", "",	NULL, NULL, NULL, NULL,	dmch },
	{     0, 't', "cc",    "",	NULL, NULL, NULL, NULL,	dnul }
};

/*
** Option and argument information.
*/
#define CCOPT	0x001		/* Argument flags in argf[] */
#define PPOPT	0x002		/* Also in option table, at least */
#define LDOPT	0x004		/* those that fit in a byte */
#define LD2OPT	0x008
#define CCLIB	0x010
#define LDLIB	0x020
#define CCARG	0x040
#define ASARG	0x080
#define MARG	0x100
#define LDARG	0x200

#define FLAG_c	1		/* -c flag */
#define FLAG_O	2		/* -O flag */
#define FLAG_f	4		/* -f flag */
#define FLAG_K	010		/* -K or -VKEEP flag */
#define VS	020		/* Turn on strict messages */
#define VDB	040		/* Turn on debugging */
#define FLAG_A	0100		/* Auto edit mode */
#define FLAG_Z	0200		/* Floppy change prompts */
#define FLAG_V	0400		/* Verbose flag */

#define FLAG_GEMAPP	01000	/* Gem application compile */
#define FLAG_GEMACC	02000	/* Gem accessory compile */

struct option {			/* option table */
	char o_kind;
	char o_flag;
	char *o_name;
	int o_bits;
} option[] = {
/* Strict */
	{ 0,	CCOPT,	"VSUREG",	VSUREG	},
	{ 0,	CCOPT,	"VSUVAR",	VSUVAR	},
	{ 0,	CCOPT,	"VSNREG",	VSNREG	},
	{ 0,	CCOPT,	"VSRTVC",	VSRTVC	},
	{ 0,	CCOPT,	"VSMEMB",	VSMEMB	},
	{ 0,	CCOPT,	"VSBOOK",	VSBOOK	},
	{ 0,	CCOPT,	"VSLCON",	VSLCON	},
	{ 0,	CCOPT,	"VSPVAL",	VSPVAL	},
	{ 0,	CCOPT,	"VSCCON",	VSCCON	},
/* Debug */
	{ 0,	CCOPT,	"VDEBUG",	VDEBUG  },
	{ 0,	CCOPT,	"VDLINE",	VLINES	},
	{ 0,	CCOPT,	"VDTYPE",	VTYPES	},
	{ 0,	CCOPT,	"VDSYMB",	VDSYMB	},
	{ 0,	CCOPT,	"VDCALL",	VCALLS	},
/* Miscellaneous */
	{ 0,	CCOPT,	"VSTAT",	VSTAT	},
	{ 0,	CCOPT,	"VSINU",	VSINU	},
	{ 0,	CCOPT,	"VPEEP",	VPEEP	},
	{ 0,	CCOPT,	"VCOMM",	VCOMM	},
	{ 0,	CCOPT,	"VQUIET",	VQUIET	},
	{ 0,	CCOPT,	"VPSTR",	VPSTR	},
	{ 0,	CCOPT,	"VROM",		VROM	},
	{ 0,	CCOPT,	"VASM",		VASM	},
	{ 0,	CCOPT,	"VPROF",	VPROF	},
	{ 0,	CCOPT,	"VALIEN",	VALIEN	},
	{ 0,	CCOPT,	"VNOOPT",	VNOOPT	},
	{ 0,	CCOPT,	"VCHASM",	VCHASM	},
	{ 0,	CCOPT,	"VCPP",		VCPP	},
	{ 0,	CCOPT,	"VCPPE",	VCPPE	},
	{ 0,	CCOPT,	"VTPROF",	VTPROF	},
	{ 0,	CCOPT,	"V3GRAPH",	V3GRAPH	},
#if 1
/* Intel flags */
	{ 0,	CCOPT,	"VSMALL",	VSMALL	},
	{ 0,	CCOPT,	"VLARGE",	VLARGE	},
	{ 0,	CCOPT,	"V8087",	V8087	},
	{ 0,	CCOPT,	"VRAM",		VRAM	},
	{ 0,	CCOPT,	"VOMF",		VOMF	},
	{ 0,	CCOPT,	"V80186",	V80186	},
	{ 0,	CCOPT,	"V80287",	V80287	},
	{ 0,	CCOPT,	"VALIGN",	VALIGN	},
	{ 0,	CCOPT,	"VEMU87",	VEMU87	},
#endif
/* Motorola and Gemdos flags */
	{ 12,	CCOPT,	"VGEMACC",	FLAG_GEMACC	},
	{ 12,	CCOPT,	"VGEMAPP",	FLAG_GEMAPP	},
	{ 12,	CCOPT,	"VGEM",		FLAG_GEMAPP	},
	{ 10,	CCOPT,	"VSPLIM",	VSPLIM	},
	{ 10,	CCOPT,	"VNOTRAPS",	VNOTRAPS	},
/* More miscellaneous flags */
	{ 2,	CCOPT,	"VDB",		VDB	},
	{ 2,	CCOPT,	"VS",		VS	},
	{ 12,	CCOPT,	"A",		FLAG_A	},
	{ 12,	CCOPT,	"Z",		FLAG_Z	},
	{ 12,	CCOPT,	"c",		FLAG_c	},
	{ 10,	CCOPT,	"S",		VASM	},
	{ 2,	CCOPT,	"O",		FLAG_O	},
	{ 0,	CCOPT,	"E",		VCPPE	},
	{ 12,	CCOPT,	"f",		FLAG_f	},
	{ 12,	CCOPT,	"VFLOAT",	FLAG_f	},
	{ 0,	CCOPT,	"p",		VPROF	},
	{ 12,	CCOPT,	"VKEEP",	FLAG_K	},
	{ 12,	CCOPT,	"K",		FLAG_K	},
	{ 0,	CCOPT,	"Q",		VQUIET	},
	{ 12,	CCOPT,	"V",		FLAG_V	},
	{ 12,	CCOPT,	"v",		FLAG_V	},
/* Preprocessor options */
	{ 3,	PPOPT,	"D"	},
	{ 3,	PPOPT,	"I"	},
	{ 3,	PPOPT,	"U"	},
/* Loader options */
	{ 4,	LDOPT,	"d"	},
	{ 4,	LDOPT,	"i"	},
	{ 4,	LDOPT,	"n"	},
	{ 5,	LDOPT,	"o"	},
	{ 4,	LDOPT,	"r"	},
	{ 4,	LDOPT,	"s"	},
	{ 4,	LDOPT,	"w"	},
	{ 4,	LDOPT,	"x"	},
	{ 4,	LDOPT,	"X"	},
#if 0
	{ 4,	LDOPT,	"L"	},
#endif
	{ 5,	LDOPT,	"e"	},
	{ 5,	LDOPT,	"k"	},
	{ 5,	LDOPT,	"u"	},
/* C dispatcher options */
	{ 6,	CCOPT,	"q"	},
	{ 7,	CCOPT,	"t"	},
	{ 7,	CCOPT,	"B"	},
	{ 7,	CCOPT,	"M"	},
	{ 7,	CCOPT,	"N"	},
	{ 8,	CCLIB,	"l"	},

	{ -1,	-1,	""	}
};

int	ccvariant = 0;			/* Variant template */
VARIANT	variant;			/* Binary variant template */
char	vstr[2*VMAXIM/VGRANU + 1];	/* Ascii variant string */
char	vstr2[2*VMAXIM/VGRANU + 1];	/* Ascii variant string for .m */

/*
** Compiler parameters and flags
*/
#define	NCMDA	128		/* Size of argument pointer buffer */
#define	NCMDB	1024		/* Size of argument buffer */
#define	NTMP	L_tmpnam	/* Temp file buffer size, from stdio.h */
#define	MAGIC	127		/* Exit status mask */
#define	NSLEEP	10		/* # of sleeps */
#define	DELAY	5		/* Sleep delay, seconds */

int	nldob;			/* Number of objects created */
int	nfile;			/* Number of files to compile or assemble */
int	nname;			/* Number of file name arguments */
int	ndota;
int	ndotc;
int	ndotm;
int 	ndoto;
int	ndots;
int	partial;			/* Partial link specified */

#define	cflag	((ccvariant&FLAG_c)!=0)
#define pflag	isvariant(VPROF)
#define	fflag	((ccvariant&FLAG_f)!=0)
#define	Kflag	((ccvariant&FLAG_K)!=0)
#define	Oflag	((ccvariant&FLAG_O)!=0)
#define Sflag	isvariant(VASM)
#define	Eflag	isvariant(VCPPE)
#define Vflag	((ccvariant&FLAG_V)!=0)
#define	Qflag	isvariant(VQUIET)
#define	Aflag	((ccvariant&FLAG_A)!=0)
#define	Zflag	((ccvariant&FLAG_Z)!=0)
#define GEMAPPflag	((ccvariant&FLAG_GEMAPP)!=0)
#define GEMACCflag	((ccvariant&FLAG_GEMACC)!=0)

int	qpass = NONE;		/* Quit after this pass */
int	nload;			/* No load phase */
char	*outf;			/* Output file */
char	*doutf;			/* Default output file stem */
char	*dpath = DEFPATH;
char	*dlibpath = DEFLIBPATH;
int	xstat;			/* Exit status */

char	*cmda[NCMDA];		/* Argument pointer buffer */
char	cmdb[NCMDB];		/* Argument buffer */
char	newo[NTMP];		/* New object to be removed */
char	tmp[7][NTMP];		/* Intermediate filenames */
char	istmp[7];		/* Truth about their temporariness */
int	argf[NCMDA];		/* Argument flags */
char	optb[NCMDB];		/* Option rewrite buffer */
char	*optp = &optb[0];	/* Option rewrite position */

char	*makepass();
char	*makelib();
int	cleanup();
char	*getenv();
char	*tempnam();
char	*path();

main(argc, argv)
char *argv[];
{
	register char *p;
	register struct option *op;
	int i, narg, c;

#if COHERENT
	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		signal(SIGINT, cleanup);
#endif
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);
	setvariant(VSUVAR);
	setvariant(VSMEMB);
	setvariant(VSLCON);
	setvariant(VSPVAL);
	setvariant(VPEEP);
	setvariant(VCOMM);
	setvariant(V80186);

	if (p = getenv("PATH")) dpath = p;
	if (p = getenv("LIBPATH")) dlibpath = p;
	if (p = getenv("EDITOR")) strcpy(pass[ED].p_pln, p);

	for (i=1; i<argc; i+=narg) {
		narg = 1;
		p = argv[i];
		if (*p++ == '-') {
			static char tppopt[64] = "-";
			static char tldopt[64] = "-";
			char *tpp = &tppopt[1];
			char *tlp = &tldopt[1];

			if (*p == '\0') {
		badopt:
				whatopt(argv[i]);
				continue;
			}
			while (*p != '\0') {
				for (op = &option[0]; ; op += 1)
					if (op->o_name[0] == '\0')
						goto badopt;
					else if (opeq(op->o_name, p))
						break;
				if (VeryVflag && Vflag)
					printf("Option: -%s\n", op->o_name);
				argf[i] |= op->o_flag;
				switch (op->o_kind) {
				case 0:
					chgvariant(op->o_bits);
					p += strlen(op->o_name);
					continue;
				case 10:
					setvariant(op->o_bits);
					p += strlen(op->o_name);
					continue;
				case 2:
					ccvariant ^= op->o_bits;
					p += strlen(op->o_name);
					/* Set strict */
					if ((ccvariant&VS) != 0) {
						for (op = &option[0];; op += 1)
							if (op->o_name[1]=='S')
								break;
						while (op->o_name[1]=='S') {
							setvariant(op->o_bits);
							op += 1;
						}
					}
					/* Set debug */
					if ((ccvariant&VDB) != 0) {
						for (op = &option[0];; op += 1)
							if (op->o_name[1]=='D')
								break;
						while (op->o_name[1]=='D') {
							setvariant(op->o_bits);
							op += 1;
						}
					}
					continue;
				case 12:
					ccvariant |= op->o_bits;
					p += strlen(op->o_name);
					continue;
				case 3:
					while (*tpp++ = *p)
						p++;
					continue;
				case 5:
					if (*p == 'o') {
						outf = argv[i+narg];
					}
					if (*p == 'u')
						ndoto += 1;
					if (i+narg >= argc)
						missingname();
					argf[i+narg++] = LDOPT;
					/* Fall through */
				case 4:
					if (*p == 'r')
						++partial;
					strcpy(tlp, op->o_name);
					tlp += strlen(op->o_name);
					p += strlen(op->o_name);
					continue;
				case 6:
					if ((c = p[1]) == '\0'
					 || (c = getpsn(c)) < CPP)
						goto badopt;
					qpass = c;
					p += 2;
					continue;
				case 7:
					c = p[0];
					getpass(c, p+1);
					p = dnul;
					continue;
				case 8:
					p = dnul;
					continue;
				default:
					cfatal("options");
				}
			}
			if (((unsigned)argf[i]-1)&((unsigned)argf[i])) {
				strcpy(optp, argv[i]);
				argv[i] = optp;
				optp += strlen(optp)+1;
				strcpy(optp, tppopt);
				optp += strlen(optp)+1;
				strcpy(optp, tldopt);
				optp += strlen(optp)+1;
			}
		} else {
			while (*p) p+=1;
			c = *--p;
			if (*--p != '.') c = 0;
			if (c >= 'A' && c <= 'Z')
				c |= 'a'-'A';
			switch (c) {
			case 'h':
			case 'c':
				if (doutf == NULL) doutf = argv[i];
				ndotc += 1;
				argf[i] = CCARG;
				if (VeryVflag && Vflag)
					printf("cc file: %s\n", argv[i]);
				break;
			case 's':
				if (doutf == NULL) doutf = argv[i];
				ndots += 1;
				argf[i] = ASARG;
				if (VeryVflag && Vflag)
					printf("as file: %s\n", argv[i]);
				break;
			case 'm':
				if (doutf == NULL) doutf = argv[i];
				ndotm += 1;
				argf[i] = MARG;
				if (VeryVflag && Vflag)
					printf("m file: %s\n", argv[i]);
				break;
			case 'o':
				if (doutf == NULL) doutf = argv[i];
				ndoto += 1;
				argf[i] = LDARG;
				if (VeryVflag && Vflag)
					printf("ld file: %s\n", argv[i]);
				break;
			case 'a':
			default:
				ndota += 1;
				argf[i] = LDLIB;
				if (VeryVflag && Vflag)
					printf("library: %s\n", argv[i]);
				break;
			}
		}
	}
#if GEMDOS
	if (Vflag) {
	 fprintf(stderr, "Mark Williams C for the Atart ST, %s\n", VERS);
	 fprintf(stderr, "Copyright 1984-1987, Mark Williams Co., Chicago\n");
	}
#endif
	resolve();
	compile(argc, argv);
	if (nload==0 && runld(argc, argv)==0) {
		if (Kflag==0 && nldob==1 && newo[0]!=0)
			rm(newo);
	}
	exit(xstat);
}

/*
 * Resolve remaining ambiguities.
 * Initialize variant arguments and files.
 */
resolve()
{
	register int i;
	extern char *index();

	for (i = CPP; i < ALL; i += 1) {
		if (pass[i].p_flag & P_BACK)
			;
		else if (pass[i].p_flag & P_LIB)
			pass[i].p_dir = dlibpath;
		else if (pass[i].p_flag & P_BIN)
			pass[i].p_dir = dpath;
		strcpy(cmdb, pass[i].p_mch);
		strcat(cmdb, pass[i].p_pln);
#if GEMDOS
		if (i <= ED && index(pass[i].p_pln, '.') == NULL)
			strcat(cmdb, i==ED ? ".tos" : ".prg");
#endif
		if (strlen(cmdb) > PTMP-1)
			cfatal("pass name \"%s\" is too long", cmdb);
		strcpy(pass[i].p_pln, cmdb);
	}
#if 1
	if (isvariant(VOMF)) {
		clrvariant(VCOMM);
		if (isvariant(VLARGE) && isvariant(VSMALL))
			cfatal("invalid model specification");
		if (notvariant(VLARGE) && notvariant(VSMALL))
			setvariant(VSMALL);
	} else {
		clrvariant(VLARGE);
		setvariant(VSMALL);
	}
#endif
	if (GEMAPPflag && GEMACCflag)
		cfatal("specify VGEMAPP or VGEMACC, not both");
	if (GEMAPPflag) getpass('N', "rcrtsg.o");
	if (GEMACCflag) getpass('N', "rcrtsd.o");
	if (pflag) getpass('N', "rmcrts0.o");
	if (Eflag) setvariant(VCPP);
	if (qpass == NONE) {
		if (isvariant(VCPP))
			qpass = CPP;
		else if (Sflag)
			qpass = CC3;
		else if (cflag)
			qpass = AS;
		else
			qpass = LD;
	} else if (qpass == CPP)
		setvariant(VCPP);
	if (VeryVflag && Vflag)
		printf("quit pass is %s\n", pass[qpass].p_pln);
	makvariant(vstr);
	if (ndotm != 0 && notvariant(VCPP) && notvariant(VCPPE)) {
		setvariant(VCPP);
		setvariant(VCPPE);
		makvariant(vstr2);
		clrvariant(VCPP);
		clrvariant(VCPPE);
	}
	nfile = ndotc + ndots + ndotm;
	nname = nfile + ndoto + ndota;
	if (nname == 0) {
		fprintf(stderr,
			"Usage: cc [ -o output ] [ options ] file [ ... ]\n");
		exit(1);
	}
	if (qpass < LD)
		++nload;
	if (Aflag)
		maketempfile(6);
	if ( ! Eflag) {
		chkofile();
		if (Kflag) {
			pass[CPP].p_ofn = pass[CC0].p_ifn = tmp[0];
			pass[CC0].p_ofn = pass[CC1].p_ifn = tmp[1];
			pass[CC1].p_ofn = pass[CC2].p_ifn = tmp[2];
			if ( ! Sflag) {
				pass[CC2].p_sfn = tmp[3];
				pass[CC2].p_ofn = tmp[5];
			} else {
				pass[CC3].p_ifn = pass[CC2].p_ofn = tmp[3];
				pass[CC3].p_ofn = tmp[5];
			}
		} else {
			pass[CPP].p_ofn = pass[CC0].p_ifn = tmp[0];
			pass[CC0].p_ofn = pass[CC1].p_ifn = tmp[1];
			pass[CC1].p_ofn = pass[CC2].p_ifn = tmp[0];
			if ( ! Sflag) {
				pass[CC2].p_sfn = tmp[1];
				pass[CC2].p_ofn = tmp[5];
			} else {
				pass[CC3].p_ifn = pass[CC2].p_ofn = tmp[1];
				pass[CC3].p_ofn = tmp[5];
			}
			maketempfile(0);
			maketempfile(1);
		}
		pass[AS].p_ofn = tmp[5];
	}
}

maketempfile(i) int i;
{
	char *p;
	strcpy(tmp[i], p = tempnam(pass[TMP].p_dir, pass[TMP].p_pln));
	free(p);
	++istmp[i];
}

/*
** Compilation.
*/
compile(argc, argv)
char *argv[];
{
	register char *p1;
	register c, i, err;

	err = 0;
	for (i=1; i<argc; ++i) {
		if ((c = (argf[i] & (CCARG | ASARG | MARG))) == 0)
			continue;
		p1 = argv[i];
		if (err) {
			err = 0;
			if (runme(p1)) {
				++nload;
			} else {
				--i;
				xstat = 0;
			}
			continue;
		}
		if (nfile > 1 && Qflag == 0)
			printf("%s:\n", p1);
		setfiles(c, p1);
		if (c == CCARG) {
			if (runpp(argc, argv, vstr)) {
				if (Aflag) {
					++err; --i;
				} else
					++nload;
				continue;
			}
			if (qpass <= CC0)
				continue;
			if (runcc(CC1)) {
				++nload;
				continue;
			}
			if (qpass <= CC1)
				continue;
			if (runcc(CC2)) {
				++nload;
				continue;
			}
			if (qpass <= CC2)
				continue;
			if (Sflag) {
				if (runcc(CC3)) {
					++nload;
					continue;
				}
				if (qpass <= CC3)
					continue;
			}
		} else if (c == ASARG && qpass>=AS) {
			if (runas()) {
				if (Aflag) {
					++err; --i;
				} else
					++nload;
				continue;
			}
			if (qpass <= AS)
				continue;
		} else if (c == MARG) {
			if (runpp(argc, argv, vstr2)) {
				if (Aflag) {
					++err; --i;
				} else
					++nload;
				continue;
			}
			if (qpass < AS)
				continue;
			if (runas()) {
				if (Aflag) {
					++err; --i;
				} else
					++nload;
				continue;
			}
			if (qpass == AS)
				continue;
		}
		makeft(p1, p1, "o");
	}
	cleanup(0);
}

runpp(argc, argv, var)
register char *argv[];
char var[];
{
	register char **cp;
	register int i;
	char *p1;

	cp = cmda;
	p1 = cmdb;
	p1 = makepass(CC0, *cp++ = p1, AEXEC);
	*cp++ = var;
	*cp++ = pass[CPP].p_ifn;
	if (qpass < CC0)
		*cp++ = Kflag ? pass[CPP].p_ofn : "-";
	else
		*cp++ = pass[CC0].p_ofn;
	for (i=1; i<argc; ++i) {
		if ((argf[i]&PPOPT) != 0) {
			*cp++ = argv[i];
			if ((argf[i]&~PPOPT) != 0)
				cp[-1] += strlen(cp[-1])+1;
		}
	}
	*cp = 0;
	return (run(Aflag));
}

runcc(pn)
{
	register char **cp;

	cp = cmda;
	makepass(pn, *cp++ = cmdb, AEXEC);
	*cp++ = vstr;
	*cp++ = pass[pn].p_ifn;
	*cp++ = pass[pn].p_ofn;
	if (pn == CC2)
		*cp++ = pass[CC2].p_sfn;
	*cp = 0;
	return (run(0));
}

runas()
{
	register char **cp;

	cp = cmda;
	makepass(AS, *cp++ = cmdb, AEXEC);
	*cp++ = "-gx";
	*cp++ = "-o";
	*cp++ = pass[AS].p_ofn;
	*cp++ = pass[AS].p_ifn;
	*cp = 0;
	return (run(Aflag));
}

runme(fn)
char *fn;
{
	register char **cp;

	cp = cmda;
	makepass(ED, *cp++ = cmdb, AEXEC);
	*cp++ = fn;
	*cp++ = "-e";
	*cp++ = tmp[6];
	*cp = 0;
	return (run(0));
}
runld(argc, argv)
char *argv[];
{
	char *p1;
	register char *p2;
	register char **cp;
	char **cp1;
	register char **cp2;
	int i;
	static char buff[32];

	cp = cmda;
	p1 = makepass(LD, *cp++ = cmdb, AEXEC);
	*cp++ = "-X";
	if (outf == NULL) {
		*cp++ = "-o";
		if (partial || doutf == NULL) {
#if COHERENT
			*cp++ = "l.out";
#endif
#if GEMDOS
			*cp++ = "l.prg";
#endif
		} else {
			basename(doutf, buff);
#if GEMDOS
			strcat(buff, ".prg");
#endif
			*cp++ = buff;
		}
	}
	for (i=1; i<argc; ++i) {
		if ((argf[i]&LDOPT)!=0) {
			*cp++ = argv[i];
			if ((argf[i]&~LDOPT)!=0) {
				cp[-1] += strlen(cp[-1])+1;
				cp[-1] += strlen(cp[-1])+1;
			}
		}
	}
	cp1 = cp;
	p1 = makepass(CRT, *cp++ = p1, AREAD);
	if (fflag) {
		*cp++ = "-u";
		*cp++ = "_dtefg_";
	}
	for (i=1; i<argc; ++i) {
		p2 = argv[i];
		if ((argf[i]&CCLIB)!=0) {
			while (*p2++ != 'l');
			p1 = makelib(LIB, *cp++ = p1, p2);
		} else if ((argf[i]&(CCARG|ASARG|MARG|LDARG))!=0) {
			for (cp2=cp1; cp2<cp; ++cp2)
				if (strcmp(*cp2, p2) == 0)
					break;
			if (cp2 == cp) {
				*cp++ = p2;
				++nldob;
			}
		} else if ((argf[i]&LDLIB)!=0) {
			*cp++ = p2;
		}
	}
	if ( ! partial && (GEMAPPflag || GEMACCflag)) {
		p1 = makelib(LIB, *cp++ = p1, "aes");
		p1 = makelib(LIB, *cp++ = p1, "vdi");
	}
	p1 = makelib(LIB, *cp++ = p1, "c");
	*cp = 0;
	return (run(0));
}

basename(in, out)
char *in, *out;
{
	register char *ip = in, *op = out;
	register int c;

	while (*ip++)
		;
	ip -= 2;
	while (ip >= in && *ip != PATHSEP)
		--ip;
	while ((c = *++ip) && c != '.')
		*op++ = c;
	*op = '\0';
}

run(catch_errors) int catch_errors;
{
	int new_fd;
	int saved_fd;
	int s;
	extern char **environ;

	if (Vflag) {
		char *cp, **cpp;
		cpp = cmda;
		while ((cp = *cpp) != NULL) {
			if (cpp != cmda)
				putchar(' ');
			printf("%s", cp);
			++cpp;
		}
		putchar('\n');
		fflush(stdout);
	}
	if (catch_errors) {
		fflush(stdout);
		saved_fd = dup(1);
		if ((new_fd = creat(tmp[6], 0644)) < 0)
			cfatal("%s: %s", tmp[6], sys_errlist[errno]);
		if (dup2(new_fd, 1) < 0)
			cfatal("dup2 failed");
	}
#if	COHERENT
	{
		int nsleep;
		int status;
		int pid;
		nsleep = NSLEEP;
		while ((pid=fork())<0 && nsleep--)
			sleep(DELAY);
		if (pid < 0)
			cfatal("can't fork");
		if (pid == 0) {
			execve(cmda[0], cmda, environ);
			exit(MAGIC);
		}
		while (wait(&status) != pid)
			;
		if ((s = status&0177)!=0 && s!=SIGINT) {
			extern char *signame[];

			fprintf(stderr, "cc: %s ", cmda[0]);
			if (s==SIGSYS && (status&0200)==0)
				fprintf(stderr, "exec failed");
			else
				fprintf(stderr, signame[s]);
			if ((status&0200) != 0)
				fprintf(stderr, " -- core dumped");
			fprintf(stderr, "\n");
		} else if ((s = (status>>8)&0377) == MAGIC)
			cfatal("cannot execute %s", cmda[0]);
	}
#else
	if (s = execve(cmda[0], cmda, environ))
		if (s < 0)
			perror(cmda[0]);
#endif
	if (catch_errors) {
		fflush(stdout);
		if (dup2(saved_fd, 1) < 0)
			cfatal("dup2 failed");
		if (close(new_fd) < 0)
		/* cfatal("error on close") */ ;
	}
	if (s != 0)
		xstat = 1;
	return (s);
}

/*
** Option processing.
*/

/*
 * Compare option name to argument.
 */
opeq(op, ap)
register char *op, *ap;
{
	while (*op!=0)
		if (*op++ != *ap++)
			return (0);
	return (1);
}


/*
 * Get short pass name.
 *	translate single character code into pass index number.
 */
getpsn(c)
register int c;
{
	register int pn;

	for (pn = CPP; pn < ALL; pn += 1)
		if (pass[pn].p_psn == c)
			return (pn);
	return (NONE);
}

/*
 * Process -t*, -B*, -M* options.
 *	-t*	sets take flag for specified passes.
 *	-B*	copies prefix string for flagged passes
 *		or for all machine dependent passes if none flagged.
 *	-M*	copies machine string for machine dependent passes.
 *	-N*	renames a pass, such as crts0.o.
 */
getpass(c, cp)
register int c;
register char *cp;
{
	if (c == 't') {		/* Take passes from backup */
		if (*cp != 0)
			while ((c = *cp++) != 0)
				if ((c = getpsn(c)) >= CPP) {
					pass[c].p_flag |= P_TAKE;
					pass[c].p_dir = dnul;
					if (Vflag)
						passrpt(c);
				} else {
					badpsn(cp[-1]);
				}
		else
			getpass('t', "01234");
	} else if (c == 'B') {	/* Get backup string */
		for (c = CPP; c < ALL; c += 1) {
			if ((pass[c].p_flag&P_TAKE) != 0) {
				pass[c].p_flag ^= P_TAKE;
				pass[c].p_flag |= P_BACK;
				pass[c].p_dir = cp;
				if (Vflag)
					passrpt(c);
			}
		}
	} else if (c == 'M') {	/* Machine prefix string */
		if (strlen(cp) > PTMP-1)
			toolong(cp);
		strcpy(dmch, cp);
		if (Vflag)
			printf("Prefix: %s\n", dmch);
	} else if (c == 'N') {	/* New pass name */
		if ((c = getpsn(*cp++)) >= CPP) {
			if (strlen(cp) > PTMP-1)
				toolong(cp);
			if (Vflag)
				printf("Rename: %s to %s\n", pass[c].p_pln, cp);
			strcpy(pass[c].p_pln, cp);
		} else
			badpsn(cp[-1]);
	}
}

passrpt(c)
register int c;
{
	printf("Use: {%s}%s%s\n", pass[c].p_dir, pass[c].p_mch, pass[c].p_pln);
}

missingname()
{
	fprintf(stderr, "cc: missing name in -o, -e or -u\n");
	exit(1);
}

badoutput()
{
	fprintf(stderr, "cc: improbable name in -o: %s\n", outf);
	exit(1);
}

toomany()
{
	fprintf(stderr, "cc: too many files for -o option\n");
	exit(1);
}

badpsn(c)
{
	fprintf(stderr, "cc: unknown short path name: %c\n", c);
	exit(1);
}

toolong(cp)
char *cp;
{
	fprintf(stderr, "cc: name, prefix, or directory too long: %s\n", cp);
	exit(1);
}

/*
 * Check specified output file name,
 * or construct name from first file
 * name seen.
 */
chkofile()
{
	register char *fn;
	register char c;

	if ((fn = outf) == NULL)
		return;
	if (fn[0] == '-')
		badoutput();
	if (nname > 1 && qpass < LD)
		toomany();
	while (*fn != 0)
		++fn;
	while (*fn != '.' && fn > outf)
		--fn;
	if (*fn++ == '.' && (c = *fn++) &&  *fn == '\0') {
		if (c == 'c'
		|| c == 'h'
		|| c == 'm'
		|| (c != 'o' && cflag)
		|| (c == 'o' && !cflag)
		|| (c != 's' && Sflag)
		|| (c == 's' && !Sflag))
			badoutput();
	} else {
		if (Sflag || cflag)
			badoutput();
	}
}

whatopt(s)
char *s;
{
	fprintf(stderr, "cc: unrecognized option: %s\n", s);
	exit(1);
}

cfatal(s)
char *s;
{
	fprintf(stderr, "cc: fatal error: %r\n", &s);
	cleanup(0);
	exit(1);
}

/*
** Intermediate file processing.
*/
/*
 * Given cp -> `name.[chms]' rewrite ultimate destination and intermediates
 * if necessary.
 */
setfiles(c, cp)
int c;
register char *cp;
{
	register char *ip;

	ip = cp;
	if (qpass < LD && outf != NULL)
		cp = outf;
	if (c == CCARG) {
		pass[CPP].p_ifn = ip;
		if (Eflag)
			return;
		if (Kflag) {
			makeft(pass[CPP].p_ofn, cp, "i");
			makeft(pass[CC0].p_ofn, cp, "0");
			makeft(pass[CC1].p_ofn, cp, "1");
			if ( ! Sflag)
				makeft(pass[CC2].p_sfn, cp, "2");
			else
				makeft(pass[CC2].p_ofn, cp, "2");
		}
	} else if (c == MARG) {
		pass[CPP].p_ifn = ip;
		if (Kflag)
			makeft(pass[CC0].p_ofn, cp, "s");
		pass[AS].p_ifn = pass[CC0].p_ofn;
	} else if (c == ASARG)
		pass[AS].p_ifn = ip;
	makeft(tmp[5], cp, Sflag ? "s" : "o");
	if (!Sflag && newo[0] == 0 && access(tmp[5], 0) < 0)
		strcpy(newo, tmp[5]);
}

/*
 * Unlink a file.
 * If `-V' echo the rm.
 */
rm(s)
register char *s;
{
	if (Vflag)
		printf("rm %s\n", s);
	unlink(s);
}

/*
 * Cleanup after yourself if
 * the user hits interrupt during a compile (sig == SIGINT),
 * or when all compiles have completed (sig == 0).
 */
cleanup(sig)
{
	register int i;

#if COHERENT
	if (sig == SIGINT)
		signal(SIGINT, SIG_IGN);
#endif
	for (i = 0; i < 7; i += 1)
		if (istmp[i])
			rm(tmp[i]);
#if COHERENT
	if (sig == SIGINT && nldob==1 && newo[0]!=0)
		rm(newo);
	if (sig == SIGINT)
		exit(1);
#endif
}

/*
** Name construction.
*/
char *
ccpath(cp, p, n, mode) register char *cp; char *p, *n; int mode;
{
	register char *pn;
	if (VeryVflag && Vflag)
		printf("search %s for %s\n", p, n);
	while ((pn = path(p, n, mode)) == NULL) {
		if (Zflag) {	/* Disk swap dialog flag */
fprintf(stderr, "Insert %s's disk in a free drive and press <Return>\n", n);
			gets(cp);
			continue;
		}
		pn = n;
		break;
	}
	while (*cp++ = *pn++) ;
	return cp;
}

char *
makepass(pn, cp, mode)
int pn;
register char *cp;
int mode;
{
	return ccpath(cp, pass[pn].p_dir, pass[pn].p_pln, mode);
}

char *
makelib(pn, cp, lp)
int pn;
char *cp;
char *lp;
{
	strcpy(cp, pass[pn].p_pln);
	strcat(cp, lp);
	strcat(cp, ".a");
	return ccpath(cp, pass[pn].p_dir, cp, AREAD);
}

makeft(op, ip, ft)
char *op, *ip, *ft;
{
	register char *ep, *tp;
	register c;
	char *dp;

	ep = ip;
	tp = op;
	dp = NULL;
	while ((c = *ep++) != '\0') {
		if (c == PATHSEP && ip != outf) {
			tp = op;
			dp = NULL;
		} else {
			if (c == '.')
				dp = tp;
			*tp++ = c;
		}
	}
	if (ip == outf)
		return;
	if (dp != NULL)
		tp = dp;
	*tp++ = '.';
	ep = ft;
	while (*tp++ = *ep++)
		;
}
