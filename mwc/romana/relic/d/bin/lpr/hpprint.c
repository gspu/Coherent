/*
 * hpprint.c
 * 12/13/90
 * Print a file for the hp daemon.
 * This is nontrivial for two reasons.
 * First, the device might be a serial line,
 * so this needs to worry about getting the mode bits right.
 * Second, "hpr foo" should expand tabs in foo,
 * but "hpr -f 1 font" should not muck with the binary font bits.
 * This routine therefore knows some gory details about HP PCL.
 */

#include	<stdio.h>
#include	<sgtty.h>

extern	int	printing;
extern	FILE	*lp;

char ebuf[64];

/*
 * Print the given file.
 * If fontnum is not -1, the file contains a soft font;
 * precede and follow it with PCL commands to load it as font fontnum.
 */
print(file, fontnum) char *file; int fontnum;
{
	register FILE	*f;
	register int c;
	register int nraw = 0;
	int col = 0;
	struct sgttyb sg;

	f = fopen( file, "r");
	if (f == NULL)
		return (1);
	ioctl(fileno(lp), TIOCGETP, &sg);
	c = sg.sg_flags;
	sg.sg_flags &= ~(XTABS|CRMOD);
	ioctl(fileno(lp), TIOCSETP, &sg);
	sg.sg_flags = c;
	if (fontnum != -1)
		fprintf(lp, "\033*c%dD", fontnum);	/* assign font # */
	while ((c=getc(f)) != EOF && printing > 0) {
		if (nraw) {
			putc(c, lp);
			--nraw;
			continue;
		}
		switch (c) {
		case 033:
			c = escape(f);
			/* PCL commands followed by binary data. */
			if ((c == 'W' && ebuf[1] == '*' && ebuf[2] == 'b')
			 || (c == 'W' && ebuf[1] == '(' && ebuf[2] == 's')
			 || (c == 'W' && ebuf[1] == ')' && ebuf[2] == 's')
			 || (c == 'X' && ebuf[1] == '&' && ebuf[2] == 'p'))
				nraw = atoi(ebuf+3);		/* data count */
			continue;
		case '\n':
			putc('\r', lp);
			col = 0;
			putc(c, lp);
			continue;
		case '\r':
			col = 0;
			putc(c, lp);
			continue;
		case '\t':
			do putc(' ', lp); while ((++col%8) != 0);
			continue;
		case '\b':
			if (col)
				col -= 1;
			putc(c, lp);
			continue;
		default:
			if (c >= ' ' && c < 0177)
				++col;
			putc(c, lp);
			continue;
		}
	}
	if (fontnum != -1)
		fprintf(lp, "\033*c5F");	/* make font permanent */
	ioctl(fileno(lp), TIOCSETP, &sg);
	fclose( f);
	return (0);
}

/*
 * Copy a PCL command (escape sequence) to the printer,
 * but save a copy in the buffer for later examination.
 */
escape(fp) register FILE *fp;
{
	register char *p;
	register int c;

	p = ebuf;
	*p++ = 033;
	putc(033, lp);
	if ((c = getc(fp)) != EOF && printing > 0) {
		putc(c, lp);
		*p++ = c;
		*p = 0;
		if (c == 'E' || c == '=' || c == '9' || c == 'Y' || c == 'Z')
			return c;
		while ((c = getc(fp)) != EOF && printing > 0) {
			putc(c, lp);
			*p++ = c;
			*p = 0;
			if (c >= '@' && c <= 'Z')
				return c;	/* read until UPPERCASE char */
		}
	}
	return 0;
}

/* end of hpprint.c */
