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
 *	vidputs(newmode, outc)
 *
 *	newmode is taken to be the logical 'or' of the symbols in curses.h
 *	representing graphic renditions.  The teminal is set to be in all of
 *	the given modes, if possible.
 *
 *  $Log:	RCS/lib_vidattr.v $
 * Revision 2.3  93/02/16  17:47:46  munk
 * color attributes added
 *
 * Revision 2.2  91/02/12  14:29:50  munk
 * New algorithm for every mix of attributes
 *
 * Revision 2.1  82/10/25  14:49:45  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/24  15:18:15  pavel
 * Beta-one Test Release
 * 
 * Revision 1.3  82/08/23  22:31:08  pavel
 * The REAL Alpha-one Release Version
 * 
 * Revision 1.2  82/08/19  19:11:46  pavel
 * Alpha Test Release One
 * 
 * Revision 1.1  82/08/12  18:48:23  pavel
 * Initial revision
 * 
 *
 */

#ifdef RCSHDR
static char RCSid[] =
	"$Header:   RCS/lib_vidattr.v  Revision 2.3  93/02/16  17:47:46  munk   Exp$";
#endif

#include "curses.h"
#include "curses.priv.h"
#include "term.h"


vidputs(newmode, outc)
chtype newmode;
int	 (*outc)();
{
    static chtype		previous_attr = 0;
    register chtype		turn_off, turn_on;
    char			*tparm();

#ifdef TRACE
    if (_tracing)
	_tracef("vidputs(%o,%o) called", newmode, outc);
#endif

    turn_off = ~newmode & previous_attr;

    if ((turn_off & A_ALTCHARSET)  &&  exit_alt_charset_mode) {
	tputs(exit_alt_charset_mode, 1, outc);
	turn_off &= ~A_ALTCHARSET;
    }

    if ((turn_off & A_UNDERLINE)  &&  exit_underline_mode) {
	tputs(exit_underline_mode, 1, outc);
	turn_off &= ~A_UNDERLINE;
    }

    if ((turn_off & A_STANDOUT)  &&  exit_standout_mode) {
	tputs(exit_standout_mode, 1, outc);
	turn_off &= ~A_STANDOUT;
    }

    if (turn_off && exit_attribute_mode) {
	tputs(exit_attribute_mode, 1, outc);
	turn_off = 0;
    }

    turn_on = newmode & ~turn_off;

    if ((turn_on & A_ALTCHARSET)  &&  enter_alt_charset_mode)
	tputs(enter_alt_charset_mode, 1, outc);

    if ((turn_on & A_BLINK)  &&  enter_blink_mode)
	tputs(enter_blink_mode, 1, outc);

    if ((turn_on & A_BOLD)  &&  enter_bold_mode)
	tputs(enter_bold_mode, 1, outc);

    if ((turn_on & A_INVIS)  &&  enter_secure_mode)
	tputs(enter_secure_mode, 1, outc);

    if ((turn_on & A_DIM)  &&  enter_dim_mode)
	tputs(enter_dim_mode, 1, outc);

    if ((turn_on & A_PROTECT)  &&  enter_protected_mode)
	tputs(enter_protected_mode, 1, outc);

    if ((turn_on & A_REVERSE)  &&  enter_reverse_mode)
	tputs(enter_reverse_mode, 1, outc);

    if ((turn_on & A_STANDOUT)  &&  enter_standout_mode)
	tputs(enter_standout_mode, 1, outc);

    if ((turn_on & A_UNDERLINE)  &&  enter_underline_mode)
	tputs(enter_underline_mode, 1, outc);

    if (__color__) {
	if ((newmode & A_COLOR) != (previous_attr & A_COLOR) || __pair_changed__) {
	    if (__pairs__[0][PAIR_NUMBER(newmode)] != -1) {
		tputs(tparm(set_foreground, __pairs__[0][PAIR_NUMBER(newmode)]), 1, outc);
		tputs(tparm(set_background, __pairs__[1][PAIR_NUMBER(newmode)]), 1, outc);
	    }
	}
    }

    previous_attr = newmode;
}


vidattr(newmode)
chtype	newmode;
{
    int		_outc();

#ifdef TRACE
    if (_tracing)
	_tracef("vidattr(%o) called", newmode);
#endif

    vidputs(newmode, _outc);
}
