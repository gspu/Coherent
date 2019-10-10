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
**	lib_insch.c
**
**	The routine winsch().
**
** $Log:	lib_insch.c,v $
 * Revision 1.11  93/04/12  14:13:44  bin
 * Udo: third color update
 * 
 * Revision 1.5  92/06/02  12:05:16  bin
 * *** empty log message ***
 * 
 * Revision 1.2  92/04/13  14:37:38  bin
 * update by vlad
 * 
 * Revision 2.2  91/04/20  19:09:25  munk
 * Usage of register variables
 *
 * Revision 2.1  82/10/25  14:47:39  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/25  13:46:02  pavel
 * Beta-one Test Release
 * 
**
*/

#ifdef RCSHDR
static char RCSid[] =
	"$Header: /src386/usr/lib/ncurses/RCS/lib_insch.c,v 1.11 93/04/12 14:13:44 bin Exp Locker: bin $";
#endif

#include "curses.h"
#include "curses.priv.h"


winsch(win, c)
register WINDOW	*win;
chtype c;
{
	register chtype	*temp1, *temp2;
	chtype		*end;
	int from, to;

#ifdef TRACE
	if (_tracing)
	    _tracef("winsch(%o,%o) called", win, c);
#endif

	end = &win->_line[win->_cury][win->_curx];
	temp1 = &win->_line[win->_cury][win->_maxx];
	temp2 = temp1 - 1;

	while (temp1 > end)
	    *temp1-- = *temp2--;

	*temp1 = c | win->_attrs;

	to = win->_lastchar[win->_cury] = win->_maxx;
	from = win->_firstchar[win->_cury];
	if ((from == _NOCHANGE) || (from > win->_curx))
	    from = win->_firstchar[win->_cury] = win->_curx;
	win->_numchngd[win->_cury] = to - from;
}
