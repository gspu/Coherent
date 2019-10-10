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
**	lib_clreol.c
**
**	The routine wclrtoeol().
**
** $Log:	lib_clreol.c,v $
 * Revision 1.8  93/04/12  14:13:26  bin
 * Udo: third color update
 * 
 * Revision 1.2  92/04/13  14:37:16  bin
 * update by vlad
 * 
 * Revision 2.2  91/04/20  18:11:10  munk
 * Usage of register variables
 *
 * Revision 2.1  82/10/25  14:46:42  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/24  15:20:38  pavel
 * Beta-one Test Release
 * 
**
*/

#ifdef RCSHDR
static char RCSid[] =
	"$Header: /src386/usr/lib/ncurses/RCS/lib_clreol.c,v 1.8 93/04/12 14:13:26 bin Exp Locker: bin $";
#endif

#include "curses.h"
#include "curses.priv.h"


wclrtoeol(win)
register WINDOW	*win;
{
	register chtype	*ptr;
	chtype		*maxx, *end;
	int		y, x, minx;
	chtype		blank = ' ' | win->_attrs;

#ifdef TRACE
	if (_tracing)
	    _tracef("wclrtoeol(%o) called", win);
#endif

	y = win->_cury;
	x = win->_curx;

	end = &win->_line[y][win->_maxx];
	minx = _NOCHANGE;
	maxx = &win->_line[y][x];

	for (ptr = maxx; ptr < end; ptr++)
	{
	    if (*ptr != blank)
	    {
		maxx = ptr;
		if (minx == _NOCHANGE)
		    minx = ptr - win->_line[y];
		*ptr = blank;
		win->_numchngd[y] += 1;
	    }
	}

	if (minx != _NOCHANGE)
	{
	    if (win->_firstchar[y] > minx || win->_firstchar[y] == _NOCHANGE)
		win->_firstchar[y] = minx;

	    if (win->_lastchar[y] < maxx - win->_line[y])
		win->_lastchar[y] = maxx - win->_line[y];
	}
}
