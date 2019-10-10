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
**	lib_erase.c
**
**	The routine werase().
**
** $Log:	lib_erase.c,v $
 * Revision 1.8  93/04/12  14:13:38  bin
 * Udo: third color update
 * 
 * Revision 1.2  92/04/13  14:37:27  bin
 * update by vlad
 * 
 * Revision 2.3  91/04/20  18:32:50  munk
 * Usage of register variables
 *
 * Revision 2.2  82/11/03  12:27:41  pavel
 * Fixed off-by-one error...  If only I had used an invariant...
 * 
 * Revision 2.1  82/10/25  14:47:17  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/25  13:45:12  pavel
 * Beta-one Test Release
 * 
**
*/

#ifdef RCSHDR
static char RCSid[] =
	"$Header: /src386/usr/lib/ncurses/RCS/lib_erase.c,v 1.8 93/04/12 14:13:38 bin Exp Locker: bin $";
#endif

#include "curses.h"
#include "curses.priv.h"


werase(win)
register WINDOW	*win;
{
	register int	y;
	chtype		*sp, *end, *start, *maxx;
	int		minx;
	chtype		blank = ' ' | win->_attrs;

#ifdef TRACE
	if (_tracing)
	    _tracef("werase(%o) called", win);
#endif

	for (y = win->_regtop; y <= win->_regbottom; y++)
	{
	    minx = _NOCHANGE;
	    start = win->_line[y];
	    end = &start[win->_maxx];

	    for (sp = start; sp <= end; sp++)
	    {
		if (*sp != blank)
		{
		    maxx = sp;
		    if (minx == _NOCHANGE)
			minx = sp - start;

		    *sp = blank;

		    win->_numchngd[y] += 1;
		}
	    }

	    if (minx != _NOCHANGE)
	    {
		if (win->_firstchar[y] > minx
					    ||  win->_firstchar[y] == _NOCHANGE)
		    win->_firstchar[y] = minx;

		if (win->_lastchar[y] < maxx - win->_line[y])
		    win->_lastchar[y] = maxx - win->_line[y];
	    }
	}

	win->_curx = win->_cury = 0;
}
