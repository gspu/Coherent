/*
 * hp.c
 * 11/13/90
 * Hewlett Packard LaserJet printer filter.
 * Translate page format and nroff font information
 * into HP LaserJet printer escape sequences (PCL).
 * Usage: hp [-acflr] [-in] [-pn] [-tn] [file ...]
 * Options:
 *	-a	Use HP font acute accent character for '\''; default is '\''.
 *	-c	Font cartridge not available, use underline for italics.
 *	-f	[Archaic] Print pages in normal order; this is now the default.
 *	-in	Set left margin (indent) to n; default is 0.
 *	-l	Print in landscape mode; default is portrait mode.
 *	-pn	Set text length to n; default is NLINES, i.e. 66.
 *	-r	Print pages in reverse order (for LaserJet I).
 *		[Writes to temp FILE 'output', then readback() rewrites
 *		temp file to stdout with pages reversed.]
 *	-tn	Set top margin to n; default is 0.
 * Exit status is 0 if successful, 1 if error, 2 if interrupted.
 *
 * Edit history:
 *	3/21/85	(jtk) -- Changed so that pages were printed in reverse order,
 *		and fixed bug where trailing lines with only
 *		non-blank character were deleted.
 *	4/09/85 (ella) -- Changed so that pages were printed in
 *		the landscape mode with the pitch 16.66, and 
 *		fixed bug where underlining in the landscape mode
 *		and/or !cartridge wasn't ever terminated.
 *		Incorporated rma's fix (-a option) to reflect the
 *		manual page and properly process acute accents:
 *		default: straight (apostrophe), with -a: slanted
 *		(single quote).
 *		Included all available options into Usage message.
 *	10/31/90 (norm) -- fixed the brain damage regarding
 *		formfeeds. Change default to printing pages in
 *		order since almost all printers do.  Close any
 *		files that we open. Guess that shows how much testing
 *		was done by the author...
 *	11/7/90	(steve) -- Reordered and rewrote text extensively
 *		for legibility, hopefully no substantive changes.
 */

#include <stdio.h>
#include <signal.h>

extern	long	ftell();

/* Miscellany. */
#define	VERSION		"1.3"
#define	ACCENTACUTE	0xA8
#define	ACCENTGRAVE	0xA9
#define	LINESZ		512
#define	NLINES		66
#define	NPAGES		1000
#define	USAGEMSG	"Usage: hp [ -acflr ] [ -in ] [ -pn ] [ -tn ] [ file ... ]"

/* Fonts. */
#define	ROMAN_F		0		/* all except below means Roman */
#define	BOLD_F		1		/* "x\bx" means bold 'x' */
#define	ITALIC_F	2		/* "_\bx" means italic '_' */

/* HP Printer Control Language (PCL) commands. */
/* These are printf() format strings, some require additional arguments. */
#define	HMOVEFWD	"\033&a+%dH"
#define	HMOVEBACK	"\033&a-%dH"
#define	LANDSCAPE	"\033&l%dO"	/* arg 0==portrait, 1==landscape */
#define	LEFTMARGIN	"\033&a%dL"
#define	LINESPACING	"\033&l%dD"
#define	MOVETOCOL	"\033&a%dC"
#define	PAGELENGTH	"\033&l%dP"
#define	PITCH16		"\033(s16.66H"	/* pitch 16.66 cpi */
#define	RESET		"\033E"
#define	STROKEWEIGHT	"\033(s%dB"
#define	STYLE		"\033(s%dS"
#define	STYLEWEIGHT	"\033(s%ds%dB"
#define	TEXTLENGTH	"\033&l%dF"
#define	TOPMARGIN	"\033&l%dE"
#define	UNDEROFF	"\033&d@"
#define	UNDERON		"\033&dD"

/* Forward. */
#if	0
void	debug();
#endif
void	done();
void	fakebold();
void	fatal();
int	hasFF();
void	initialize();
void	newfont();
int	nonblank();
void	printfile();
void	printline();
void	readback();
void	trap();
void	usage();

/* Globals. */
int	aflag;			/* Accent flag				*/
char	*argv0;			/* Command name for error messages	*/
int	cartridge = 1;		/* Font cartridge is available		*/
int	font = ROMAN_F;		/* Current font				*/
int	landscape;		/* 0 for portrait mode, 1 for landscape	*/
int	leftmargin;		/* Left margin				*/
int	linespacing = 6;	/* Vertical lines per inch		*/
int	nfiles;			/* Number of files to print		*/
int	npages;			/* Number of pages marked on input	*/
FILE	*output = NULL;		/* FILE containing unreversed pages	*/
char	page[NLINES][LINESZ];	/* Current page buffer			*/
long	pageaddr[NPAGES];	/* Seek pointers to top of output pages	*/
int	quit;			/* Signal to interrupt program		*/
int	rflag;			/* Reverse page order			*/
char	tempfile[20];		/* Temporary file name			*/
int	textlength = NLINES;	/* Text length -- must satisfy		*/
				/*	textlength <= NLINES-topmargin	*/
