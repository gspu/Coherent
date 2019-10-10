/*
 *      vsh: main modul
 *
 *      Copyright (c) 1990-93 by Udo Munk
 */

#ifdef AIX
#define NLS
#endif

#include <curses.h>
#include <term.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <setjmp.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <sys/utsname.h>
#if defined(SCO32)
#include <sys/termio.h>
#include "ptem.h"
#endif
#ifdef SYSV4
#include <termios.h>
#endif
#ifdef AIX
#include <sys/ioctl.h>
#endif
#include "winfun.h"
#include "vsh.h"

#define WORKDIR         "cwd="          /* lable for current directory */
#define SHELL           "shell="        /* lable for configurated shell */
#define EDITOR          "editor="       /* lable for configurated editor */
#define PRINTER         "print-spooler="/* lable for configurated print spooler */
#define FILEVIEW        "view="         /* lable for configurated file viewer */
#define MEATTR          "me-disp-attr=" /* lable for configurated attribute of main menu */
#define PDCOLOR		"pd-disp-color="/* lable for configurated color of puldown menus */
#define PDATTR          "pd-disp-attr=" /* lable for configurated attribute of pulldown menus */
#define SEATTR          "se-disp-attr=" /* lable for configurated attribute of select boxes */
#define DISPABOLD       "bold"          /* name of possible attributes */
#define DISPAUNDE       "underline"     /* ... */
#define DISPABOTH       "both"          /* ... */
#define DISPANORM       "normal"        /* ... */
#define DISPAREV        "reverse"       /* ... */
#define CMDHIST         "cmd="          /* lable for command history */
#define PFKEYS          "pfkey"         /* lable for function keys */
#define IDLE		"idle-time="	/* lable idle time fuer screen saver */
#define	REACTION	"reaction="	/* lable reaction on file actions */
#define REAAUTO		"auto"		/* name of possible reactions */
#define REAASK		"ask"		/* ... */
#define ACTIONS		"actions="	/* lable for file actions */

int tflag;                              /* flag for terminals/curses */
					/* without alternate character set */
int eflag;				/* flag for buggy altcharset */
int rflag;                              /* flag for restricted user */
int dflag;                              /* flag for don't change to stored dir */
int iflag;				/* flag for restricted installation */

char cwd[MAXTMP+1];                     /* current working directory */
char *home;                             /* HOME directory */
char *mail;                             /* MAIL directory */
char defmail[30] = "/usr/spool/mail/";	/* default MAIL directory */
char ed[MAXEXCMD+1];                    /* editor */
char sh[MAXEXCMD+1];                    /* shell */
char pr[MAXEXCMD+1];                    /* print spooler */
char vi[MAXEXCMD+1];                    /* file viewer */
char sc[3] = "3";			/* idle time for screen saver */
char *cmd[MAXCMD];                      /* command history */
char *actions[MAXACT];			/* file actions */
char tmp[MAXTMP];                       /* working buffer */
char tmp1[MAXTMP];                      /* 2. working buffer */
char tmp2[MAXTMP];			/* 3. working buffer */
struct stat file_stat;                  /* buffer for stat() */
struct stat mail_stat;                  /* same, but for MAIL file */

struct filelist files[MAXFILES+1];      /* list of files in currect directory */
int nfiles;                             /* number of entrys in files */
int afile;                              /* number of file where cursor is on */
int tagged;                             /* number of tagged files */
#ifndef POSDIR
int longnames;				/* =1, if file names longer then 14 */
#endif
chtype meattrib = A_REVERSE;		/* default attribute of main menu */
chtype pdcolor	= A_NORMAL;		/* default color of pulldown menu */
chtype pdattrib = A_BOLD|A_UNDERLINE;	/* default attribute of pulldown menu */
chtype seattrib = A_BOLD|A_UNDERLINE;	/* default attribute of select boxes */
int execact;				/* flag for execution of file actions */
int save;                               /* flag for save of environment */
int lsave;                              /* minute of last call of l0_time() */
int idle;				/* idle time for screen saver */
int cursor;				/* return value from cursoroff() */

WINDOW *fl;                             /* sub-window for file window */
int wfl, hfl;                           /* width and high of sub-windows */

WINDOW *pf;				/* sub-findow for current directory */

SCROLLBAR *scp;				/* scrollbar for file window */

int hist_index;				/* index into command history */
int dir_sp;				/* stackpointer for directory stack */
struct dstack *dir_stack[DSTKSIZE];	/* directory stack */

struct utsname sname;			/* system name */

extern struct pfkey pfkeys[];		/* function keys */

#if defined(SIGWINCH) && defined(TIOCGWINSZ)
jmp_buf reset_win;
#endif

extern char *malloc(), *calloc();
extern void free();

main(argc, argv)
int argc;
char *argv[];
{
	extern char *getcwd();
	extern char *msgs[], *options[];
	extern char *getenv(), *getlogin();
	int sighup_handler(), sigterm_handler(), sigwinch_handler();
	register char *s;
	int i;

	uname(&sname);

	check_copy();

	while (--argc > 0 && (*++argv)[0] == '-')
		for (s = argv[0]+1; *s != '\0'; s++)
			switch (*s) {
			case 't':
				tflag = 1;
				break;
			case 'r':
				rflag = 1;
				break;
			case 'd':
				dflag = 1;
				getcwd(cwd, MAXTMP-1);
				if (argc <= 1)
					break;
				if ((*++argv)[0] == '-') {
					argv--;
					break;
				}
				strcpy(cwd, *argv);
				argc--;
				break;
			case 'e':
				eflag = 1;
				break;
			case 'i':
				iflag = 1;
				break;
			default:
				fprintf(stderr, options[0], *s);
				i = 1;
				while (options[i])
					fprintf(stderr, options[i++]);

#ifdef MAXUSE
				remove_vsh();
#endif
				exit(1);
			}

	if (!isatty(fileno(stdin)) || !isatty(fileno(stdout))) {
		fprintf(stderr, "%s\n", msgs[1]);
#ifdef MAXUSE
		remove_vsh();
#endif
		exit(1);
	}

	signal(SIGHUP, sighup_handler);
	signal(SIGTERM, sigterm_handler);
	home = getenv("HOME");
	mail = getenv("MAIL");
	if (mail == NULL) {
		mail = &defmail[0];
		strcat(mail, getlogin());
	}

	alloc_flist();

#if defined(SIGWINCH) && defined(TIOCGWINSZ)
	setjmp(reset_win);
#endif
	initwin(tflag, eflag, 23, 80, msgs[2]);
#ifndef POSDIR
	if (sizeof(files[0].f_dir->d_name) > 14)
		longnames = 1;
#endif
	read_env();
	do_l0();
	if (save || iflag)
		write_ins_file();
	endwin();
	clrscr();
#ifdef MAXUSE
	remove_vsh();
#endif
	exit(0);
}

