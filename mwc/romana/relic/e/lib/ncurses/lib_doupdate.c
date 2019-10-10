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
 *	lib_doupdate.c
 *
 *	The routine doupdate() and its dependents
 *
 *  $Log:	lib_doupdate.c,v $
 * Revision 3.4  93/04/10  00:18:56  munk
 * color attributes added
 *
 * Revision 3.3  92/11/01  18:34:18  munk
 * Switch attributes of in ClearScreen
 *
 * Revision 3.2  91/04/20  18:20:16  munk
 * Usage of register variables
 *
 * Revision 3.1  84/12/13  11:20:28  john
 * Revisions by Mark Horton
 * 
 * Revision 2.1  82/10/25  14:47:05  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/25  13:44:33  pavel
 * Beta-one Test Release
 * 
 *
 */

#ifdef RCSHDR
static char RCSid[] =
	"$Header: lib_doupdate.c,v 3.4 93/04/10 00:18:56 munk Exp $";
#endif

#include <signal.h>
#include "curses.h"
#include "curses.priv.h"
#include "term.h"

#define GoTo(row,col)		mvcur(SP->_cursrow, SP->_curscol, row, col);  \
				SP->_cursrow = row;                           \
				SP->_curscol = col;

#define PutAttrChar(ch)		if ((curscr->_attrs != (ch & A_ATTRIBUTES))   \
				    || (__pair_changed__ == TRUE))	      \
				{                                             \
				    curscr->_attrs = ch & A_ATTRIBUTES;       \
				    vidputs(curscr->_attrs, outc);            \
				    __pair_changed__ = FALSE;		      \
				}                                             \
				putc((int) (ch & A_CHARTEXT), SP->_ofp);

#define PutChar(ch)		if (!auto_right_margin  ||                    \
						SP->_cursrow != lines - 1  || \
						SP->_curscol != columns - 1)  \
				{                                             \
				    PutAttrChar(ch);                          \
				    SP->_curscol++;                           \
				    if (SP->_curscol >= columns)              \
					if (auto_right_margin)                \
					{                                     \
					    SP->_curscol = 0;                 \
					    SP->_cursrow++;                   \
					}                                     \
					else                                  \
					    SP->_curscol--;                   \
				}



outc(ch)
char	ch;
{
    	putc(ch, SP->_ofp);
}


doupdate()
{
	register int	i;
	int		(*oldsig)();

#ifdef TRACE
	if (_tracing)
	    _tracef("doupdate() called");
#endif

#ifdef SIGTSTP
	oldsig = signal(SIGTSTP, SIG_IGN);
#endif

	if (curscr->_clear)
	{
	    ClrUpdate(curscr);
	    curscr->_clear = FALSE;

	    GoTo(curscr->_cury, curscr->_curx);
	}
	else
	{
	    if (newscr->_clear)
	    {
		ClrUpdate(newscr);
		newscr->_clear = FALSE;
	    }
	    else if (curscr->_idlok)
		IdlUpdate();
	    else
		NoIdlUpdate();

	    for (i = 0; i < lines; i++)
	    {
		newscr->_firstchar[i] = _NOCHANGE;
		newscr->_lastchar[i] = _NOCHANGE;
		newscr->_numchngd[i] = 0;
	    }

	    curscr->_curx = newscr->_curx;
	    curscr->_cury = newscr->_cury;

	    GoTo(curscr->_cury, curscr->_curx);
	}


	fflush(SP->_ofp);

#ifdef SIGTSTP
	signal(SIGTSTP, oldsig);
#endif
}


/*
**	ClrUpdate(scr)
**
**	Update by clearing and redrawing the entire screen.
**
*/

