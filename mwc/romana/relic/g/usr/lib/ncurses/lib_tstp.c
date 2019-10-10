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
**	lib_tstp.c
**
**	The routine tstp().
**
** $Log:	lib_tstp.c,v $
 * Revision 1.8  93/04/12  14:14:38  bin
 * Udo: third color update
 * 
 * Revision 1.2  92/04/13  14:38:48  bin
 * update by vlad
 * 
 * Revision 2.1  82/10/25  14:49:39  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/25  13:50:01  pavel
 * Beta-one Test Release
 * 
**
*/

#ifdef RCSHDR
static char RCSid[] =
	"$Header: /src386/usr/lib/ncurses/RCS/lib_tstp.c,v 1.8 93/04/12 14:14:38 bin Exp Locker: bin $";
#endif

#include "term.h"
#include "curses.h"
#include "curses.priv.h"
#include <signal.h>


static
outc(ch)
char	ch;
{
    	putc(ch, SP->_ofp);
}


tstp()
{
#ifdef TRACE
	if (_tracing)
	    _tracef("tstp() called");
#endif

	endwin();

#ifdef SIGTSTP
	kill(0, SIGTSTP);
	signal(SIGTSTP, tstp);
#endif

	fixterm();
	flushinp();
	tputs(enter_ca_mode, 1, outc);
	wrefresh(curscr);
}
