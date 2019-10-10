/*
 *	Output list box and edit the list
 *
 *      Copyright (c) 1993 by Udo Munk
 */

#ifdef AIX
#define NLS
#endif

#include <curses.h>
#include "winfun.h"
 
extern WINDOW *open_window();
extern SCROLLBAR *create_scrollbar();
extern int edit_field();
extern char *malloc();

static int cur_pos;	/* current position in list */

list_box(box, list, entrys)
LISTBOX *box;
char *list[];
int entrys;
{
	register int i, j;
	int bh, bw;
	int eoj = 1;

	bh = box->l_rows + 6;
	bw = (strlen(box->l_text) > box->l_cols) ? strlen(box->l_text) + 6 : box->l_cols + 6;

	if ((box->wb = open_window(bh, bw, (LINES-bh)/2, (COLS-bw)/2)) == (WINDOW *) 0)
		nomem();

#ifndef COHERENT
	/* why does COHERENT's subwin return WIN_ERR ??? */
	if ((box->wl = subwin(box->wb, box->l_rows, box->l_cols, (LINES-bh)/2+4, (COLS-bw)/2+3)) == (WINDOW *) 0)
#else
	if ((box->wl = open_window(box->l_rows, box->l_cols, (LINES-bh)/2+4, (COLS-bw)/2+3)) == (WINDOW *) 0)
#endif
		nomem();

	wattron(box->wb, A_REVERSE);
	for (i = 0; i < bw; i++)
		waddch(box->wb, ' ');

	waddch(box->wb, ' ');
	waddstr(box->wb, box->l_text);
	for (i = 0; i < (bw - strlen(box->l_text) - 1); i++)
		waddch(box->wb, ' ');

	for (i = 0; i < bw; i++)
		waddch(box->wb, ' ');

	waddch(box->wb, ' ');
	wattroff(box->wb, A_REVERSE);
	waddch(box->wb, ' ');
	waddch(box->wb, ul_corner);
	for (i = 0; i < box->l_cols; i++)
		waddch(box->wb, h_line);
	waddch(box->wb, ur_corner);
	waddch(box->wb, ' ');
	waddch(box->wb, ' ' | A_REVERSE);

	for (i = 0; i < box->l_rows; i++) {
		waddch(box->wb, ' ' | A_REVERSE);
		waddch(box->wb, ' ');
		waddch(box->wb, v_line);
		for (j = 0; j < box->l_cols; j++)
			waddch(box->wb, ' ');
		waddch(box->wb, v_line);
		waddch(box->wb, ' ');
		waddch(box->wb, ' ' | A_REVERSE);
	}

	waddch(box->wb, ' ' | A_REVERSE);
	waddch(box->wb, ' ');
	waddch(box->wb, ll_corner);
	for (i = 0; i < box->l_cols; i++)
		waddch(box->wb, h_line);
	waddch(box->wb, lr_corner);
	waddch(box->wb, ' ');
	waddch(box->wb, ' ' | A_REVERSE);

	for (i = 0; i < bw; i++)
		waddch(box->wb, ' ' | A_REVERSE);

	wrefresh(box->wb);

	if (box->l_vbar)
		box->bar = create_scrollbar((LINES-bh)/2+4, (COLS-bw)/2+bw-4, box->l_rows);

	cur_pos = 0;
	write_list(box, list, entrys, 0);

	while (eoj) {
		switch (wgetch(box->wb)) {
		case KEY_DOWN:
		case 'j':
			cursor_down(box, list, entrys);
			break;
		case KEY_UP:
		case 'k':
			cursor_up(box, list, entrys);
			break;
		case KEY_NPAGE:
		case W_KEY_NPAGE:
			page_down(box, list, entrys);
			break;
		case KEY_PPAGE:
		case W_KEY_PPAGE:
			page_up(box, list, entrys);
			break;
		case KEY_HOME:
		case W_KEY_HOME:
			if (cur_pos > 0) {
				cur_pos = 0;
				write_list(box, list, entrys, 0);
				slider(box, list, entrys);
			}
			break;
		case KEY_LL:
		case W_KEY_LL:
			for (i = 0; i < entrys; i++) {
				if (list[i] == NULL) {
					i--;
					break;
				}
			}
			if (i != cur_pos) {
				cur_pos = i;
				write_list(box, list, entrys, cur_pos - cur_pos % box->l_rows);
				slider(box, list, entrys);
			}
			break;
		case '\r':
		case KEY_ENTER:
			edit_line(box, list);
			break;
		case W_KEY_REFR:
#ifdef KEY_REFR
		case KEY_REFR:
#endif
			wrefresh(curscr);
			break;
		case KEY_IC:
		case W_KEY_IC:
			insert_line(box, list, entrys);
			break;
		case KEY_DC:
		case W_KEY_DC:
			delete_line(box, list, entrys);
			break;
		case 033:
			eoj = 0;
			break;
		}
	}

	if (box->l_vbar)
		remove_scrollbar(box->bar);
#ifndef COHERENT
	delwin(box->wl);
#else
	close_window(box->wl);
#endif
	close_window(box->wb);
}

/*
 *	Write the list from position pos into the list window
 */
static write_list(box, list, entrys, pos)
LISTBOX *box;
char *list[];
int entrys, pos;
{
	register int i, j;
	int k;

	werase(box->wl);
	for (i = pos, j = 0; j < box->l_rows; i++, j++) {
		if ((list[i] == NULL) || (i == entrys - 1))
			break;
		if (i == cur_pos)
			wattron(box->wl, A_REVERSE);
		mvwaddstr(box->wl, j, 0, list[i]);
		if (i == cur_pos)
			for (k = 0; k < box->l_cols - strlen(list[i]) - 1; k++)
				waddch(box->wl, ' ');
		if (i == cur_pos)
			wattroff(box->wl, A_REVERSE);
	}
	refreshall();
}

