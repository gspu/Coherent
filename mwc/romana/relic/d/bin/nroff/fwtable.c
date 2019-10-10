/*
 * fwtable.c
 * 6/7/91
 * Usage: fwtable [ -cptv ] [ -ssymset ] [ infile [ outfile ] ]
 * Read HP PCL bitmap font, PostScript AFM file, or HP TFM file
 * from infile or stdin, write font width table to outfile or stdout.
 *
 * Requires floating point output, PostScript routines in fwt_PS.c,
 * TFM routines in fwt_TFM.c:
 *	cc fwtable.c fwt_PS.c fwt_TFM.c -f
 *
 * Options:
 *	-c		Write C instead of binary
 *	-p		Input is PostScript AFM file
 *	-ssymset	Specify desired symbol set with -t option
 *	-t		Input is HP TFM file
 *	-v		Write one-line font description to stderr
 *
 * Understands PCL bitmap fonts, PostScript AFM files, HP TFM files.
 * Does not understand Intellifont scalable fonts.
 *
 * The following had better agree about the binary FWT format:
 *	troff/fwtable.c/dump_chartab()	writes binary FWT from HP PCL
 *	troff/fwt_PS.c/outputPS()	writes binary FWT from PostScript AFM
 *	troff/fwt_TFM.c/outputTFM()	writes binary FWT from HP TFM
 *	troff/fonts.c/loadfont()	reads binary FWT for troff
 *
 * Modified 12/12/90-12/28/90 by steve from dag's original hptable.c source;
 * the coding could definitely be cleaner.
 */

#include <stdio.h>
#include <canon.h>
#include "fwtable.h"

/* PCL bitmap font descriptor format.  Cf. "HP LJ III Tech Ref Man", p. 10-7. */
typedef struct fnt_hdr {
	short	f_hsize;
	uchar	f_desc_format;
	uchar	f_font_type;
	uchar	f_style_msb;
	uchar	f_reserved1;
	short	f_baseline;
	short	f_cell_width;
	short	f_cell_height;
	uchar	f_orientation;
	uchar	f_spacing;
	short	f_symbol_set;
	short	f_pitch;
	short	f_height;
	short	f_xheight;
	uchar	f_width_type;
	uchar	f_style;
	char	f_weight;		/* specifically signed!	*/
	uchar	f_face;
	uchar	f_comment[0];		/* remainder in add_info */
} font_header;

typedef struct add_info {
	char	a_slant;		/* Slant information	*/
	uchar	a_serif;		/* Serif style (see serif table) */
	uchar	a_quality;		/* Quality level	*/
	uchar	a_placement;		/* Placement		*/
	char	a_underline;		/* Underline position	*/
	uchar	a_uheight;		/* Underline height in dots	*/
	short	a_lspacing;		/* Optimum line spacing in dots * 4 */
	short	a_nwidth;		/* Average lower-case char width * 4 */
	short	a_firstc;		/* First code		*/
	short	a_lastc;		/* Last code		*/
	uchar	a_piextend;		/* Extend for pitch	*/
	uchar	a_poextend;		/* Extend for height	*/
	short	a_cap_height;		/* Cap height		*/
	long	a_fontnum;		/* Font Number		*/
	char	a_fontname[16];		/* Font name		*/
} font_extra;

typedef struct chr_hdr {
	uchar	c_format;		/* Character format (4)	*/
	uchar	c_continuation;		/* Continuation flag	*/
	uchar	c_size;			/* Header size to follow*/
	uchar	c_class;		/* Format class 1=raster*/
	uchar	c_orientation;		/* Orientation		*/
	uchar	c_empty2;		/* more padding...	*/
	short	c_left_offset;		/* left offset		*/
	short	c_top_offset;		/* top offset		*/
	short	c_char_width;		/* character width	*/
	short	c_char_height;		/* character height	*/
	short	c_delta_x;		/* delta X		*/
	uchar	c_data[0];		/* character data	*/
} character_header;
character_header *char_def();

/* Global arrays. */
/*
 * Typefaces.
 * Cf. "HP LJ III Tech Ref Man", pp. 10-27ff.
 * The list given there gives all values 0-84 and some values 87-168.
 * This list is truncated in the name of sanity.
 */
