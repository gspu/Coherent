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
**	lib_delwin.c
**
**	The routine delwin().
**
** $Log:	lib_delwin.c,v $
 * Revision 1.8  93/04/12  14:13:30  bin
 * Udo: third color update
 * 
 * Revision 1.2  92/04/13  14:37:21  bin
 * update by vlad
 * 
 * Revision 2.2  91/04/20  18:17:38  munk
 * Usage of register variables
 *
 * Revision 2.1  82/10/25  14:47:01  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/25  13:44:21  pavel
 * Beta-one Test Release
 * 
**
*/

#ifdef RCSHDR
static char RCSid[] =
	"$Header: /src386/usr/lib/ncurses/RCS/lib_delwin.c,v 1.8 93/04/12 14:13:30 bin Exp Locker: bin $";
#endif

#include "curses.h"
#include "curses.priv.h"


delwin(win)
register WINDOW	*win;
{
	void		free();
	register int	i;

#ifdef TRACE
	if (_tracing)
	    _tracef("delwin(%o) called", win);
#endif

	if (! (win->_flags & _SUBWIN))
	{
	    for (i = 0; i <= win->_maxy  &&  win->_line[i]; i++)
		free(win->_line[i]);
	}

	free(win->_numchngd);
	free(win->_firstchar);
	free(win->_lastchar);
	free(win->_line);
	free(win);
}