/*
 *	Move cursor one position down in list
 */
static cursor_down(box, list, entrys)
LISTBOX *box;
char *list[];
int entrys;
{
	register int i;

	if (cur_pos == entrys - 1 || list[cur_pos + 1] == NULL)
		return;
	wmove(box->wl, cur_pos % box->l_rows, 0);
	wclrtoeol(box->wl);
	waddstr(box->wl, list[cur_pos]);
	cur_pos++;
	if (cur_pos % box->l_rows == 0) {
		write_list(box, list, entrys, cur_pos);
	} else {
		wmove(box->wl, cur_pos % box->l_rows, 0);
		wattron(box->wl, A_REVERSE);
		waddstr(box->wl, list[cur_pos]);
		for (i = 0; i < box->l_cols - strlen(list[cur_pos]) - 1; i++)
			waddch(box->wl, ' ');
		wattroff(box->wl, A_REVERSE);
		wrefresh(box->wl);
	}
	slider(box, list, entrys);
}

/*
 *	Move cursor one position up in list
 */
static cursor_up(box, list, entrys)
LISTBOX *box;
char *list[];
int entrys;
{
	register int i;

	if (cur_pos == 0)
		return;
	wmove(box->wl, cur_pos % box->l_rows, 0);
	wclrtoeol(box->wl);
	waddstr(box->wl, list[cur_pos]);
	cur_pos--;
	if (((cur_pos + 1) % box->l_rows == 0) && cur_pos > 0) {
		write_list(box, list, entrys, cur_pos - box->l_rows + 1);
	} else {
		wmove(box->wl, cur_pos % box->l_rows, 0);
		wattron(box->wl, A_REVERSE);
		waddstr(box->wl, list[cur_pos]);
		for (i = 0; i < box->l_cols - strlen(list[cur_pos]) - 1; i++)
			waddch(box->wl, ' ');
		wattroff(box->wl, A_REVERSE);
		wrefresh(box->wl);
	}
	slider(box, list, entrys);
}

/*
 *	Move cursor to next page in list
 */
static page_down(box, list, entrys)
LISTBOX *box;
char *list[];
int entrys;
{
	register int i;

	if ((cur_pos < entrys - box->l_rows) && (list[cur_pos + box->l_rows] != NULL))
		cur_pos += box->l_rows;
	else {
		for (i = 0; i < entrys; i++) {
			if (list[i] == NULL) {
				i--;
				break;
			}
		}
		cur_pos = i;
	}
	write_list(box, list, entrys, cur_pos - cur_pos % box->l_rows);
	slider(box, list, entrys);
}

/*
 *	Move cursor to previous page in list
 */
static page_up(box, list, entrys)
LISTBOX *box;
char *list[];
int entrys;
{
	if (cur_pos > box->l_rows)
		cur_pos -= box->l_rows;
	else
		cur_pos = 0;
	write_list(box, list, entrys, cur_pos - cur_pos % box->l_rows);
	slider(box, list, entrys);
}

/*
 *	Edit the current line of the list
 */
static edit_line(box, list)
LISTBOX *box;
char *list[];
{
	register int i;
	char *buf = malloc(box->l_cols);

	strcpy(buf, list[cur_pos]);
	if (edit_field(box->wl, cur_pos % box->l_rows, 0, buf, box->l_cols - 1, 1, NULL) ==  WIN_OK) {
		free(list[cur_pos]);
		list[cur_pos] = malloc(strlen(buf) + 1);
		strcpy(list[cur_pos], buf);
	}
	wmove(box->wl, cur_pos % box->l_rows, 0);
	wclrtoeol(box->wl);
	wattron(box->wl, A_REVERSE);
	waddstr(box->wl, list[cur_pos]);
	for (i = 0; i < box->l_cols - strlen(list[cur_pos]) - 1; i++)
		waddch(box->wl, ' ');
	wattroff(box->wl, A_REVERSE);
	wrefresh(box->wl);
}

/*
 *	Insert a new empty line after the current one
 */
static insert_line(box, list, entrys)
LISTBOX *box;
char *list[];
int entrys;
{
	register int i;

	if (list[entrys - 1] != NULL) {
		beep();
		return;
	}

	for (i = entrys - 1; i > cur_pos; i--)
		if (list[i - 1] != NULL)
			list[i] = list[i - 1];
	list[cur_pos + 1] = malloc(1);
	*list[cur_pos + 1] = '\0';
	write_list(box, list, entrys, cur_pos - cur_pos % box->l_rows);
	slider(box, list, entrys);
}

/*
 *	Delete the current line from the list
 */
static delete_line(box, list, entrys)
LISTBOX *box;
char *list[];
int entrys;
{
	register int i;

	free(list[cur_pos]);
	if (list[cur_pos + 1] == NULL) {
		list[cur_pos] = NULL;
		cur_pos--;
	} else {
		for (i = cur_pos; i < entrys - 1; i++)
			list[i] = list[i + 1];
		list[entrys - 1] = NULL;
	}
	write_list(box, list, entrys, cur_pos - cur_pos % box->l_rows);
	slider(box, list, entrys);
}

/*
 *	Move the slider for the list to the right position
 */
static slider(box, list, entrys)
LISTBOX *box;
char *list[];
int entrys;
{
	register int i;

	if (box->l_vbar) {
		for (i = 0; i < entrys; i++)
			if (list[i] == NULL)
				break;
		update_slider(box->bar, i, cur_pos);
	}
}
