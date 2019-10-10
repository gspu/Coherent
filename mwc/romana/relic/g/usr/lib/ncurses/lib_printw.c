/*********************************************************************
*                         COPYRIGHT NOTICE                           *
**********************************************************************
*        This software is copyright (C) 1982 by Pavel Curtis         *
*                                                                    *
*        Permission is granted to reproduce and distribute           *
*        this file by any means so long as no fee is charged         *
*        above a nominal handling fee and so long as this            *
*        notice is always included in the copies.                    *
*                                                                    *
*        Other rights are reserved except as explicitly granted      *
*        by written permission of the author.                        *
*                Pavel Curtis                                        *
*                Computer Science Dept.                              *
*                405 Upson Hall                                      *
*                Cornell University                                  *
*                Ithaca, NY 14853                                    *
*                                                                    *
*                Ph- (607) 256-4934                                  *
*                                                                    *
*                Pavel.Cornell@Udel-Relay   (ARPAnet)                *
*                decvax!cornell!pavel       (UUCPnet)                *
*********************************************************************/

/*
**	lib_printw.c
**
**	The routines printw(), wprintw() and friend.
**
** $Log:	lib_printw.c,v $
 * Revision 1.8  93/04/12  14:13:58  bin
 * Udo: third color update
 * 
 * Revision 1.2  92/04/13  14:38:17  bin
 * update by vlad
 * 
 * Revision 2.2  91/01/27  15:17:12  munk
 * Rewritten for COHERENT and portable usage of variable arguments
 *
 * Revision 2.1  82/10/25  14:48:38  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/25  13:48:22  pavel
 * Beta-one Test Release
 * 
**
*/

#ifdef RCSHDR
static char RCSid[] =
	"$Header: /src386/usr/lib/ncurses/RCS/lib_printw.c,v 1.8 93/04/12 14:13:58 bin Exp Locker: bin $";
#endif

#include "curses.h"
#include "curses.priv.h"

static char buf[512];


printw(string)
char	*string;
{
#ifdef TRACE
	if (_tracing)
	    _tracef("printw(%s,...) called", string);
#endif

	sprintf(buf, "%r", (char **) &string);
	return(waddstr(stdscr, buf));
}


wprintw(win, string)
WINDOW	*win;
char	*string;
{
#ifdef TRACE
	if (_tracing)
	    _tracef("wprintw(%o,%s,...) called", win, string);
#endif

	sprintf(buf, "%r", (char **) &string);
	return(waddstr(win, buf));
}


mvprintw(y, x, string)
int		y, x;
char		*string;
{
	if (move(y, x) != OK)
		return(ERR);
	sprintf(buf, "%r", (char **) &string);
	return(waddstr(stdscr, buf));
}


mvwprintw(win, y, x, string)
WINDOW	*win;
int	y, x;
char	*string;
{
	if (wmove(win, y, x) != OK)
		return(ERR);
	sprintf(buf, "%r", (char **) &string);
	return(waddstr(win, buf));
}
