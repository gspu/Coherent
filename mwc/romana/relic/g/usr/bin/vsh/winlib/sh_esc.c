/*
 *	Escape from curses to execute an external command
 *
 *      Copyright (c) 1990-93 by Udo Munk
 */

#ifdef AIX
#define NLS
#endif

#include <curses.h>

shell_escape()
{
	extern unsigned alarm();

	alarm(0);
	endwin();
	clrscr();
	cursoron();
}
