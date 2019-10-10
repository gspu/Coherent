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
**	lib_mvwin.c
**
**	The routine mvwin().
**
** $Log:	lib_mvwin.c,v $
 * Revision 1.8  93/04/12  14:13:53  bin
 * Udo: third color update
 * 
 * Revision 1.2  92/04/13  14:38:09  bin
 * update by vlad
 * 
 * Revision 2.2  91/04/20  19:27:27  munk
 * Usage of register variables
 *
 * Revision 2.1  82/10/25  14:48:10  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/25  13:47:03  pavel
 * Beta-one Test Release
 * 
**
*/

#ifdef RCSHDR
static char RCSid[] =
	"$Header: /src386/usr/lib/ncurses/RCS/lib_mvwin.c,v 1.8 93/04/12 14:13:53 bin Exp Locker: bin $";
#endif

#include "curses.h"
#include "curses.priv.h"


mvwin(win, by, bx)
register WINDOW	*win;
int		by, bx;
{
#ifdef TRACE
	if (_tracing)
	    _tracef("mvwin(%o,%d,%d) called", win, by, bx);
#endif

	if (by + win->_maxy > LINES -1  ||  bx + win->_maxx > COLS - 1)
	    return(ERR);

	win->_begy = by;
	win->_begx = bx;

	touchwin(win);

	return(OK);
}
