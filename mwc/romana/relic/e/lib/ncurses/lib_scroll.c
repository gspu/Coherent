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
**	lib_scroll.c
**
**	The routine scroll().
**
** $Log:	lib_scroll.c,v $
 * Revision 1.11  93/04/12  14:14:03  bin
 * Udo: third color update
 * 
 * Revision 1.5  92/06/02  12:05:32  bin
 * *** empty log message ***
 * 
 * Revision 1.2  92/04/13  14:38:26  bin
 * update by vlad
 * 
 * Revision 2.2  91/04/20  21:40:21  munk
 * Usage of register variables
 *
 * Revision 2.1  82/10/25  14:48:54  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/25  13:49:22  pavel
 * Beta-one Test Release
 * 
**
*/

#ifdef RCSHDR
static char RCSid[] =
	"$Header: /src386/usr/lib/ncurses/RCS/lib_scroll.c,v 1.11 93/04/12 14:14:03 bin Exp Locker: bin $";
#endif

#include "curses.h"
#include "curses.priv.h"


scroll(win)
register WINDOW	*win;
{
	register int	i;
	chtype		*ptr, *temp;
	chtype		blank = ' ' | win->_attrs;

#ifdef TRACE
	if (_tracing)
	    _tracef("scroll(%o) called", win);
#endif

	if (!win->_scroll)
	    return;

	/* scroll up within scrolling region */
	temp = win->_line[win->_regtop];
	for (i = win->_regtop; i < win->_regbottom; i++) {
	    win->_line[i] = win->_line[i+1];

	    win->_firstchar[i] = 0;
	    win->_numchngd[i] = win->_lastchar[i] = win->_maxx;
	}

	/* clear last line */
	for (ptr = temp; ptr - temp <= win->_maxx; ptr++)
	    *ptr = blank;

	win->_line[win->_regbottom] = temp;
	win->_firstchar[win->_regbottom] = 0;
	win->_numchngd[win->_regbottom] = win->_lastchar[win->_regbottom] =
		win->_maxx;
	
	/* if cursor is in scrolling region move it up */
	if ((win->_cury > win->_regtop) && (win->_cury <= win->_regbottom))
		win->_cury--;
}
