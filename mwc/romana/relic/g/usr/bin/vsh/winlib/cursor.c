/*
 *	Switch cursor on/off using terminfo
 *
 *      Copyright (c) 1991-93 by Udo Munk
 */

#ifdef AIX
#define NLS
#endif

#include <curses.h>
#include <term.h>
#include "winfun.h"

int cursor_status = 1;

cursoroff()
{
	cursor_status = 0;
	if ((cursor_invisible != NULL) && (*cursor_invisible != '\0')) {
		putp(cursor_invisible);
		fflush(stdout);
		return(WIN_OK);
	} else
		return(WIN_ERR);
}

cursoron()
{
	cursor_status = 1;
	if ((cursor_invisible != NULL) && (*cursor_invisible != '\0')) {
		putp(cursor_visible);
		fflush(stdout);
		return(WIN_OK);
	} else
		return(WIN_ERR);
}
