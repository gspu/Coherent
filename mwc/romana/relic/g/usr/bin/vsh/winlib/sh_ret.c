/*
 *	Return from an external command to curses
 *
 *      Copyright (c) 1990-93 by Udo Munk
 */

#ifdef AIX
#define NLS
#endif

#include <curses.h>

shell_return()
{
	fixterm();
	doupdate();
	cursoroff();
}
