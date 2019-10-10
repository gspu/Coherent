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
**	lib_initscr.c
**
**	The routine initscr().
**
** $Log:	RCS/lib_initscr.v $
 * Revision 2.1  82/10/25  14:47:36  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/25  13:45:54  pavel
 * Beta-one Test Release
 * 
**
*/

#ifdef RCSHDR
static char RCSid[] =
	"$Header:   RCS/lib_initscr.v  Revision 2.1  82/10/25  14:47:36  pavel  Exp$";
#endif

#include "curses.h"
#include "curses.priv.h"


WINDOW *
initscr()
{
	char	*getenv();
#ifdef TRACE
	_init_trace();

	if (_tracing)
	    _tracef("initscr() called");
#endif

    	if (newterm(getenv("TERM"), stdout) == ERR)
	    return(ERR);
	else
	    return(stdscr);
}
