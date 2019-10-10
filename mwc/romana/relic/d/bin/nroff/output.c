/*
 * output.c
 * troff
 * Back end to output device-dependent bits for troff.
 * Writes either PCL escape sequences for the Hewlett-Packard LaserJet
 * or (if pflag) PostScript.
 */

#include <ctype.h>
#include "roff.h"

extern	char	*fontname();
extern	char	*ndiv10();

/* HP LaserJet PCL. */
#define	HPLJEJECT	"\033&l0H"	/* page eject			*/

/* PostScript. */
#define	PSLINIT	"90 rotate\n0 -612 translate\n"
#define	PSEJECT	"\nshowpage\n"
#if	ZKLUDGE
#define	PSSAVE1	"/state1 save def\n"
#define	PSREST1	"state1 restore\n"
#endif

/*
 * Device parameters.
 */
int	ntroff	=	TROFF;		/* Programme is TROFF type	*/

long	semmul	=	1;		/* Multiplier for em space	*/
long	semdiv	=	1;		/* Divisor for em space		*/

long	senmul	=	1;		/* Multiplier for en space	*/
long	sendiv	=	2;		/* Divisor for en space		*/

long	shrmul	=	12;		/* Horizontal resolution (mul)	*/
long	shrdiv	=	5;		/* Horizontal resolution (div)	*/

long	sinmul	=	720;		/* Multiplier for inch		*/
long	sindiv	=	1;		/* Divisor for inch		*/

long	snrmul	=	1;		/* Narrow space (mul)		*/
long	snrdiv	=	6;		/* Narrow space (div)		*/

long	svrmul	=	12;		/* Vertical resolution (mul)	*/
long	svrdiv	=	5;		/* Vertical resolution (div)	*/

/*
 * Local variables for output writer.
 */
static	int	hposd;			/* device horizontal postition	*/
static	int	vposd;			/* device vertical position	*/
static	int	inword;			/* in word flag for PostScript	*/
static	int	lastfont = -1;		/* current output font		*/

#if	ZKLUDGE
static	int	npages;			/* Output page count		*/
#endif

/*
 * Initialize the device.
 */
dev_init()
{
#if	ZKLUDGE
	if (Zflag)
		printf(PSSAVE1);
#endif
}

#if	ZKLUDGE
/*
 * Close the device.
 */
dev_close()
{
	if (Zflag != 0)
		printf(PSREST1);
}
#endif

/*
 * Compute the scaled width of a character:
 *	width(c) == unit(swdmul*fonwidt[c]*psz, swddiv).
 */
int
width(c) register int c;
{
	register long l;

	/* Watch out for sign-extension of argument! */
	l = swdmul * fonwidt[c & 0xFF] * psz;
	return (swddiv == 1) ? (int)l : (int)(l/swddiv);
}

/*
 * Given a font number, change to the given font.
 */
dev_font(n) register int n;
{
	register FWTAB	*fp;

#if	0
	fprintf(stderr, "dev_font(%d)\n", n);
#endif
	if (n >= nfonts)
		panic("bad font %d at dev_font, nfonts=%d", n, nfonts);
	curfont = n;
	addidir(DFONT, n);
	fp = fwptab[n];
	csz = fcsz[n];
	swdmul = (long)fp->f_num;
	swddiv = (long)fp->f_den;
	fonwidt = fp->f_width;
	varsp = fp->f_spacing;
	dev_ps(fpsz[n]);
}

/*
 * Change to the given pointsize.
 * There are several different notions of pointsize:
 *	global psz (and oldpsz) in the environment,
 *	fpsz for each font in the environment,
 *	fp->f_psz for each font, and
 *	wpsz in flushl() (see comment below).
 * The font fp->f_psz is used to initialize fpsz in the environment
 * but is subsequently unused.
 * The environment fpsz[n] gives the current pointsize for each font.
 * The environment psz is the pointsize of the current input font,
 * wpsz is the pointsize of the current output font.
 * Changing pointsize with a .ps directive or \s escape
 * changes fpsz[] for each font.
 */
dev_ps(n) register int n;
{
	if (n <= 0) {
		printe("illegal pointsize %d, ignored", n/10);
		return;
	}
	oldpsz = psz;
	psz = n;
	addidir(DPSZE, n);

	/* Scale space width for pointsize change. */
	if (psz != oldpsz && sszdiv != 0)
		ssz = (int)((long)sszmul * psz / sszdiv);
}

/*
 * Set a fixed pointsize for a font.
 */
dev_fz(font, size) char *font, size;
{
	char name[2];
	register int n;

	argname(font, name);
	if ((n = font_number(name, ".fz: ")) < 0)
		return;
	fwptab[n]->f_flags |= F_FIXED;
	fpsz[n] = number(size, SMPOIN, SDPOIN, fpsz[n], 0, fpsz[n]);
	if (n == curfont)
		dev_ps(fpsz[n]);
}

