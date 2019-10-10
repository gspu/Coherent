/*********************************************************************
*                         COPYRIGHT NOTICE                           *
**********************************************************************
*        This software is copyright (C) 1993 by Udo Munk             *
*                                                                    *
*        Permission is granted to reproduce and distribute           *
*        this file by any means so long as no fee is charged         *
*        above a nominal handling fee and so long as this            *
*        notice is always included in the copies.                    *
*                                                                    *
*        Other rights are reserved except as explicitly granted      *
*        by written permission of the author.                        *
*                Udo Munk					     *
*                Oberstr. 21					     *
*                4040 Neuss 1					     *
*                Germany					     *
*                                                                    *
*                udo@umunk.GUN.de				     *
*********************************************************************/

/*
**	lib_color.c
**
**	The color system of SV.3
**
** $Log:	lib_color.c,v $
 * Revision 1.3  93/04/12  14:13:28  bin
 * Udo: third color update
 * 
 * Revision 1.0  93/02/16  17:47:46  munk
 * Initial version
 * 
**
*/

#ifdef RCSHDR
static char RCSid[] =
	"$Header: /src386/usr/lib/ncurses/RCS/lib_color.c,v 1.3 93/04/12 14:13:28 bin Exp Locker: bin $";
#endif

#include "curses.h"
#include "curses.priv.h"
#include "term.h"

start_color()
{
	register int i;

	for (i = 1; i < 63; i++)
		__pairs__[0][i] = __pairs__[1][i] = -1;

	__pairs__[0][0] = COLOR_WHITE;
	__pairs__[1][0] = COLOR_BLACK;
	__pair_changed__ = FALSE;

	COLORS = max_colors;
	COLOR_PAIRS = max_pairs;

	if ((COLORS > 0) && (COLOR_PAIRS > 0)) {
		__color__ = TRUE;
		return(OK);
	} else {
		__color__ = FALSE;
		return(ERR);
	}
}

has_colors()
{
	if ((max_colors > 0) && (max_pairs > 0))
		return(TRUE);
	else
		return(FALSE);
}

can_change_color()
{
	if (can_change)
		return(TRUE);
	else
		return(FALSE);
}

init_color(new_color, r, g, b)
short new_color, r, g, b;
{
	return(ERR);
}

color_content(the_color, r, g, b)
short the_color, *r, *g, *b;
{
	return(ERR);
}

init_pair(pair, f, b)
short pair, f, b;
{
	short pair_changed = 0;
	register int i, j;

	if (!__color__)
		return(ERR);
	if ((pair > 63) || (pair < 0))
		return(ERR);
	if (pair >= COLOR_PAIRS)
		return(ERR);
	if ((f >= COLORS) || (f < 0))
		return(ERR);
	if ((b >= COLORS) || (b < 0))
		return(ERR);

	__pair_changed__ = TRUE;

	pair_changed = __pairs__[0][pair];
	__pairs__[0][pair] = f;
	__pairs__[1][pair] = b;

	if (pair_changed != -1) {
		for (i = 0; i < LINES; i++) {
			for (j = 0; j < COLS; j++) {
				if (PAIR_NUMBER(curscr->_line[i][j]) == pair) {
					curscr->_line[i][j] = ' ';
				}
			}
		}
		doupdate();
	}

	return(OK);
}

pair_content(pair, f, b)
short pair, *f, *b;
{
	if (!__color__)
		return(ERR);
	if ((pair > 63) || (pair < 0))
		return(ERR);
	if (pair >= COLOR_PAIRS)
		return(ERR);
	if (__pairs__[0][pair] == -1)
		return(ERR);

	*f = __pairs__[0][pair];
	*b = __pairs__[1][pair];

	return(OK);
}
