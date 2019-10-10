/*
 * prps.c
 * 7/10/92
 * Produce PostScript pages containing input files.
 * By default, each page has a header line and text enclosed in a box.
 * See usage() for usage and options.
 * Requires fp output: cc prps.c -f
 */

#include <stdio.h>
#include <time.h>
#if	COHERENT
#include <sys/types.h>
#endif

/* Manifest constants. */
#define	VERSION		"2.3"
#define	DEFFONT		"Courier"	/* default font			*/
#define	DEFFONTB	"-Bold"		/* default boldface font suffix	*/
#define	DEFFONTI	"-Oblique"	/* default italic font suffix	*/
#define	DEFFONTR	""		/* default Roman font suffix	*/
#define	DEFPTSIZE	10		/* default point size		*/
#define	DEFPTSIZEL2	8		/* default point size with -l2	*/
#define	DEFTAB		8		/* default tab setting		*/
#define	NBUF		512		/* buffer size			*/
#define	PAGEDX		8.5		/* page width			*/

/* Macros. */
#define	inch(d)	((int)((d) * 72))	/* (double)d inches to (int)pts	*/

/* Forward. */
void	fatal();
void	file();
int	font_of();
char	*Foption();
void	init();
int	printline();
char	*PSstring();
void	usage();

/* External. */
double	atof();
char	*index();

/* Global. */
char	*argv0;				/* Command name.		*/
int	bflag;				/* Suppress box.		*/
char	buf[NBUF];			/* Input buffer.		*/
char	buf2[NBUF];			/* Buffer for PSstring.		*/
char	buf3[NBUF];			/* Buffer for printline.	*/
char	curfont;			/* Current font ('R','B','I').	*/
int	dodate = 1;			/* Display local date/time	*/
char	*fontname = DEFFONT;		/* Font name.			*/
char	*fontBsuffix = DEFFONTB;	/* Boldace font suffix.		*/
char	*fontIsuffix = DEFFONTI;	/* Italic font suffix.		*/
char	*fontRsuffix = DEFFONTR;	/* Roman font suffix.		*/
int	hflag;				/* Suppress header.		*/
char	*hdrname = NULL;		/* Name for header line.	*/
int	indent;				/* Indent.			*/
int	lflag;				/* Landscape mode.		*/
int	nlines;				/* Lines per page.		*/
double	ptsize;				/* Point size.			*/
int	rhpage;				/* Right-hand page flag.	*/
int	skip;				/* Output pages to skip.	*/
int	tab = DEFTAB;			/* Tab setting.			*/

/*
 * Output page format.
 * The page format is hardwired into this table for now;
 * the page is assumed to be 8.5 x 11, or 11 x 8.5 in landscape mode.
 * The format[] array is indexed by lflag:
 *	0	portrait mode
 *	1	landscape mode
 *	2	landscape mode, two side-by-side pages per output page
 */
struct	pformat {
	double	p_boxx;			/* box ll x	*/
	double	p_boxy;			/* box ll y	*/
	double	p_boxdx;		/* box width	*/
	double	p_boxdy;		/* box height	*/
	double	p_hdry;			/* header y	*/
	double	p_textx;		/* text x	*/
	double	p_textdy;		/* text height	*/
} format[3] = {
      /*  boxx boxy  boxdx boxdy  hdry   textx textdy */
	{ .75, .50,  7.250, 9.750, 10.500, 1.000, 9.175 },
	{ .50, .50, 10.000, 7.125,  7.875,  .750, 6.750 },
	{ .25, .25,  5.125, 7.500,  7.950,  .375, 7.400 }
};

