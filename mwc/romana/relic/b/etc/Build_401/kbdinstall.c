/*
 * kbdinstall.c
 * 07/26/92
 * Keyboard support installation, including virtual console questions.
 *
 * cc kbdinstall.c -lcurses [-lterm]
 * Usage: kbdinstall [ -bu ]
 * Options:
 *	-b	invoked from build
 *	-u	invoked from update install, don't patch running system
 */

#include <stdio.h>
#include <string.h>
#include <curses.h>
#include "build0.h"

extern	char	*fgets();
extern	char	*malloc();

#define	VERSION	"3.4"			/* Version number.	*/
#define	KBDDIR	"/conf/kbd"		/* Keyboard directory.	*/
#define	KBDLIST	"/conf/kbd/.list"	/* List of keyboards.	*/
#define	KBDFILE	"/tmp/kbd"		/* File of driver object name(s) */
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

/* Forward */
char	*copystr();
void	display_line();
void	display_rev();
void	fatal();
void	my_fatal();			/* handles window cleanup */
void	format_error();
void	nonfatal();
void	my_nonfatal();			/* handles window cleanup */
void	read_klist();

/* Globals. */
int	bflag;				/* Invoked from build.	*/
char	buf[NBUF];			/* Input buffer.	*/
KLINE	klist[NKBDS];			/* Keyboard list.	*/
int	nkbds;				/* Number of keyboards.	*/
int	initflag;			/* Curses initialized.	*/
int	uflag;				/* Invoked from update.	*/
int	virtual;			/* Virtual consoles wanted */
int	default_entry = -1;		/* Which entry to default to */
int	color_sessions;			/* number of color sessions */
int	mono_sessions;			/* number of mono sessions */

main(argc, argv) int argc; char *argv[];
{
	register int n, c, i;
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

	/* Initialize screen and terminal modes. */
	initscr();
	initflag = 1;
	noecho();
	raw();
	clear();

#if _I386
	if (bflag || uflag) {
		/*
		 * If 386 and invoked from "build" or "install", output some
		 * info about virtual consoles.  Prompt the user if he/she
		 * wants V.C. support and set variable "virtual" as needed.
		 */
		mvaddstr(0, 0,
"This release of COHERENT supports multiple simultaneous sessions on the");
		mvaddstr(1, 0,
"system console.  This feature, known as virtual consoles, supports both");
		mvaddstr(2, 0,
"monochrome and color video cards (text mode only) with multiple \"login\"");
		mvaddstr(3, 0,
"sessions on each, depending upon your hardware configuration.  If you have");
		mvaddstr(4, 0,
"both color and monochrome adapters on your system, you can run multiple");
		mvaddstr(5, 0,
"sessions on each of them at the same time.");
		mvaddstr(6, 0,
"Virtual console support only works with systems which have at least one");
		mvaddstr(7, 0,
"megabyte of usable system memory, and with keyboards which are 100% IBM AT");
		mvaddstr(8, 0,
"hardware compatible.  If you are using a low cost clone keyboard which is not");
		mvaddstr(9, 0,
"100% hardware compatible (i.e., does not correctly support scan code set 3");
		mvaddstr(10, 0,
"operation), you will not be able to use COHERENT's virtual console features,");
		mvaddstr(11, 0,
"nor will you be able to use COHERENT's \"loadable\" keyboard driver.");
		mvaddstr(13, 0,
"Do you wish to include support for virtual consoles? [y or n] ");
		refresh();
		echo();
		for (;;) {
			c = getch();
			switch (c) {
			case 'Y':
			case 'y':
				++virtual;
				break;
			case 'N':
			case 'n':
				break;
			default:
				continue;
			}
			noecho();
			refresh();
			mvaddstr(14, 0, "Please hit <Enter> ...");
			refresh();
			while ((c = getch()) != '\r' && c != '\n')
				;
			mvaddstr(14, 0, "One moment, please ....");
			refresh();
			break;
		}
		clear();
	}
#endif

	/* Read the keyboard list file. */
	read_klist();

	/* Display instructions at top of screen. */
	mvaddstr(0, 0,
"Select the entry below which indentifies your keyboard type.  Hit <Enter> to");
	mvaddstr(1, 0,
"select the highlighted entry.  Hit <space> to move to the next entry.");

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
	 *	table into /tmp/drvld.all (386),
	 *	or to /etc/drvld.all (286 update).
	 */

/*
 * COH 4.2 adds vt support with non-loadable tables. If such a keyboard is
 * selected by the user, then the table field will be "nl". In this instance,
 * we don't want to build a drvld entry.  bob h 7/14/93
 */
	if (klist[n].k_loadable) {
		sprintf(buf,"/bin/echo %s >%s",(virtual)?klist[n].k_driver : "nkb.a mm.a", KBDFILE);
		if(strcmp(klist[n].k_table,"nl")){
			sprintf(buf,"/bin/echo %s/%s >>%s/drvld.all",
				KBDDIR, klist[n].k_table,
#if _I386
				"/tmp");

#else
				(bflag) ? "/tmp" : "/etc");
#endif

		}
	} else {
		sprintf(buf, "/bin/echo %s mm.a >%s",
			klist[n].k_driver, KBDFILE);
	}
	sys(buf, S_FATAL);

	printf(	"Your keyboard selection will not take effect until after\n"
		"your system %s has completed and the system has rebooted!\n\n",
		bflag ? "installation" : "update");

	/*
	 * During installation, if V.C.'s are in use,
	 * don't provide the console device.
	 */
	if (bflag && !virtual) {
		sprintf(cmd, "/bin/echo \"1lPconsole\" >/tmp/ttys");
		sys(cmd, S_FATAL);
	}

