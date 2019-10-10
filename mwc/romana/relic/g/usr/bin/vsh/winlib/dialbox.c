/*
 *      Create a dialog box and edit the fields
 *
 *      Copyright (c) 1991-93 Udo Munk
 */

#ifdef AIX
#define NLS
#endif

#include <curses.h>
#include <string.h>
#include "winfun.h"     

extern WINDOW *open_window();
extern int close_window();
extern int tab_active;

dialog_box(bp)
struct dial *bp;
{
	register WINDOW *w;
	register int i, j;
	int bw, bh, ret;
	int active = 0;
	int eoj = 1;

	bh = bp->d_anz * 2 + 3;		/* compute high of box */
	bw = strlen(bp->d_text) + 2;	/* compute width of box */
	for (i = 0; i < bp->d_anz; i++) {
		j = bp->d_ptr[i]->e_len + strlen(bp->d_ptr[i]->e_text) + 3;
		if (j > bw)
			bw = j;
	}

	if ((w = open_window(bh, bw, (LINES-bh)/2, (COLS-bw)/2)) == (WINDOW *) 0)
		nomem();

	wattron(w, A_REVERSE);		/* output inverted box */
	for (i = 0; i < bh; i++)
		for (j = 0; j < bw; j++)
			mvwaddch(w, i, j, ' ');
	mvwaddstr(w, 1, 1, bp->d_text);	  /* output title */
	for (i = 0; i < bp->d_anz; i++) { /* output label and field of element */
		mvwaddstr(w, i*2+3, 1, bp->d_ptr[i]->e_text);
		waddch(w, ' ');
		wattroff(w, A_REVERSE);
		for (j = 0; j < bp->d_ptr[i]->e_len; j++)
			waddch(w, ' ');
		mvwaddstr(w, i*2+3, strlen(bp->d_ptr[i]->e_text)+2, bp->d_ptr[i]->e_data);
		wattron(w, A_REVERSE);
	}
	wattroff(w, A_REVERSE);
	wrefresh(w);

	tab_active = 1;	/* activate TAB key to switch between elements */
	while (eoj) {	/* edit the elements */
		ret = edit_field(w, active*2+3, strlen(bp->d_ptr[active]->e_text)+2, bp->d_ptr[active]->e_data, bp->d_ptr[active]->e_len, 1, NULL);
		switch (ret) {
		case WIN_OK:
		case WIN_ABORT:
			eoj = 0;
			break;
		case WIN_PREVFIE:
			if (--active < 0)
				active = bp->d_anz - 1;
			break;
		case WIN_NEXTFIE:
			if (++active > (bp->d_anz - 1))
				active = 0;
			break;
		}
	}
	close_window(w);
	tab_active = 0;
	return(ret);
}