/*
 * Specify a constant size font.
 */
dev_cs(n, size) register int n, size;
{
	fcsz[n] = size;
	if (n == curfont)
		csz = size;
}

/*
 * Change all non-fixed font pointsizes to the given pointsize,
 * then change the current pointsize.
 */
newpsze(n) register int n;
{
	register int i;

	if (n <= 0) {
		printe("illegal pointsize %d, ignored", n/10);
		return;
	}
	for (i = 0; i < nfonts; i++)
		if (((fwptab[i]->f_flags) & F_FIXED) == 0) {
			fwptab[i]->f_flags &= ~F_USED;
			fpsz[i] = n;
		}
	dev_ps(n);
}

/*
 * Given a pointer to a buffer containing stream directives
 * and a pointer to the end of the buffer,
 * print the buffer.
 * The output writer maintains its own notion of current font and pointsize
 * because [nt]roff buffers output and then flushes at the end of a line;
 * the environment "curfont" is the current font for input stream processing,
 * the output writer "font" is the current output stream font.
 * The font change is implicit until a character is written in the new font.
 */
flushl(buffer, bufend) CODE *buffer, *bufend;
{
	register CODE	*cp;
	register int	code;
	int		i;
	char		*tp;
	static	FWTAB	*fp;		/* current font table entry	*/
	static	int	newpage = 1;	/* new page flag		*/
	static	int	hpost;		/* troff horizontal pos (u's)	*/
	static	int	vpost;		/* troff vertical pos (u's)	*/
	static	int	font = -1;	/* current font			*/
	static	unsigned char *wtab;	/* current font width table	*/
	static	long	wnum;		/* current width numerator	*/
	static	long	wden;		/* current width denominator	*/
	static	int	wpsz;		/* current pointsize		*/

	for (cp = buffer; cp < bufend; cp++) {
		code = cp->l_arg.c_code;
		i = cp->l_arg.c_iarg;
		if (pflag && is_dir(code))
			endword();
#if	0
		fprintf(stderr, "output: code=%d arg=%d\n", code, i);
#endif
		switch (code) {
		case DNULL:			/* null code */
		case DHYPH:			/* place to hyphenate */
			continue;
		case DHMOV:			/* move horizontally */
		case DPADC:			/* paddable character */
			hpost += i;
			if (hpost < 0)
				hpost = 0;
			continue;
		case DVMOV:			/* move vertically */
			vpost += i;
			if (vpost < 0)
				vpost = 0;
			continue;
		case DFPOS:			/* fix position */
			move();
			continue;
		case DFONT:			/* change font */
			if (i == font)
				continue;	/* unchanged */
			if ((unsigned)i >= nfonts)
				panic("bad font %d, nfonts=%d", i, nfonts);
			font = i;
			fp = fwptab[font];
			wtab = fp->f_width;
			wpsz = fpsz[font];
			wnum = (long)fp->f_num * wpsz;
			wden = (long)fp->f_den;
			continue;
		case DTRAB:			/* transparent line */
			tp = cp->b_arg.c_bufp;
			while (*tp)
				putchar(*tp++);
			free(cp->b_arg.c_bufp);
			continue;
		case DPSZE:			/* change pointsize */
			if (wpsz != i) {
				wpsz = i;
				wnum = (long)fp->f_num * wpsz;
				if (pflag) {
					/* Mask off used bit to force rescaling */
					lastfont = -1;
					fp->f_flags &= ~F_USED;
				} else {
					printf("\033(s%sV", ndiv10(wpsz));
				}
			}
			continue;
		case DSPAR:			/* space down and return */
			hposd = hpost = 0;
			vpost += i;
			if (vpost < 0)
				vpost = 0;
			else if (vpost >= pgl) {
				/* New page. */
				endpage();
				vpost %= pgl;
				vposd = vpost;
				newpage = 1;
			}
			continue;
		case DHLIN:
		case DVLIN:
			if (code == DHLIN && i < 0) {
				hpost += i;
				i = -i;
			}
			hposd = hpost;
			vposd = vpost;
			move();
			if (code == DHLIN) {
				printf("\n%s 0 L", ndiv10(i));
				hpost += i;
			} else {
				printf("\n0 %s L", ndiv10(-i));
				vpost += i;
			}
			continue;
		default:			/* print something */
			/* Start a new page. */
			if (newpage) {
				if (lflag && pflag)
					printf(PSLINIT);
				hposd = hpost;
				vposd = vpost;
				move();
				newpage = 0;
			}
			/*
			 * Check whether we are on a new page.
			 * Note, the laser goes to funny places when it
			 * crosses a page boundary.
			 */
			if (vpost >= pgl) {
				/* New page. */
				endpage();
				vpost %= pgl;
				vposd = vpost;
				hposd = hpost;
				newpage = 1;
			}
			/* Output move to new position if appropriate. */
			if (hpost != hposd || vpost != vposd) {
				hposd = hpost;
				vposd = vpost;
				move();
			}
			/* Change to different font. */
			if (lastfont != font)
				selectfont(lastfont=font, wpsz);
			if (code == DHYPC)
				code = '-';
			if (code < 0 || code >= NWIDTH)
				panic("bad directive %d", code);
			hpost += cp->c_arg.c_move;
			hposd += unit(wnum*wtab[code], wden);
			i = code;
			if (pflag && !inword)
				startword();
			outchar(code);
			if (enb != 0) {	/* dag's enbolden...	*/
				hposd -= unit(wnum*wtab[i], wden);
				vposd -= enb;
				move();
				putchar(code);
				hposd -= enb;
				move();
				putchar(code);
				vposd += enb;
				putchar(code);
				hposd += (unit(wnum*wtab[i], wden) + enb);
				move();
			}
		}
	}
}

