#define TEST 0

/*
 * kbdinstall.c - 386 only!
 *
 * Revised: Thu Jul 29 05:02:49 1993 CDT
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
#include <ctype.h>
#include <curses.h>
#include <sys/kb.h>

#include "build0.h"

extern	char	*fgets();
extern	char	*malloc();

#define	VERSION	"4.2"			/* Version number.	*/
#define	KBDDIR	"/conf/kbd"		/* Keyboard directory.	*/

#if TEST
#define	KBDLIST	"kblist"		/* List of keyboards.	*/
#define TTYFILE "ttys"
#define DRVLDFILE "drvld.all"
#else
#define	KBDLIST	"/conf/kbd/kblist"	/* List of keyboards.	*/
#define TTYFILE "/tmp/ttys"
#define DRVLDFILE "/tmp/drvld.all"
#endif

#define KB_LANG_VAR	"kb_lang"
#define MONO_SESSION_VAR "MONO_COUNT"	/* Number of monochrome sessions. */
#define COLOR_SESSION_VAR "VGA_COUNT"	/* Number of color sessions. */
#define	KBDY	4			/* y-coordinate of keyboard list. */
#define	NLINE	512			/* Line length.		*/
#define	NKBDS	(24-KBDY)		/* Max number of keyboards */
#define	USAGE	"Usage: /etc/kbdinstall [-bu]\n"
#define MAX_NUM_SESSIONS	8

/* Language specifiers for nonloadable kb's. */
#define DE_LANG_STR	"DE"
#define FR_LANG_STR	"FR"
#define US_LANG_STR	"-"

/* Keyboard list file entries. */
typedef	struct	kline	{
	int	k_loadable;		/* True: loadable tables supported */
	int	k_lang;			/* Used only for nonloadable kb. */
	char	*k_driver;		/* Name of keyboard driver object */
	char	*k_table;		/* Keyboard table name	*/
	char	*k_desc;		/* Keyboard description	*/
}	KLINE;

/* Forward */
char	*copystr();
void	display_line();
void	display_rev();
int	blank_line();
void	get_sessions();
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

