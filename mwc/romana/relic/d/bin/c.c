/*
 * c.c
 * 9/23/91
 * Usage: c [ -lN ] [ -wN ] [ -012 ]
 * Columnize from stdin to stdout.
 */

#include	<stdio.h>

#define	VERSION	"1.3"

extern	char	*getenv();

#define	NAV	6			/* max # environmental args */

typedef	struct line {
	struct line	*l_next;
	char		*l_line;
} LINE;

/* Forward. */
char	*item();
char	*sel0();
char	*sel1();
char	*sel2();
void	nomem();
void	fatal();

/* Globals. */
int	fieldmax;			/* # columns of widest field	*/
int	length;				/* # lines down page		*/
LINE	*lhead;				/* head of LINE list		*/
LINE	*ltail;				/* tail of LINE list		*/
int	nacross;			/* # fields across page		*/
int	ndown;				/* # fields down page		*/
int	nfields;			/* # fields read from input	*/
int	npp;				/* # fields per page		*/
char	*(*select)() = sel1;		/* selector function		*/
char	*usage = "\nUsage: c [ -lN ] [ -wN ] [ -012 ]";
int	width = 80;			/* # columns across page	*/

main(argc, argv) int argc; char *argv[];
{
	char obuf[BUFSIZ], av[NAV];

	envargs(av);
	setflags(av);
	setflags(&argv[1]);
	setbuf(stdout, obuf);
	input();
	output();
	exit(0);
}

setflags(av) register char **av;
{

	for (; *av; ++av) {
		if (av[0][0] != '-')
			fatal("%s is not an option%s", av[0], usage);
		switch (av[0][1]) {
		case '0':
			select = sel0;
			break;
		case '1':
			select = sel1;
			break;
		case '2':
			select = sel2;
			break;
		case 'w':
			width = atoi(&av[0][2]);
			break;
		case 'l':
			length = atoi(&av[0][2]);
			break;
		case 'V':
			fprintf(stderr, "c: V%s\n", VERSION);
			break;
		default:
			fatal("bad option %s%s", av[0], usage);
		}
	}
}

input()
{
	register LINE	*lp;
	register	i;
	char		lbuf[BUFSIZ];

	while ((i=getline(lbuf)) >= 0) {
		lp = malloc(sizeof *lp);
		if (lp == NULL)
			nomem();
		if (lhead)
			ltail->l_next = lp;
		else
			lhead = lp;
		ltail = lp;
		lp->l_next = NULL;
		if (i) {
			lp->l_line = malloc(i+1);
			if (lp->l_line == NULL)
				nomem();
			strcpy(lp->l_line, lbuf);
		}
		else
			lp->l_line = NULL;
		++nfields;
	}

	++fieldmax;
}

output()
{
	register int i, j;

	nacross = (width+1) / fieldmax;
	if (nacross <= 0)
		nacross = 1;
	if (length != 0)
		npp = length * nacross;
	ndown = (nfields + nacross - 1) / nacross;
	for (i = 0; i < ndown; ++i) {
		for (j = 0; j < nacross; ++j)
			putline((*select)(i, j));
		putline((char *)0);
	}

	fclose(stdout);
}


getline(lbuf) char lbuf[];
{
	register char	*p;
	register int	col, xcol;
	int		c;

	p = lbuf;
	col = 0;
	xcol = 0;

	for (; ;) {
		switch (c = getchar()) {
		case EOF:
			return (-1);
		case '\n':
			break;
		case ' ':
			++xcol;
			continue;
		case '\t':
			xcol = (xcol|7) + 1;
			continue;
		case '\b':
			if (xcol > col)
				--xcol;
			else if (col) {
				--col;
				--xcol;
				*p++ = c;
			}
			continue;
		default:
			while (col < xcol) {
				*p++ = ' ';
				++col;
			}
			*p++ = c;
			++col;
			++xcol;
			continue;
		}
		break;
	}

	if (col > fieldmax)
		fieldmax = col;
	*p = '\0';
	return (p - lbuf);
}

putline(lbuf) char *lbuf;
{
	register	c;
	register char	*p;
	static		col,
			xcol;

	p = lbuf;
	if (p == NULL) {
		col = 0;
		xcol = 0;
		putchar('\n');
		return;
	}

	while (c = *p++)
		switch (c) {
		case ' ':
			++xcol;
			break;
		case '\b':
			--col;
			--xcol;
			putchar(c);
			break;
		default:
			while ((col|7)+1 <= xcol) {
				putchar('\t');
				col = (col|7) + 1;
			}
			while (col < xcol) {
				putchar(' ');
				++col;
			}
			putchar(c);
			++col;
			++xcol;
			break;
		}

	xcol += fieldmax - xcol%fieldmax;
}

char *
item(n) int n;
{
	register LINE *lp;
	register int m;

	for (m = 0, lp = lhead; lp; lp = lp->l_next)
		if (++m > n)
			return (lp->l_line ? lp->l_line : "");
	return "";
}

/*
 * Across each row, then on to next row.
 */
char *
sel0(i, j) int i, j;
{
	return item(i*nacross + j);
}

/*
 * Down each column, then on to next column.
 * Leave spaces in last column.
 */
char *
sel1(i, j) int i, j;
{
	register int page, rows, line;

	if (length == 0)
		return item(i + j*ndown);
	page = i / length;			/* page number */
	line = i % length;			/* line on page */
	if (page == (ndown-1)/length)
		rows = ((ndown-1)%length) + 1;	/* rows on last page */
	else
		rows = length;			/* rows on full page */
	return item(page * npp + line + j * rows);
}

/*
 * Down each column, then on to next column.
 * Leave spaces in last row.
 */
char *
sel2(i, j) int i, j;
{
	register int ncols, page, line, adjust;

	if (i * nacross + j >= nfields)
		return "";
	ncols = nfields % nacross;		/* columns in incomplete last line */
	adjust = (j > ncols) ? ncols : j;	/* last line adjust */
	if (length == 0)
		return item(i			/* n on prev. lines this col. */
			   + j*(ndown-1)	/* n on prev. cols. */
			   + adjust);		/* n on last line */
	page = i / length;			/* page number */
	line = i % length;			/* line on page */
	if (page != (ndown-1)/length)		/* not last page */
		return item(page * npp + line + j * length);
	else					/* last page */
		return item(page * npp		/* n on prev. pages. */
			   + line		/* n on prev. lines this col. */
			   + j*((ndown-1)%length)	/* n on prev. cols. */
			   + adjust);		/* n on last line */
}

envargs(av) char **av;
{
	register	fl;
	register char	*p;
	register char	**ap;

	ap = av;
	*ap = NULL;
	if ((p=getenv("C")) == NULL)
		return;

	fl = 0;
	while (*p)
		switch (*p++) {
		case '\t':
		case '\n':
		case ' ':
			fl = 0;
			p[-1] = '\0';
			break;
		default:
			if (fl)
				break;
			if (ap >= &av[NAV-1])
				fatal("too many environmental arguments");
			*ap++ = &p[-1];
			++fl;
			break;
		}

	*ap = NULL;
}

void
nomem()
{

	fatal("out of memory");
}

void
fatal(arg0)
char	*arg0;
{

	fflush(stdout);
	fprintf(stderr, "c: %r\n", &arg0);
	exit(1);
}

/* end of c.c */
