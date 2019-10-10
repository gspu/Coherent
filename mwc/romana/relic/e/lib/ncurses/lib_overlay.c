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
**	lib_overlay.c
**
**	The routines overlay() and overwrite().
**
** $Log:	RCS/lib_overlay.v $
 * Revision 2.3  91/04/20  21:33:39  munk
 * Usage of register variables
 *
 * Revision 2.2  82/10/28  18:10:11  pavel
 * Fixed confusion about which direction the copy was supposed to go and
 * also added updates to win2->_{first,last}char.
 * 
 * Revision 2.1  82/10/25  14:48:35  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/25  13:48:09  pavel
 * Beta-one Test Release
 * 
**
*/

#ifdef RCSHDR
static char RCSid[] =
	"$Header:   RCS/lib_overlay.v  Revision 2.3  91/04/20  21:33:39  munk   Exp$";
#endif

#include "curses.h"
#include "curses.priv.h"


/*
**
**	overlay(win1, win2)
**
**
**	overlay() writes win1 on win2 non-destructively.
**
**/

overlay(win1, win2)
WINDOW	*win1, *win2;
{
	int		col, line, last_line, last_col;
	short		*firstchar, *lastchar;
	register chtype	*w1ptr, *w2ptr;
	chtype		attrs;

#ifdef TRACE
	if (_tracing)
	    _tracef("overlay(%o, %o) called", win1, win2);
#endif
	
	last_col = min(win1->_maxx, win2->_maxx);
	last_line = min(win1->_maxy, win2->_maxy);
	attrs = win2->_attrs;
	firstchar = win2->_firstchar;
	lastchar = win2->_lastchar;

	for(line = 0;  line <= last_line;  line++)
	{
	    short   fc, lc;
	    
	    w1ptr = win1->_line[line];
	    w2ptr = win2->_line[line];
	    fc = _NOCHANGE;

	    for(col = 0;  col <= last_col;  col++)
	    {
		if ((*w1ptr & A_CHARTEXT) != ' ')
		{
		    *w2ptr = (*w1ptr & A_CHARTEXT) | attrs;
		    if (fc == _NOCHANGE)
		        fc = col;
		    lc = col;
		}

		w1ptr++;
		w2ptr++;
	    }
	    
	    if (*firstchar == _NOCHANGE)
	    {
	        *firstchar = fc;
		*lastchar = lc;
	    }
	    else if (fc != _NOCHANGE)
	    {
	        if (fc < *firstchar)
		    *firstchar = fc;

	        if (lc > *lastchar)
		    *lastchar = lc;
            }

	    firstchar++;
	    lastchar++;
	}
}


/*
**
**	overwrite(win1, win2)
**
**
**	overwrite() writes win1 on win2 destructively.
**
**/

overwrite(win1, win2)
WINDOW	*win1, *win2;
{
	int		col, line, last_line, last_col;
	short		*firstchar, *lastchar;
	register chtype	*w1ptr, *w2ptr;
	chtype		attrs;

#ifdef TRACE
	if (_tracing)
	    _tracef("overwrite(%o, %o) called", win1, win2);
#endif
	
	last_col = min(win1->_maxx, win2->_maxx);
	last_line = min(win1->_maxy, win2->_maxy);
	attrs = win2->_attrs;
	firstchar = win2->_firstchar;
	lastchar = win2->_lastchar;

	for(line = 0;  line <= last_line;  line++)
	{
	    short   fc, lc;
	    
	    w1ptr = win1->_line[line];
	    w2ptr = win2->_line[line];
	    fc = _NOCHANGE;

	    for(col = 0;  col <= last_col;  col++)
	    {
		if ((*w1ptr & A_CHARTEXT) != (*w2ptr & A_CHARTEXT))
		{
		    *w2ptr = (*w1ptr & A_CHARTEXT) | attrs;
		    if (fc == _NOCHANGE)
		        fc = col;
		    lc = col;
		}

		w1ptr++;
		w2ptr++;
	    }

	    if (*firstchar == _NOCHANGE)
	    {
	        *firstchar = fc;
		*lastchar = lc;
	    }
	    else if (fc != _NOCHANGE)
	    {
	        if (fc < *firstchar)
		    *firstchar = fc;

	        if (lc > *lastchar)
		    *lastchar = lc;
            }
	    
	    firstchar++;
	    lastchar++;
	}
}