sighup_handler()
{
	if (save)
		write_ins_file();
#ifdef MAXUSE
	remove_vsh();
#endif
	exit(0);
}

sigterm_handler()
{
	if (save)
		write_ins_file();
	endwin();
	cursoron();
#ifdef MAXUSE
	remove_vsh();
#endif
	exit(0);
}

/*
 *      level 0: create the display and to the work for users input
 */
do_l0()
{
	extern char *ttyname();
	extern unsigned alarm();
	extern long time();
	extern struct tm *localtime();
	extern SCROLLBAR *create_scrollbar();
	extern int cursor_status;
	extern char *exit_box[], *restrict_box[];
	extern struct menu f_pulldown[], d_pulldown[], o_pulldown[],
	                   i_pulldown[], c_pulldown[], r_pulldown[],
	                   e_pulldown[], h_pulldown[];
	extern char *d_nf_box[], *f_nd_box[];
	extern char *f_rm_text, *f_rm_box[];
	extern char *ins_shell_box[], *ins_editor_box[], *ins_print_box[],
		    *ins_sav_box[], *ins_view_box[];
	register struct tm *t;
	long tp;
	int input;

	l0_picture();
	l0_menu();
	print_pfkeys();
	wfl = COLS*2/3-3;
	hfl = LINES-7;
	if ((fl = subwin(stdscr, hfl, wfl, 5, 2)) == (WINDOW *) 0)
		nomem();
	if ((pf = subwin(stdscr, 1, COLS-2, 3, 1)) == (WINDOW *) 0)
		nomem();
	afile = 0;
	scp = create_scrollbar(5, COLS*2/3-1, hfl);
	l0_rebuild();
	l0_time();
	cursor = cursoroff();
	while (1) {
		time(&tp);
		t = localtime(&tp);
		if (t->tm_min == lsave) {
			signal(SIGALRM, l0_time);
			alarm(60 - t->tm_sec);
		} else
			l0_time();
		move(LINES-1, 0);
		refresh();
		input = getch();
		if ((input <= 0177) && (input >= 0) && islower(input))
			input = toupper(input);
		idle = 0;
		switch (input) {
		case M_FILE:            /* file menue */
f_label:
			switch (pulldown_menu(&f_pulldown[0], 0, 0, pdattrib, pdcolor)) {
			case 1:         /* copy */
				if (!tagged) {
					if (!is_dir(afile))
						do_fcopy();
					else
						select_box(f_nd_box, 1, seattrib);
				} else
					do_tfcopy();
				break;
			case 2:         /* move */
				if (!tagged) {
					if (!is_dir(afile))
						do_fmv();
					else
						select_box(f_nd_box, 1, seattrib);
				} else
					do_tfmv();
				break;
			case 3:         /* delete */
				if (!tagged) {
					if (!is_dir(afile))
						do_frm();
					else
						select_box(f_nd_box, 1, seattrib);
				} else {
					sprintf(tmp, f_rm_text, tagged);
					f_rm_box[0] = tmp;
					switch (select_box(f_rm_box, 1, seattrib)) {
					case 1:         /* remove nothing */
						break;
					case 2:         /* remove tagged files */
						do_frmt();
						break;
					case 3:         /* interactive remove of tagged files */
						do_frmti();
						break;
					}
				}
				break;
			case 4:         /* rename */
				if (!is_dir(afile))
					do_rename();
				else
					select_box(f_nd_box, 1, seattrib);
				break;
			case 5:		/* execute */
				if (!is_dir(afile))
					do_fexec();
				else
					select_box(f_nd_box, 1, seattrib);
				break;
			case 6:         /* access */
				if (!tagged) {
					if (!is_dir(afile))
						do_access();
					else
						select_box(f_nd_box, 1, seattrib);
				} else
					do_taccess();
				break;
			case 7:		/* owner */
				if (!tagged) {
					if (!is_dir(afile))
						do_chown();
					else
						select_box(f_nd_box, 1, seattrib);
				} else
					do_tchown();
				break;
			case 8:         /* print */
				if (!tagged) {
					if (is_file(afile))
						do_print();
					else
						select_box(f_nd_box, 1, seattrib);
				} else
					do_tprint();
				break;
			case 9:         /* view */
				if (!tagged) {
					if (is_file(afile))
						do_view();
					else
						select_box(f_nd_box, 1, seattrib);
				} else
					do_tview();
				break;
			case 10:         /* edit */
				if (!tagged) {
					if (is_file(afile))
						do_edit();
					else
						select_box(f_nd_box, 1, seattrib);
				} else
					do_tedit();
				break;
			case 11:	/* edit new */
				do_nedit();
				break;
			case 12:        /* touch */
				if (!tagged) {
					if (is_file(afile))
						do_touch();
					else
						select_box(f_nd_box, 1, seattrib);
				} else
					do_ttouch();
				break;
			case 13:        /* tag all */
				do_tagall();
				break;
			case 14:        /* untag all */
				do_untagall();
				break;
			case 15:	/* select */
				do_fselect();
				break;
			case 16:        /* file type */
				if (!is_dir(afile))
					do_fshow();
				else
					select_box(f_nd_box, 1, seattrib);
				break;
			case 17:        /* file info */
				do_finfo();
				break;
			case WIN_ABORT:
				break;
			case WIN_PREVMEN:
				goto h_label;
			case WIN_NEXTMEN:
				goto d_label;
			}
			break;
		case M_DIRECTORY:       /* directory menue */
d_label:
			switch (pulldown_menu(&d_pulldown[0], 0, 9, pdattrib, pdcolor)) {
			case 1:         /* change */
				if (!rflag)
					do_cd();
				else
					select_box(restrict_box, 1, seattrib);
				break;
			case 2:         /* home */
				if (!rflag)
					cd_dir(home);
				else
					select_box(restrict_box, 1, seattrib);
				break;
			case 3:         /* users's home */
				if (!rflag)
					do_cduhome();
				else
					select_box(restrict_box, 1, seattrib);
				break;
			case 4:         /* push */
				if (!rflag)
					do_pushdir();
				else
					select_box(restrict_box, 1, seattrib);
				break;
			case 5:         /* pop & cd */
				if (!rflag)
					do_popdir();
				else
					select_box(restrict_box, 1, seattrib);
				break;
			case 6:		/* switch current and TOS */
				if (!rflag)
					do_switchdir();
				else
					select_box(restrict_box, 1, seattrib);
				break;
			case 7:		/* copy */
				if (!rflag) {
					if (is_dir(afile))
						do_dcopy();
					else
						select_box(d_nf_box, 1, seattrib);
				} else
					select_box(restrict_box, 1, seattrib);
				break;
			case 8:         /* delete */
				if (!rflag) {
					if (is_dir(afile))
						do_drm();
					else
						select_box(d_nf_box, 1, seattrib);
				} else
					select_box(restrict_box, 1, seattrib);
				break;
			case 9:         /* rename */
				if (!rflag) {
					if (is_dir(afile))
						do_rename();
					else
						select_box(d_nf_box, 1, seattrib);
				} else
					select_box(restrict_box, 1, seattrib);
				break;
			case 10:         /* create */
				if (!rflag)
					do_dcreate();
				else
					select_box(restrict_box, 1, seattrib);
				break;
			case 11:        /* access */
				if (!rflag) {
					if (is_dir(afile))
						do_access();
					else
						select_box(d_nf_box, 1, seattrib);
				} else
					select_box(restrict_box, 1, seattrib);
				break;
			case 12:	/* owner */
				if (!rflag) {
					if (is_dir(afile))
						do_chown();
					else
						select_box(d_nf_box, 1, seattrib);
				} else
					select_box(restrict_box, 1, seattrib);
				break;
			case 13:        /* read again */
				l0_rebuild();
				break;
			case 14:	/* info */
				do_finfo();
				break;
			case WIN_ABORT:
				break;
			case WIN_PREVMEN:
				goto f_label;
			case WIN_NEXTMEN:
				goto o_label;
			}
			break;
		case M_OPTIONS:         /* option menue */
o_label:
			switch (pulldown_menu(&o_pulldown[0], 0, 18, pdattrib, pdcolor)) {
			case 1:         /* shell */
				if (!rflag)
					do_shell();
				else
					select_box(restrict_box, 1, seattrib);
				break;
			case 2:         /* lock terminal */
				do_lock();
				break;
			case 3:         /* messages on/off */
				do_msg();
				break;
			case 4:		/* online manuals */
				do_man();
				break;
			case WIN_ABORT:
				break;
			case WIN_PREVMEN:
				goto d_label;
			case WIN_NEXTMEN:
				goto i_label;
			}
			break;
		case M_INSTALL:         /* install menue */
i_label:
			if (iflag) {
				select_box(restrict_box, 1, seattrib);
				break;
			}
			switch (pulldown_menu(&i_pulldown[0], 0, 27, pdattrib, pdcolor)) {
			case 1:         /* display */
				do_insdis();
				break;
			case 2:		/* screen saver */
				if (edit_box(ins_sav_box, sc, 2, 1, NULL) == WIN_OK)
					save = 1;
				break;
			case 3:         /* function keys */
				if (!rflag)
					do_inskey();
				else
					select_box(restrict_box, 1, seattrib);
				break;
			case 4:         /* shell */
				if (!rflag) {
					if (edit_box(ins_shell_box, sh, MAXEXCMD, 1, NULL) == WIN_OK)
						save = 1;
				} else
					select_box(restrict_box, 1, seattrib);
				break;
			case 5:         /* editor */
				if (!rflag) {
					if (edit_box(ins_editor_box, ed, MAXEXCMD, 1, NULL) == WIN_OK)
						save = 1;
				} else
					select_box(restrict_box, 1, seattrib);
				break;
			case 6:         /* print spooler */
				if (!rflag) {
					if (edit_box(ins_print_box, pr, MAXEXCMD, 1, NULL) == WIN_OK)
						save = 1;
				} else
					select_box(restrict_box, 1, seattrib);
				break;
			case 7:         /* file viewer */
				if (!rflag) {
					if (edit_box(ins_view_box, vi, MAXEXCMD, 1, NULL) == WIN_OK)
						save = 1;
				} else
					select_box(restrict_box, 1, seattrib);
				break;
			case 8:         /* file actions */
				if (!rflag)
					do_insact();
				else
					select_box(restrict_box, 1, seattrib);
				break;
			case WIN_ABORT:
				break;
			case WIN_PREVMEN:
				goto o_label;
			case WIN_NEXTMEN:
				goto c2_label;
			}
			break;
		case '\r':              /* change dir or execute file */
		case KEY_ENTER:
			if (is_dir(afile) && !rflag)
				cd_dir(files[afile].f_dir->d_name);
			else if (is_file(afile))
				do_exec();
			break;
		case ' ':               /* invert tagg of file */
			if (!is_dir(afile))
				do_tag();
			break;
		case M_COMMAND:         /* enter command line */
c1_label:
			if (!rflag) {
				do_cmd();
				save = 1;
			} else
				select_box(restrict_box, 1, seattrib);
			break;
c2_label:
			switch (pulldown_menu(&c_pulldown[0], 0, 36, pdattrib, pdcolor)) {
			case WIN_ABORT:
				break;
			case WIN_PREVMEN:
				goto i_label;
			case WIN_NEXTMEN:
				goto r2_label;
			default:
				goto c1_label;
			}
			break;
		case M_REFRESH:         /* refresh screen */
#ifdef KEY_REFR
		case KEY_REFR:
#endif
		case W_KEY_REFR:
r1_label:
			wrefresh(curscr);
			cursoroff();
			break;
r2_label:
			switch (pulldown_menu(&r_pulldown[0], 0, 45, pdattrib, pdcolor)) {
			case WIN_ABORT:
				break;
			case WIN_PREVMEN:
				goto c2_label;
			case WIN_NEXTMEN:
				goto e2_label;
			default:
				goto r1_label;
			}
			break;
		case KEY_DOWN:          /* select next file */
		case 'J':
			do_nline();
			update_slider(scp, nfiles, afile);
			break;
		case KEY_UP:            /* select previous file */
		case 'K':
			do_pline();
			update_slider(scp, nfiles, afile);
			break;
		case KEY_NPAGE:         /* next page */
		case W_KEY_NPAGE:
			if (afile < (nfiles - hfl))
				afile += hfl;
			else
				afile = nfiles - 1;
			l0_files(afile-afile%hfl);
			break;
		case KEY_PPAGE:         /* previous page */
		case W_KEY_PPAGE:
			if (afile >= hfl) {
				afile -= hfl;
				l0_files(afile-afile%hfl);
			} else {
				afile = 0;
				l0_files(0);
			}
			break;
		case KEY_HOME:          /* go to top of file list */
		case W_KEY_HOME:
			afile = 0;
			l0_files(0);
			break;
		case KEY_LL:            /* go to bottom of file list */
		case W_KEY_LL:
			afile = nfiles - 1;
			l0_files(afile-afile%hfl);
			break;
		case KEY_F(1):          /* function key 1 */
		case '1':
			do_fnkey(pfkeys[0].cmd);
			break;
		case KEY_F(2):          /* function key 2 */
		case '2':
			do_fnkey(pfkeys[1].cmd);
			break;
		case KEY_F(3):          /* function key 3 */
		case '3':
			do_fnkey(pfkeys[2].cmd);
			break;
		case KEY_F(4):          /* function key 4 */
		case '4':
			do_fnkey(pfkeys[3].cmd);
			break;
		case KEY_F(5):          /* function key 5 */
		case '5':
			do_fnkey(pfkeys[4].cmd);
			break;
		case KEY_F(6):          /* function key 6 */
		case '6':
			do_fnkey(pfkeys[5].cmd);
			break;
		case KEY_F(7):          /* function key 7 */
		case '7':
			do_fnkey(pfkeys[6].cmd);
			break;
		case KEY_F(8):          /* function key 8 */
		case '8':
			do_fnkey(pfkeys[7].cmd);
			break;
		case KEY_F(9):          /* function key 9 */
		case '9':
			do_fnkey(pfkeys[8].cmd);
			break;
		case KEY_LEFT:          /* not allowed */
			break;
		case KEY_RIGHT:         /* not allowed */
			break;
		case M_EXIT:            /* exit program */
e1_label:
			if (select_box(exit_box, 1, seattrib) == 1) {
				cursoron();
				return;
			}
			break;
e2_label:
			switch (pulldown_menu(&e_pulldown[0], 0, 54, pdattrib, pdcolor)) {
			case WIN_ABORT:
				break;
			case WIN_PREVMEN:
				goto r2_label;
			case WIN_NEXTMEN:
				goto h_label;
			default:
				goto e1_label;
			}
			break;
		case M_HELP:		/* help menu */
h_label:
			switch (pulldown_menu(&h_pulldown[0], 0, 63, pdattrib, pdcolor)) {
			case 1:
				do_copyright();
				break;
			case WIN_ABORT:
				break;
			case WIN_PREVMEN:
				goto e2_label;
			case WIN_NEXTMEN:
				goto f_label;
			}
			break;
		default:
			flushinp();
		}
	}
}

