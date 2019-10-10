/*
 *	Close one window, free memory and update remaining windows
 *
 *      Copyright (c) 1990-93 by Udo Munk
 */

#ifdef AIX
#define NLS
#endif

#include <curses.h>
#include "winfun.h"

extern W_LIST *wl;
extern void free();

close_window(w)
WINDOW *w;
{
	register W_LIST *wlp = wl->w_next;
	register W_LIST *lwlp = wl;

	if ((lwlp->w_next == (W_LIST *) 0) || (lwlp->w_w == w)) /* don't close */
		return(WIN_ERR);				/* stdscr !! */
	do {
		if (wlp->w_w == w) {
			lwlp->w_next = wlp->w_next;
			delwin(wlp->w_w);
			free((char *) wlp);
			wlp = lwlp->w_next;
			touchwin(lwlp->w_w);
			wnoutrefresh(lwlp->w_w);
		} else {
			touchwin(lwlp->w_w);
			wnoutrefresh(lwlp->w_w);
			lwlp = wlp;
			wlp = wlp->w_next;
		}
	} while (wlp != (W_LIST *) 0);
	touchwin(lwlp->w_w);
	wnoutrefresh(lwlp->w_w);
	doupdate();
	return(WIN_OK);
}
