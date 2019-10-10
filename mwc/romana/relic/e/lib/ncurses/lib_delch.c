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
**	lib_delch.c
**
**	The routine wdelch().
**
** $Log:	lib_delch.c,v $
 * Revision 1.11  93/04/12  14:13:29  bin
 * Udo: third color update
 * 
 * Revision 1.5  92/06/02  12:05:05  bin
 * *** empty log message ***
 * 
 * Revision 1.2  92/04/13  14:37:18  bin
 * update by vlad
 * 
 * Revision 2.2  91/04/20  18:13:33  munk
 * Usage of register variables
 *
 * Revision 2.1  82/10/25  14:46:52  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/24  15:20:47  pavel
 * Beta-one Test Release
 * 
**
*/

#ifdef RCSHDR
static char RCSid[] =
	"$Header: /src386/usr/lib/ncurses/RCS/lib_delch.c,v 1.11 93/04/12 14:13:29 bin Exp Locker: bin $";
#endif

#include "curses.h"
#include "curses.priv.h"
#include "term.h"


wdelch(win)
register WINDOW	*win;
{
	register chtype	*temp1, *temp2;
	chtype		*end;
	int cury;

#ifdef TRACE
	if (_tracing)
	    _tracef("wdelch(%o) called", win);
#endif

	cury = win->_cury;
	end = &win->_line[cury][win->_maxx];
	temp2 = &win->_line[cury][win->_curx + 1];
	temp1 = temp2 - 1;

	while (temp1 < end)
	    *temp1++ = *temp2++;

	*temp1 = ' ' | win->_attrs;

	win->_lastchar[cury] = win->_maxx;

	if (win->_firstchar[cury] == _NOCHANGE
				   || win->_firstchar[cury] > win->_curx)
	    win->_firstchar[cury] = win->_curx;

	if (delete_character)
	    win->_numchngd[cury] += 1;
	else
	    win->_numchngd[cury] += win->_maxx - win->_curx + 1;
}
