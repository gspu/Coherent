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
**	lib_box.c
**
**	The routine box().
**
** $Log:	lib_box.c,v $
 * Revision 1.11  93/04/12  14:13:22  bin
 * Udo: third color update
 * 
 * Revision 1.5  92/06/02  12:04:59  bin
 * *** empty log message ***
 * 
 * Revision 1.2  92/04/13  14:37:07  bin
 * update by vlad
 * 
 * Revision 2.2  91/04/20  18:02:49  munk
 * Usage of register variables
 *
 * Revision 2.1  82/10/25  14:46:31  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/24  15:20:07  pavel
 * Beta-one Test Release
 * 
**
*/

#ifdef RCSHDR
static char RCSid[] =
	"$Header: /src386/usr/lib/ncurses/RCS/lib_box.c,v 1.11 93/04/12 14:13:22 bin Exp Locker: bin $";
#endif

#include "curses.h"
#include "curses.priv.h"


box(win, vert, hor)
register WINDOW	*win;
char		vert, hor;
{
	register int	i;
	int		endy, endx;
	chtype		*fp, *lp;

#ifdef TRACE
	if (_tracing)
	    _tracef("box(%o,%c,%c) called", win, vert, hor);
#endif

	endx = win->_maxx;
	endy = win->_maxy;

	fp = win->_line[0];
	lp = win->_line[endy];

	for (i = 0; i <= endx; i++)
	    fp[i] = lp[i] = hor;

	for (i = 0; i <= endy; i++) {
	    win->_line[i][0] = win->_line[i][endx] = vert;
	    win->_firstchar[i] = 0;
	    win->_lastchar[i] = endx;
	    win->_numchngd[i] += 2;
	}

	win->_numchngd[0] = win->_numchngd[endy] = endx;

	if (! win->_scroll  &&  (win->_flags & _SCROLLWIN))
	    fp[0] = fp[endx] = lp[0] = lp[endx] = ' ';
}
