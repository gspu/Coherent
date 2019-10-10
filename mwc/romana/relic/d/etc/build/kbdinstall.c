/*
 * kbdinstall.c
 * 10/24/91
 * Keyboard support installation.
 *
 * cc kbdinstall.c -lcurses -lterm
 * Usage: kbdinstall [ -bu ]
 * Options:
 *	-b	invoked from build
 *	-u	invoked from update install, don't patch running system
 */

#include <stdio.h>
#include <string.h>
#include <curses.h>

extern	char	*fgets();
extern	char	*malloc();

#define	VERSION	"2.2"			/* Version number.	*/
#define	KBDDIR	"/conf/kbd"		/* Keyboard directory.	*/
#define	KBDLIST	"/conf/kbd/.list"	/* List of keyboards.	*/
#define	KBDFILE	"/tmp/kbd"		/* File containing driver object name */
#define	KBDY	4			/* y-coordinate of keyboard list. */
#define	NLINE	512			/* Line length.		*/
#define	NKBDS	(24-KBDY)		/* Max number of keyboards */
#define	USAGE	"Usage: /etc/kbdinstall [-bu]\n"

/* Keyboard list file entries. */
typedef	struct	kline	{
	int	k_loadable;		/* True: loadable tables supported */
	char	*k_driver;		/* Name of keyboard driver object */
	char	*k_table;		/* Keyboard table name	*/
	char	*k_desc;		/* Keyboard description	*/
}	KLINE;

/* Forward. */
char	*copystr();
void	display_line();
void	display_rev();
void	fatal();
void	format_error();
void	nonfatal();
void	read_klist();

/* Globals. */
int	bflag;				/* Invoked from build.	*/
char	buf[NLINE];			/* Input buffer.	*/
KLINE	klist[NKBDS];			/* Keyboard list.	*/
int	nkbds;				/* Number of keyboards.	*/
int	initflag;			/* Curses initialized.	*/
int	uflag;				/* Invoked from update.	*/
int	default_entry = -1;		/* Which entry to default to */

main(argc, argv) int argc; char *argv[];
{
	register int n, c;
	char *s;

	/* Process command line options. */
	while (argc > 1 && argv[1][0] == '-') {
		for (s = &argv[1][1]; *s != '\0'; s++) {
			switch (*s) {
			case 'b':	++bflag;			break;
			case 'u':	++uflag;			break;
			case 'V':	nonfatal("V%s", VERSION);	break;
			default:
				fprintf(stderr, USAGE);
				exit(1);
			}
		}
		--argc;
		++argv;
	}

	if (bflag && uflag) {
		fprintf(stderr, USAGE);
		exit(1);
	}

	/* Read the keyboard list file. */
	read_klist();

	/* Initialize screen and terminal modes. */
	initscr();
	initflag = 1;
	noecho();
	raw();
	clear();

	/* Display instructions at top of screen. */
	mvaddstr(0, 0, "Select the entry below which indentifies your keyboard type.");
	mvaddstr(1, 0, "Hit <Enter> to select the highlighted entry.");
	mvaddstr(2, 0, "Hit <space> or use arrow keys to move to the next entry.");

	/* Display choices. */
	for (n = 0; n < nkbds; n++)
		display_line(n);

	/* Interactive input loop: display choice n highlighted. */
	for (n = (default_entry >= 0) ? default_entry : 0; ; ) {
		display_rev(n);		/* display default choice in reverse */
		refresh();
		switch (c = getch()) {
		case 'A':		/* n.b.:  ESC [ A is Up Arrow key */
			if (--n < 0)
				n = nkbds - 1;
			continue;
		case ' ':		/* space/Down Arrow: try next choice */
		case 'B':		/* n.b.:  ESC [ B is Down Arrow key */
			if (++n >= nkbds)
				n = 0;
			continue;
		case '\n':
		case '\r':		/* enter: take default value */
			break;
		default:
			continue;	/* ignore */
		}
		break;			/* done, choice is in n */
	}
	endwin();

	/*
	 * Execute the keyboard table file only if invoked by the user
	 * (i.e., not Build nor Update).
	 */
	if (!uflag && !bflag) {
		if (klist[n].k_loadable) {
			sprintf(buf, "%s/%s", KBDDIR, klist[n].k_table);
			if (system(buf) != 0)
			    fatal("Loadable keyboard tables not supported by this kernel", buf);
		} else {
			fatal("Selected entry is not a loadable keyboard table");
		}
		exit(0);
	}

	/*
	 * Build or Update:
	 *	Store keyboard driver name in file specified by KBDFILE.
	 *	If loadable keyboard table selected, store name of keyboard
	 *	table into /tmp/drvld.all if invoked from build,
	 *	or to /etc/drvld.all if an update.
	 */
	if (klist[n].k_loadable) {
		sprintf(buf,
		"/bin/echo %s >%s ; /bin/echo %s/%s >>%s/drvld.all",
			klist[n].k_driver, KBDFILE, 
			KBDDIR, klist[n].k_table,
			(bflag) ? "/tmp" : "/etc");
	} else {
		sprintf(buf, "/bin/echo %s >%s",
			klist[n].k_driver, KBDFILE);
	}
	if (system(buf) != 0)
		fatal("command \"%s\" failed", buf);

	printf(	"Your keyboard selection will not take effect until after\n"
		"your system %s has completed and the system has rebooted!\n",
		bflag ? "installation" : "update");
	/* Done. */
	exit(0);
}

