/*
 *	Initialize the windows system
 *
 *      Copyright (c) 1990-93 by Udo Munk
 */

#ifdef AIX
#define NLS
#endif

#include <curses.h>
#include <term.h>
#include <setjmp.h>
#include <signal.h>
#ifdef SCO32
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

extern char *malloc();
extern void free();

chtype ul_corner, ur_corner, ll_corner, lr_corner, r_tee, l_tee, b_tee, t_tee,
       h_line, v_line, u_arrow, d_arrow, checkboard, fullboard;

W_LIST *wl = (W_LIST *) 0;

int tab_active = 0;

initwin(tflag, eflag, lin, col, errmsg)
int tflag;
int lin, col;
char *errmsg;
{
	register char *p;

#if defined(SIGWINCH) && defined(TIOCGWINSZ)
	int sigwinch_handler();
	signal(SIGWINCH, sigwinch_handler);
#endif
	initscr();
	keypad(stdscr, TRUE);
	raw();
	noecho();
	nonl();
#ifdef INTRFLUSH
	intrflush(stdscr, FALSE);
#endif
#ifdef NOTYPEAHEAD
	typeahead(-1);
#endif

	if(LINES < lin || COLS < col) {
		endwin();
		fprintf(stderr, "%s\n", errmsg);
		exit(1);
	}

	if (wl == (W_LIST *) 0) {
		if ((wl = (W_LIST *) malloc(sizeof(W_LIST))) == NULL)
			nomem();
		wl->w_w = stdscr;
		wl->w_next = (W_LIST *) 0;
	}

	if (!tflag) {
		ul_corner  = '+';
		ur_corner  = '+';
		ll_corner  = '+';
		lr_corner  = '+';
		r_tee      = '+';
		l_tee      = '+';
		b_tee      = '+';
		t_tee      = '+';
		h_line     = '-';
		v_line     = '|';
		u_arrow	   = '^';
		d_arrow	   = 'v';
		checkboard = ' ' | A_REVERSE;
		fullboard  = '#';
 	} else {
		ul_corner  = 'l' | A_ALTCHARSET;
		ur_corner  = 'k' | A_ALTCHARSET;
		ll_corner  = 'm' | A_ALTCHARSET;
		lr_corner  = 'j' | A_ALTCHARSET;
		r_tee      = 'u' | A_ALTCHARSET;
		l_tee      = 't' | A_ALTCHARSET;
		b_tee      = 'v' | A_ALTCHARSET;
		t_tee      = 'w' | A_ALTCHARSET;
		h_line     = 'q' | A_ALTCHARSET;
		v_line     = 'x' | A_ALTCHARSET;
		u_arrow	   = '^';
		d_arrow	   = 'v';
		checkboard = ' ' | A_REVERSE;
		fullboard  = '#';
	}

	if (eflag)
		return;

#ifdef AIX
	p = box_chars_1;
	ul_corner = *(p+0) & 0377 | A_ALTCHARSET;
	h_line = *(p+1) & 0377 | A_ALTCHARSET;
	ur_corner = *(p+2) & 0377 | A_ALTCHARSET;
	v_line = *(p+3) & 0377 | A_ALTCHARSET;
	lr_corner = *(p+4) & 0377 | A_ALTCHARSET;
	ll_corner = *(p+5) & 0377 | A_ALTCHARSET;
	t_tee = *(p+6) & 0377 | A_ALTCHARSET;
	r_tee = *(p+7) & 0377 | A_ALTCHARSET;
	b_tee = *(p+8) & 0377 | A_ALTCHARSET;
	l_tee = *(p+9) & 0377 | A_ALTCHARSET;
#else
#ifdef ACS_ULCORNER
	ul_corner = ACS_ULCORNER;
	ur_corner = ACS_URCORNER;
	lr_corner = ACS_LRCORNER;
	ll_corner = ACS_LLCORNER;
	t_tee = ACS_TTEE;
	b_tee = ACS_BTEE;
	l_tee = ACS_LTEE;
	r_tee = ACS_RTEE;
	h_line = ACS_HLINE;
	v_line = ACS_VLINE;
	if (ACS_UARROW)
		u_arrow = ACS_UARROW;
	if (ACS_DARROW)
		d_arrow = ACS_DARROW;
	if (ACS_CKBOARD)
		checkboard = ACS_CKBOARD;
	if (ACS_BLOCK)
		fullboard = ACS_BLOCK;
#else
#ifdef OLDACS
	p = acs_chars;
	do {
		switch (*p) {
		case 'l':       /* upper left corner */
			ul_corner = *++p & 0377 | A_ALTCHARSET;
			break;
		case 'k':       /* upper right corner */
			ur_corner = *++p & 0377 | A_ALTCHARSET;
			break;
		case 'm':       /* lower left corner */
			ll_corner = *++p & 0377 | A_ALTCHARSET;
			break;
		case 'j':       /* lower right corner */
			lr_corner = *++p & 0377 | A_ALTCHARSET;
			break;
		case 'q':       /* horizontale line */
			h_line = *++p & 0377 | A_ALTCHARSET;
			break;
		case 'x':       /* verticale line */
			v_line = *++p & 0377 | A_ALTCHARSET;
			break;
		case 't':       /* left tee */
			l_tee = *++p & 0377 | A_ALTCHARSET;
			break;
		case 'u':       /* right tee */
			r_tee = *++p & 0377 | A_ALTCHARSET;
			break;
		case 'w':       /* upper tee */
			t_tee = *++p & 0377 | A_ALTCHARSET;
			break;
		case 'v':       /* lower tee */
			b_tee = *++p & 0377 | A_ALTCHARSET;
			break;
		case '-':	/* arrow up */
			u_arrow = *++p & 0377 | A_ALTCHARSET;
			break;
		case '.':	/* arrow down */
			d_arrow = *++p & 0377 | A_ALTCHARSET;
			break;
		case 'a':	/* checkboard */
			checkboard = *++p & 0377 | A_ALCHARSET;
			break;
		case '0':	/* full board */
			fullboard = *++p & 0377 | A_ALTCHARSET;
			break;
		default:
			p++;
		}
	} while (*p && *++p);
#endif
#endif
#endif
}

#if defined(SIGWINCH) && defined(TIOCGWINSZ)
static int sigwinch_handler()
{
	extern jmp_buf reset_win;
	struct winsize ws;

	if (ioctl(fileno(stdin), TIOCGWINSZ, (char *) &ws) == 0) {
		close_all_windows();
		free(wl);
		wl = (W_LIST *) 0;
		delwin(stdscr);
		endwin();
		LINES = ws.ws_row;
		COLS = ws.ws_col;
		signal(SIGWINCH, sigwinch_handler);
		longjmp(reset_win, 0);
	}
}
#endif
