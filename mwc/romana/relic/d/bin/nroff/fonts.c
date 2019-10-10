/*
 * fonts.c
 * Troff.
 * Font loading.
 */

#include <ctype.h>
#include <canon.h>
#include "roff.h"

static	int	errflag;		/* font table read error flag */
int		nfonts;			/* number of fonts available	*/

/*
 * Map user font names to font numbers.
 * Several troff font names can map to the same font number.
 * Entries get added by .lf or .rf.
 */
FTB	fontab[NFNAMES];

/*
 * Font width table pointers, indexed by font number.
 * The order of built-in entries corresponds to the indices in "fonts.h".
 * The .lf request adds additional entries to this table.
 */
FWTAB	*fwptab[NFONTS];

/* Read a canonical short from file. */
int
get_short(fp) register FILE *fp;
{
	short s;

	if (fread(&s, sizeof s, 1, fp) != 1)
		errflag = 1;
	canint(s);
	return (int)s;
}

/*
 * Read a NUL-terminated string from file.
 * Return a pointer to a newly allocated copy of it.
 * Die if it is too long.
 */
#define	NBUF	256
char *
get_str(fp) register FILE *fp;
{
	static char buf[NBUF];
	register char *s;
	register int c;

	for (s = buf; s < &buf[NBUF]; ) {
		c = fgetc(fp);
		*s++ = c;
		if (c == '\0')
			break;
		else if (c == EOF) {
			errflag = 1;
			return NULL;
		}
	}
	if (s == &buf[NBUF]) {
		errflag = 1;
		return;
	}
	s = nalloc(strlen(buf) + 1);
	strcpy(s, buf);
	return s;
}

void
load_font(s, file) char *s, *file;
{
	register FWTAB *p;
	register FILE *fp;
	int i, new, newflag;

	if ((fp = fopen(file, "rb")) == NULL) {
		/* Not found, look in default fwt directory. */
		sprintf(miscbuf, "%s%sfwt/%s",
			LIBDIR, (pflag) ? TPSDIR : TPCLDIR, file);
		if ((fp = fopen(miscbuf, "rb")) == NULL) {
			printe(".lf: cannot open file \"%s\"", file);
			return;
		}
	}
	if ((new = font_num(s)) == -1) {
		/* Font does not already exist, allocate new FWTAB entry. */
		if (nfonts >= NFONTS) {
			printe(".lf: cannot load more than %d fonts", NFONTS);
			return;
		}
		newflag = 1;
		new = nfonts;			/* assign new font number */
		fwptab[new] = nalloc(sizeof(FWTAB)); /* allocate new FWTAB */
	} else
		newflag = 0;
	errflag = 0;
	p = fwptab[new];
	p->f_descr = get_str(fp);
	p->f_PSname = get_str(fp);
	p->f_flags = get_short(fp);
	p->f_fonttype = get_short(fp);
	p->f_orientation = get_short(fp);
	p->f_spacing = get_short(fp);
	p->f_symset = get_short(fp);
	p->f_pitch = get_short(fp);
	p->f_psz = get_short(fp);
	p->f_style= get_short(fp);
	p->f_weight = get_short(fp);
	p->f_face = get_short(fp);
	p->f_num = get_short(fp);
	p->f_den = get_short(fp);
	for (i = 0; i < NWIDTH; i++)
		p->f_width[i] = fgetc(fp);
	i = fgetc(fp);				/* should return EOF */
	fclose(fp);
	if (i != EOF || errflag) {
		printe(".lf: %s, file \"%s\"",
			(i != EOF) ? "bad font width table" : "read error",
			file);
		if (newflag)
			free(p);
		return;
	} else if ((p->f_flags & F_PCL) == 0 && !pflag)
		printe(".lf: \"%s\" is not a PCL font width table", file);
	else if ((p->f_flags & F_PS) == 0 && pflag)
		printe(".lf: \"%s\" is not a PostScript font width table", file);
	fpsz[new] = p->f_psz;			/* enter pointsize in env */
	fcsz[new] = 0;				/* enter const char size in env */
	assign_font(s, new);			/* and assign desired name */
	if (newflag)
		++nfonts;
}

/*
 * List all the font names and descriptions in this version.
 */
void
font_display()
{
	register FTB *p;
	register int a, b;

	fprintf(stderr, "Fonts available in this version:\n");
	for (p = fontab; p < &fontab[NFNAMES]; p++) {
		if ((a = p->f_name[0]) == 0)
			break;
		if ((b = p->f_name[1]) == 0)
			b = ' ';
		fprintf(stderr," %c%c %s\n", a, b, fwptab[p->f_font]->f_descr);
	}
	fprintf(stderr,
"Additional fonts may be loaded with the .lf request.\n"
"Fonts may be renamed with the .rf request.\n"
		);
}

/*
 * Return fontname associated with font number n.
 * Because the mapping is many->one, the user might have
 * specified the font with a different name.
 * The returned value points to a statically allocated buffer.
 */
char *
fontname(n) register int n;
{
	static char buf[3];
	register FTB *p;

	for (p = fontab; p < &fontab[NFNAMES]; p++) {
		if (p->f_font == n) {
			buf[0] = p->f_name[0];
			buf[1] = p->f_name[1];
			buf[2] = '\0';
			return buf;
		}
	}
	return NULL;
}

/* end of fonts.c */