static
ClrUpdate(scr)
register WINDOW	*scr;
{
    	register int	i, j;

	ClearScreen();

	for (i=0; i < lines; i++)
	{
	    int	lastNonBlank, lastCol;

	    for (lastNonBlank = columns - 1;
		 lastNonBlank && scr->_line[i][lastNonBlank] == ' ';
		 lastNonBlank--)
		;

            if (!lastNonBlank)
	        continue;

	    lastCol = lastNonBlank == (columns - 1);
	    if (lastCol && i == lines)
		lastNonBlank--;

	    for (j = 0; j <= lastNonBlank; j++) {
		if (' ' != scr->_line[i][j]) {
			GoTo(i, j);
			break;
		}
	    }

	    for (;j <= lastNonBlank; j++) {
		PutAttrChar(scr->_line[i][j]);
	    }

	    if (auto_right_margin && lastCol) {
		SP->_cursrow = i + 1;
	    	SP->_curscol = 0;
	    }
	    else
		SP->_curscol = j;
	}

	if (scr != curscr)
	    for (i = 0; i < LINES; i++)
		for (j = 0; j < COLS; j++)
			curscr->_line[i][j] = scr->_line[i][j];
}


/*
**	NoIdlUpdate()
**
**	Update screen without using Insert/Delete Line capabilities
**
*/

static
NoIdlUpdate()
{
	register int	i;

#ifdef TRACE
	if (_tracing)
	    _tracef("NoIdlUpdate() called");
#endif

	for (i=0; i < lines; i++)
	    if (newscr->_numchngd[i])
		TransformLine(i);
}


/*
**	IdlUpdate()
**
**	Update screen using Insert/Delete Line capabilities
**
*/

#define UNCHANGED(n)	(newscr->_numchngd[n] <= columns/10		\
				||  newscr->_lastchar[n]		\
					  - newscr->_firstchar[n] <= columns/10)

static
IdlUpdate()
{
	int		firstLine, lastLine;
	register int	thisLine;

#ifdef TRACE
	if (_tracing)
	    _tracef("IdlUpdate() called");
#endif

	firstLine = -1;

	for (thisLine = 0; thisLine < lines; thisLine++)
	{
	    if (UNCHANGED(thisLine))
	    {
		if (firstLine != -1)
		{
		    lastLine = thisLine - 1;
		    Gosling(firstLine, lastLine);
		    firstLine = -1;
		}
		
		if (newscr->_firstchar[thisLine] != _NOCHANGE)
		    TransformLine(thisLine);
	    }
	    else if (firstLine == -1)
		firstLine = thisLine;
	}

	if (firstLine != -1)
	    Gosling(firstLine, lines - 1);
}


/*
**	TransformLine(lineno)
**
**	Call either IDcTransformLine or NoIDcTransformLine to do the
**	update, depending upon availability of insert/delete character.
*/

static
TransformLine(lineno)
int	lineno;
{
    	if ( (insert_character  ||  (enter_insert_mode  &&  exit_insert_mode))
		 &&  delete_character)
	    IDcTransformLine(lineno);
	else
	    NoIDcTransformLine(lineno);
}


/*
**	NoIDcTransformLine(lineno)
**
**	Transform the given line in curscr to the one in newscr, without
**	using Insert/Delete Character.
**
**		firstChar = position of first different character in line
**		lastChar = position of last different character in line
**
**		overwrite all characters between firstChar and lastChar.
**
*/

static
NoIDcTransformLine(lineno)
int	lineno;
{
	register int	firstChar, lastChar;
	register chtype	*newLine = newscr->_line[lineno];
	register chtype	*oldLine = curscr->_line[lineno];
        register int     k;

#ifdef TRACE
	if (_tracing)
	    _tracef("NoIDcTransformLine(%d) called", lineno);
#endif

	firstChar = 0;
	while (firstChar < columns
			 &&  newLine[firstChar] == oldLine[firstChar])
	    firstChar++;

	if (firstChar >= columns)
	    return;

	lastChar = columns - 1;
	while (lastChar > firstChar  &&  newLine[lastChar] == oldLine[lastChar])
	    lastChar--;
    	
	GoTo(lineno, firstChar);

	for (k=firstChar; k <= lastChar; k++)
	{
	    PutChar(newLine[k]);
	    oldLine[k] = newLine[k];
	}
}


