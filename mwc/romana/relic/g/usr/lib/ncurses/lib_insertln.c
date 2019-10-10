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
**	lib_insertln.c
**
**	The routine winsertln().
**
** $Log:	RCS/lib_insertln.v $
 * Revision 2.2  91/04/20  19:11:20  munk
 * Usage of register variables
 *
 * Revision 2.1  82/10/25  14:47:44  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/25  13:46:12  pavel
 * Beta-one Test Release
 * 
**
*/

#ifdef RCSHDR
static char RCSid[] =
	"$Header:   RCS/lib_insertln.v  Revision 2.2  91/04/20  19:11:20  munk   Exp$";
#endif

#include "curses.h"
#include "curses.priv.h"


winsertln(win)
register WINDOW	*win;
{
	chtype		*temp, *end;
	register int	y;

#ifdef TRACE
	if (_tracing)
	    _tracef("winsertln(%o) called", win);
#endif

	temp = win->_line[win->_regbottom];

	win->_firstchar[win->_cury] = 0;
	win->_numchngd[win->_cury] = win->_lastchar[win->_cury] = win->_maxx;

	for (y = win->_regbottom;  y > win->_cury;  y--)
	{
	    win->_line[y] = win->_line[y-1];

	    win->_firstchar[y] = 0;
	    win->_numchngd[y] = win->_lastchar[y] = win->_maxx;
	}

	win->_line[win->_cury] = temp;

	for (end = &temp[win->_maxx];  temp <= end;  temp++)
	    *temp = ' ' | win->_attrs;
}