/*
 *      create the main display
 */
l0_picture()
{
	extern char *ttyname(), *getlogin();
	extern struct passwd *getpwuid();
	extern struct group *getgrgid();
	extern void endpwent(), endgrent();
	extern char *l0txt[], *l0mail[], *l0msg[];
	register int i;
	register char *p;
	struct passwd *pw;
	struct group *gr;
	int lc = COLS*2/3+2;    /* left columne in info box */
	int rc = COLS*2/3+11;   /* right columne in info box */

	/* create the header box */
	mvaddch(0, 0, ul_corner);
	for (i = 0; i < COLS-2; i++)
		addch(h_line);
	addch(ur_corner);
	mvaddch(1, 0, v_line);
	mvaddch(1, COLS-1, v_line);
	mvaddch(2, 0, l_tee);
	for (i = 0; i < COLS-2; i++)
		addch(h_line);
	addch(r_tee);

	/* create the dirctory box */
	mvaddch(3, 0, v_line);
	mvaddch(3, COLS-1, v_line);
	mvaddch(4, 0, l_tee);
	for (i = 0; i < COLS-2; i++)
		addch(h_line);
	addch(r_tee);

	/* create the file, info and status box */
	for (i = 5; i < LINES-2; i++) {
		mvaddch(i, 0, v_line);
		mvaddch(i, COLS*2/3, v_line);
		mvaddch(i, COLS-1, v_line);
	}
	mvaddch(LINES-2, 0, ll_corner);
	for (i = 0; i < COLS - 2; i++)
		addch(h_line);
	addch(lr_corner);
	mvaddch(4, COLS*2/3, t_tee);
	mvaddch(LINES-2, COLS*2/3, b_tee);

	/* horizontal line between info and status box */
	mvaddch(12, COLS*2/3, l_tee);
	for (i = COLS*2/3+1; i < COLS-1; i++)
		addch(h_line);
	addch(r_tee);

	/* fill the info box */
	p = l0txt[0];
	mvaddstr(5, lc, p);
	p = sname.nodename;
	mvaddstr(5, rc, p);
	p = l0txt[3];
	mvaddstr(6, lc, p);
	p = ttyname(0);
	p = strchr(++p, '/');
	mvaddstr(6, rc, ++p);
	p = l0txt[4];
	mvaddstr(7, lc, p);
	p = getlogin();
	mvaddstr(7, rc, ((p) ? p : "???"));
	p = l0txt[5];
	mvaddstr(8, lc, p);
	i = getuid();
	pw = getpwuid(i);
	endpwent();
	p = pw->pw_name;
	mvaddstr(8, rc, p);
	printw(" (%d)", i);
	p = l0txt[6];
	mvaddstr(9, lc, p);
	i = getgid();
	gr = getgrgid(i);
	endgrent();
	p = gr->gr_name;
	mvaddstr(9, rc, p);
	printw(" (%d)", i);
	p = l0txt[1];
	mvaddstr(10, lc, p);
	p = l0txt[2];
	mvaddstr(11, lc, p);

	/* fill the status box */
	i = COLS * 2 / 3 + 2;
	mvprintw(13, i, "%s%9d", l0txt[7], 0);
	mvprintw(14, i, "%s%9ld", l0txt[8], 0L);
	mvprintw(15, i, "%s%9d", l0txt[9], 0);
	mvprintw(16, i, "%s%9ld", l0txt[10], 0L);
	mvprintw(17, i, "%s%9d", l0txt[11], 0);
	mvprintw(18, i, "%s%9s", l0txt[12], l0mail[0]);
	mvprintw(19, i, "%s", mail);
	mvprintw(20, i, "%s", l0msg[0]);
}

