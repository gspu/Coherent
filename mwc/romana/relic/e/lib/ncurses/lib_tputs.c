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
 *	tputs.c
 *
 *  $Log:	lib_tputs.c,v $
 * Revision 1.11  93/04/12  14:14:34  bin
 * Udo: third color update
 * 
 * Revision 1.5  92/06/02  12:05:41  bin
 * *** empty log message ***
 * 
 * Revision 1.2  92/04/13  14:38:41  bin
 * update by vlad
 * 
 * Revision 3.2  91/04/20  21:57:22  munk
 * Usage of register variables
 *
 * Revision 3.1  84/12/13  11:21:03  john
 * Revisions by Mark Horton
 * 
 * Revision 2.1  82/10/25  14:49:31  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/24  15:18:06  pavel
 * Beta-one Test Release
 * 
 * Revision 1.3  82/08/23  22:30:52  pavel
 * The REAL Alpha-one Release Version
 * 
 * Revision 1.2  82/08/19  19:11:38  pavel
 * Alpha Test Release One
 * 
 * Revision 1.1  82/08/12  18:46:00  pavel
 * Initial revision
 * 
 *
 */

#ifdef RCSHDR
static char RCSid[] =
	"$Header: /src386/usr/lib/ncurses/RCS/lib_tputs.c,v 1.11 93/04/12 14:14:34 bin Exp Locker: bin $";
#endif

#include <ctype.h>
#include <stdio.h>
#include "curses.h"
#include "curses.priv.h"
#include "term.h"


tputs(string, affcnt, outc)
register char	*string;
int		affcnt;
int		(*outc)();
{
	char		*index();
	float		number;
	int		baud = baudrate();
	char		null = '\0';
	register int	i;

#ifdef TRACE
	if (_tracing)
	    _tracef("tputs(%s,%d,%o) called", string, affcnt, outc);
#endif

	if (NULL == string)
		return;

	if (pad_char)
	    null = pad_char[0];

	while (*string)
	{
	    if (*string != '$')
		(*outc)(*string);
	    else
	    {
		string++;
		if (*string != '<')
		{
		    (*outc)('$');
		    (*outc)(*string);
		}
		else
		{

		    number = 0;
		    string++;

		    if (!isdigit(*string) &&
		         *string != '.' || (index(string, '>') == NULL)) {
			(*outc)('$');
			(*outc)('<');
			continue;
		    }
		    while (isdigit(*string))
		    {
			number = number * 10 + *string - '0';
			string++;
		    }

		    if (*string == '.')
		    {
			string++;
			if (isdigit(*string))
			{
			    number += (float) (*string - '0') / 10.;
			    string++;
			}
		    }

		    if (*string == '*')
		    {
			number *= affcnt;
			string++;
		    }

		    if (padding_baud_rate  &&  baud >= padding_baud_rate && !xon_xoff)
		    {
			number = ((baud / 10.) * number) / 1000.;
			
			for (i=0; i < number; i++)
			    (*outc)(null);
		    }

		} /* endelse (*string == '<') */
	    } /* endelse (*string == '$') */

	    if (*string == '\0')
		break;

	    string++;
	}
}


_outc(ch)
{
    putchar(ch);
}


putp(string)
register char *string;
{
#ifdef TRACE
	if (_tracing)
	    _tracef("putp(%s) called", string);
#endif
	tputs(string, 1, _outc);
}
