/*
 * epson.c
 * 11/8/90
 * Epson MX-80 printer driver.
 * Usage: epson [ - cdfrw8 ] [ -b banner ] [ -in ] [ -o ofile ] [ -sn ] [ file ... ]
 */

#include <stdio.h>

#define	VERSION	"1.8"			/* Version number. */
#define	ESC	"\033"			/* ASCII escape */
#define	USAGE	"Usage: epson [ -cdfrw8 ] [ -b banner] [ -in ] [ -o ofile ] [ -sn ] [ file ... ]\n"

/* Epson special characters. */
#define	COMPON	'\017'
#define COMPOFF	'\222'
#define FORMFD	'\014'
#define	SELECT	'\021'
#define	WIDEON	'\016'
#define	WIDEOFF	'\224'

/* Default output device. */
#ifdef	COHERENT
#define	OUTFILE	"/dev/lp"		/* COHERENT */
#else
#define	OUTFILE	"prn:"			/* MS-DOS */
#endif

/* Forward. */
void	fatal();
void	indent();
void	process();
void	usage();

/* Globals. */
char	*banner;			/* banner			*/
int	cflag;				/* compressed			*/
int	dflag;				/* double struck (default: emphasized) */
int	eightflag;			/* eight lines per inch		*/
int	fflag;				/* suppress formfeed		*/
int	nindent;			/* indent			*/
char	*ofile = OUTFILE;		/* output file			*/
FILE	*ofp;				/* output FILE			*/
int	rflag;				/* Roman (default: italic)	*/
int	spaces = 1;			/* vertical spaces		*/
int	wflag;				/* double width			*/

main(argc, argv) int argc; char *argv[];
{
	register int i;

	/* Process option flags. */
	while (--argc > 0 && **++argv == '-') {
		switch (*++*argv) {
		case 'b':
			if (--argc == 0)
				usage();
			banner = *++argv;
			break;
		case 'c':	++cflag;		break;
		case 'd':	++dflag;		break;
		case 'f':	++fflag;		break;
		case 'i':
			nindent = atoi(++*argv);
			if (nindent < 0)
				fatal("bad -i arg %d", nindent);
			break;
		case 'o':
			if (--argc == 0)
				usage();
			ofile = *++argv;
			break;
		case 'r':	++rflag;		break;
		case 's':
			spaces = atoi(++*argv);
			if (spaces <1 || spaces > 3)
				fatal("bad -s arg %d", spaces);
			break;
		case 'w':	++wflag;		break;
		case 'V':
			fprintf(stderr, "epson: V%s\n", VERSION);
			break;
		case '8':	++eightflag;		break;
		default:	usage();		break;
		}
	}

	/* Open output file and initialize Epson. */
	if ((ofp = fopen(ofile, "w")) == NULL)
		fatal("cannot open file \"%s\"", ofile);
	fputs(ESC "@", ofp);

	/* Print banner if given. */
	if (banner != NULL) {
		i = (wflag ? nindent * 2 : cflag ? nindent / 2 : nindent);
		while (i-- > 0)
			fputc(' ', ofp);
		fputc(WIDEON, ofp);
		fputs(banner, ofp);
		fputc(WIDEOFF, ofp);
		fputc('\n', ofp);
	}
	if (cflag)
		fputc(COMPON, ofp);
	if (eightflag)
		fputs(ESC "0" ESC "CX", ofp);

	/* Process input files. */
	if (argc == 0)
		process(NULL);			/* NULL means stdin */
	else
		while (argc-- > 0)
			process(*argv++);

	/* Cleanup and close output file. */
	if (cflag)
		fputc(COMPOFF, ofp);
	if (eightflag)
		fputs(ESC "2" ESC "CB", ofp);
	if (wflag)
		fputc(WIDEOFF, ofp);
	fputs(ESC "@", ofp);
	if (fclose(ofp) == EOF)
		fatal("cannot close file \"%s\"", ofile);
	exit(0);
}

/* Print fatal error message and exit. */
/* VARARGS */
void
fatal(s) char *s;
{
	fprintf(stderr, "epson: %r\n", &s);
	exit(1);
}

/* Perform indentation. */
void
indent()
{
	register int i;

	if (wflag)
		fputc(WIDEON, ofp);
	for (i = nindent; i-- > 0; )
		fputc(' ', ofp);
}

/* Process input file ifp. */
void
process(fname) char *fname;
{
	register FILE *ifp;
	register int this, next, i;

	if (fname == NULL)
		ifp = stdin;
	else if ((ifp = fopen(fname, "r")) == NULL)
		fatal("cannot open file \"%s\"", fname);
	indent();
	while ((this = fgetc(ifp)) != EOF) {
		if (this == '\n') {		/* newline */
			for (i = spaces; i-- > 0; )
				fputc(this, ofp);
			indent();
		} else if (this == FORMFD) {	/* formfeed */
			fputc(this, ofp);
			indent();
		} else if ((next = fgetc(ifp)) != '\b') {
			fputc(this, ofp);
			ungetc(next, ifp);
		} else {
			/* Next char is backspace, could mean bold or italic. */
			if (this == '_' && !rflag) {
				/* "_\bx" means italic x. */
				fputc(fgetc(ifp) + 0200, ofp);
			} else if (!dflag) {
				if ((next = fgetc(ifp)) == this) {
					/* "x\bx" means bold x. */
					fputs(ESC "E", ofp);
					fputc(this, ofp);
					fputs(ESC "F", ofp);
				} else {
					/* "x\by", print as is. */
					fputc(this, ofp);
					fputc('\b', ofp);
					ungetc(next, ifp);
				}
			} else {
				/* Just print as is... */
				fputc(this, ofp);
				ungetc(next, ifp);
			}
		}
	}
	if (!fflag)
		fputc(FORMFD, ofp);
	if (fname != NULL)
		fclose(ifp);
}

/* Print usage message and exit. */
void
usage()
{
	fprintf(stderr, "Usage:  epson [ -cdfrw8 ] [ -b banner] [ -in ] [ -o ofile ] [ -sn ] [ file ... ]\n");
	exit(1);
}

/* end of epson.c */
