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
**	lib_addstr.c
*
**	The routine waddstr().
**
** $Log:	lib_addstr.c,v $
 * Revision 2.3  92/11/01  16:03:23  munk
 * No sign extension for 8-bit chars
 *
 * Revision 1.2  92/04/13  14:37:02  bin
 * update by vlad
 * 
 * Revision 2.2  91/04/20  17:57:24  munk
 * Usage of register variables
 *
 * Revision 2.1  82/10/25  14:46:26  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/24  15:19:59  pavel
 * Beta-one Test Release
 * 
**
*/

#ifdef RCSHDR
static char RCSid[] =
	"$Header: /src386/usr/lib/ncurses/RCS/lib_addstr.c,v 1.2 92/04/13 14:37:02 bin Exp Locker: bin $";
#endif

#include "curses.h"
#include "curses.priv.h"


waddstr(win, str)
register WINDOW	*win; 
register char	*str;
{
#ifdef TRACE
	if (_tracing)
	    _tracef("waddstr(%o,%o) called", win, str);
#endif

	while (*str)
	{
	    if (waddch(win, (chtype)(*str++ & 0377)) == ERR)
		return(ERR);
	}

	return(OK);
}
