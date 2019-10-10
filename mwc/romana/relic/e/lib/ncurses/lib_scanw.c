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
**	lib_scanw.c
**
**	The routines scanw(), wscanw() and friend.
**
** $Log:	lib_scanw.c,v $
 * Revision 1.8  93/04/12  14:14:02  bin
 * Udo: third color update
 * 
 * Revision 1.2  92/04/13  14:38:23  bin
 * update by vlad
 * 
 * Revision 2.2  91/01/27  15:17:12  munk
 * Rewritten for COHERENT and portable usage of variable arguments
 *
 * Revision 2.1  82/10/25  14:48:51  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/25  13:49:07  pavel
 * Beta-one Test Release
 * 
**
*/

#ifdef RCSHDR
static char RCSid[] =
	"$Header: /src386/usr/lib/ncurses/RCS/lib_scanw.c,v 1.8 93/04/12 14:14:02 bin Exp Locker: bin $";
#endif

#include "curses.h"
#include "curses.priv.h"

#define VARARGS	v1, v2, v3, v4, v5, v6, v7, v8, v9, v10


scanw(fmt, VARARGS)
char	*fmt;
int	VARARGS;
{
#ifdef TRACE
	if (_tracing)
	    _tracef("scanw(%s,...) called", fmt);
#endif

	return(sscans(stdscr, fmt, VARARGS));
}


wscanw(win, fmt, VARARGS)
WINDOW	*win;
char	*fmt;
int	VARARGS;
{
#ifdef TRACE
	if (_tracing)
	    _tracef("wscanw(%o,%s,...) called", win, fmt);
#endif

	return(sscans(win, fmt, VARARGS));
}


mvscanw(y, x, fmt, VARARGS)
int	y, x;
char	*fmt;
int	VARARGS;
{
	return(move(y, x) == OK ? sscans(stdscr, fmt, VARARGS) : ERR);
}


mvwscanw(win, y, x, fmt, VARARGS)
WINDOW	*win;
int	y, x;
char	*fmt;
int	VARARGS;
{
	return(wmove(win, y, x) == OK ? sscans(win, fmt, VARARGS) : ERR);
}


/*
**	This routine actually executes the scanf from the window.
*/

static
sscans(win, fmt, VARARGS)
WINDOW	*win;
char	*fmt;
int	*VARARGS;
{
	static char buf[100];

	if (wgetstr(win, buf) == ERR)
	    return(ERR);
	return(sscanf(buf, fmt, VARARGS));
}