/*
 *      create the main menu and refresh the display
 */
l0_menu()
{
	register int i;
	register char *p;
	extern char *l0menu[];

	move(1, 2);
	for (i = 0; i <= 7; i++) {
		p = l0menu[i];
		attrset(meattrib);
		addch(*p++);
		attrset(A_NORMAL);
		addstr(p);
		addch(' ');
	}
	refreshall();
}

/*
 *      update all display boxes after execution of functions
 */
l0_rebuild()
{
	register int n;

	l0_status(1);
	nfiles = get_files(&files[0], cwd, MAXFILES);
	if (afile >= nfiles)
		afile = nfiles - 1;
	n = afile - (afile % hfl);
	if (n < 0)
		n = 0;
	l0_files(n);
	update_slider(scp, nfiles, afile);
	l0_fileno();
	l0_filesize();
	tagged = 0;
	l0_tagged();
	l0_tfilesize();
	l0_mail();
	l0_status(0);
}

/*
 *      display contents of file window
 */
l0_files(offs)
int offs;
{
	extern struct tm *localtime();
	extern char *reduce_path();
	register int i, j;
	int fnlen = wfl - 36;
	static char dfm[10], ffm[10];


	sprintf(dfm, "[%%.%ds]", fnlen);
	sprintf(ffm, "%%.%ds", fnlen);
	werase(pf);
	if (strlen(cwd) <= (COLS - 4))
		mvwaddstr(pf, 0, 1, cwd);
	else
		mvwaddstr(pf, 0, 1, reduce_path(cwd, COLS-4));
	werase(fl);
	for (i = 0, j = offs; i < hfl; i++, j++) {
		if (j >= nfiles)
			break;
		if (files[j].f_mflag)
			mvwaddch(fl, i, 0, '*');
		if (j == afile)
			wattron(fl, A_REVERSE);
		if (is_dir(j)) {
			mvwprintw(fl, i, 1, dfm, files[j].f_dir->d_name);
			if (j == afile)
				wattroff(fl, A_REVERSE);
#ifdef POSDIR
			if (strlen(files[j].f_dir->d_name) > fnlen)
				waddch(fl, '>');
#else
			if (longnames && strlen(files[j].f_dir->d_name) > fnlen)
				waddch(fl, '>');
#endif
			pr_date(i, localtime(&(files[j].f_stat->st_ctime)));
			pr_mode(i, files[j].f_stat->st_mode);
		} else {
			mvwprintw(fl, i, 1, ffm, files[j].f_dir->d_name);
			if (j == afile)
				wattroff(fl, A_REVERSE);
#ifdef POSDIR
			if (strlen(files[j].f_dir->d_name) > fnlen)
				waddch(fl, '>');
#else
			if (longnames && strlen(files[j].f_dir->d_name) > fnlen)
				waddch(fl, '>');
#endif
			mvwprintw(fl, i, wfl-33, "%8ld", files[j].f_stat->st_size);
			pr_date(i, localtime(&(files[j].f_stat->st_mtime)));
			pr_mode(i, files[j].f_stat->st_mode);
		}
	}
	refreshall();
	update_slider(scp, nfiles, afile);
}

