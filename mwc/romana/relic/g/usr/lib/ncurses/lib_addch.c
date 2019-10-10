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
**	lib_addch.c
**
**	The routine waddch().
**
** $Log:	lib_addch.c,v $
 * Revision 1.8  93/04/12  14:13:19  bin
 * Udo: third color update
 * 
 * Revision 2.3  92/11/01  15:55:32  munk
 * Second parameter is a chtype now
 *
 * Revision 1.2  92/04/13  14:36:58  bin
 * update by vlad
 * 
 * Revision 2.2  91/04/20  17:52:20  munk
 * Usage of register variables
 *
 * Revision 2.1  82/10/25  14:46:23  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/24  15:19:46  pavel
 * Beta-one Test Release
 * 
**
*/

#ifdef RCSHDR
static char RCSid[] =
	"$Header: /src386/usr/lib/ncurses/RCS/lib_addch.c,v 1.8 93/04/12 14:13:19 bin Exp Locker: bin $";
#endif

#include "curses.h"
#include "curses.priv.h"
#include "terminfo.h"
#include "unctrl.h"


waddch(win, ch)
register WINDOW *win;
chtype	ch;
{
	register int	x, y;
	int		newx;

#ifdef TRACE
	if (_tracing)
	    _tracef("waddch(%o,%o) called", win, ch);
#endif

	x = win->_curx;
	y = win->_cury;

	if (y > win->_maxy  ||  x > win->_maxx  ||  y < 0  ||  x < 0)
	    return(ERR);

	switch (ch & A_CHARTEXT)
	{
	    case '\t':
		for (newx = x + (8 - (x & 07)); x < newx; x++)
		    if (waddch(win, ' ') == ERR)
			return(ERR);
		return(OK);

	    case '\n':
		wclrtoeol(win);
		if (SP->_nl)
		    x = 0;
		goto newline;

	    case '\r':
		x = 0;
		break;

	    case '\b':
		if (--x < 0)
		    x = 0;
		break;

	    default:
		if ((ch & A_CHARTEXT) < ' ')
		    return(waddstr(win, unctrl(ch)));

		ch |= (ch & A_COLOR) ? (win->_attrs & ~A_COLOR) : win->_attrs;

/*		ch |= win->_attrs; */

		if (win->_line[y][x] != ch)
		{
		    if (win->_firstchar[y] == _NOCHANGE)
			win->_firstchar[y] = win->_lastchar[y] = x;
		    else if (x < win->_firstchar[y])
			win->_firstchar[y] = x;
		    else if (x > win->_lastchar[y])
			win->_lastchar[y] = x;

		    win->_numchngd[y]++;
		}

		win->_line[y][x++] = ch;
		if (x > win->_maxx)
		{
		    x = 0;
newline:
		    y++;
		    if (y > win->_regbottom)
		    {
			y--;
			if (win->_scroll)
			    scroll(win);
			else
			    return ERR;
		    }
		}
		break;
	}

	win->_curx = x;
	win->_cury = y;

	return(OK);
}
