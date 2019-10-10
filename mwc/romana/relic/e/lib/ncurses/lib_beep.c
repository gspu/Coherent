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
 *	beep.c
 *
 *	Routines beep() and flash()
 *
 *  $Log:	lib_beep.c,v $
 * Revision 1.8  93/04/12  14:13:21  bin
 * Udo: third color update
 * 
 * Revision 1.2  92/04/13  14:37:04  bin
 * update by vlad
 * 
 * Revision 2.1  82/10/25  14:46:29  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/24  15:17:31  pavel
 * Beta-one Test Release
 * 
 * Revision 1.3  82/08/23  22:30:22  pavel
 * The REAL Alpha-one Release Version
 * 
 * Revision 1.2  82/08/19  19:11:02  pavel
 * Alpha Test Release One
 * 
 * Revision 1.1  82/08/12  18:40:14  pavel
 * Initial revision
 * 
 *
 */

#ifdef RCSHDR
static char RCSid[] =
	"$Header: /src386/usr/lib/ncurses/RCS/lib_beep.c,v 1.8 93/04/12 14:13:21 bin Exp Locker: bin $";
#endif

#include "curses.h"
#include "curses.priv.h"
#include "term.h"


static
outc(ch)
char    ch;
{
        putc(ch, SP->_ofp);
}


/*
 *	beep()
 *
 *	Sound the current terminal's audible bell if it has one.   If not,
 *	flash the screen if possible.
 *
 */

beep()
{
#ifdef TRACE
	if (_tracing)
	    _tracef("beep() called");
#endif
	if (bell)
	    tputs(bell, 1, outc);
	else if (flash_screen)
	    tputs(flash_screen, 1, outc);
}


/*
 *	flash()
 *
 *	Flash the current terminal's screen if possible.   If not,
 *	sound the audible bell if one exists.
 *
 */

flash()
{
#ifdef TRACE
	if (_tracing)
	    _tracef("flash() called");
#endif
	if (flash_screen)
	    tputs(flash_screen, 1, outc);
	else if (bell)
	    tputs(bell, 1, outc);
}
