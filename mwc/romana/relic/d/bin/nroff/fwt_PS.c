/*
 * fwt_PS.c
 * 6/7/91
 * Build troff font width table from PostScript AFM file.
 * Reference: "Adobe Font Metric Files Specification Version 3.0", 3/8/90.
 */

#include <stdio.h>
#include <canon.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "fwtable.h"

/*
 * Recognized keywords.
 * Most of the AFM file is ignored, recognized keys are listed here.
 */
#define	OTHER		0		/* anything other than listed below */
#define	COMMENT		1
#define	ENDCHAR		2
#define	ENDFONT		3
#define	FONTNAME	4
#define	FULLNAME	5
#define	STARTCHAR	6
#define	STARTFONT	7
#define	WEIGHT		8
typedef	struct	key {
	char	*key_name;
	int	key_val;
	size_t	key_len;
} KEY;
KEY	keys[] = {
	"Comment",		COMMENT,	 7,
	"EndCharMetrics",	ENDCHAR,	14,
	"EndFontMetrics",	ENDFONT,	14,
	"FontName",		FONTNAME,	 8,
	"FullName",		FULLNAME,	 8,
	"StartCharMetrics",	STARTCHAR,	16,
	"StartFontMetrics",	STARTFONT,	16,
	"Weight",		WEIGHT,		 6
};
#define	NKEYS	(sizeof keys/sizeof (struct key))

/* Globals. */
char	*bufp;				/* buffer pointer		*/
int	chartab[NWIDTH];		/* character movement table	*/
char	*FontName;			/* AFM FontName			*/
char	*FullName;			/* AFM FullName			*/
int	lineno;				/* input line number		*/
int	maxwidth;			/* maximum character movement	*/
char	*Weight;			/* AFM Weight			*/

/*
 * Process an AFM file character metric line
 * and store the character width in the width table.
 * Very ad hoc.
 * Understands only "C", "CH", "W", "WX"; everything else is ignored.
 * Should probably understand "WX0" as well.
 */
void
character()
{
	register char *s, *next;
	int code, width, base;

	code = -2;			/* -1 to 255 are legal */
	width = -1;
	for (s = buf; s != NULL; s = next) {
		if ((next = strchr(s, ';')) != NULL)
			*next++ = '\0';
		while(isspace(*s))
			s++;
		if (*s == 'C') {
			++s;
			if (*s == 'H') {
				++s;
				base = 16;
			} else
				base = 10;
			code = (int)strtol(s, NULL, base);
			if (code < -1 || code >= NWIDTH)
				fatal("illegal character code %d\n", code);
		} else if (*s == 'W' && isspace(*(s+1)))
			width = atoi(s+2);
		else if (*s == 'W' && *(s+1) == 'X')
			width = atoi(s+2);
	}
	if (code == -2)
		fatal("missing character code");
	if (width == -1)
		fatal("missing character width");
	if (code != -1) {
		chartab[code] = width;
		if (width > maxwidth)
			maxwidth = width;
	}
}

/*
 * Process the AFM file character metrics section.
 */
void
charmetrics()
{
	register int cmsize, nchars;

	cmsize = atoi(bufp);		/* number of chars to expect */
	nchars = 0;			/* number of chars seen so far */
	while (fgets(buf, sizeof buf, ifp) != NULL) {
		++lineno;
		switch(lookup()) {
		case COMMENT:
			continue;
		case ENDCHAR:
			if (nchars != cmsize)
				nonfatal("warning: %d chars in character metrics section, expected %d\n",
					nchars, cmsize);
				return;
		default:
			++nchars;
			character();
			break;
		}
	}
	fatal("missing EndCharMetrics");
}

/*
 * Process AFM input file.
 * Most of the specifications are ignored.
 */
