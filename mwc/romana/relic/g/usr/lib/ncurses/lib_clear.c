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
**	lib_clear.c
**
**	The routine wclear().
**
** $Log:	lib_clear.c,v $
 * Revision 1.8  93/04/12  14:13:24  bin
 * Udo: third color update
 * 
 * Revision 1.2  92/04/13  14:37:10  bin
 * update by vlad
 * 
 * Revision 2.1  82/10/25  14:46:34  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/24  15:20:17  pavel
 * Beta-one Test Release
 * 
**
*/

#ifdef RCSHDR
static char RCSid[] =
	"$Header: /src386/usr/lib/ncurses/RCS/lib_clear.c,v 1.8 93/04/12 14:13:24 bin Exp Locker: bin $";
#endif

#include "curses.h"
#include "curses.priv.h"


wclear(win)
WINDOW	*win;
{
#ifdef TRACE
	if (_tracing)
	    _tracef("wclear(%o) called", win);
#endif

	werase(win);

	win->_clear = TRUE;

	return;
}