/*
**	IDcTransformLine(lineno)
**
**	Transform the given line in curscr to the one in newscr, using
**	Insert/Delete Character.
**
**		firstChar = position of first different character in line
**		oLastChar = position of last different character in old line
**		nLastChar = position of last different character in new line
**
**		move to firstChar
**		overwrite chars up to min(oLastChar, nLastChar)
**		if oLastChar < nLastChar
**			insert newLine[oLastChar+1..nLastChar]
**		else
**			delete oLastChar - nLastChar spaces
*/

static
IDcTransformLine(lineno)
int	lineno;
{
	int		firstChar, oLastChar, nLastChar;
	chtype		*newLine = newscr->_line[lineno];
	chtype		*oldLine = curscr->_line[lineno];
        register int	k, n;

#ifdef TRACE
	if (_tracing)
	    _tracef("IDcTransformLine(%d) called", lineno);
#endif

	firstChar = 0;
	while (firstChar < columns  &&  newLine[firstChar] == oldLine[firstChar])
	    firstChar++;

	if (firstChar >= columns)
	    return;

	oLastChar = columns - 1;
	while (oLastChar > firstChar  &&  oldLine[oLastChar] == ' ')
	    oLastChar--;

	nLastChar = columns - 1;
	while (nLastChar > firstChar  &&  newLine[nLastChar] == ' ')
	    nLastChar--;

	while (newLine[nLastChar] == oldLine[oLastChar])
	{
	    nLastChar--;
	    oLastChar--;
	}

	n = min(oLastChar, nLastChar);
	GoTo(lineno, firstChar);

	for (k=firstChar; k <= n; k++)
	    PutChar(newLine[k]);

	if (oLastChar < nLastChar)
	    InsStr(&newLine[k], nLastChar - oLastChar);
	else if (oLastChar > nLastChar)
	    DelChar(oLastChar - nLastChar);

	for (k=firstChar; k < columns; k++)
	    oldLine[k] = newLine[k];
}


/*
**	Gosling(firstLine, lastLine)
**
**	Change the given range of lines on curscr into the same lines
**	on newscr, using Gosling's Algorithm.
*/

static short lineCost[MAXLINES][MAXLINES];
static short lineOps[MAXLINES][MAXLINES];
static short lineDels[MAXLINES];
static short lineIRs[MAXLINES];

#define INSERT	1
#define DELETE	2
#define REPLACE 3

static
Gosling(firstLine, lastLine)
int	firstLine, lastLine;
{
	register int	i, count;

#ifdef TRACE
	if (_tracing)
	    _tracef("Gosling(%d,%d) called", firstLine, lastLine);
#endif

	Goscost(firstLine, lastLine - firstLine + 1);

	for (i=0; i <= lastLine - firstLine + 1; i++)
	    lineDels[i] = lineIRs[i] = 0;

	Gosdraw(lastLine - firstLine + 1, lastLine - firstLine + 1);

	count = 0;
	for (i = lastLine - firstLine + 1; i > 0; i--)
	{
	    if (lineDels[i] == DELETE)
		count++;
	    else if (count)
	    {	
		DelLine(count, firstLine + i, lastLine);
		count = 0;
	    }
	}

	if (count)
	    DelLine(count, firstLine, lastLine);


	for (i = 1; i <= lastLine - firstLine + 1; i++)
	{
	    switch (lineIRs[i])
	    {
		case REPLACE:
		    TransformLine(firstLine + i - 1);
		    break;

		case INSERT:
		    InsLine(firstLine + i - 1, lastLine);
		    break;

		default:
		    /* do nothing */
		    break;
	    }
	}
}


#define RPLCOST(old,new)	(oHash[old] == nHash[new]  ?  0  :  columns)

