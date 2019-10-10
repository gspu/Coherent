/*
 * fwt_TFM.c
 * 3/30/93
 * Build troff font width table from HP TFM file.
 * Reference: Hewlett Packard, "Tagged Font Metric Specification".
 * However, as of 6/7/91, steve has not seen the official spec,
 * so this code is based on the braindamaged source code for HP TFM reader.
 * Prints useful debugging output to stderr if compiled with DEBUG true.
 * INCOMPLETE.
 */

#include <stdio.h>
#include "fwtable.h"

/* Manifest constants. */
#define	INTEL_ORDER	0x4949
#define	MOTOROLA_ORDER	0x4D4D
#define	N_SYMBOL_SET	14		/* bytes per symbol set entry */

/* Tag values; this list does not include unused tags. */
#define	T_SYMBOL_SET	404
#define	T_POINT		406
#define	T_NOMINAL_PT	407
#define	T_DESIGN_UNITS	408
#define	T_TYPE_STRUCT	410
#define	T_STROKE_WT	411
#define	T_SPACING	412
#define	T_SLANT		413
#define	T_APPEAR_WIDTH	414
#define	T_TYPEFACE	417
#define	T_HORIZ_ESC	433
#define	T_SELECT_STR	442

/* Type. */
typedef	struct	rational {
	long	r_mul;		/* multiplier */
	long	r_div;		/* divisor */
} RATIONAL;

/* Data type lengths. */
#define	MAXTYPE	18		/* Max data type */
int type_size[MAXTYPE+1] = {
	0,			/* 0  == unused		*/
	1,			/* 1  == BYTE		*/
	1,			/* 2  == ASCII		*/
	2,			/* 3  == SHORT		*/
	4,			/* 4  == LONG		*/
	8,			/* 5  == RATIONAL	*/
	0,0,0,0,0,0,0,0,0,0,	/* 6 to 15 == unused	*/
	1,			/* 16 == SIGNEDBYTE	*/
	2,			/* 17 == SIGNEDSHORT	*/
	4			/* 18 == SIGNEDLONG	*/
};

/* Global. */
int	appear_width;		/* Width.			*/
RATIONAL design_units;		/* Design units.		*/
int	Intel_order;		/* Use Intel i8086 byte order.	*/
RATIONAL nominal_point;		/* Nominal pointsize.		*/
RATIONAL point;			/* Point size.			*/
int	selector;		/* Selector.			*/
int	slant;			/* Slant.			*/
int	spacing;		/* Spacing.			*/
int	stroke_weight;		/* Stroke weight.		*/
short	*symbol_horiz;		/* Symbol map horizontal esc.	*/
short	*symbol_set;		/* Symbol set indices.		*/
int	symbol_set_size;	/* Symbol set size.		*/
int	symset_val;		/* Encoded symbol set value.	*/
char	*typeface;		/* Typeface name.		*/
int	type_structure;		/* Type structure.		*/

/*
 * Return the GCD of two longs.
 * Euclid's algorithm.
 */
long
gcd(m, n) long m, n;
{
	long r;

	while ((r = m % n) != 0) {
		m = n;
		n = r;
	}
	return n;
}

/*
 * Read and return a long, observing specified byte order.
 */
long
getlong()
{
	register int c;

	c = getword();
	if (Intel_order)
		return (((long)getword()) << 16) | c;
	else
		return (((long)c) << 16) | getword();
}

/*
 * Read a rational and store it through the supplied pointer.
 */
void
getrational(rp) RATIONAL *rp;
{
	rp->r_mul = getlong();
	rp->r_div = getlong();
}

/*
 * Read a NUL-terminated string into buf[] and return a pointer to it.
 * No overflow check, for shame...
 */
char *
getstring()
{
	register char *cp;

	for (cp = buf; *cp++ = getuchar(); )
		;
	return buf;
}

/*
 * Read and return a short (2 bytes), observing specified byte order.
 */
int
getword()
{
	register int c;

	c = getuchar();
	if (Intel_order)
		return (getuchar() << 8) | c;
	else
		return (c << 8) | getuchar();
}

/*
 * Read TFM file.
 */
