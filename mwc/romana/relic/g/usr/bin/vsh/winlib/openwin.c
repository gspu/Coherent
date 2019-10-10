/*
 *      Open a new window
 *
 *      Copyright (c) 1990-93 by Udo Munk
 */

#ifdef AIX
#define NLS
#endif

#include <curses.h>
#include "winfun.h"

extern char	*malloc();

extern W_LIST	*wl;

WINDOW *open_window(nlines, ncols, begin_y, begin_x)
int nlines, ncols, begin_y, begin_x;
{
	register W_LIST *wlp1, *wlp2;
	WINDOW *w;

	if ((wlp1 = (W_LIST *) malloc(sizeof(W_LIST))) == NULL)
		return((WINDOW *) 0);
	w = newwin(nlines, ncols, begin_y, begin_x);
	keypad(w, TRUE);
#ifdef INTRFLUSH
	intrflush(w, FALSE);
#endif
	wlp2 = wl;
	while (wlp2->w_next != (W_LIST *) 0)
		wlp2 = wlp2->w_next;
	wlp1->w_w = w;
	wlp1->w_next = (W_LIST *) 0;
	wlp2->w_next = wlp1;
	return(w);
}
