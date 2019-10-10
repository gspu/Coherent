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
**	lib_newterm.c
**
** 	The newterm() function.
**
** $Log:	RCS/lib_newterm.v $
 * Revision 2.2  91/07/28  12:41:35  munk
 * Print error message if terminal not found
 *
 * Revision 2.1  82/10/25  14:48:14  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/25  13:47:11  pavel
 * Beta-one Test Release
 * 
**
*/

#ifdef RCSHDR
static char RCSid[] =
	"$Header:   RCS/lib_newterm.v  Revision 2.2  91/07/85  12:41:35  munk   Exp$";
#endif

#include <signal.h>
#include <stdio.h>
#include "curses.h"
#include "term.h"
#include "curses.priv.h"



struct screen *
newterm(term, fp)
char	*term;
FILE	*fp;
{
	int	errret;
	int	tstp();
	char	*malloc();

#ifdef TRACE
	_init_trace();

	if (_tracing)
	    _tracef("newterm(%s,%o) called", term, fp);
#endif

/*	if (setupterm(term, fileno(fp), &errret) != 1) */
	if (setupterm(term, fileno(fp), 0) != 1)
	    return(ERR);

	if ((SP = (struct screen *) malloc(sizeof *SP)) == NULL)
	    return(ERR);

	if (fp == stdout)
	{
	    SP->_ofp       = stdout;
	    SP->_ifp       = stdin;
	}
	else
	{
	    SP->_ofp       = fp;
	    SP->_ifp       = fp;
	}
	SP->_term      = cur_term;
	SP->_cursrow   = -1;
	SP->_curscol   = -1;
        SP->_keytry    = UNINITIALISED;
	SP->_nl        = TRUE;
	SP->_raw       = FALSE;
	SP->_cbreak    = FALSE;
	SP->_echo      = TRUE;
	SP->_nlmapping = TRUE;
	SP->_costinit  = FALSE;

	LINES = lines;
	COLS = columns;

	if (enter_ca_mode)
	    putp(enter_ca_mode);

	if ((newscr = newwin(lines, columns, 0, 0)) == ERR)
	    return(ERR);

	if ((curscr = newwin(lines, columns, 0, 0)) == ERR)
	    return(ERR);

	SP->_newscr = newscr;
	SP->_curscr = curscr;

	newscr->_clear = TRUE;
	curscr->_clear = FALSE;

#ifdef SIGTSTP
	signal(SIGTSTP, tstp);
#endif

	if (stdscr == NULL)
	    if ((stdscr = newwin(lines, columns, 0, 0)) == ERR)
		return(ERR);

#ifdef TRACE
	if (_tracing)
	    _tracef("\tnewterm returns %o", SP);
#endif

	return(SP);
}
