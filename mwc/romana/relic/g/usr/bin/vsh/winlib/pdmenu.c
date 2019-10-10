/*
 *	Output an pulldown menu and select one option
 *
 *      Copyright (c) 1990-93 by Udo Munk
 */

#ifdef AIX
#define NLS
#endif

#include <curses.h>
#include <ctype.h>
#include <string.h>
#include "winfun.h"

extern WINDOW *open_window();
extern int close_window();
 
static int inp;

pulldown_menu(b, y, x, attr, color)
struct menu *b;
int x, y;
chtype attr, color;
{
	register char *p;
	register int i, j;
	int bh, bw, ni, ai, eoj, search;
	register struct menu *m;
	WINDOW *w;

	i = bh = bw = 0;
	m = b;
	while (m->m_text != NULL) {
		if (strlen(m->m_text) > bw)
			bw = strlen(m->m_text);
		i++;
		m++;
	}
	bw += 4;
	bh = i + 3;
	ni = i - 1;
	if ((w = open_window(bh, bw, y, x)) == (WINDOW *) 0)
		nomem();
	cursoroff();
	leaveok(w, TRUE);
	wattron(w, color);
	mvwaddch(w, 0, 0, ul_corner);
	for (i = 0; i < bw-2; i++)
		waddch(w, h_line);
	waddch(w, ur_corner);
	mvwaddch(w, 1, 0, v_line);
	waddch(w, ' ');
	waddstr(w, b->m_text);
	i = strlen(b->m_text);
	while (i < bw - 2) {
		waddch(w, ' ');
		i++;
	}
	mvwaddch(w, 1, bw-1, v_line);
	mvwaddch(w, 2, 0, l_tee);
	for (i = 0; i < bw-2; i++)
		waddch(w, h_line);
	waddch(w, r_tee);
	for (i = 0; i < ni; i++) {
		mvwaddch(w, i+3, 0, v_line);
		mvwaddch(w, i+3, bw-1, v_line);
	}
	mvwaddch(w, i+3, 0, ll_corner);
	for (j = 0; j < bw-2; j++)
		waddch(w, h_line);
	waddch(w, lr_corner);
	ai = 1;
	menu_options(w, bw-3, b, ni, ai, attr, color);
	eoj = 1;
	while (eoj) {
		if (inp == -1)
			inp = wgetch(w);
		switch (inp) {
		case KEY_DOWN:
			ai++;
			if (ai > ni)
				ai = 1;
			menu_options(w, bw-3, b, ni, ai, attr, color);
			break;
		case KEY_UP:
			ai--;
			if (ai < 1)
				ai = ni;
			menu_options(w, bw-3, b, ni, ai, attr, color);
			break;
		case KEY_LEFT:
			ai = WIN_PREVMEN;
			eoj = 0;
			break;
		case KEY_RIGHT:
			ai = WIN_NEXTMEN;
			eoj = 0;
			break;
		case '\r':
		case KEY_ENTER:
			eoj = 0;
			break;
		case 033:       /* ESC, abort menu */
		case ' ':
			ai = WIN_ABORT;
			eoj = 0;
			break;
		case W_KEY_REFR:
#ifdef KEY_REFR
		case KEY_REFR:
#endif
			wrefresh(curscr);
			inp = -1;
			break;
		default:
			if (!((inp >= '0' && inp <= '9') ||
			      (inp >= 'A' && inp <= 'Z') ||
			      (inp >= 'a' && inp <= 'z'))) {
				inp = -1;
				break;
			}
			m = b + 1;
			search = 0;
			j = 1;
			while (m->m_text) {
				p = m->m_text;
				if (toupper(inp&0177) == toupper(*(p + m->m_opt - 1))) {
					search = j;
					break;
				}
				m++;
				j++;
			}
			if (search) {
				ai = search;
				eoj = 0;
			} else
				inp = -1;
			break;
		}
	}
	close_window(w);
	return(ai);
}

/*
 *      output options of pulldown menu
 */
static menu_options(w, l, s, n, a, attr, color)
WINDOW *w;
struct menu *s;
int l, n, a;
chtype attr, color;
{
	register int i, j;
	register char *p;
	register chtype mark = (color == A_NORMAL) ? A_REVERSE : A_NORMAL;
	register chtype mark1 = (color == A_NORMAL) ? ((attr == A_NORMAL) ? A_REVERSE : attr) : ((attr == A_NORMAL) ? A_NORMAL : A_REVERSE | attr);

	s++;
	for (i = 1; i <= n; i++, s++) {
		wmove(w, i+2, 1);
		if (i == a)
			wattrset(w, mark);
		else
			wattrset(w, color);
		waddch(w, ' ');
		j = 1;
		p = s->m_text;
		while (*p) {
			if (j == s->m_opt) {
				if (i == a)
					wattrset(w, attr);
				else
					wattrset(w, mark1);
				waddch(w, *p++);
				if (i == a)
					wattrset(w, mark);
				else
					wattrset(w, color);
				waddstr(w, p);
				break;
			} else {
				waddch(w, *p++);
				j++;
			}
		}
		j = strlen(s->m_text);
		while (j < l) {
			waddch(w, ' ');
			j++;
		}
	}
	nodelay(w, TRUE);
	inp = wgetch(w);
	nodelay(w, FALSE);
	if (inp == -1)
		wrefresh(w);
}
