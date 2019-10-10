/*
 *      Use window without a box and enter a command line
 *
 *      Copyright (c) 1990-93 by Udo Munk
 */

#ifdef AIX
#define NLS
#endif

#include <curses.h>

extern WINDOW *open_window();
extern int edit_field(), close_window();

cmd_box(s, fn)
char *s;
int (*fn) ();
{
	register WINDOW *w;
	register int ret;

	if ((w = open_window(1, COLS, LINES-1, 0)) == (WINDOW *) 0)
		nomem();
	touchwin(w);
	ret = edit_field(w, 0, 0, s, COLS-1, 1, fn);
	close_window(w);
	return(ret);
}
