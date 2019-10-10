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
**	lib_touchwin.c
**
**	The routine touchwin().
**
** $Log:	RCS/lib_touchwin.v $
 * Revision 2.2  91/04/20  21:47:29  munk
 * Usage of register variables
 *
 * Revision 2.1  82/10/25  14:49:13  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/25  13:49:52  pavel
 * Beta-one Test Release
 * 
**
*/

#ifdef RCSHDR
static char RCSid[] =
	"$Header:   RCS/lib_touchwin.v  Revision 2.2  91/04/20  21:47:29  munk   Exp$";
#endif

#include "curses.h"
#include "curses.priv.h"


touchwin(win)
register WINDOW	*win;
{
	register int	y;
	int		maxy, maxx;

#ifdef TRACE
	if (_tracing)
	    _tracef("touchwin(%o) called", win);
#endif

	maxy = win->_maxy;
	maxx = win->_maxx;

	for (y = 0; y <= maxy; y++)
	{
	    win->_firstchar[y] = 0;
	    win->_lastchar[y] = maxx;
	    win->_numchngd[y] = maxx;
	}
}
