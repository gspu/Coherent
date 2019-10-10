/*
 * n3/cc3.c
 * Print out C compiler intermediate language files.
 * It is a machine dependent program,
 * but most, if not all, of the machine dependencies are in tables.
 * It understands all favours of intermediate files;
 * that is, it understands both trees and CODE nodes!
 */

#ifdef	vax
#include "INC$LIB:cc3.h"
#else
#include "cc3.h"
#endif

/*
 * Possibly impure areas.
 * All external impure data is here
 * and is reinitialized by the overlaid version of the mainline.
 * This permits the compiler to be called many times if loaded
 * as one big block.
 */
#if	!OVERLAID
int	line;		/* Source line */
int	dotseg;		/* Current segment */
char	file[NFNAME];	/* Source file name */
char	id[NCSYMB];	/* Identifier buffer */
FILE	*ifp;		/* Input file pointer */
FILE	*ofp;		/* Output file pointer */
VARIANT	variant;	/* Variant template */
#endif

extern	char	*segnames[];
/*
 * Temp. file opcodes.
 */
char	*ilonames[] = {
	0,
	"file name",
	0,
	0,
	"global label",
	"static label",
	"block",
	"prolog",
	"epilog",
	0,
	0,
	0,
	0,
	"line number",
	"return expression",
	"effect expression",
	"switch expression",
	"switch body",
	"initialization expression",
	"initialize absolute block",
	0,
	"jump if true",
	"jump if false",
	0,	/* ENTER */
	"module name",
	0,	/* COMM */
	"",	/* DLOCAT */
	"",	/* DLABEL */
	"segment guess",
	"undefined reference"
};

#if	!OVERLAID
main(argc, argv)
char	*argv[];
{
	passname = "cc3";
	if (argc < 3)
		usage();
	getvariant(argv[1]);
#if GEMDOS
	{
		extern long gemdos();
		extern char *lmalloc();
		free(lmalloc((gemdos(0x48,-1L)-4096) & ~1023L));
	}
#endif
	if ((ifp=fopen(argv[2], SRMODE)) == NULL) {
		fprintf(stderr, "%s: cannot open.\n", argv[2]);
		exit(BAD);
	}
	if (argc == 4) {
		if ((ofp=fopen(argv[3], "w")) == NULL) {
			fprintf(stderr, "%s: cannot create.\n", argv[3]);
			exit(BAD);
		}
	} else {
		ofp = stdout;
	}
	dotseg = -1;
	exit(work3());
}

usage()
{
	fprintf(stderr, "Usage: cc3 variant input [output]\n");
	exit(BAD);
}

#else
/*
 * This is the mainline of the intermediate interpreter
 * if the compiler is being run as an overlay job.
 * The impure area must be reinitialized if monolithic
 * because the code can be called over and over again.
 */
cc3()
{
	passname = "cc3";
	if (setjmp(death) != 0) {
		return (ABORT);
	}
	dotseg = -1;
#if	MONOLITHIC
	line = 0;
#endif
	return (work3());
}
#endif

/*
 * This is the nuts and bolts of the operation.
 */
work3()
{
	register int	op;
	register long	ifseek;

	cc3init();
	for (;;) {
#if	TEMPBUF
		ifseek = (ifp == NULL) ? inbufp - inbuf : ftell(ifp);
#else
		ifseek = ftell(ifp);
#endif
		if ((op=bget())==EOF || op==FINISH) {
			cc3close();
			break;
		}
		if (notvariant(VASM))
			fprintf(ofp, "%ld:\t", ifseek);
		switch (op) {

		case FNAME:
		case MNAME:
		case GLABEL:
		case SLABEL:
		case UREFER:
			sget(id, NCSYMB);
			if (isvariant(VASM))
				genname(op, id);
			else {
				fprintf(ofp, "\t%s\t%s\n", ilonames[op], id);
				if (op==GLABEL || op==SLABEL)
					fprintf(ofp, "\t%s:\n", id);
			}
			break;

		case LLABEL:
			if (isvariant(VASM))
				genival(op, (long)iget());
			else
				fprintf(ofp, "L" I_FMT ":\n", iget());
			break;

		case LINE:
			line = iget();
			if (isvariant(VASM))
				genival(op, (long)line);
			else
				fprintf(ofp, "\t%s\t%d\n", ilonames[op], line);
			break;

		case BLOCK:
			if (isvariant(VASM))
				genival(op, (long)zget());
			else
				fprintf(ofp, "%s\t%ld\n",
					ilonames[op], (long)zget());
			break;

		case DLOCAT:
			cc3dloc();
			break;
		case DLABEL:
			cc3dbug(1);
			break;

		case PROLOG:
		case EPILOG:
			fprintf(ofp, "\t%s\n", ilonames[op]);
			break;

		case AUTOS:
			genautos();
			break;

		case JUMP:
			fprintf(ofp, "\tjump\tL" I_FMT "\n", iget());
			break;

		case COMM:
			gencomm();
			break;

		case ENTER:
			if (isvariant(VASM))
				genival(op, (long)bget());
			else
				fprintf(ofp, "\tenter %s\n", segnames[bget()]);
			break;

		case ALIGN:
			if (isvariant(VASM))
				genival(op, (long)bget());
			else
				fprintf(ofp, "\talign %d\n", bget());
			break;

		case SBODY:
		{
			register ival_t deflab, ncases, cvalue, clabel;

			deflab = iget();
			ncases = iget();
			fprintf(ofp, "%s\n", ilonames[op]);
			while (ncases--) {
				cvalue = iget();
				clabel = iget();
				fprintf(ofp, "\t\t" I_FMT "\tL" I_FMT "\n", cvalue, clabel);
			}
			fprintf(ofp, "\t\tdef\tL" I_FMT "\n", deflab);
			break;
		}

		case IBLOCK:
		{
			register int firstb, nitems;

			fprintf(ofp, "byte block\t");
			nitems = bget();
			firstb = 0;
			while (nitems--) {
				if (firstb++ != 0)
					fprintf(ofp, " ");
				fprintf(ofp, "%d", bget());
			}
			fprintf(ofp, "\n");
			break;
		}

		case REXPR:
		case SEXPR:
		case TEXPR:
		case FEXPR:
		case IEXPR:
		case EEXPR:
			itree(op);
			break;

		case CODE:
			icode();
			break;

		case EOF:
			cfatal("unexpected EOF");

		default:
			cbotch("bad temporary file opcode %d", op);
		}
	}
	if (op != FINISH)
		cfatal("unexpected EOF");
	return (OK);
}

/* end of n3/cc3.c */