/*
 * Allocate a copy of a string and return a pointer to it.
 */
char *
copystr(s) register char *s;
{
	register char *cp;

	if ((cp = malloc(strlen(s) + 1)) == NULL)
		fatal("no space for string \"%s\"");
	strcpy(cp, s);
	return cp;
}

/*
 * Display choice n.
 */
void
display_line(n) register int n;
{
	mvaddstr(KBDY+n, 12, klist[n].k_desc);
}

/*
 * Display choice n in reverse video.
 */
void
display_rev(n) int n;
{
	static int last = -1;

	if (last != -1)
		display_line(last);	/* redisplay last in normal */
	standout();
	display_line(n);		/* display n in reverse */
	standend();
	last = n;
}

/*
 * Cry and die.
 */
/* VARARGS */
void
fatal(s) char *s;
{
	if (initflag)
		endwin();
	fprintf(stderr, "kbdinstall: %r\n", &s);
	exit(1);
}

/*
 * Issue nonfatal informative message.
 */
/* VARARGS */
void
nonfatal(s) char *s;
{
	fprintf(stderr, "kbdinstall: %r\n", &s);
}

/*
 * Read a file containing keyboard file names and descriptions,
 * build a keyboard list.
 *
 * Format of input is:
 *	# Comment line
 * or
 *	Default <tab> Loadable <tab> Driver <tab> Table <tab> Descr.
 */
void
read_klist()
{
	register FILE *fp;
	register unsigned char *s, *s1;

	if ((fp = fopen(KBDLIST, "r")) == NULL)
		fatal("cannot open \"%s\"", KBDLIST);
	while (fgets(buf, sizeof buf, fp) != NULL) {
		if (buf[0] == '#')
			continue;		/* ignore comments */
		if (nkbds == NKBDS) {
			nonfatal("more than %d keyboard entries", NKBDS-1);
			continue;
		}

		/*
		 * Parse input lines and emit error messages if input bad.
		 */
		if (buf[0] == 'Y' || buf[0] == 'y')
			default_entry = nkbds;
		if ((s = strchr(buf, '\t')) == NULL) {
			format_error(buf);
			continue;
		}

		switch (*++s) {
		case 'Y': case 'y':
			klist[nkbds].k_loadable = TRUE;
			break;
		case 'N': case 'n':
			if (!bflag && !uflag)
				continue;	/* ignore if interactive */
			break;
		default:
			format_error(buf);
			continue;
		}
		if ((s = strchr(++s, '\t')) == NULL) {
			format_error(buf);
			continue;
		}
		if ((s1 = strchr(++s, '\t')) == NULL) {
			format_error(buf);
			continue;
		}
		*s1++ = '\0';
		klist[nkbds].k_driver = copystr(s);	/* keyboard driver */
		if ((s = strchr(s1, '\t')) == NULL) {
			format_error(s1);
			continue;
		}
		*s++ = '\0';
		klist[nkbds].k_table = copystr(s1);	/* keyboard table */
		if ((s1 = strchr(s, '\n')) == NULL) {
			nonfatal("no newline in \"%s\"", s);
			continue;
		}
		*s1 = '\0';
		klist[nkbds].k_desc = copystr(s);	/* kbd description */

		nkbds++;
	}
	fclose(fp);
}

/*
 * Input error found: display an error message and continue.
 */
void
format_error(line)
char *line;
{
	nonfatal("Input format error found in \"%s\" -- ignored.", line);
}

/* end of kbdinstall.c */