char	*faces[] = {
	"LinePrinter",			/*  0 */
	"Pica",				/*  1 */
	"Elite",			/*  2 */
	"Courier",			/*  3 */
	"Helvetica",			/*  4 */
	"Times",			/*  5 */
	"LetterGothic",			/*  6 */
	"Script",			/*  7 */
	"Prestige",			/*  8 */
	"Caslon",			/*  9 */
	"Orator",			/* 10 */
	"Presentation",			/* 11 */
	"HelveticaCondensed",		/* 12 */
	"Serifa",			/* 13 */
	"Futura",			/* 14 */
	"Palatino",			/* 15 */
	"ITCSouvenir",			/* 16 */
	"Optima",			/* 17 */
	"ITCGaramond",			/* 18 */
	"CooperBlack",			/* 19 */
	"Coronet",			/* 20 */
	"Broadway",			/* 21 */
	"BauerBodoniBlack",		/* 22 */
	"NewCenturySchlbk",		/* 23 */
	"               "		/* >23 */
 };
#define	NFACES	((sizeof faces / sizeof faces[0]) - 1)

/* f_serif values */
char *serif_tab[] = {
	"sans serif square",		/* 0 */
	"sans serif round",		/* 1 */
	"serif line",			/* 2 */
	"serif triangle",		/* 3 */
	"serif swath",			/* 4 */
	"serif block",			/* 5 */
	"serif bracket",		/* 6 */
	"rounded bracket",		/* 7 */
	"flair serif",			/* 8 */
	"script nonconnecting",		/* 9 */
	"script joining",		/* 10 */
	"script calligraphic",		/* 11 */
	"script broken letter",		/* 12 */
	"serif value out-of-range"	/* >12 */
};
#define	NSERIFS	((sizeof serif_tab / sizeof serif_tab[0]) - 1)

/* Style posture values.  The width and structure info is ignored for now. */
char *posture_tab[] = {
	"upright",			/* 0 */
	"italic",			/* 1 */
	"alternate italic",		/* 2 */
	"reserved"			/* 3 */
};

/* Orientation. */
char *orient_tab[] = {
	"portrait",			/* 0 */
	"landscape",			/* 1 */
	"reverse portrait",		/* 2 */
	"reverse landscape"		/* 3 */
};

/* Globals. */
char		buf[NBUF];		/* String conversion	*/
int		cflag;			/* Write C not binary	*/
int		char_datasize[NWIDTH];	/* Character size	*/
int		char_movement[NWIDTH];	/* Character movement	*/
font_header	*fhp;			/* Font header pointer	*/
FILE		*ifp = stdin;		/* The input FILE	*/
int		ipointsz;		/* Integer point size	*/
FILE		*ofp = stdout;		/* The output FILE	*/
int		pflag;			/* PostScript input	*/
char		*symset;		/* Desired symbol set	*/
int		tflag;			/* TFM input		*/
int		this_char;		/* Current character	*/
int		vflag;			/* Verbose		*/

main(argc, argv) int argc; char *argv[];
{
	register char *s;

	/* Process command-line options. */
	while (argc > 1 && argv[1][0] == '-') {
		for (s = &argv[1][1]; *s; s++) {
			switch(*s) {
			case 'c':
				++cflag;
				break;
			case 'p':
				++pflag;
				break;
			case 's':
				symset = ++s;
				while (*s)
					++s;
				--s;
				break;
			case 't':
				++tflag;
				break;
			case 'v':
				++vflag;
				break;
			case 'V':
				fprintf(stderr, "fwtable: V%s\n", VERSION);
				break;
			default:
				usage();
			}
		}
		--argc;
		++argv;
	}
	if (pflag && tflag)
		fatal("options -p and -t are mutually exclusive");
	if (symset != NULL && !tflag)
		fatal("-s option requires -t option");

	/* Set up input and output FILEs. */
	if (argc > 1 && (ifp = fopen(argv[1], "rb")) == NULL)
		fatal("cannot open input file \"%s\"", argv[1]);
	else if (argc > 2 && (ofp = fopen(argv[2], "w")) == NULL)
		fatal("cannot open output file \"%s\"", argv[2]);
	else if (argc > 3)
		usage();

	/* Do the work. */
	if (pflag) {
		/* PostScript. */
		inputPS();
		outputPS();
	} else if (tflag) {
		/* TFM. */
		inputTFM();
		outputTFM();
	} else {
		/* PCL. */
		if (argc > 1 && cflag)
			fprintf(ofp,"/* File %s */\n", argv[1]);
		base();
	}

	/* Close FILEs and exit. */
	if (ifp != stdin && fclose(ifp) == EOF)
		fatal("cannot close input file \"%s\"", argv[1]);
	else if (ofp != stdout && fclose(ofp) == EOF)
		fatal("cannot close output file \"%s\"", argv[2]);
	exit(0);
}