static
Goscost(lineno, length)
int	lineno, length;
{	
	register int	i, j;
	int		cost, ILcost, DLcost;
	long		nHash[MAXLINES], oHash[MAXLINES];
        long    	HashFn();

#ifdef TRACE
	if (_tracing)
	    _tracef("Goscost(lineno=%d,length=%d) called", lineno, length);
#endif

	ILcost = (insert_line ? strlen(insert_line) : 9999) + columns;
	DLcost = (delete_line ? strlen(delete_line) : 9999);

	for (i=1; i <= length; i++)
	{
	    nHash[i] = HashFn(newscr->_line[lineno + i - 1]);
	    oHash[i] = HashFn(curscr->_line[lineno + i - 1]);
	}

	lineCost[0][0] = 0;

	for (i=1; i <= length; i++)
	{
	    lineCost[i][0] = lineCost[i-1][0] + DLcost;
	    lineOps[i][0] = DELETE;

	    lineCost[0][i] = lineCost[0][i-1] + ILcost;
	    lineOps[0][i] = INSERT;
	}

	for (i=1; i <= length; i++)
	{
	    for (j=1; j <= length; j++)
	    {
		lineCost[i][j] = lineCost[i-1][j-1] + RPLCOST(i, j);
		lineOps[i][j] = REPLACE;

		cost = lineCost[i][j-1] + ILcost;
		if (cost < lineCost[i][j])
		{
		    lineCost[i][j] = cost;
		    lineOps[i][j] = INSERT;
		}

		cost = lineCost[i-1][j] + DLcost;
		if (cost < lineCost[i][j])
		{
		    lineCost[i][j] = cost;
		    lineOps[i][j] = DELETE;
		}
	    }
	}

	return(lineCost[columns][columns]);
}


/*
**	_PrintCosts(length)
**
**	Print out the cost matrix.  Called only from sdb.
**
**
**	_DumpNewscr(first, last)
**
**	Print the specified range of lines from newscr.  Called only from sdb.
**
**
**	_DumpCurscr(first, last)
**
**	Print the specified range of lines from curscr.  Called only from sdb.
**
*/

_PrintCosts(length)
int	length;
{
    	register int	i, j;

	for (i=0; i <= length; i++)
	{
	    for (j=0; j <= length; j++)
	    {
		printf("%5d/%d", lineCost[i][j], lineOps[i][j]);
	    }

	    putchar('\n');
	}

	fflush(stdout);
}


_DumpNewscr(first, last)
int	first, last;
{
    	register int	i, j;

	for (i=first; i <= last; i++)
	{
	    for (j=0; j < columns; j++)
		putchar((int) (newscr->_line[i][j]));
	    
	    putchar('\n');
	}
}


_DumpCurscr(first, last)
int	first, last;
{
    	register int	i, j;

	for (i=first; i <= last; i++)
	{
	    for (j=0; j < columns; j++)
		putchar((int) (curscr->_line[i][j]));
	    
	    putchar('\n');
	}
}


long
HashFn(line)
register chtype	*line;
{
    	register int	i = 0;
	long		hash = 0;

	while(i < columns  &&  (line[i] | A_CHARTEXT) == ' ')
	    i++;

	for (; i+1 < columns; i += 2)
	    hash += (line[i] << 8) + line[i+1];

	return (hash);
}


static
Gosdraw(i, j)
register int	i, j;
{
    	if (i == 0  &&  j == 0)
	    return;
	
	switch (lineOps[i][j])
	{
	    case INSERT:
		Gosdraw(i, j-1);
		lineIRs[j] = INSERT;
		break;

	    case DELETE:
		lineDels[i] = DELETE;
		Gosdraw(i-1, j);
		break;

	    case REPLACE:
		Gosdraw(i-1, j-1);
		lineIRs[j] = REPLACE;
		break;
	}
}


/*
**	ClearScreen()
**
**	Clear the physical screen and put cursor at home
**
*/