main(argc, argv) int argc; char *argv[];
{
	register char *s;
	register char c;

	/* Process command line options. */
	argv0 = argv[0];
	while (argc > 1 && (argv[1][0] == '-' || argv[1][0] == '+')) {
		s = &argv[1][1];
		if (argv[1][0] == '+') {
			skip = atoi(s);
			--argc;
			++argv;
			continue;
		}
		while ((c = *s++) != '\0') {
			switch(c) {
			case 'b':
				++bflag;
				continue;
			case 'd':
				dodate = 0;
				continue;
			case 'f':
				fontname = s;
				break;
			case 'F':
				if (*s == 's') {
					if ((c = *++s) == 'B')
						fontBsuffix = ++s;
					else if (c == 'I')
						fontIsuffix = ++s;
					else if (c == 'R')
						fontRsuffix = ++s;
					else
						fatal("option \"-Fs\" must be followed by [RBI] and suffix");
					break;
				} else
					s = Foption(s);
				continue;
			case 'h':
				++hflag;
				continue;
			case 'i':
				indent = atoi(s);
				if (indent < 0)
					fatal("illegal \"-i\" argument");
				break;
			case 'l':
				lflag = 1;
				if (*s == '2') {
					lflag = 2;
					++s;
				}
				continue;
			case 'n':
				hdrname = s;
				break;
			case 'p':
				nlines = atoi(s);
				if (nlines < 1)
					fatal("illegal \"-p\" argument");
				break;
			case 't':
				tab = atoi(s);
				if (tab < 2)
					fatal("tab setting must be 2 or larger");
				break;
			case 'V':
				fprintf(stderr, "%s: V%s\n", argv0, VERSION);
				continue;
			default:
				if (c >= '1' && c <= '9') {
					ptsize = atof(--s);
					break;
				}
				/* else fall through... */
			case '?':
				usage();
				break;
			}
			while (*++s)
				;			/* scan to end of arg */
		}
		--argc;
		++argv;
	}

	/* Process input files. */
	init();
	if (argc == 1)
		file(NULL);
	else
		while (--argc > 0)
			file(*++argv);
	if (lflag == 2 && rhpage)
		printf("endpage\n");
	printf("state restore\n");

	/* Done. */
	exit(0);
}

/*
 * Cry and die.
 */
/* VARARGS */
void
fatal(args) char *args;
{
	fprintf(stderr, "%s: %r\n", argv0, &args);
	exit(1);
}

/*
 * Process a file.
 */
void
file(name) char *name;
{
	register FILE *fp;
	register int line, page;
	time_t t;

	if (name == NULL)
		fp = stdin;
	else if ((fp = fopen(name, "r")) == NULL)
		fatal("cannot open \"%s\"", name);

	/* Define name and date fields for header line. */
	if (!hflag) {
		printf("/hdrname %s def\n",
			PSstring((hdrname != NULL) ? hdrname : 
				 (name != NULL)    ? name    : ""));
		time(&t);
		printf("/hdrdate %s def\n\n",
			dodate	? PSstring(asctime(localtime(&t)))
				: PSstring(""));
	}

	/* Process the file. */
	if (skip) {
		if (lflag == 2)
			skip &= ~1;		/* round down to even */
		for (page = 0; page < skip; page++) {
			for (line = 0; line < nlines; line++) {
				if (fgets(buf, NBUF, fp) == NULL)
					break;
				if (index(buf, '\f') != NULL)
					break;
			}
		}
	}
	line = 0;
	page = skip + 1;
	while (fgets(buf, NBUF, fp) != NULL) {
		if (line++ == 0) {
			/* Start of page. */
			printf("%% Page %d.\n", page);
			if (!hflag)
				printf("/hdrpage (Page %d, line %d) def\n",
					page, (page-1)*nlines+1);
			if (lflag == 2)
				printf("%chpage\n", (rhpage) ? 'r' : 'l');
			else
				printf("startpage\n");
			curfont = 'R';		/* startpage sets Roman */
		}
		if (printline() || line == nlines) {
			/* End of page. */
			if (lflag == 2 && !rhpage != 0)
				putchar('\n');
			else
				printf("endpage\n\n");
			page++;
			line = 0;
			rhpage = !rhpage;
		}
	}
	if (line != 0) {
		if (lflag != 2 || rhpage)
			printf("endpage\n\n");
		rhpage = !rhpage;
	}

	if (name != NULL)
		fclose(fp);
}

/*
 * Return the font ('R', 'I', 'B', or 'W') of the non-NUL character *s.
 * Return 'W' for whitespace, which always leaves font unchanged.
 */
int
font_of(s) register char *s;
{
	register char c1, c3;

	if ((c1 = *s) == ' ' || c1 == '\t' || c1 == '\n')
		return 'W';
	if (*++s != '\b')
		return 'R';		/* Not "x\bx" nor "_\bx", i.e. Roman */
	if ((c3 = *++s) == '\0')
		return 'R';		/* unlikely */
	if (c1 == '_')
		return 'I';		/* Italic */
	return ((c1 == c3) ? 'B' : 'R');	/* Bold or unlikely Roman */
}

/*
 * Process a -F command line font specification.
 * Return a pointer past the last character used.
 */
char *
Foption(s) register char *s;
{
	register char c;

	switch (c = *s++) {
	case 'A':
		fontname = "AvantGarde";
		fontBsuffix = "-Demi";
		fontIsuffix = "-BookOblique";
		fontRsuffix = "-Book";
		break;
	case 'B':
		fontname = "Bookman";
		fontBsuffix = "-Demi";
		fontIsuffix = "-LightItalic";
		fontRsuffix = "-Light";
		break;
	case 'H':
		fontname = "Helvetica";
		break;
	case 'N':
		fontname = "Helvetica-Narrow";
		break;
	case 'P':
		fontname = "Palatino";
		goto lab;
	case 'S':
		fontname = "NewCenturySchlbk";
		goto lab;
	case 'T':
		fontname = "Times";
lab:
		fontIsuffix = "-Italic";
		fontRsuffix = "-Roman";
		break;
	default:
		fatal("option \"-F\" must be followed by 's' or by one of [ABHNPST]");
		break;
	}
	return s;
}