/*
 * Allocate size bytes.
 */
char *
alloc(size) register unsigned int size;
{
	register char *s;

	if ((s = malloc(size)) == NULL)
		fatal("out of space");
	return s;
}

void
base()
{
	register int c;

	/* Initialize. */
	for (c = 0 ; c < NWIDTH; c++) {
		char_movement[c] = -1;
		char_datasize[c] = 0;
	}

	/* Read the font description. */
	while ((c = fgetc(ifp)) != EOF) {
		if (c == '\033')
			escape();
		else
			if (c)
				nonfatal("unexpected char (0x%02x)", c);
	}

	/* Dump character table. */
	dump_chartab();
	if (fhp != NULL)
		free(fhp);
}

int
char_code(code) int code;
{
#ifdef	VERBOSE
	if (cflag)
		fprintf(ofp,"Character Code 0x%02x\n", code);
#endif
	this_char = code;
	return code;
}

character_header *
char_def(size) int size;
{
	register character_header *ch;

	ch = alloc(size);
	ch->c_format = getuchar();
	ch->c_continuation = getuchar();
	ch->c_size = getuchar();
	ch->c_class = getuchar();
	ch->c_orientation = getuchar();
	ch->c_empty2 = getuchar();
	ch->c_left_offset = getshort();
	ch->c_top_offset = getshort();
	ch->c_char_width = getshort();
	ch->c_char_height = getshort();
	ch->c_delta_x = getshort();
	if (size > 16)
		getextra(ch->c_data, size-16);
	return ch;
}

/*
 * Dump the character width table.
 */
void
dump_chartab()
{
	register int c, first, last, max, w;
	register long dsize;
	int mult, div, scale;

	/* Find first, last, max values in char_movement[]. */
	for (c = first = last = max = 0; c < NWIDTH; c++) {
		if (char_movement[c] >= 0) {
			if (first == 0)
				first = c;
			if (max < char_movement[c])
				max = char_movement[c];
			last = c;
		} else
			char_movement[c] = 0;
	}

	/*
	 * Calculate multiplier and divisor to convert entries to troff units.
	 * Character deltax is in quarter dots, so movement in troff units is
	 *	deltax * 720 / 1200.
	 * and 720/1200 == 3/5.
	 * troff multiplies by pointsize, this predivides accordingly;
	 * this simplifies scaling.
	 */
	mult = 3;
	div = ipointsz * 5;
	if (max > 255) {
		/* Max deltax is too big to fit in char, scale accordingly. */
		scale = (max / 256) + 1;
		mult *= scale;
		for (c = 0; c < NWIDTH; c++)
			if (char_movement[c] > 0)
				char_movement[c] /= scale;
	}
	/* Reduce if possible. */
	for (w = mult; w > 1; w--) {
		if ((mult % w) == 0 && (div % w) == 0) {
			div /= w;
			mult /= w;
		}
	}
	putshort(mult);				/* short f_num		*/
	putshort(div);				/* short f_den		*/

	/* Dump the movement table. */
	if (cflag) {
		fprintf(ofp,"\t\t/* Movement table: first = %d, last = %d */\n",
			first, last);
		fprintf(ofp,"\t\t{\n");
	}
	dsize = 0L;
	for (w = c = 0; c < NWIDTH; c++) {
		dsize += char_datasize[c];
		if (cflag) {
			if (w == 0)
				fprintf(ofp,"\t\t\t");
			fprintf(ofp,"%3d", char_movement[c]);
			fputc((c < NWIDTH-1) ? ',' : ' ', ofp);
			if (++w > 7) {
				w = 0;
				fprintf(ofp,"\t/* 0x%02x-0x%02x */\n", c-7, c);
			}
		} else
			fputc(char_movement[c], ofp);
	}
	if (cflag) {
		fprintf(ofp,"\t\t}\n");
		fprintf(ofp,"\t/* Size of font data (in bytes): %ld */\n", dsize);
		fprintf(ofp,"\t}\n");
	}
}

