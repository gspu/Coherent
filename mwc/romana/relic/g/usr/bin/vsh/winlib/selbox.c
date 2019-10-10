/*
 *      Output a select box and select one option
 *
 *      Copyright (c) 1990-93 by Udo Munk
 */

#ifdef AIX
#define NLS
#endif

#include <curses.h>
#include <string.h>
#include "winfun.h"

extern WINDOW *open_window();
extern int close_window();

select_box(b, opt, attr)
char *b[];
int opt;
chtype attr;
{
	register int i, j;
	int bw, bh, nt, ni, ai, itoff, ilen, eoj;
	WINDOW *w;

	i = bw = bh = 0;
	ai = opt;
	while (b[i] != NULL)
		if ((j = strlen(b[i++]) + 2) > bw)
			bw = j; /* width of the box */
	bh = i + 4;             /* high of the box */
	nt = i;                 /* number of options */
	itoff = ++i;
	ilen = 0;
	while (b[i] != NULL) {
		ilen += strlen(b[i]) + 3;
		i++;
	}
	ilen--;
	ni = i - itoff;
	if (ilen > (bw - 2))
		bw = ilen + 2;
	if ((w = open_window(bh, bw, (LINES-bh)/2, (COLS-bw)/2)) == (WINDOW *) 0)
		nomem();
	cursoroff();
	leaveok(w, TRUE);
	wattron(w, A_REVERSE);
	for (i = 0; i < bh; i++)/* output the inverted box */
		for (j = 0; j < bw; j++)
			mvwaddch(w, i, j, ' ');
	for (i = 0; i < nt; i++)
		mvwaddstr(w, i+1, 1, b[i]);
	wattroff(w, A_REVERSE);
	dialog_options(w, nt+2, (bw-ilen)/2, b, itoff, ai, attr);
	eoj = 1;
	while (eoj) {
		switch (wgetch(w)) {
		case ' ':
		case W_KEY_TAB:
		case KEY_RIGHT:
			ai++;
			if (ai > ni)
				ai = 1;
			dialog_options(w, nt+2, (bw-ilen)/2, b, itoff, ai, attr);
			break;
		case W_KEY_BTAB:
		case KEY_LEFT:
			ai--;
			if (ai < 1)
				ai = ni;
			dialog_options(w, nt+2, (bw-ilen)/2, b, itoff, ai, attr);
			break;
		case '\r':
		case KEY_ENTER:
			eoj = 0;
			break;
		case W_KEY_REFR:
#ifdef KEY_REFR
		case KEY_REFR:
#endif
			wrefresh(curscr);
			break;
		default:
			break;
		}
	}
	close_window(w);
	return(ai);
}

/*
 *      output options in to the box
 */
static dialog_options(w, y, x, s, o, a, attr)
WINDOW *w;
char *s[];
int y, x, o, a;
chtype attr;
{
	register int j;

	j = 1;
	wmove(w, y, x);
	while (s[o] != NULL) {
		waddch(w, ' ');
		if (j == a) {
			wattron(w, attr);
			waddstr(w, s[o++]);
			wattroff(w, attr);
		} else
			waddstr(w, s[o++]);
		waddch(w, ' ');
		wattron(w, A_REVERSE);
		waddch(w, ' ');
		wattroff(w, A_REVERSE);
		j++;
	}
	wrefresh(w);
}