/*
 *      output time and date stamps of a file
 */
pr_date(line, t)
register int line;
register struct tm *t;
{
	mvwprintw(fl, line, wfl-24, "%2d:%02d", t->tm_hour, t->tm_min);
#ifdef USDATE
	mvwprintw(fl, line, wfl-18, "%2d-%02d-%02d", t->tm_mon+1, t->tm_mday, t->tm_year);
#else
	mvwprintw(fl, line, wfl-18, "%2d.%02d.%02d", t->tm_mday, t->tm_mon+1, t->tm_year);
#endif
}

/*
 *      output permissions of a file
 */
pr_mode(line, m)
register int line, m;
{
	mvwprintw(fl, line, wfl-9, "%c", (m & S_IRUSR) ? 'r' : '-');
	wprintw(fl, "%c", (m & S_IWUSR) ? 'w' : '-');
	if (m & S_ISUID)
		waddch(fl, 's');
	else
		wprintw(fl, "%c", (m & S_IXUSR) ? 'x' : '-');
	wprintw(fl, "%c", (m & S_IRGRP) ? 'r' : '-');
	wprintw(fl, "%c", (m & S_IWGRP) ? 'w' : '-');
	if (m & S_ISGID)
		waddch(fl, 's');
	else
		wprintw(fl, "%c", (m & S_IXGRP) ? 'x' : '-');
	wprintw(fl, "%c", (m & S_IROTH) ? 'r' : '-');
	wprintw(fl, "%c", (m & S_IWOTH) ? 'w' : '-');
	if (m & S_ISVTX)
		waddch(fl, 't');
	else
		wprintw(fl, "%c", (m & S_IXOTH) ? 'x' : '-');
}