/*
 * Initialize PostScript program.
 */
void
init()
{
	register struct pformat *p;
	double d, hdrx, hdrdx, texty;
	int n;

	p = &format[lflag];
	hdrx = p->p_boxx;
	hdrdx = p->p_boxdx;
	if (hflag) {
		/* Reclaim the space reserved for the header. */
		d = p->p_hdry - p->p_boxdy - p->p_boxy;	/* added height */
		p->p_boxdy += d;
		p->p_textdy += d;
	}

	/* Determine the point size and text lines per page. */
	/* lines = inches * points/inch / points/line */
	if (ptsize == 0 && nlines == 0)		/* neither specified */
		ptsize = (lflag == 2) ? DEFPTSIZEL2 : DEFPTSIZE;
	else if (ptsize == 0)			/* nlines specified */
		ptsize = p->p_textdy * 72 / nlines;
	else if (nlines != 0) {			/* both specified */
		n = p->p_textdy * 72 / ptsize;
		if (n < nlines) {
			fprintf(stderr, "%s: Warning: -p argument ignored (too large for point size)\n",
				argv0);
			nlines = n;
		}
	}
	if (nlines == 0)
		nlines = p->p_textdy * 72 / ptsize;

	/* Center text in the box. */
	p->p_textdy = ptsize * nlines / 72;	/* adjust length for roundoff */
	texty = p->p_boxy + p->p_boxdy - (p->p_boxdy - p->p_textdy) / 2;

	/* PostScript globals. */
	printf(
		"%% Global definitions.\n"
		"/state save def\n"
		"/FS { findfont ptsize scalefont } bind def\n"
		"/S { show } bind def\n"
		);
	printf("/ptsize %.2f def\n", ptsize);
	printf("/fontH /%s findfont %d scalefont def\n",
		DEFFONT, (lflag==2) ? DEFPTSIZEL2 : DEFPTSIZE);
	printf("/fontR /%s%s FS def\n", fontname, fontRsuffix);
	printf("/fontB /%s%s FS def\n", fontname, fontBsuffix);
	printf("/fontI /%s%s FS def\n", fontname, fontIsuffix);
	printf(
		"/fH {fontH setfont} bind def\n"
		"/fR {fontR setfont} bind def\n"
		"/fB {fontB setfont} bind def\n"
		"/fI {fontI setfont} bind def\n"
		);
	putchar('\n');

	/* PostScript routines. */
	printf("%% Routines.\n");
	if (!bflag)
		printf(
			"/box\t%% boxdx boxdy boxx boxy box -\n"
			"{\n"
			"\tnewpath\n"
			"\tmoveto\n"
			"\texch dup 0 rlineto\n"
			"\texch 0 exch rlineto\n"
			"\tneg 0 rlineto\n"
			"\tclosepath\n"
			"\tclip\n"
			"\t0.02 setlinewidth\n"
			"\tstroke\n"
			"} bind def\n"
			);
	printf("/endpage {grestore showpage} bind def\n");
	if (!hflag) {
		printf(
			"/hdr\t%% Uses hdrname, hdrdate, hdrpage.\n"
			"{\n"
			"\tgsave\n"
			"\tfH\n"
			);
		printf("\t%d %d translate 0 0 moveto\n",
			inch(hdrx), inch(p->p_hdry));
		printf("\thdrname S\n");
		printf("\t%d hdrdate stringwidth pop 2 div sub %d neg moveto\n",
			inch(hdrdx/2), (lflag==2) ? DEFPTSIZEL2 : DEFPTSIZE);
		printf("\thdrdate S\n");
		printf("\t%d hdrpage stringwidth pop sub 0 moveto\n",
			inch(hdrdx));
		printf(
			"\thdrpage S\n"
			"\tgrestore\n"
			"} bind def\n"
			);
	}
	if (indent) {
		printf("/indent fR (");
		for (n = indent; n--; )
			putchar(' ');
		printf(") stringwidth pop def\n");
	}
	if (lflag == 2)
		printf("/lhpage {90 rotate 0 %d translate startpage} bind def\n",
			inch(-PAGEDX));
	printf("/nl {0 %.2f translate 0 0 moveto} bind def\n", -ptsize);
	printf("/BS { stringwidth pop neg 0 rmoveto } bind def\n");
	printf("/N {show nl} bind def\n");
	if (lflag == 2)
		printf("/rhpage {grestore %d 0 translate startpage} bind def\n",
			inch(p->p_boxx + p->p_boxdx));
	printf("/startpage {\n");
	printf("\tgsave\n");
	if (lflag == 1)
		printf("\t90 rotate 0 %d translate\n", inch(-PAGEDX));
	if (!hflag)
		printf("\thdr\n");
	if (!bflag)
		printf("\t%d %d %d %d box\n",
			inch(p->p_boxdx), inch(p->p_boxdy),
			inch(p->p_boxx), inch(p->p_boxy));
	printf("\t%d%s %d translate\n",
		inch(p->p_textx),
		(indent) ? " indent add" : "",
		inch(texty) - (int)ptsize);
	printf("\t0 0 moveto\n\tfR\n} bind def\n");
	printf("\n%% Text.\n");
}