#ifdef	VERBOSE
#ifdef	GLYPH
void
dump_glyph(width, height, data) register uchar *data;
{
	register int c, d, f, g;

	width += 7;
	width >>= 3;
	fprintf(ofp,"-----------------------------------\n");
	for (c=0; c<height; c++) {
		for (d=0; d<width; d++) {
			f = *data++;
			for (g = 0; g < 8; g++) {
				if (f & 0x80)
					fprintf(ofp,"*");
				else
					fprintf(ofp,".");
				f <<= 1;
			}
		}
		fprintf(ofp,"\n");
	}
	fprintf(ofp,"-----------------------------------\n");
}
#endif
#endif

/*
 * Process an escape sequence.
 * The <Esc> has already been read.
 */
void
escape()
{
	register int c;

	c = getuchar();
	switch (c) {
	case '*':	escape_star();
			break;
	case '(':	escape_oparen();
			break;
	case ')':	escape_cparen();
			break;
	default:	nonfatal("unknown escape sequence (0x1B 0x%02x ...)", c);
			break;
	}
}

/*
 * Process a font defintition:
 *	<Esc> ) s <#> W	<font header>
 */
void
escape_cparen()
{
	register int c, q;
	register font_extra *fe;
	char *cp;
	double pointsz;

	if ((c = getuchar()) != 's') {
		nonfatal("unknown sequence \\033)%c", c);
		return;
	}
	q = getparm();
	if ((c = getuchar()) != 'W') {
		nonfatal("unknown sequence \\033)s%d%c", q, c);
		return;
	}
	read_header(q);
	/* pointsz is height*72/1200, 72/1200 == 3/50 */
	pointsz = ((double)(3.0*fhp->f_height))/50.0;
	if (cflag) {
		fprintf(ofp, "/*\n * Font Definition:\n");
		fprintf(ofp, " * Header size: %d\n", fhp->f_hsize);
		fprintf(ofp, " * %s bits\n", ((fhp->f_font_type) ? "Eight" : "Seven"));
		fprintf(ofp, " * Orientation: %s\n", orient_tab[fhp->f_orientation]);
		fprintf(ofp, " * Spacing: %s\n", ((fhp->f_spacing) ? "variable" : "fixed"));
		fprintf(ofp, " * Baseline: %d\n", fhp->f_baseline);
		fprintf(ofp, " * Cell width: %d\n", fhp->f_cell_width);
		fprintf(ofp, " * Cell height: %d\n", fhp->f_cell_height);
		fprintf(ofp, " * Pitch: %d\n", fhp->f_pitch);
		fprintf(ofp, " * Height: %d\n", fhp->f_height);
		fprintf(ofp, " * Weight: %d\n", fhp->f_weight);
		fprintf(ofp, " * Style: %s\n", posture_tab[(fhp->f_style)%4]);
		fprintf(ofp, " * Typeface: %s\n", faces[fhp->f_face]);
		fprintf(ofp, " * Designator: %d%c\n",
			fhp->f_symbol_set / 32,
			fhp->f_symbol_set % 32 + 64);
	}
	if (fhp->f_hsize > 26) {
		fe = fhp->f_comment;
		pointsz += ((double) fe->a_poextend)/4275.0;
		if (fe->a_serif > NSERIFS)
			fe->a_serif = NSERIFS;
		if (cflag) {
			fprintf(ofp, " * Slant: %d\n", fe->a_slant);
			fprintf(ofp, " * %s\n", serif_tab[fe->a_serif]);
			fprintf(ofp, " * Quality: %d\n", fe->a_quality);
			fprintf(ofp, " * First: 0x%02x\n", fe->a_firstc);
			fprintf(ofp, " * Last: 0x%02x\n", fe->a_lastc);
			fprintf(ofp, " * Underline pos: %d\n", fe->a_underline);
			fprintf(ofp, " * Height: %d\n", fe->a_uheight);
			fprintf(ofp, " * Line spacing: %u\n", fe->a_lspacing);
			fprintf(ofp, " * Nominal width: %u\n", fe->a_nwidth);
			fprintf(ofp, " * Pitch extend: %d\n", fe->a_piextend);
			fprintf(ofp, " * Height extend: %d\n", fe->a_poextend);
			fprintf(ofp, " * Font number: %ld\n", fe->a_fontnum);
			fprintf(ofp, " * Font name: %16s\n", fe->a_fontname);
		}
	}
	if (cflag && q > fhp->f_hsize) {
		register uchar *sp, *ep;
		register int c;

		c = 0;
		sp = (char *)fhp;
		ep = sp + q;
		sp += fhp->f_hsize;
		while (sp < ep) {
			if (c == 0)
				fprintf(ofp," # ");
			if (*sp >= 0x20 && *sp < 0x7F)
				fprintf(ofp,"%c", *sp++);
			else {
				fprintf(ofp,"\\x%02x", *sp++);
				c += 3;
			}
			if (++c > 72) {
				c = 0;
				fprintf(ofp," #\n");
			}
		}
	}
	ipointsz = (int)(pointsz*10+.5);	/* integer pointsize */
	if (cflag)
		fprintf(ofp,"\n */\n");

	/* Build a descriptive name. */
	sprintf(buf, "%s %.2f point ", faces[fhp->f_face], pointsz);
	cp = &buf[strlen(buf)];
	if (fhp->f_weight == 3)
		strcpy(cp, "bold ");
	else if (fhp->f_weight != 0)
		sprintf(cp, "%d weight ", fhp->f_weight);
	cp = &buf[strlen(buf)];
	if (fhp->f_style%4 != 0)
		sprintf(cp, "%s ", posture_tab[(fhp->f_style)%4]);
	if (fhp->f_orientation != 0)
		strcat(cp, orient_tab[fhp->f_orientation]);
	if (vflag)
		fprintf(stderr,"\t%s\n", buf);
	if (cflag)
		fprintf(ofp, "\t{\n\t\t\"%s\",\n", buf);
	else
		putstring(buf);			/* char *f_descr	*/

	if (cflag)
		fprintf(ofp, "\t\t\"\",\t/* PostScript name */\n");
	else
		putstring("");			/* char *f_PSname	*/

	/* Font parameters. */
	putshort(FLAG_PCL);			/* flags */
	putshort(fhp->f_font_type);
	putshort(fhp->f_orientation);
	putshort(fhp->f_spacing);
	putshort(fhp->f_symbol_set);
	putshort(fhp->f_pitch);
	putshort(ipointsz);
	putshort(fhp->f_style);
	putshort(fhp->f_weight);
	putshort(fhp->f_face);
}

