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
**	lib_scrreg.c
**
**	The routine wsetscrreg().
**
** $Log:	lib_scrreg.c,v $
 * Revision 1.11  93/04/12  14:14:05  bin
 * Udo: third color update
 * 
 * Revision 1.5  92/06/02  12:05:34  bin
 * *** empty log message ***
 * 
 * Revision 1.2  92/04/13  14:38:28  bin
 * update by vlad
 * 
 * Revision 2.2  91/04/20  21:42:05  munk
 * Usage of register variables
 *
 * Revision 2.1  82/10/25  14:49:00  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/25  13:49:29  pavel
 * Beta-one Test Release
 * 
**
*/

#ifdef RCSHDR
static char RCSid[] =
	"$Header: /src386/usr/lib/ncurses/RCS/lib_scrreg.c,v 1.11 93/04/12 14:14:05 bin Exp Locker: bin $";
#endif

#include "curses.h"
#include "curses.priv.h"


wsetscrreg(win, top, bottom)
register WINDOW	*win;
int		top, bottom;
{
#ifdef TRACE
	if (_tracing)
	    _tracef("wsetscrreg(%o,%d,%d) called", win, top, bottom);
#endif

    	if ((0 <= top) && (bottom <= win->_maxy) && (top < bottom)) {
	    win->_regtop = top;
	    win->_regbottom = bottom;

	    if (win->_cury > bottom)	/* get cursor in scroll region */
	    	win->_cury = bottom;
	    else if (win->_cury < top)
	    	win->_cury = top;

	    return(OK);
	}
	else
	    return(ERR);
}