/*
 * Print a line of text from buf[].
 * This would be one line of code without the font handling.
 */
printline()
{
	register char *s, *cp;
	char c;
	int new;
	int found_ff = 0;		/* saw formfeed in input stream */

	s = buf;			/* input pointer */
	cp = buf3;			/* output string pointer */
	while ((c = *s) != '\0' && c != '\n') {
		if (c == '\f') {
			/* Formfeed. */
			++found_ff;
			++s;
			continue;
		} else if (c == '\b') {
			/* Backspace. */
			if (cp != buf3) {
				*cp-- = '\0';
				c = *cp;		/* last char printed */
				printf("%s S ", PSstring(buf3));
				cp = buf3;
			} else
				c = ' ';		/* at start of line */
			printf("(%c) BS ", c);		/* backspace */
			++s;
			continue;
		}
		new = font_of(s);
		if (new != curfont && new != 'W') {
			/* Font change. */
			if (cp != buf3) {
				*cp = '\0';
				printf("%s S ", PSstring(buf3));
				cp = buf3;
			}
			printf("f%c ", new);	/* switch to new font */
			curfont = new;
		}
		if (new == 'R' || new == 'W')
			s++;
		else {
			s += 2;			/* skip 2 if bold or italic */
			c = *s++;		/* third gives the char */
			if (new == 'B') {
				/* Watch for Unix braindamage "c\bc\bc", GOK. */
				while (*s == '\b' && *(s+1) == c)
					s += 2;
			}
		}
		*cp++ = c;			/* store the character */
	}
	if (cp != buf3) {
		*cp = '\0';
		printf("%s N\n", PSstring(buf3));
	} else
		printf("nl\n");
	return found_ff;
}

/*
 * Convert string cp to PostScript string in buf2[].
 * PostScript understands tabs but the tab stops it uses seem screwy,
 * so this routine expands tabs for now.
 * The tab expansion here assumes the string starts at the left margin.
 */
char *
PSstring(cp) register char *cp;
{
	register char *s, c;
	register int col;

	col = 1;			/* current output column */
	s = buf2;
	*s++ = '(';
	while ((c = *cp++) != '\n' && c != '\0') {
		if (c == '\t') {
			do {
				*s++ = ' ';
			} while ((col++ % tab) != 0);
		} else {
			if (c == '(' || c == ')' || c == '\\')
				*s++ = '\\';
			*s++ = c;
			++col;
		}
	}
	*s++ = ')';
	*s = '\0';
	return buf2;
}

/*
 * Print a usage message and die.
 */
void
usage()
{
	fprintf(stderr,
"Usage: prps [ options ] [ file ... ]\n"
"Options:\n"
"\t+n\tSkip first n pages of output.\n"
"\t-n\tUse point size n (default: 10).\n"
"\t-b\tSuppress the box around the page text.\n"
"\t-d\tSuppress printing the date and time.\n"
"\t-ffont\tUse the given font name (default: Courier).\n"
"\t-FX\tUse font X, which must be [ABHNPST].\n"
"\t-FsXsfx\tUse sfx as suffix for font X, which must be [RBI].\n"
"\t\tDefault suffixes are \"\" (R), \"-Bold\" (B), \"-Oblique\" (I).\n"
"\t-h\tSuppress the header line.\n"
"\t-in\tIndent by an additional n spaces.\n"
"\t-l\tLandscape mode (default: portrait).\n"
"\t-l2\tLandscape mode, two pages per output page.\n"
"\t-nhead\tUse the given name instead of filename in the header line.\n"
"\t-pn\tPrint n lines of text per output page.\n"
"\t-tn\tSet tab stops to every n characters (default: 8).\n"
		);
	exit(1);
}

/* end of prps.c */