/*
 *	update date and time, check users mailbox, save environment
 *	if neccessary, aktivate screen saver after timeout
 */
l0_time()
{
#ifdef SHAREWARE
	extern char *share_box[];
#endif
	extern unsigned alarm();
	extern long time();
	extern int cursor_status;
	register struct tm *t;
	long tp;
	int i;

	alarm(0);

	/* save environment into $HOME/.vsh if neccessary */
	if (save) {
		write_ins_file();
		save = 0;
	}

	/* is user sleeping? */
	sscanf(sc, "%d", &i);
	if (idle >= i) {
		clrscr();
		i = cursor_status;
		cursoroff();
		getch();
		flushinp();
		wrefresh(curscr);
		if (i)
			cursoron();
		else
			cursoroff();
		idle = -1;
	}
	idle++;

	/* update time and date */
	time(&tp);
	t = localtime(&tp);
	lsave = t->tm_min;
#ifdef USDATE
	mvprintw(10, COLS*2/3+11, "%d-%d-%d", t->tm_mon+1, t->tm_mday, t->tm_year);
#else
	mvprintw(10, COLS*2/3+11, "%d.%d.%d", t->tm_mday, t->tm_mon+1, t->tm_year);
#endif
	mvprintw(11, COLS*2/3+11, "%02d:%02d", t->tm_hour, t->tm_min);

	/* display MAIL status */
	l0_mail();

#ifdef SHAREWARE
	if (!(t->tm_min % 3))
		select_box(share_box, 1, seattrib);
#endif

	/* restart alarm timer */
	signal(SIGALRM, l0_time);
	alarm(60 - t->tm_sec);

	/* cursor in lower left corner if off */
	if (!cursor_status)
		move(LINES-1, 0);
	refreshall();
}

/*
 *      update number of files in directory
 */
l0_fileno()
{
	extern char *l0txt[];

	mvprintw(13, COLS*2/3+2+strlen(l0txt[7]), "%9d", nfiles);
	refreshall();
}

/*
 *      update size of files in directory
 */
l0_filesize()
{
	extern long size_files();
	extern char *l0txt[];

	mvprintw(14, COLS*2/3+2+strlen(l0txt[10]), "%9ld", size_files(&files[0], nfiles));
	refreshall();
}

/*
 *      update number of tagged files
 */
l0_tagged()
{
	extern char *l0txt[];

	mvprintw(15, COLS*2/3+2+strlen(l0txt[9]), "%9d", tagged);
	refreshall();
}

/*
 *	update size of tagged files
 */
l0_tfilesize()
{
	extern long size_tfiles();
	extern char *l0txt[];

	mvprintw(16, COLS*2/3+2+strlen(l0txt[10]), "%9ld", size_tfiles(&files[0], nfiles));
	refreshall();
}

/*
 *      update directory stack
 */
l0_dirstack(n)
int n;
{
	extern char *l0txt[];

	mvprintw(17, COLS*2/3+2+strlen(l0txt[11]), "%9d", n);
	refreshall();
}

/*
 *	update mail status
 */
l0_mail()
{
	int ms;
	extern char *l0txt[], *l0mail[];

	ms = stat(mail, &mail_stat);
	if ((mail_stat.st_size == 0L) || (ms == -1))
		mvprintw(18, COLS*2/3+2+strlen(l0txt[12]), "%9s", l0mail[0]);
	else if (mail_stat.st_mtime <= mail_stat.st_atime)
		mvprintw(18, COLS*2/3+2+strlen(l0txt[12]), "%9s", l0mail[1]);
	else {
		attron(A_BLINK);
		mvprintw(18, COLS*2/3+2+strlen(l0txt[12]), "%9s", l0mail[2]);
		attroff(A_BLINK);
	}
}

/*
 *      update program status
 */
l0_status(n)
int n;
{
	extern WINDOW *open_window();
	extern int close_window();
	extern char *sand[];
	static WINDOW *sand_win;
	register int i;

	if (n) {
		if (!sand_win) {
			sand_win = open_window(4, 7, LINES-6, COLS-8);
			wattrset(sand_win, A_REVERSE);
			for (i = 0; i <= 3; i++)
				waddstr(sand_win, sand[i]);
			refreshall();
		}
	} else {
		if (sand_win) {
			close_window(sand_win);
			sand_win = (WINDOW *) 0;
		}
	}
}

/*
 *      read environment from $HOME/.vsh
 */
