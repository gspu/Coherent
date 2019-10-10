/*
 *      Refresh all active windows
 *
 *      Copyright (c) 1990-93 by Udo Munk
 */

#ifdef AIX
#define NLS
#endif

#include <curses.h>
#include "winfun.h"

extern W_LIST *wl;

refreshall()
{
	register W_LIST *wlp;

	wlp = wl;
	while (wlp != (W_LIST *) 0) {
		touchwin(wlp->w_w);
		wnoutrefresh(wlp->w_w);
		wlp = wlp->w_next;
	}
	doupdate();
}
