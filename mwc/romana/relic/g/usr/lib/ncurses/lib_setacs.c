/*********************************************************************
*                         COPYRIGHT NOTICE                           *
**********************************************************************
*        This software is copyright (C) 1992 by Udo Munk             *
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
**	lib_setacs.c
**
**	Set up the alternate character set map for the terminal
**
** $Log:	lib_setacs.c,v $
 * Revision 1.6  93/04/12  14:14:06  bin
 * Udo: third color update
 * 
 * Revision 1.0  92/10/26  22:35:55  munk
 * Initial version
 * 
**
*/

#ifdef RCSHDR
static char RCSid[] =
	"$Header: /src386/usr/lib/ncurses/RCS/lib_setacs.c,v 1.6 93/04/12 14:14:06 bin Exp Locker: bin $";
#endif

#include "curses.h"
#include "term.h"

setup_acs()
{
	register char *p;

	/* initialize acs_map with ASCII defaults for possible chars */
	acs_map['l'] = acs_map['m'] = acs_map['k'] = acs_map['j'] = '+';
	acs_map['u'] = acs_map['t'] = acs_map['v'] = acs_map['w'] = '+';
	acs_map['q'] = '-';
	acs_map['x'] = '|';
	acs_map['n'] = '+';
	acs_map['o'] = '_';
	acs_map[','] = '<';
	acs_map['+'] = '>';

	/* now get the alternate characters from acs_chars and fill map */
	p = acs_chars;
	do {
		switch (*p) {
		case 'l':
			acs_map['l'] = *++p & 0377 | A_ALTCHARSET;
			break;
		case 'm':
			acs_map['m'] = *++p & 0377 | A_ALTCHARSET;
			break;
		case 'k':
			acs_map['k'] = *++p & 0377 | A_ALTCHARSET;
			break;
		case 'j':
			acs_map['j'] = *++p & 0377 | A_ALTCHARSET;
			break;
		case 'u':
			acs_map['u'] = *++p & 0377 | A_ALTCHARSET;
			break;
		case 't':
			acs_map['t'] = *++p & 0377 | A_ALTCHARSET;
			break;
		case 'v':
			acs_map['v'] = *++p & 0377 | A_ALTCHARSET;
			break;
		case 'w':
			acs_map['w'] = *++p & 0377 | A_ALTCHARSET;
			break;
		case 'q':
			acs_map['q'] = *++p & 0377 | A_ALTCHARSET;
			break;
		case 'x':
			acs_map['x'] = *++p & 0377 | A_ALTCHARSET;
			break;
		case 'n':
			acs_map['n'] = *++p & 0377 | A_ALTCHARSET;
			break;
		case 'o':
			acs_map['o'] = *++p & 0377 | A_ALTCHARSET;
			break;
		case 's':
			acs_map['s'] = *++p & 0377 | A_ALTCHARSET;
			break;
		case '`':
			acs_map['`'] = *++p & 0377 | A_ALTCHARSET;
			break;
		case 'a':
			acs_map['a'] = *++p & 0377 | A_ALTCHARSET;
			break;
		case 'f':
			acs_map['f'] = *++p & 0377 | A_ALTCHARSET;
			break;
		case 'g':
			acs_map['g'] = *++p & 0377 | A_ALTCHARSET;
			break;
		case '~':
			acs_map['~'] = *++p & 0377 | A_ALTCHARSET;
			break;
		case ',':
			acs_map[','] = *++p & 0377 | A_ALTCHARSET;
			break;
		case '+':
			acs_map['+'] = *++p & 0377 | A_ALTCHARSET;
			break;
		case '.':
			acs_map['.'] = *++p & 0377 | A_ALTCHARSET;
			break;
		case '-':
			acs_map['-'] = *++p & 0377 | A_ALTCHARSET;
			break;
		case 'h':
			acs_map['h'] = *++p & 0377 | A_ALTCHARSET;
			break;
		case 'i':
			acs_map['i'] = *++p & 0377 | A_ALTCHARSET;
			break;
		case '0':
			acs_map['0'] = *++p & 0377 | A_ALTCHARSET;
			break;
		default:
			p++;
		}
	} while (*p && *++p);
}
