/*
 * Copyright (c) 1981 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef COHERENT
#ifndef lint
static uchar sccsid[] = "@(#)mvprintw.c	5.4 (Berkeley) 6/30/88";
#endif /* not lint */
#endif /* not COHERENT */

# include	"curses.ext"

/*
 * implement the mvprintw commands.  Due to the variable number of
 * arguments, they cannot be macros.  Sigh....
 *
 */

mvprintw(y, x, fmt /*, args */ )
reg int		y, x;
uchar		*fmt;
{
	uchar	buf[128];

	if (move(y, x) != OK)
		return ERR;
	(void) sprintf(buf, "%r", &fmt);
	return waddstr(stdscr, buf);
}

mvwprintw(win, y, x, fmt /*, args */)
reg WINDOW	*win;
reg int		y, x;
uchar		*fmt;
{
	uchar	buf[128];

	if (wmove(win, y, x) != OK)
		return ERR;
	(void) sprintf(buf, "%r", &fmt );
	return waddstr(win, buf);
}