int	topmargin;		/* Top margin				*/

main(argc, argv) int argc; char *argv[];
{
	/* Process arguments. */
	argv0 = argv[0];
	argc--;
	argv++;
	while (argc > 0)  {
		switch(**argv)  {
		case '-':
			switch(*++*argv)  {
			case 'a':
				aflag = 1;
				break;
			case 'c':
				cartridge = 0;
				break;
			case 'f':
				/* Recognized for historical reasons, ignore. */
				break;
			case 'i':
				leftmargin = atoi(++*argv);
				break;
			case 'l':
				landscape = 1;
				linespacing = 8;
				break;
			case 'p':
				textlength = atoi(++*argv);
				break;
			case 'r':
				rflag = 1;
				break;
			case 't':
				topmargin = atoi(++*argv);
				break;
			case 'V':
				fprintf(stderr, "%s: V%s\n", argv0, VERSION);
				break;
			default:
				usage();
				break;
			}
			break;
		default:
			if (nfiles++ == 0)
				initialize();
			printfile(*argv);
			break;
		}
		argv++;
		argc--;
	}

	/* If no command line files, process stdin. */
	if (nfiles == 0) {
		initialize();
		printfile(NULL);
	}

	/* Done. */
	if (rflag)
		readback();
	done(0);
}

#if	0
/*
 * Print debugging output to file "hp.db".
 */
/* VARARGS */
void
debug(s) char *s;
{
	register FILE *fp;

	if ((fp = fopen("hp.db", "ar")) == NULL)
		return;
	fprintf(fp, "hp: %r\n" &s);
	fclose(fp);
}
#endif

/*
 * Finish up.  Remove temporary file if it has been opened, then exit.
 */
void
done(status) int status;
{
	printf(RESET);
	if (output != NULL) {
		fclose(output);
		if (output != stdout)
			unlink(tempfile);
	}
	exit(status);
}

/*
 * Fake boldface of given character when font not available.
 */
void
fakebold(c) register int c;
{
	register int d;

	if (font != ROMAN_F)
		newfont(ROMAN_F);		/* terminate previous font */
	d = (landscape) ? 7 : 8;		/* decipoint distance */
	putc(c, output);			/* print it */
	fprintf(output, MOVETOCOL, -1);		/* move back one column */
	fprintf(output, HMOVEFWD, d);		/* and forward d decipoints */
	putc(c, output);			/* print it again */
	fprintf(output, HMOVEBACK, d);		/* and move back d decipoints */
}

/*
 * Print a fatal error message and die.
 */
/* VARARGS */
void
fatal(s) char *s;
{
	fprintf(stderr, "%s: %r\n", argv0, &s);
	done(1);
}

/*
 * Test whether string s contains a formfeed.
 */
int
hasFF(s) register char *s;
{
	register char c;

	while (c = *s++)
		if (c == '\f')
			return 1;
	return 0;
}

/*
 * Set interrupt trap and initialize the printer.
 * Open temporary file if rflag is specified,
 * otherwise output just goes to stdout.
 */
void
initialize()
{
	/* Trap SIGINT to set quit flag. */
	signal(SIGINT, trap);

	/* HP initialization. */
	printf(RESET);
	if (landscape)
		printf(PITCH16);
	printf(LANDSCAPE, landscape);
	printf(PAGELENGTH, NLINES);
	printf(LINESPACING, linespacing);
	printf(TOPMARGIN, topmargin);
	printf(TEXTLENGTH, textlength);
	printf(LEFTMARGIN, leftmargin);

	/* Initialize output file. */
	if (quit)
		done(2);
	if (rflag) {
		sprintf(tempfile, "/tmp/hptmp.%d", getpid());
		if ((output = fopen(tempfile, "wr")) == NULL)
			fatal("cannot open temporary file \"%s\"", tempfile);
	} else 
		output = stdout;
}

/* 
 * Change to a new font.
 * For each of the three fonts (ROMAN_F, BOLD_F, ITALIC_F),
 * this routine prints the font escape appropriate to the given configuration.
 * There are currently four possible printer configurations:
 *	normal	portrait  && cartridge
 *	-l	landscape && cartridge
 *	-c	portrait  && no cartridge
 *	-c -l	landscape && no cartridge
 * In the first case, do bold and italic with style and weight changes.
 * bold is faked from printline() per-character in the other cases,
 * italic is underlined Roman in the other cases.
 */