static
ClearScreen()
{
	vidputs(A_NORMAL, outc);
	if (clear_screen)
	{
	    tputs(clear_screen, 1, outc);
	    SP->_cursrow = SP->_curscol = 0;
	}
	else if (clr_eos)
	{
	    SP->_cursrow = SP->_curscol = -1;
	    GoTo(0,0);

	    tputs(clr_eos, 1, outc);
	}
	else if (clr_eol)
	{
	    SP->_cursrow = SP->_curscol = -1;

	    while (SP->_cursrow < lines)
	    {
		GoTo(SP->_cursrow, 0);
		tputs(clr_eol, 1, outc);
	    }

	    GoTo(0,0);
	}
}


/*
**	InsStr(line, count)
**
**	Insert the count characters pointed to by line.
**
*/

InsStr(line, count)
register chtype	*line;
register int	count;
{
	char	*tparm();

#ifdef TRACE
	if (_tracing)
	    _tracef("InsStr(%o,%d) called", line, count);
#endif

    	if (enter_insert_mode  &&  exit_insert_mode)
	{
	    tputs(enter_insert_mode, 1, outc);
	    while (count)
	    {
		PutChar(*line);
		line++;
		count--;
	    }
	    tputs(exit_insert_mode, 1, outc);
	}
	else if (parm_ich)
	{
	    tputs(tparm(parm_ich, count), 1, outc);
	    while (count)
	    {
		PutChar(*line);
		line++;
		count--;
	    }
	}
	else
	{
	    while (count)
	    {
		tputs(insert_character, 1, outc);
		PutChar(*line);
		line++;
		count--;
	    }
	}
}


/*
**	DelChar(count)
**
**	Delete count characters at current position
**
*/

DelChar(count)
register int	count;
{
	char	*tparm();

#ifdef TRACE
	if (_tracing)
	    _tracef("DelChar(%d) called", count);
#endif

    	if (parm_dch)
	{
	    tputs(tparm(parm_dch, count), 1, outc);
	}
	else
	{
	    while (count--)
		tputs(delete_character, 1, outc);
	}
}


/*
**	InsLine(lineno, lastLine)
**
**	Insert line number lineno, affecting up to lastLine
**
*/

InsLine(lineno, lastLine)
int	lineno, lastLine;
{
	register int	i, j, k;
	chtype		*temp;
	chtype		*line = newscr->_line[lineno];

#ifdef TRACE
	if (_tracing)
	    _tracef("InsLine(%d,%d) called", lineno, lastLine);
#endif
	
	GoTo(lineno, 0);

    	tputs(insert_line, 1, outc);

	for (i=0; i < columns; i++)
	    PutChar(line[i]);

	temp = curscr->_line[lastLine];
	for (k = lastLine; k > lineno; k--)
	    curscr->_line[k] = curscr->_line[k - 1];

	curscr->_line[k] = temp;

	for (j=0; j < columns; j++)
	    curscr->_line[k][j] = newscr->_line[k][j];
}


/*
**	DelLine(count, lineno, lastLine)
**
**	Delete count lines at lineno, affecting up to lastLine
**
*/

DelLine(count, lineno, lastLine)
int	count, lineno, lastLine;
{
	char		*tparm();
	register int	j, k;
	chtype		*temp;

#ifdef TRACE
	if (_tracing)
	    _tracef("DelLine(%d,%d,%d) called", count, lineno, lastLine);
#endif

	GoTo(lineno, 0);

    	if (parm_delete_line)
	{
	    tputs(tparm(parm_delete_line, count), 1, outc);
	}
	else
	{
	    while (count--)
		tputs(delete_line, 1, outc);
	}

	for (k = lineno; k + count <= lastLine; k++)
	{
	    temp = curscr->_line[k];
	    curscr->_line[k] = curscr->_line[k + count];
	    curscr->_line[k + count] = temp;
	}

	for (; k <= lastLine; k++)
	    for (j=0; j < columns; j++)
		curscr->_line[k][j] = ' ';
}