/*
 * Process a character definition:
 *	<Esc> ( s <#> W <char data>
 */
void
escape_oparen()
{
	register int c;
	register int q;
	register character_header *ch;

	if ((c = getuchar()) != 's') {
		nonfatal("unknown sequence \\033(%c", c);
		return;
	}
	q = getparm();
	if ((c = getuchar()) != 'W') {
		nonfatal("unknown sequence \\033(s%d%c", q, c);
		return;
	}
	ch = char_def(q);

#ifdef	VERBOSE
	fprintf(ofp,"** %s, left off=%d, top off=%d char width=%d, char height=%d\n",
		orient_tab[ch->c_orientation],
		ch->c_left_offset, ch->c_top_offset,
		ch->c_char_width, ch->c_char_height);
	fprintf(ofp,"**  movement=%d (%d), data length = %d\n",
		ch->c_delta_x, ch->c_delta_x/4, q-16);
#ifdef	GLYPH
	dump_glyph(ch->c_char_width, ch->c_char_height, ch->c_data);
#endif
#endif
	char_movement[this_char] = ch->c_delta_x;
	char_datasize[this_char] = q-16;

	free(ch);
}

/*
 * Process a font id:
 *	<Esc> * c <#> D
 * or a character code:
 *	<Esc> * c <#> E
 */
void
escape_star()
{
	register int c, q;

	c = getuchar();
	if (c != 'c') {
		nonfatal("unknown escape sequence (0x1B 0x%02x 0x%02x ...)",
			'*', c);
		return;
	}
	q = getparm();
	c = getuchar();
	if (c == 'D')
		nonfatal("Font ID %d", q);
	else if (c == 'E')
		char_code(q);
	else
		nonfatal("unknown sequence \\033*c%d%c", q, c);
}