main(argc, argv) int argc; char *argv[];
{
	register int n, c;
	char *s;

#if TEST
printf("TEST version - recompile with TEST disabled for production.\n"
"Press enter: ");
getchar();
#endif

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

	if (bflag || uflag) {
		/*
		 * If invoked from "build" or "install", output some
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
		mvaddstr(7, 0,
"Virtual console support only works with systems which have at least one");
		mvaddstr(8, 0,
"megabyte of usable system memory. You may choose a loadable keyboard table");
		mvaddstr(9, 0,
"or a non-loadable keyboard table. Loadable keyboard tables will give you the");
		mvaddstr(10, 0,
"flexibility of remapping your keys and to load new tables at any time while");
		mvaddstr(11, 0,
"your system is active. This requires keyboards that are 100% IBM AT compliant.");
		mvaddstr(12, 0,
"If you chose a non-loadable keyboard, you will not have this flexibility.");
		mvaddstr(14,0,
"Not all keyboards are 100% IBM AT compliant and will not work with loadable");
		mvaddstr(15,0,
"keyboard tables. If your keyboard is not 100% IBM AT compliant, it may result");
		mvaddstr(16,0,
"in COHERENT not booting properly after the installation process is completed,");
		mvaddstr(17,0,
"or you may find that your keyboard keys are not mapped as they are labeled and");
		mvaddstr(18,0,
"that some keys cease to function at all. For this reason, the default selection");
		mvaddstr(19,0,
"is the non-loadable keyboard.");
		mvaddstr(21, 0,
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
			mvaddstr(23, 0, "Please hit <Enter> ...");
			refresh();
			while ((c = getch()) != '\r' && c != '\n')
				;
			mvaddstr(23, 0, "One moment, please ....");
			refresh();
			break;
		}
		clear();
	}

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

	/*
	 * Execute the keyboard table file only if invoked by the user
	 * (i.e., not Build nor Update).
	 */
	if (!uflag && !bflag) {
#if! TEST
/* Omit this during testing! */
		if (klist[n].k_loadable) {
			sprintf(buf, "%s/%s", KBDDIR, klist[n].k_table);
			if (system(buf) != 0)
			    fatal("Loadable keyboard tables not supported by this kernel", buf);
		} else {
			fatal("Selected entry is not a loadable keyboard table");
		}
#endif
		exit(0);
	}

	/*
	 * Must be in Build or Update mode now.
	 */

	/* Write idenable line to IDCMDFILE. */
	buf[0] = '\0';
	if (virtual)
		strcpy(buf, "vt");
	strcat(buf, klist[n].k_driver);
	idenable_dev(buf);

	if (klist[n].k_loadable) {

		/*
		 * Loadable keyboard table selected.
		 * Store name of keyboard executable into
		 * drvld.all.
		 */
		sprintf(buf,"/bin/echo %s/%s >> %s",
		  KBDDIR, klist[n].k_table, DRVLDFILE);
		sys(buf,S_FATAL);

	} else {

		char * kb_lang_val;
		char line[80];

		/*
		 * Non-loadable keyboard table selected.
		 * If Language is German (DE) or French (FR),
		 * write cohtune line.
		 */
		switch(klist[n].k_lang) {
		case kb_lang_us:
			kb_lang_val = "kb_lang_us";
			break;
		case kb_lang_de:
			kb_lang_val = "kb_lang_de";
			break;
		case kb_lang_fr:
			kb_lang_val = "kb_lang_fr";
			break;
		}
		sprintf(line, "int %s = %s;", KB_LANG_VAR, kb_lang_val);
		cohtune_ent("console", KB_LANG_VAR, line);
	}


	clear();
	mvaddstr(0,0,"Your keyboard selection will not take effect until after");
	move(1,0);
	printw("your system %s has completed and the system has rebooted!\n",
		bflag ? "installation" : "update");


	mvaddstr(3,0,"Press <Enter> to continue...");
	refresh();

	while ((c = getch()) != '\r' && c != '\n')
		;

#if 0
	clear();
	refresh();
#endif

	/* Back to non-curses output. */
	endwin();

	/*
	 * During installation, if V.C.'s are in use,
	 * don't enable the console device.
	 */
	if (bflag) {
		sprintf(cmd, "/bin/echo \"%dlPconsole\" > %s",
		  virtual ? 0 : 1, TTYFILE);
		sys(cmd, S_FATAL);
	}

	/* For virtual consoles, ask how many sessions. */
	if (virtual) {
		clear();
		get_sessions();
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
 *      blank line
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
		if (buf[0] == '#' || blank_line(buf))
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
			klist[nkbds].k_loadable = FALSE;
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

		/* k_table must contain DE/FR/- for non-loadable driver. */
		if (!klist[nkbds].k_loadable) {
			if (strcmp(klist[nkbds].k_table, DE_LANG_STR) == 0)
				klist[nkbds].k_lang = kb_lang_de;
			else if (strcmp(klist[nkbds].k_table, FR_LANG_STR) == 0)
				klist[nkbds].k_lang = kb_lang_fr;
			else if (strcmp(klist[nkbds].k_table, US_LANG_STR) == 0)
				klist[nkbds].k_lang = kb_lang_us;
			else {
				format_error(s1);
				continue;
			}
		}

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

/*
 * Return TRUE if line consists only of whitespace.
 */
int
blank_line(str)
char * str;
{
	int is_blank = TRUE;

	while(*str) {
		if (isspace(*str))
			str++;
		else {
			is_blank = FALSE;
			break;
		}
	}

	return is_blank;
}

/*
 * For virtual console users only, ask how may mono/color sessions.
 */
void
get_sessions()
{
	int	color_sessions;		/* number of color sessions */
	int	mono_sessions;		/* number of mono sessions */
	int	mono_display;		/* 1 if mono display is present */
	int	color_display;		/* 1 if color display is present */
	int	high_prompt;
	int	i;

	for (;;) {
		mono_sessions = 0;
		color_sessions = 0;
		mono_display =
			yes_no("\nDoes this system have a monochrome "
			  "(i.e., MDA) video adapter");
		color_display =
			yes_no("\nDoes this system have a color "
			  "(i.e., CGA/EGA/VGA) video adapter");
		printf("\nYou may select a total of up to %d virtual "
		  "console sessions.\n", MAX_NUM_SESSIONS);

		high_prompt = MAX_NUM_SESSIONS;

		if (color_display) {
			sprintf(buf, "\nHow many active virtual console sessions "
			  "would you like on your\ncolor video display "
			  "[1-%d]?", high_prompt);
			color_sessions = get_int(1, high_prompt, buf);
			high_prompt = MAX_NUM_SESSIONS - color_sessions;
		}

		if (mono_display) {
			sprintf(buf, "\nHow many active virtual console sessions "
			  "would you like on your\nmonchrome video display "
			  "[1-%d]?", high_prompt);
			mono_sessions = get_int(1, high_prompt, buf);
		}

		if (mono_sessions + color_sessions > 0)
			break;

		/* else - try again - no sessions enabled! */
		printf("\nYou must select either a monochrome or a color video "
		  "adapter!\n");
	}

	/* Write device entries to ttys file. */
	for (i = 0; i < color_sessions; ++i) {
		sprintf(cmd, "/bin/echo \"1lPcolor%d\" >> %s",
			i, TTYFILE);
		sys(cmd, S_FATAL);
	}

	for (i = 0; i < mono_sessions; ++i) {
		sprintf(cmd, "/bin/echo \"1lPmono%d\" >> %s",
			i, TTYFILE);
		sys(cmd, S_FATAL);
	}

	/*
	 * After linking new kernel, will patch it for number of mono
	 * and color sessions.  Do the patch even if number of sessions
	 * for the given type is zero.
	 */
	idtune_var(MONO_SESSION_VAR, mono_sessions);
	idtune_var(COLOR_SESSION_VAR, color_sessions);
}

/* end of kbdinstall.c */
