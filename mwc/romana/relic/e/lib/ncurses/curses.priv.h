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
 *	curses.priv.h
 *
 *	Header file for curses library objects which are private to
 *	the library.
 *
 *  $Log:	RCS/curses.priv.v $
 * Revision 2.3  92/10/26  21:42:44  munk
 * reduced MAXLINES and MAXCOLUMNS only under COHERENT 3.x not under 4.0
 *
 * Revision 2.2  91/04/20  17:46:12  munk
 * MAXLINES reduced to 25 and MAXCOLUMNS reduced to 80 if compiled
 * under COHERENT, to reduce size of memory used
 * 
 * Revision 2.1  82/10/25  14:46:17  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/25  13:50:25  pavel
 * Beta-one Test Release
 */

#include <stdio.h>
#include "curses.h"

#define min(a,b)	((a) > (b)  ?  (b)  :  (a))
#define max(a,b)	((a) < (b)  ?  (b)  :  (a))

#define CHANGED     -1

WINDOW	*stdscr, *curscr, *newscr;
int	LINES, COLS, COLORS, COLOR_PAIRS, __color__, __pair_changed__;
short	__pairs__[2][64];

extern int  _tracing;


struct try
{
        struct try      *child;     /* ptr to child.  NULL if none          */
        struct try      *sibling;   /* ptr to sibling.  NULL if none        */
        char            ch;         /* character at this node               */
        short           value;      /* code of string so far.  NULL if none */
};


struct screen
{
    	FILE		*_ifp;	    /* input file ptr for this terminal     */
    	FILE		*_ofp;	    /* output file ptr for this terminal    */
	struct term	*_term;	    /* used by terminfo stuff               */
	WINDOW		*_curscr;   /* windows specific to a given terminal */
	WINDOW		*_newscr;
        struct try      *_keytry;   /* "Try" for use with keypad mode       */
	char            _backbuf[10]; /* Buffer for pushed back characters  */
	int             _backcnt;   /* How many characters in _backbuf?     */
        int             _cursrow;   /* Row and column of physical cursor    */
        int             _curscol;
	bool		_nl;	    /* True if terminal has CRMOD bit on    */
	bool		_raw;	    /* True if in raw mode                  */
	bool		_cbreak;    /* True if in cbreak mode               */
	bool		_echo;	    /* True if echo on                      */
	bool		_nlmapping; /* True if terminal is really doing     */
				    /* NL mapping (fn of raw and nl)	    */
	int		_costs[9];  /* costs of cursor movements for mvcur  */
	int		_costinit;  /* set if _costs[] is initialized       */
};

extern struct screen *SP;

#define MAXCOLUMNS    135
#define MAXLINES      66
#define UNINITIALISED ((struct try * ) -1)
