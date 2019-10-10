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
**	lib_move.c
**
**	The routine wmove().
**
** $Log:	lib_move.c,v $
 * Revision 1.8  93/04/12  14:13:45  bin
 * Udo: third color update
 * 
 * Revision 1.2  92/04/13  14:37:40  bin
 * update by vlad
 * 
 * Revision 2.2  91/04/20  19:18:10  munk
 * Usage of register variables
 *
 * Revision 2.1  82/10/25  14:47:51  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/25  13:46:31  pavel
 * Beta-one Test Release
 * 
**
*/

#ifdef RCSHDR
static char RCSid[] =
	"$Header: /src386/usr/lib/ncurses/RCS/lib_move.c,v 1.8 93/04/12 14:13:45 bin Exp Locker: bin $";
#endif

#include "curses.h"
#include "curses.priv.h"


wmove(win, y, x)
register WINDOW	*win;
int	y, x;
{
#ifdef TRACE
	if (_tracing)
	    _tracef("wmove(%o,%d,%d) called", win, y, x);
#endif

	if (0 <= x  &&  x <= win->_maxx  &&
		win->_regtop <= y  &&  y <= win->_regbottom)
	{
	    win->_curx = x;
	    win->_cury = y;

	    return(OK);
	}
	else
	    return(ERR);
}