read_env()
{
	extern char *getcwd();

	/* first fill up variables with defaults */
	strcpy(ed, CMD_EDITOR);
	strcpy(sh, CMD_SH);
	strcpy(pr, CMD_PRINT);
	strcpy(vi, CMD_VIEW);
	if ((actions[0] = malloc(sizeof(ACTION1))) == NULL)
		nomem();
	strcpy(actions[0], ACTION1);
	if ((actions[1] = malloc(sizeof(ACTION2))) == NULL)
		nomem();
	strcpy(actions[1], ACTION2);
	if ((actions[2] = malloc(sizeof(ACTION3))) == NULL)
		nomem();
	strcpy(actions[2], ACTION3);
	if ((actions[3] = malloc(sizeof(ACTION4))) == NULL)
		nomem();
	strcpy(actions[3], ACTION4);
	if ((actions[4] = malloc(sizeof(ACTION5))) == NULL)
		nomem();
	strcpy(actions[4], ACTION5);
	if ((actions[5] = malloc(sizeof(ACTION6))) == NULL)
		nomem();
	strcpy(actions[5], ACTION6);
	if ((actions[6] = malloc(sizeof(ACTION7))) == NULL)
		nomem();
	strcpy(actions[6], ACTION7);
	if ((actions[7] = malloc(sizeof(ACTION8))) == NULL)
		nomem();
	strcpy(actions[7], ACTION8);
	if ((actions[8] = malloc(sizeof(ACTION9))) == NULL)
		nomem();
	strcpy(actions[8], ACTION9);
	if ((actions[9] = malloc(sizeof(ACTION10))) == NULL)
		nomem();
	strcpy(actions[9], ACTION10);
	if ((actions[10] = malloc(sizeof(ACTION11))) == NULL)
		nomem();
	strcpy(actions[10], ACTION11);
	if ((actions[11] = malloc(sizeof(ACTION12))) == NULL)
		nomem();
	strcpy(actions[11], ACTION12);
	if ((actions[12] = malloc(sizeof(ACTION13))) == NULL)
		nomem();
	strcpy(actions[12], ACTION13);

	/* now read environment from dot file, create a new if not available */
	if (read_ins_file() == ERR)
		write_ins_file();

	/* now change to stored directory, $HOME if none */
	if (chdir(cwd) == -1) {
		strcpy(cwd, home);
		chdir(cwd);
	}
}

/*
 *      write environment into $HOME/.vsh
 */
write_ins_file()
{
	register FILE *fp;
	register int i;
	static char buf[128], vshfile[128], vshtfile[128];

	strcpy(vshfile, home);
	strcat(vshfile, "/");
	strcpy(vshtfile, vshfile);
	strcat(vshfile, VSHFILE);
	strcat(vshtfile, VSHTFILE);
	if ((fp = fopen(vshtfile, "w")) == NULL)
		return(ERR);
	fprintf(fp, "%s%s\n", WORKDIR, cwd);
	fprintf(fp, "%s%s\n", SHELL, sh);
	fprintf(fp, "%s%s\n", EDITOR, ed);
	fprintf(fp, "%s%s\n", PRINTER, pr);
	fprintf(fp, "%s%s\n", FILEVIEW, vi);
	strcpy(buf, MEATTR);
	if (meattrib == A_BOLD)
		strcat(buf, DISPABOLD);
	else if (meattrib == A_UNDERLINE)
		strcat(buf, DISPAUNDE);
	else
		strcat(buf, DISPAREV);
	fprintf(fp, "%s\n", buf);
	fprintf(fp, "%s%s\n", PDCOLOR, ((pdcolor == A_NORMAL) ? DISPANORM : DISPAREV));
	strcpy(buf, PDATTR);
	if (pdattrib == A_BOLD)
		strcat(buf, DISPABOLD);
	else if (pdattrib == A_UNDERLINE)
		strcat(buf, DISPAUNDE);
	else if (pdattrib == A_NORMAL)
		strcat(buf, DISPANORM);
	else
		strcat(buf, DISPABOTH);
	fprintf(fp, "%s\n", buf);
	strcpy(buf, SEATTR);
	if (seattrib == A_BOLD)
		strcat(buf, DISPABOLD);
	else if (seattrib == A_UNDERLINE)
		strcat(buf, DISPAUNDE);
	else
		strcat(buf, DISPABOTH);
	fprintf(fp, "%s\n", buf);
	fprintf(fp, "%s%s\n", IDLE, sc);
	for (i = 1; i <= 9; i++) {
		if (pfkeys[i-1].cmd)
			fprintf(fp, "%s%1d=%s %s\n", PFKEYS, i, pfkeys[i-1].label, pfkeys[i-1].cmd);
	}
	fprintf(fp, "%s\n", CMDHIST);
	for (i = 0; i < MAXCMD; i++) {
		if (cmd[i] == NULL)
			break;
		fprintf(fp, "\t%s\n", cmd[i]);
	}
	fprintf(fp, "%s%s\n", REACTION, ((execact) ? REAAUTO : REAASK));
	fprintf(fp, "%s\n", ACTIONS);
	for (i = 0; i < MAXACT; i++) {
		if (actions[i] == NULL)
			break;
		fprintf(fp, "\t%s\n", actions[i]);
	}
	fclose(fp);
	mv_file(vshtfile, vshfile);
	return(OK);
}

/*
 *      read environment from $HOME/.vsh
 */
