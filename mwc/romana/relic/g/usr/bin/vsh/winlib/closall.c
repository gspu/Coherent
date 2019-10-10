/*
 *      Close all windows and free memory
 *
 *      Copyright (c) 1991-93 by Udo Munk
 */

#ifdef AIX
#define NLS
#endif

#include <curses.h>
#include "winfun.h"

extern W_LIST *wl;
extern int close_window();

close_all_windows()
{
	register W_LIST *wlp = wl->w_next;

	while (wlp) {
		close_window(wlp->w_w);
		wlp = wl->w_next;
	}
}