void
newfont(new) int new;
{
	static int style = 0;		/* initial style == upright */
	static int weight = 0;		/* initial weight == medium */
	static int under = 0;		/* initial underline state == off */
	register int newstyle, newweight, newunder;

	if (font == new)
		return;					/* unchanged */
	font = new;
	newstyle = newweight = newunder = 0;		/* defaults */
	switch(font) {
	case ROMAN_F:
		break;					/* upright medium */
	case BOLD_F:
		newweight = 1;				/* bold */
		break;
	case ITALIC_F:
		if (cartridge && !landscape) {
			newstyle = 1;			/* italic */
			newweight = -1;			/* weight -1 */
		} else
			newunder = 1;		/* underlined upright medium */
		break;
	default:
		fatal("unrecognized font type %d", font);
		break;
	}
	if (under != newunder) {
		under = newunder;
		fprintf(output, (under) ? UNDERON : UNDEROFF);
	}
	if (style != newstyle && weight != newweight) {
		style = newstyle;
		weight = newweight;
		fprintf(output, STYLEWEIGHT, style, weight);
	} else if (style != newstyle) {
		style = newstyle;
		fprintf(output, STYLE, style);
	} else if (weight != newweight) {
		weight = newweight;
		fprintf(output, STROKEWEIGHT, weight);
	}
}

/*
 * Return 1 if line s is not blank.
 */
int
nonblank(s) register char *s;
{
	register int c;

	while ((c = *s) == ' ' || c == '\t')
		s++;
	return (c != '\0' && c != '\n' && c != '\f');
}

/*
 * Print a file.
 * NULL means stdin.
 */
void
printfile(file) char *file;
{
	FILE *fp;
	register int i;
	int last, end;

	if (file == NULL)
		fp = stdin;
	else if ((fp = fopen(file, "r")) == NULL)
		fatal("cannot open file \"%s\"", file);

	if (font != ROMAN_F)
		newfont(ROMAN_F);
	for (end = 0; !end; ) {
		if (rflag) {
			if (npages >= NPAGES)
				fatal("more than %d pages", NPAGES);
			pageaddr[npages++] = ftell(output);
		}
		for (i = 0; i < NLINES; i++) {
			if (fgets(&page[i][0], LINESZ, fp) == NULL) {
				if (i == 0) {
					npages--;	/* last page empty */
					return;
				}
				end = 1;
				break;
			} else if (hasFF(page[i])) {	/* formfeed seen */
				++i;
				break;
			}
		}
		for (last = --i; last >= 0; --last)
			if (nonblank(page[last]))
				break;			/* nonblank line */
		for (i = 0; i <= last; i++)
			printline(page[i]);
		putc('\r', output);
		if (last < NLINES - 1)
			putc('\f', output);
	}
	if (file != NULL)
		fclose(fp);
}

/*
 * Print one line, substituting special characters from the HP character set.
 */
void
printline(cp) register char *cp;
{
	register int c1, c2, new;
	char *s;

	/* Replace accents with equivalents from the extended character set. */
	for (s = cp; *s != '\0'; ++s) {
		if (*s == '\'' && aflag)
			*s = ACCENTACUTE;
		else if (*s == '`')
			*s = ACCENTGRAVE;
	}

	/* Look for nroff font sequences. */
	while ((c1 = *cp++) != '\0') {

		/* Check for the interrupt signal. */
		if (quit)
			done(2);

		/* By default, Roman c1 gets printed at the end of this loop. */
		new = ROMAN_F;
		if (c1 == '\f')
			return;		/* chars after FF on line get eaten */
		else if (c1 == ' ' || c1 == '\t')
			new = font;	/* no font change on whitespace */
		else if (c1 == '_') {
			if ((c2 = *cp++) == '\b') {
				/* "_\bx" means italic x. */
				new = ITALIC_F;
				c1 = *cp++;
			} else
				cp--;		/* put back c2 */
		} else if ((c2 = *cp++) == '\b') {
			if ((c2 = *cp++) == c1) {
				/* "x\bx" means bold x. */
				if (!cartridge || landscape) {
					fakebold(c1);	/* fake it */
					continue;
				} else
					new = BOLD_F;
			} else
				cp -= 2;	/* put back '\b' and c2 */
		} else
			cp--;			/* put back c2 */
		if (font != new)
			newfont(new);		/* font change */
		putc(c1, output);		/* print c1 in new font */
	}
}

/*
 * Copy the temporary file to stdout with the pages in reverse order.
 */
void
readback()
{
	char c;

	while (--npages >= 0) {
		/* Check for interrupt signal at each page. */
		if (quit)
			done(2);
		fseek(output, pageaddr[npages], 0);
		while ((c = getc(output)) != '\f')
			putchar(c);
		putchar(c);
	}
}

/*
 * Set quit flag when interrupt signal is received.
 */
void
trap()
{
	quit = 1;
}

/*
 * Print a usage message and die.
 */
void
usage()
{
	fprintf(stderr, "%s\n", USAGEMSG);
	exit(1);
}

/* end of hp.c */
