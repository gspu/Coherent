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
 *  $Header: /src386/usr/lib/ncurses/RCS/terminfo.h,v 1.2 92/04/13 14:40:21 bin Exp Locker: bin $
 *
 *	terminfo.h - those things needed for programs runnning at the
 *			terminfo level.
 *
 *  $Log:	terminfo.h,v $
 * Revision 2.4  92/11/29  15:42:29  munk
 * Conditional ussage of termio
 *
 * Revision 2.3  92/10/25  22:52:49  munk
 * Now supports 16-bit characters
 * Defines for alternate character set added
 * Attribute bits changed, colors added
 *
 * Revision 1.2  92/04/13  14:40:21  bin
 * *** empty log message ***
 * 
 * Revision 2.2  91/02/10  12:27:05  munk
 * Added conditional 8-bit characters for UNIX on PC's
 * 
 * Revision 2.1  82/10/25  14:49:59  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/24  15:18:26  pavel
 * Beta-one Test Release
 * 
 * Revision 1.4  82/08/23  22:31:21  pavel
 * The REAL Alpha-one Release Version
 * 
 * Revision 1.3  82/08/19  19:24:11  pavel
 * Alpha Test Release One
 * 
 * Revision 1.2  82/08/19  19:10:56  pavel
 * Alpha Test Release One
 * 
 * Revision 1.1  82/08/15  16:42:20  pavel
 * Initial revision
 */

#ifndef A_STANDOUT

#include <stdio.h>
#ifdef USE_TERMIO
#include <termio.h>
#define DEF_VEOF	CEOF
#define DEF_VEOL	0
#define DEF_VMIN	1
#define DEF_VTIME	0
#else
#include <sgtty.h>
#define SGTTY	struct sgttyb
#endif

/* Video attributes */

#define A_NORMAL	000000000000L
#define A_ATTRIBUTES	037777600000L	/* 0xFFFF0000 */
#define A_CHARTEXT	000000177777L	/* 0x0000FFFF */
#define A_COLOR		017600000000L

#define A_STANDOUT	000000200000L
#define	_STANDOUT	A_STANDOUT    /* for compatability with old curses */
#define A_UNDERLINE	000000400000L

#define A_REVERSE	000001000000L
#define A_BLINK		000002000000L
#define A_DIM		000004000000L
#define A_BOLD		000010000000L
#define A_ALTCHARSET	000100000000L
/* The next two are subject to change so don't depend on them */
#define A_INVIS		000020000000L
#define A_PROTECT	000040000000L

#define COLOR_PAIR(n)	((n) << 25)
#define PAIR_NUMBER(n)	(((n) & A_COLOR) >> 25)

#define COLOR_BLACK	0
#define COLOR_RED	1
#define COLOR_GREEN	2
#define COLOR_YELLOW	3
#define COLOR_BLUE	4
#define COLOR_MAGENTA	5
#define COLOR_CYAN	6
#define COLOR_WHITE	7

#define ACS_ULCORNER	(acs_map['l'])	/* upper left corner */
#define ACS_LLCORNER	(acs_map['m'])	/* lower left corner */
#define ACS_URCORNER	(acs_map['k'])	/* upper right corner */
#define ACS_LRCORNER	(acs_map['j'])	/* lower right corner */
#define ACS_RTEE	(acs_map['u'])	/* right tee */
#define ACS_LTEE	(acs_map['t'])	/* left tee */
#define ACS_BTEE	(acs_map['v'])	/* bottom tee */
#define ACS_TTEE	(acs_map['w'])	/* top tee */
#define ACS_HLINE	(acs_map['q'])	/* horizontal line */
#define ACS_VLINE	(acs_map['x'])	/* vertical line */
#define ACS_PLUS	(acs_map['n'])	/* plus */
#define ACS_S1		(acs_map['o'])	/* scan line 1 */
#define ACS_S9		(acs_map['s'])	/* scan line 9 */
#define ACS_DIAMOND	(acs_map['`'])	/* diamond */
#define ACS_CKBOARD	(acs_map['a'])	/* checker board (stipple) */
#define ACS_DEGREE	(acs_map['f'])	/* degree symbol */
#define ACS_PLMINUS	(acs_map['g'])	/* plus/minus */
#define ACS_BULLET	(acs_map['~'])	/* bullet */
#define ACS_LARROW	(acs_map[','])	/* arrow pointing left */
#define ACS_RARROW	(acs_map['+'])	/* arrow pointing right */
#define ACS_DARROW	(acs_map['.'])	/* arrow pointing down */
#define ACS_UARROW	(acs_map['-'])	/* arrow pointing up */
#define ACS_BOARD	(acs_map['h'])	/* board of squares */
#define ACS_LANTERN	(acs_map['i'])	/* lantern symbol */
#define ACS_BLOCK	(acs_map['0'])	/* solid square block */

extern char	ttytype[];
extern chtype	acs_map[];
#define NAMESIZE	256
#endif