void
inputTFM()
{
	register int i, ntags;
	int tag, type;
	long size, length, curpos, offset;

	/* Read byte order word. */
	if ((i = getword()) == INTEL_ORDER)
		++Intel_order;
	else if (i != MOTOROLA_ORDER)
		fatal("unknown byte order 0x%x in TFM header", i);

	/* Skip TFM version number. */
	getword();

	/* Read typeface offset, warn if not equal to current position. */
	if ((offset = getlong()) != 8L) {
		nonfatal("warning: TFM file contains multiple typefaces; using first");
		xseek(offset);
	}

	/* Read tag count and process tags. */
	ntags = getword();
	dbprintf((stderr, "ntags=%d\n", ntags));
	for (i = 1; i <= ntags; i++) {

		/* Read tag, type, size; compute length. */
		tag = getword();
		type = getword();
		size = getlong();
		length = size * type_size[type];
		dbprintf((stderr, "tag=%d type=%d size=%ld length=%ld\n", tag, type, size, length));

		/* Save current position, seek to data position if required. */
		curpos = xtell();
		if (length > 4) {
			offset = getlong();
			dbprintf((stderr, "length=%ld, data at offset %ld\n", length, offset));
			xseek(offset);
		}
		
		/* Process the tag. */
		switch(tag) {

		case T_SYMBOL_SET:	read_symbol_set((int)size / N_SYMBOL_SET);
					break;
		case T_POINT:		getrational(&point);		break;
		case T_NOMINAL_PT:	getrational(&nominal_point);	break;
		case T_DESIGN_UNITS:	getrational(&design_units);	break;
		case T_TYPE_STRUCT:	type_structure = getuchar();	break;
		case T_STROKE_WT:	stroke_weight = getuchar();	break;
		case T_SPACING:		spacing = getword();		break;
		case T_SLANT:		slant = getword();		break;
		case T_APPEAR_WIDTH:	appear_width = PCL_width(getuchar());
					break;
		case T_TYPEFACE:	typeface = newstring(getstring());
					break;
		case T_HORIZ_ESC:	symbol_horiz = read_array((int)size);
					break;
		case T_SELECT_STR:	selector = atoi(getstring());	break;
		default:
			dbprintf((stderr, "tag %d IGNORED\n", tag));
			break;			/* Ignore other tags. */
		}

		/* Restore position for next tag. */
		xseek(curpos+4);
	}
}

/*
 * Write font width table output from TFM.
 */
void
outputTFM()
{
	register int i, j, width, max, mul;
	long g, lmul, ldiv;

	/* Calculate maximum width of characters in character set. */
	for (max = i = 0; i < NWIDTH && i < symbol_set_size; i++) {
		j = symbol_set[i];
		width = (j == -1) ? 0 : symbol_horiz[j];
		dbprintf((stderr, "i=%d j=%d width=%d\n", i, j, width));
		if (width > max)
			max = width;		
	}
	mul = (max / 256) + 1;
	dbprintf((stderr, "max=%d\n", max));
	dbprintf((stderr, "mul=%d\n", mul));
	dbprintf((stderr, "design_unit: %ld/%ld\n", design_units.r_mul, design_units.r_div));
	dbprintf((stderr, "nominal pt:  %ld/%ld\n", nominal_point.r_mul, nominal_point.r_div));
	dbprintf((stderr, "point:       %ld/%ld\n", point.r_mul, point.r_div));

	/* A little fuss to convert tag values to PCL values; thanks, HP. */
	if (vflag)
		fprintf(stderr, "%s %s\n", typeface, symset);
	fprintf(ofp, "%s %s", typeface, symset);	/* descriptor string */
	fputc('\0', ofp);		/* NUL-terminated */
	fputc('\0', ofp);		/* PS name */
	putshort(FLAG_PCL);		/* flags */
	putshort(0);			/* fonttype */
	putshort(0);			/* orientation */
	putshort(spacing == 0);		/* spacing */
	putshort(symset_val);		/* symbol set */
	if (spacing == 0)
		putshort(0);		/* pitch, proportional */
	else {
		/* UNTESTED, is this right? */
		i = (point.r_div * nominal_point.r_div * design_units.r_mul) /
		    (point.r_mul * nominal_point.r_mul * design_units.r_div * spacing);
		putshort(i);		/* pitch, fixed spacing */
	}
	i = 10 * nominal_point.r_mul / nominal_point.r_div;
	putshort(i);			/* 10 * pointsize */
	i = ((type_structure/8) << 5) | (appear_width << 2) | (slant != 0);
	putshort(i);			/* style */
	i = (stroke_weight == 0) ? -7 : (stroke_weight-1)/17 - 7;
	putshort(i);			/* weight */
	putshort(selector);		/* face */

	/* Compute multiplier and divisor. */
#if	0
	/*
	 * I think this is philosophically right,
	 * but the factors get too big when point==100/7231;
	 * just assume a point is 1/72" instead.
	 */
	if (point.r_mul == 1 && point.r_div == 72) {
		lmul = design_units.r_div * mul;
		ldiv = design_units.r_mul;
	} else {
		lmul = design_units.r_div * point.r_mul * 72 * mul;
		ldiv = design_units.r_mul * point.r_div;
	}
#else
	lmul = design_units.r_div * mul;
	ldiv = design_units.r_mul;
#endif
	dbprintf((stderr, "lmul=%ld ldiv=%ld\n", lmul, ldiv));
	if ((g = gcd(lmul, ldiv)) != 1) {
		lmul /= g;
		ldiv /= g;
	}
	dbprintf((stderr, "g=%ld lmul=%ld ldiv=%ld\n", g, lmul, ldiv));
	putshort((int)lmul);		/* mul */
	putshort((int)ldiv);		/* div */

	/* Write width table. */
	for (i = 0; i < 256; i++) {
		j = (i < symbol_set_size) ? symbol_set[i] : -1;
		width = (j == -1) ? 0 : symbol_horiz[j];
		fputc(width/mul, ofp);
		dbprintf((stderr, "i=%d width=%d\n", i, width/mul));
	}
}