again:
	mono_sessions = 0;
	if (virtual && yes_no("\nDoes this system have a monochrome (i.e., MDA) video adapter")) {
		mono_sessions = get_int(1, 4,
		"How many active virtual console sessions would you like on your\n"
		"monochrome video adapter card [1-4]?");
		for (i = 0; i < mono_sessions; ++i) {
			sprintf(cmd, "/bin/echo \"1lPmono%d\" >>%s/ttys",
#if _I386
				i, "/tmp");
#else
				i, (bflag) ? "/tmp" : "/etc");
#endif
			sys(cmd, S_FATAL);
		}
	}

	color_sessions = 0;
	if (virtual && yes_no("\nDoes this system have a color (i.e., CGA/EGA/VGA) video adapter")) {
		color_sessions = get_int(1, 4,
		"How many active virtual console sessions would you like on your\n"
		"color video adapter card [1-4]?");
		for (i = 0; i < color_sessions; ++i) {
			sprintf(cmd, "/bin/echo \"1lPcolor%d\" >>%s/ttys",
#if _I386
				i, "/tmp");
#else
				i, (bflag) ? "/tmp" : "/etc");
#endif
			sys(cmd, S_FATAL);
		}
	}

	if (virtual && (mono_sessions + color_sessions) == 0) {
		printf("\nYou must select either a monochrome or a color video adapter!\n");
		goto again;
	}

	/*
	 * After linking new kernel, will patch it for number of mono
	 * and color sessions.  Do the patch even if number of sessions
	 * for the given type is zero.
	 */
	if (virtual) {
		sprintf(cmd,
	  "echo /conf/patch /mnt/coherent VTMONO=%d:c VTVGA=%d:c >> %s\n",
		  mono_sessions, color_sessions, PATCHFILE);
		sys(cmd, S_FATAL);
	}

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
		my_fatal("no space for string \"%s\"");
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
my_fatal(s) char *s;
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
my_nonfatal(s) char *s;
{
	if (initflag)
		endwin();
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
		my_fatal("cannot open \"%s\"", KBDLIST);
	while (fgets(buf, sizeof buf, fp) != NULL) {
		if (buf[0] == '#')
			continue;		/* ignore comments */
		if (nkbds == NKBDS) {
			my_nonfatal("more than %d keyboard entries", NKBDS-1);
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
			if ((!bflag && !uflag) || virtual)
				continue;	/* ignore if interactive ... */
			break;			/* ... or if VC's desired */
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
			my_nonfatal("no newline in \"%s\"", s);
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
	my_nonfatal("Input format error found in \"%s\" -- ignored.", line);
}

/* end of kbdinstall.c */