/* VARARGS */
void
fatal(args) char *args;
{
	fprintf(stderr, "fwtable: ");
	if (lineno != 0)
		fprintf(stderr, "%d: ", lineno);
	fprintf(stderr, "%r\n", &args);
	exit(1);
}

/*
 * Read size bytes into the supplied buffer.
 */
void
getextra(bp, size) register uchar *bp; register int size;
{
	while (size-- > 0)
		*bp++ = getuchar();
}

/*
 * Read a numeric parameter and return it.
 */
int
getparm()
{
	register int a, c;

	for (a = 0; (c = getuchar()) >= '0' && c <= '9'; ) {
		a *= 10;
		a += c - '0';
	}
	ungetc(c, ifp);
	return a;
}

/*
 * Read a short and return it.
 * The byte order is the opposite of the in-memory i8086 order.
 */
short
getshort()
{
	register int s;

	s = getuchar() << 8;
	return s | getuchar();
}

/*
 * Read an unsigned char and return it.
 */
unsigned int
getuchar()
{
	register int c;

	if ((c = fgetc(ifp)) == EOF)
		fatal("unexpected end of file");
	return (c & 0xFF);
}

/* VARARGS */
void
nonfatal(args) char *args;
{
	fprintf(stderr, "fwtable: %ld: %r\n", ftell(ifp), &args);
}

/*
 * Write count bytes from buf.
 */
void
ofpwrite(buf, count) register char *buf; register unsigned int count;
{
	if (fwrite(buf, count, 1, ofp) != 1)
		fatal("write error");
}

/*
 * Write a canonical short.
 */
void
putshort(i) int i;
{
	short s;

	if (cflag)
		fprintf(ofp, "\t\t%d,\n", i);
	else {
		s = (short)i;
		canshort(s);
		ofpwrite(&s, sizeof s);
	}
}

/*
 * Write a NUL-terminated string.
 */
void
putstring(s) register char *s;
{
	fputs(s, ofp);
	fputc('\0', ofp);
}

/*
 * Read a PCL font header.
 */
void
read_header(size) register int size;
{
	register int c;

	if (fhp != NULL)
		fatal("multiple font headers");
	fhp = alloc(size);
	fhp->f_hsize = getshort();
	fhp->f_desc_format = getuchar();
	if (fhp->f_desc_format == 10)
		fatal("Intellifont scalable fonts not yet supported");
	fhp->f_font_type = getuchar();
	fhp->f_style_msb = getuchar();
	fhp->f_reserved1 = getuchar();
	fhp->f_baseline = getshort();
	fhp->f_cell_width = getshort();
	fhp->f_cell_height = getshort();
	fhp->f_orientation = getuchar();
	fhp->f_spacing = getuchar();
	fhp->f_symbol_set = getshort();
	fhp->f_pitch = getshort();
	fhp->f_height = getshort();
	fhp->f_xheight = getshort();
	fhp->f_width_type = getuchar();
	fhp->f_style = getuchar();
	fhp->f_weight = getuchar();
	fhp->f_face = getuchar();
	if (fhp->f_face > NFACES) {
		/* Use extra entry at end of table. */
		sprintf(faces[NFACES], "[Typeface %3d]", fhp->f_face);
		fhp->f_face = NFACES;
	}
	if (size > 26)
		getextra(fhp->f_comment, size-26);
	/*
	 * Many of the fonts at MWC have a bogus extra ' ' after the font header.
	 * Eat the space silently rather than complaining about unexpected char.
	 */
	if ((c = fgetc(ifp)) != ' ')
		ungetc(c, ifp);
}

void
usage()
{
	fprintf(stderr,
		"Usage: fwtable [ -cptv ] [ -ssymset ] [ infile [ outfile ] ]\n"
		"Options:\n"
		"\t-c\t\tWrite C instead of binary\n"
		"\t-p\t\tInput is PostScript AFM file\n"
		"\t-ssymset\tSpecify desired symbol set with -t option\n"
		"\t-t\t\tInput is HP TFM file\n"
		"\t-v\t\tWrite one-line font description to stderr\n"
		);
	exit(1);
}

/* end of fwtable.c */