/*
 * Convert appearance width tag value to PCL value.
 * Cf. "LJ III Printer Developer's Guide", p. 6-37.
 */
int
PCL_width(n) int n;
{
	if (n <= 20)	   return 4;
	else if (n <= 47)  return 3;
	else if (n <= 74)  return 2;
	else if (n <= 101) return 1;
	else if (n <= 182) return 0;
	else if (n <= 209) return 6;
	else		   return 7;
}

/*
 * Allocate space for n integer words.
 * Read the array of integers and return a pointer to the array.
 */
int *
read_array(n) int n;
{
	register int i;
	register short *ip, *p;

	p = ip = alloc(n * sizeof(short));
	for (i = 0; i < n; i++, ip++) {
		*ip = getword();
#if	0
		dbprintf((stderr, "index=%d val=%d\n", i, *ip));
#endif
	}
	return p;
}

/*
 * Read the symbol set directory.
 */
void
read_symbol_set(n) int n;
{
	register int i, found;
	long curpos, offset, symset_seek, symsel_seek;
	char *s;

	dbprintf((stderr, "n_symbol_sets=%d\n", n));
	for (found = i = 0; i < n && !found; i++) {
		dbprintf((stderr, "symbol set %d: ", i));
		symset_seek = getlong();
		symsel_seek = getlong();
		offset = getlong();
		symbol_set_size = getword();
		curpos = xtell();
		xseek(symset_seek);
		s = getstring();		/* symbol set */
		xseek(curpos);
		dbprintf((stderr, "symset=%s ", s));
		dbprintf((stderr, "index offset=%ld ", offset));
		dbprintf((stderr, "index length=%d\n", symbol_set_size));
		if (symset == NULL) {
			symset = newstring(s);	/* no -s option, take first */
			++found;
		} else if (strcmp(symset, s) == 0)
			++found;
	}
	if (!found)
		fatal("symbol set %s not found", symset);
	dbprintf((stderr, "found symset %s\n", symset));
	xseek(symsel_seek);
	s = getstring();		/* read symbol set selector */
	dbprintf((stderr, "selector=%s ", s));
	symset_val = atoi(s) * 32 + s[strlen(s)-1] - 64;
	dbprintf((stderr, "symset_val=%d\n", i));
	xseek(offset);
	symbol_set = read_array(symbol_set_size);
}

/*
 * Seek to given offset, die on error.
 */
void
xseek(offset) long offset;
{
	if (fseek(ifp, offset, SEEK_SET) == -1L)
		fatal("fseek failed");
}

/*
 * Return current seek position, die on error.
 */
long
xtell()
{
	register long offset;

	if ((offset = ftell(ifp)) == -1L)
		fatal("ftell failed");
	return offset;
}

/* end of fwt_TFM.c */