read_ins_file()
{
	register char *p, *p1;
	register FILE *fp;
	register int i;

	strcpy(tmp, home);
	strcat(tmp, "/");
	strcat(tmp, VSHFILE);
	if ((fp = fopen(tmp, "r")) == NULL)
		return(ERR);
	while (fgets(tmp, MAXTMP, fp) != NULL) {
		if (strncmp(tmp, WORKDIR, sizeof(WORKDIR)-1) == 0) {
			if (dflag)
				continue;
			p = strchr(tmp, '=') + 1;
			p1 = cwd;
			while ((*p != '\n') && (p1 - cwd <= MAXTMP))
				*p1++ = *p++;
			*p1 = '\0';
			continue;
		}
		if (strncmp(tmp, SHELL, sizeof(SHELL)-1) == 0) {
			p = strchr(tmp, '=') + 1;
			p1 = sh;
			while ((*p != '\n') && (p1 - sh <= MAXEXCMD))
				*p1++ = *p++;
			*p1 = '\0';
			continue;
		}
		if (strncmp(tmp, EDITOR, sizeof(EDITOR)-1) == 0) {
			p = strchr(tmp, '=') + 1;
			p1 = ed;
			while ((*p != '\n') && (p1 - ed <= MAXEXCMD))
				*p1++ = *p++;
			*p1 = '\0';
			continue;
		}
		if (strncmp(tmp, PRINTER, sizeof(PRINTER)-1) == 0) {
			p = strchr(tmp, '=') + 1;
			p1 = pr;
			while ((*p != '\n') && (p1 - pr <= MAXEXCMD))
				*p1++ = *p++;
			*p1 = '\0';
			continue;
		}
		if (strncmp(tmp, FILEVIEW, sizeof(FILEVIEW)-1) == 0) {
			p = strchr(tmp, '=') + 1;
			p1 = vi;
			while ((*p != '\n') && (p1 - vi <= MAXEXCMD))
				*p1++ = *p++;
			*p1 = '\0';
			continue;
		}
		if (strncmp(tmp, MEATTR, sizeof(MEATTR)-1) == 0) {
			p = strchr(tmp, '=') + 1;
			if (strncmp(p, DISPABOLD, sizeof(DISPABOLD)-1) == 0)
				meattrib = A_BOLD;
			else if (strncmp(p, DISPAUNDE, sizeof(DISPAUNDE)-1) == 0)
				meattrib = A_UNDERLINE;
			else
				meattrib = A_REVERSE;
			continue;
		}
		if (strncmp(tmp, PDCOLOR, sizeof(PDCOLOR)-1) == 0) {
			p = strchr(tmp, '=') + 1;
			if (strncmp(p, DISPANORM, sizeof(DISPANORM)-1) == 0)
				pdcolor = A_NORMAL;
			else
				pdcolor = A_REVERSE;
			continue;
		}
		if (strncmp(tmp, PDATTR, sizeof(PDATTR)-1) == 0) {
			p = strchr(tmp, '=') + 1;
			if (strncmp(p, DISPABOLD, sizeof(DISPABOLD)-1) == 0)
				pdattrib = A_BOLD;
			else if (strncmp(p, DISPAUNDE, sizeof(DISPAUNDE)-1) == 0)
				pdattrib = A_UNDERLINE;
			else if (strncmp(p, DISPANORM, sizeof(DISPANORM)-1) == 0)
				pdattrib = A_NORMAL;
			else
				pdattrib = A_UNDERLINE | A_BOLD;
			continue;
		}
		if (strncmp(tmp, SEATTR, sizeof(SEATTR)-1) == 0) {
			p = strchr(tmp, '=') + 1;
			if (strncmp(p, DISPABOLD, sizeof(DISPABOLD)-1) == 0)
				seattrib = A_BOLD;
			else if (strncmp(p, DISPAUNDE, sizeof(DISPAUNDE)-1) == 0)
				seattrib = A_UNDERLINE;
			else
				seattrib = A_UNDERLINE | A_BOLD;
			continue;
		}
		if (strncmp(tmp, IDLE, sizeof(IDLE)-1) == 0) {
			p = strchr(tmp, '=') + 1;
			p1 = sc;
			while ((*p != '\n') && (p1 - sc <= 2))
				*p1++ = *p++;
			*p1 = '\0';
			continue;
		}
		if (strncmp(tmp, CMDHIST, sizeof(CMDHIST)-1) == 0) {
			for (i = 0; i < MAXCMD; i++) {
				if (fgets(tmp, MAXTMP, fp) == NULL)
					continue;
				if (*tmp != '\t')
					continue;
				*(tmp + strlen(tmp) - 1) = '\0';
				if ((cmd[i] = malloc(strlen(tmp+1) + 1)) == NULL)
					nomem();
				strcpy(cmd[i], tmp+1);
			}
			continue;
		}
		if (strncmp(tmp, PFKEYS, sizeof(PFKEYS)-1) == 0 ) {
			p = strchr(tmp, '=') - 1;
			if (*p < '1' || *p > '9')
				continue;
			i = *p - '1';
			p += 2;
			if (strlen(p) < PFKLABLEN+2)
				continue;
			strncpy(pfkeys[i].label, p, PFKLABLEN);
			p += PFKLABLEN;
			if (*p++ != ' ')
				continue;
			*(p + strlen(p) - 1) = '\0';
			if ((pfkeys[i].cmd = malloc(strlen(p) + 1)) == NULL)
				nomem();
			strcpy(pfkeys[i].cmd, p);
			continue;
		} 
		if (strncmp(tmp, REACTION, sizeof(REACTION)-1) == 0) {
			p = strchr(tmp, '=') + 1;
			if (strncmp(p, REAAUTO, sizeof(REAAUTO)-1) == 0)
				execact = 1;
			continue;
		}
		if (strncmp(tmp, ACTIONS, sizeof(ACTIONS)-1) == 0) {
			for (i = 0; i < MAXACT; i++) {
				if (fgets(tmp, MAXTMP, fp) == NULL)
					continue;
				if (*tmp != '\t')
					continue;
				*(tmp + strlen(tmp) - 1) = '\0';
				if ((actions[i] = malloc(strlen(tmp+1) + 1)) == NULL)
					nomem();
				strcpy(actions[i], tmp+1);
			}
			continue;
		}
  	}
	fclose(fp);
	return(OK);
}
