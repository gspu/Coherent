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

#ifndef A_STANDOUT

#include <stdio.h>

#include <termio.h>
#define DEF_VEOF	CEOF
#define DEF_VEOL	0
#define DEF_VMIN	1
#define DEF_VTIME	0

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
