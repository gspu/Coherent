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
**	lib_getstr.c
**
**	The routine wgetstr().
**
** $Log:	lib_getstr.c,v $
 * Revision 1.8  93/04/12  14:13:42  bin
 * Udo: third color update
 * 
 * Revision 1.2  92/04/13  14:37:34  bin
 * update by vlad
 * 
 * Revision 2.2  91/04/20  19:07:12  munk
 * Usage of register variables
 *
 * Revision 2.1  82/10/25  14:47:33  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/25  13:45:39  pavel
 * Beta-one Test Release
 * 
**
*/

#ifdef RCSHDR
static char RCSid[] =
	"$Header: /src386/usr/lib/ncurses/RCS/lib_getstr.c,v 1.8 93/04/12 14:13:42 bin Exp Locker: bin $";
#endif

#include "curses.h"
#include "curses.priv.h"
#include "unctrl.h"

#define backspace() {							\
		    mvwaddstr(curscr, win->_begy + win->_cury,		\
				      win->_begx + win->_curx, "\b \b");\
		    waddstr(win, "\b \b");				\
		    fputs("\b \b", SP->_ofp);				\
		    fflush(SP->_ofp);					\
		    }


wgetstr(win,str)
register WINDOW	*win; 
register char	*str;
{
	char	erasechar(), killchar();
	bool	oldnl, oldecho, oldraw, oldcbreak;
	char	erasec;
	char	killc;
	char	*oldstr;

#ifdef TRACE
	if (_tracing)
	    _tracef("wgetstr(%o,%o) called", win, str);
#endif

	oldnl = SP->_nl;
	oldecho = SP->_echo;
	oldraw = SP->_raw;
	oldcbreak = SP->_cbreak;
	nl();
	noecho();
	noraw();
	cbreak();

	erasec = erasechar();
	killc = killchar();

	oldstr = str;

	while ((*str = getc(SP->_ifp)) != ERR  &&  *str != '\n')
	{
	    if (*str == erasec)
	    {
		if (str > oldstr)
		{
		    str--;
		    backspace();
		    if (*str < ' ' ||  *str == '\177')
			backspace();
		}
	    }
	    else if (*str == killc)
	    {
		while (str > oldstr)
		{
		    str--;
		    backspace();
		    if (*str < ' ' ||  *str == '\177')
			backspace();
		}
	    }
	    else
	    {
		mvwaddstr(curscr, win->_begy + win->_cury,
				  win->_begx + win->_curx, unctrl(*str));
		waddstr(win, unctrl(*str));
		fputs(unctrl(*str), SP->_ofp);
		fflush(SP->_ofp);
		str++;
	    }
	}

	if (! oldnl)
	    nonl();

	if (oldecho)
	    echo();

	if (oldraw)
	    raw();

	if (! oldcbreak)
	    nocbreak();

	if (*str == ERR)
        {
	    *str = '\0';
	    return(ERR);
	}

	*str = '\0';

#ifdef TRACE
	if (_tracing)
	    _tracef("\twgetstr returns %s", oldstr);
#endif

	return(OK);
}