void
inputPS()
{
	register int n, flag;

	for (flag = 1; fgets(buf, sizeof buf, ifp) != NULL; ) {
		++lineno;
		if ((n = lookup()) == COMMENT)
			continue;
		else if (flag) {
			if (n != STARTFONT)
				fatal("not an AFM file (no StartFontMetrics)");
			flag = 0;
			continue;
		}
		switch(n = lookup()) {
		case OTHER:	/* ignored... */			break;
		case ENDCHAR:	fatal("unexpected EndCharMetrics");	break;
		case ENDFONT:	return;
		case FONTNAME:	FontName = newstring(bufp);		break;
		case FULLNAME:	FullName = newstring(bufp);		break;
		case STARTCHAR:	charmetrics();				break;
		case STARTFONT:	fatal("unexpected StartFontMetrics");	break;
		case WEIGHT:	Weight = newstring(bufp);		break;
		default:	fatal("lookup botch %d", n);		break;
		}
	}
}

/*
 * See if the line in buf[] starts with a recognized keyword.
 * If so, set bufp to point past it and return its key_val.
 * If not, return OTHER.
 * This does not skip leading whitespace in the line,
 * if whitespace is legal then it should.
 */
int
lookup()
{
	register KEY *kp;
	register size_t n;
	char *s;

	if ((s = strrchr(buf, '\n')) != NULL)
		*s = '\0';			/* zap trailing newline */
	for (kp = keys; kp < &keys[NKEYS]; kp++) {
		n = kp->key_len;
		if (strncmp(buf, kp->key_name, n) == 0) {
			bufp = buf + n;
			if (*bufp != '\0') {
				if (!isspace(*bufp))
					continue;	/* no match */
				while (isspace(*bufp))
					bufp++;		/* skip trailing whitespace */
			}
			return kp->key_val;
		}
	}
	return OTHER;
}

/*
 * Return pointer to allocated copy of string s.
 */
char *
newstring(s) register char *s;
{
	return strcpy(alloc(strlen(s) + 1), s);
}

/*
 * Write a font width table for troff.
 * Most of the fields in the binary FWT are for PCL and are zeroed here.
 */
void
outputPS()
{
	register int i, mul;
	char *fullname;
	int spacing, width;

	if (FontName == NULL)
		fatal("no FontName specified");
	fullname = (FullName == NULL) ? FontName : FullName;

	/* Verbose output. */
	if (vflag) {
		if (FullName != NULL)
			fprintf(stderr, "FullName = %s\n", FullName);
		fprintf(stderr, "FontName = %s\n", FontName);
		if (Weight != NULL)
			fprintf(stderr, "Weight = %s\n", Weight);
	}

	/* Descriptive name. */
	if (cflag) {
		fprintf(ofp, "{\n\t\"%s", fullname);
		fprintf(ofp, "\",\n");
	} else {
		fputs(fullname, ofp);
		fputc(0, ofp);
	}

	/* Font name. */
	if (cflag)
		fprintf(ofp, "\t\"%s\",\n", FontName);
	else {
		fputs(FontName, ofp);
		fputc(0, ofp);
	}

	/* Determine if fixed or variable spacing. */
	width = spacing = 0;				/* assume fixed spacing */
	for (i = 0; i < NWIDTH; i++) {
		if (chartab[i] == 0)
			continue;
		else if (width == 0)
			width = chartab[i];		/* first nonzero width */
		else if (width != chartab[i]) {
			spacing = 1;			/* variable spacing */
			break;
		}
	}

	/*
	 * Write FWTAB fields.
	 * All but spacing, pitch, ptsize, num, den are zeroed.
	 * This uses a nominal point size of 10, for no particular reason.
	 */
	putshort(FLAG_PS);
	putshort(0);
	putshort(0);
	putshort(spacing);		/* spacing */
	putshort(0);
	putshort(44);			/* pitch */
	putshort(100);			/* 10 * ptsize */
	putshort(0);			/* style */
	putshort(0);			/* weight */
	putshort(0);			/* face */
	mul = (maxwidth / 256) + 1;	/* scale factor */
	putshort(mul);			/* mul */
	putshort(1000);			/* div */

	/* Write width table. */
	if (cflag) {
		fprintf(ofp, "\t{");
		for (i = 0; i < NWIDTH; i++) {
			if (i % 8 == 0)
				fprintf(ofp, "\n\t\t");
			fprintf(ofp, "%3d, ", chartab[i]/mul);
		}
		fprintf(ofp, "\n\t}\n}\n");
	} else {
		for (i = 0; i < NWIDTH; i++)
			fputc(chartab[i]/mul, ofp);
	}
}

/* end of fwt_PS.c */
