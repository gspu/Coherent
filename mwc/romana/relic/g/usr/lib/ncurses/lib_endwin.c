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
**	lib_endwin.c
**
**	The routine endwin().
**
** $Log:	lib_endwin.c,v $
 * Revision 1.9  93/04/12  14:13:36  bin
 * Udo: third color update
 * 
 * Revision 1.3  92/06/10  14:16:50  bin
 * changed ref to sys/fcntl.h
 * 
 * Revision 1.2  92/04/13  14:37:24  bin
 * update by vlad
 * 
 * Revision 2.2  91/02/09  13:54:50  munk
 * pavel don't output reset strings 1..3 ? (added)
 * and added code to mark the screen as clear
 * 
 * Revision 2.1  82/10/25  14:47:13  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/25  13:45:05  pavel
 * Beta-one Test Release
 * 
**
*/

#ifdef RCSHDR
static char RCSid[] =
	"$Header: /src386/usr/lib/ncurses/RCS/lib_endwin.c,v 1.9 93/04/12 14:13:36 bin Exp Locker: bin $";
#endif

#ifdef COHERENT
#include <fcntl.h>
#else
#include <fcntl.h>
#endif
#include "term.h"
#include "curses.h"
#include "curses.priv.h"


static
outc(ch)
char	ch;
{
    	putc(ch, SP->_ofp);
}


endwin()
{
#ifdef TRACE
	if (_tracing)
	    _tracef("endwin() called");
#endif

	resetterm();

	mvcur(-1, -1, LINES - 1, 0);

	curscr->_clear = TRUE;

	if (curscr  &&  (curscr->_attrs != A_NORMAL))
	{
	    vidputs(A_NORMAL, outc);

	    curscr->_attrs = A_NORMAL;
	}

	if (exit_ca_mode)
	    tputs(exit_ca_mode, 1, outc);
	if (reset_1string)
	    tputs(reset_1string, 1, outc);
	if (reset_2string)
	    tputs(reset_2string, 1, outc);
	if (reset_3string)
	    tputs(reset_3string, 1, outc);

	fflush(SP->_ofp);
}
