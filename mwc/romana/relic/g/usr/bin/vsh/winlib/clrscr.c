/*
 *	Clear screen using terminfo
 *
 *      Copyright (c) 1990-93 by Udo Munk
 */

#ifdef AIX
#define NLS
#endif

#include <curses.h>
#include <term.h>

clrscr()
{
	putp(clear_screen);
	fflush(stdout);
}
