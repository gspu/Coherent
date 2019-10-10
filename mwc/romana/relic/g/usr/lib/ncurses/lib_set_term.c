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
**	lib_set_term.c
**
**	The routine set_term().
**
** $Log:	RCS/lib_set_term.v $
 * Revision 2.2  92/11/08  23:50:10  munk
 * Set up alternate character set map and
 * other forgotten stuff
 * 
 * Revision 2.1  82/10/25  14:49:06  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/25  13:49:42  pavel
 * Beta-one Test Release
 * 
**
*/

#ifdef RCSHDR
static char RCSid[] =
	"$Header:   RCS/lib_set_term.v  Revision 2.1  82/10/25  14:49:06  pavel  Exp$";
#endif

#include "curses.h"
#include "curses.priv.h"
#include "term.h"


struct screen *
set_term(screen)
struct screen *screen;
{
	char		*strncpy();
    	struct screen	*oldSP;

#ifdef TRACE
	if (_tracing)
	    _tracef("set_term(%o) called", screen);
#endif

	oldSP = SP;
	SP = screen;

	cur_term = SP->_term;
	curscr   = SP->_curscr;
	newscr   = SP->_newscr;

	strncpy(ttytype, cur_term->term_names, NAMESIZE - 1);
	ttytype[NAMESIZE - 1] = '\0';

	LINES = lines;
	COLS = columns;

	setup_acs();

	return(oldSP);
}