/*
 * Move horizontally and/or vertically.
 * This used to shift PCL output right to avoid printing at left border;
 * that responsibility is now left to the user.
 */
move()
{
	static int vold = -1;

	if (pflag) {
		/* PostScript. */
		if (inword)
			endword();
		printf("\n%s", ndiv10(hposd+pof));
		printf(" %s M", ndiv10(pgl - vposd));
	} else {
		/* PCL. */
		if (vposd == vold)
			printf("\033&a%dH", hposd+pof);
		else
			printf("\033&a%dh%dV", hposd+pof, vposd);
	}
	vold = vposd;
}

/*
 * Select a font.
 */
selectfont(font, ptsize) int font, ptsize;
{
	register FWTAB *fp;
	register char *s;

	fp = fwptab[font];
	if (!pflag) {
		/* Select font via PCL. */
		printf("\033(%d%c", fp->f_symset/32, fp->f_symset%32+64); /* symbol set */
		printf("\033(s%dp", fp->f_spacing);		/* spacing */
		/* fp->f_pitch is in quarterdots/char, convert to chars/inch. */
		if (fp->f_pitch != 0)
			printf("%sh", ndiv10(12000/fp->f_pitch)); /* pitch */
		printf("%sv", ndiv10(fpsz[font]));		/* point size */
		printf("%ds", fp->f_style);			/* style */
		printf("%db", fp->f_weight);			/* stroke weight */
		printf("%dT", fp->f_face);			/* typeface */
		return;
	}
	/* Select font via PostScript. */
	if ((s = fontname(font)) == NULL)
		panic("botch: fontname(%d)", font);
	endword();
	if ((fp->f_flags & F_USED) == 0) {
		fp->f_flags |= F_USED;
		printf("\n"
			"/font%s /%s findfont %d.%d scalefont def\n"
			"/f%s { font%s setfont } bind def\n",
			s, fp->f_PSname,
			ptsize / 10, ptsize % 10,
			s, s);
	}
	printf(" f%s", s);
}

/*
 * Convert e.g. 123 to "12.3" without using floating point output.
 * Return a pointer to statically allocated buffer.
 */
char *
ndiv10(n) register int n;
{
	static char buf[10];
	register char *cp;

	cp = buf;
	if (n < 0) {
		n = -n;
		*cp++ = '-';
	}
	if ((n % 10) == 0)
		sprintf(cp, "%d", n/10);
	else
		sprintf(cp, "%d.%d", n/10, n%10);
	return buf;
}

/*
 * Output a character.
 * If writing PostScript output, watch out for "()\\"
 * and expand high-bit characters to octal escapes.
 */
outchar(n) register int n;
{
	if (pflag) {
		if (n >= 128) {
			printf("\\%03o", n);
			return;
		} else if (n == '(' || n == ')' || n == '\\')
			putchar('\\');
	}
	putchar(n);
}

startword()
{
	putchar(' ');
	putchar('(');
	inword = 1;
}

endword()
{
	if (inword) {
		printf(") S");
		inword = 0;
	}
}

endpage()
{
	if (pflag)
		endword();
	printf((pflag) ? PSEJECT : HPLJEJECT);
#if	ZKLUDGE
	if (Zflag && ++npages % Zflag == 0) {
		register int i;

		printf(PSREST1);
		printf(PSSAVE1);
		for (i = 0; i < nfonts; i++)
			fwptab[i]->f_flags &= ~F_USED;
		lastfont = -1;
		dev_font(curfont);
	}
#endif
}

/* end of output.c */
