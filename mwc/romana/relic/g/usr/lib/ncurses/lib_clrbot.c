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
**	lib_clrbot.c
**
**	The routine wclrtobot().
**
** $Log:	lib_clrbot.c,v $
 * Revision 1.8  93/04/12  14:13:25  bin
 * Udo: third color update
 * 
 * Revision 1.2  92/04/13  14:37:13  bin
 * update by vlad
 * 
 * Revision 2.2  91/04/20  18:07:38  munk
 * Usage of register variables
 *
 * Revision 2.1  82/10/25  14:46:37  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/24  15:20:24  pavel
 * Beta-one Test Release
 * 
**
*/

#ifdef RCSHDR
static char RCSid[] =
	"$Header: /src386/usr/lib/ncurses/RCS/lib_clrbot.c,v 1.8 93/04/12 14:13:25 bin Exp Locker: bin $";
#endif

#include "curses.h"
#include "curses.priv.h"


wclrtobot(win)
register WINDOW	*win;
{
	chtype		*ptr, *end, *maxx;
	register int	y;
	int		startx, minx;
	chtype		blank = ' ' | win->_attrs;

#ifdef TRACE
	if (_tracing)
	    _tracef("wclrtobot(%o) called", win);
#endif

	startx = win->_curx;

	for (y = win->_cury; y <= win->_regbottom; y++)
	{
	    minx = _NOCHANGE;
	    end = &win->_line[y][win->_maxx];

	    for (ptr = &win->_line[y][startx]; ptr <= end; ptr++)
	    {
		if (*ptr != blank)
		{
		    maxx = ptr;
		    if (minx == _NOCHANGE)
			minx = ptr - win->_line[y];
		    *ptr = blank;
		}
	    }

	    if (minx != _NOCHANGE)
	    {
		win->_numchngd[y] = (maxx - win->_line[y]) - minx;
		if (win->_firstchar[y] > minx
					||  win->_firstchar[y] == _NOCHANGE)
		    win->_firstchar[y] = minx;

		if (win->_lastchar[y] < maxx - win->_line[y])
		    win->_lastchar[y] = maxx - win->_line[y];
	    }

	    startx = 0;
	}
}
