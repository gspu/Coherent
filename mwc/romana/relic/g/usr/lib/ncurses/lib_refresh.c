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
 *	lib_refresh.c
 *
 *	The routines wrefresh() and wnoutrefresh().
 *
 *  $Log:	lib_refresh.c,v $
 * Revision 3.2  91/04/20  21:37:40  munk
 * Usage of register variables
 *
 * Revision 3.1  84/12/13  11:20:51  john
 * Revisions by Mark Horton
 * 
 * Revision 2.1  82/10/25  14:48:45  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/25  13:48:47  pavel
 * Beta-one Test Release
 * 
 *
 */

#ifdef RCSHDR
static char RCSid[] =
	"$Header: lib_refresh.c,v 3.2 91/04/20 21:37:40 munk Exp $";
#endif

#include "curses.h"
#include "curses.priv.h"


wrefresh(win)
WINDOW	*win;
{
#ifdef TRACE
	if (_tracing)
	    _tracef("wrefresh(%o) called", win);
#endif

	if (win == curscr)
	{
	    curscr->_clear = TRUE;
	    doupdate();
	}
	else
	{
	    wnoutrefresh(win);
	    doupdate();
	}
}


wnoutrefresh(win)
register WINDOW	*win;
{
	register int	i, j;
	int		begx = win->_begx;
	int		begy = win->_begy;
	int		m, n;

#ifdef TRACE
	if (_tracing)
	    _tracef("wnoutrefresh(%o) called", win);
#endif

	for (i=0, m=begy; i <= win->_maxy; i++, m++)
	{
#ifdef TRACE
	    if (_tracing)
	    {
		_tracef("win->_firstchar[%d]= %d\t_lastchar= %d\t_numchngd= %d",
			    i, win->_firstchar[i],
			       win->_lastchar[i],
			       win->_numchngd[i]);
	    }
#endif

	    if (win->_numchngd[i])
	    {
		j = win->_firstchar[i];
		n = j + begx;
		for (; j <= win->_lastchar[i]; j++, n++)
		{
		    if (win->_line[i][j] != newscr->_line[m][n])
		    {
			newscr->_line[m][n] = win->_line[i][j];
			newscr->_numchngd[m] += 1;

			if (newscr->_firstchar[m] == _NOCHANGE)
			    newscr->_firstchar[m] = newscr->_lastchar[m] = n;
			else if (n < newscr->_firstchar[m])
			    newscr->_firstchar[m] = n;
			else if (n > newscr->_lastchar[m])
			    newscr->_lastchar[m] = n;
		    }
		}
	    }

	    win->_numchngd[i] = 0;
	    win->_firstchar[i] = win->_lastchar[i] = _NOCHANGE;
	}

	if (win->_clear)
	{
	    win->_clear = FALSE;
	    newscr->_clear = TRUE;
	}

	if (! win->_leave)
	{
	    newscr->_cury = win->_cury + win->_begy;
	    newscr->_curx = win->_curx + win->_begx;
	}
}
