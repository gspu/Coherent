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
 *	lib_trace.c - Tracing/Debugging routines
 *
 *  $Log:	lib_trace.c,v $
 * Revision 1.8  93/04/12  14:14:36  bin
 * Udo: third color update
 * 
 * Revision 1.2  92/04/13  14:38:45  bin
 * update by vlad
 * 
 * Revision 2.2  91/04/20  22:00:18  munk
 * Usage of register variables
 * Made the large arrays static
 *
 * Revision 2.1  82/10/25  14:49:35  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/24  15:18:09  pavel
 * Beta-one Test Release
 * 
 * Revision 1.3  82/08/23  22:30:57  pavel
 * The REAL Alpha-one Release Version
 * 
 * Revision 1.2  82/08/19  19:11:41  pavel
 * Alpha Test Release One
 * 
 * Revision 1.1  82/08/15  17:59:45  pavel
 * Initial revision
 * 
 *
 */

#ifdef RCSHDR
static char RCSid[] =
    "$Header: /src386/usr/lib/ncurses/RCS/lib_trace.c,v 1.8 93/04/12 14:14:36 bin Exp Locker: bin $";
#endif

#include "term.h"
#include "curses.h"
#include "curses.priv.h"

static int	tracefd;


_init_trace()
{
    	static int	been_here = 0;
	extern int	errno;
	extern char	*sys_errlist[];

	if (! been_here)
	{
	    _tracing = been_here = 1;

	    if ((tracefd = creat("trace", 0644)) < 0)
	    {
		write(2, "curses: Can't open 'trace' file: ", 33);
		write(2, sys_errlist[errno], strlen(sys_errlist[errno]));
		write(2, "\n", 1);
		exit(1);
	    }

	    _tracef("_init_trace() called");
	}
}


traceon()
{
	_tracef("traceon() called");

    	_tracing = 1;
}


traceoff()
{
	_tracef("traceoff() called");

    	_tracing = 0;
}


_tracef(fmt, args)
register char	*fmt;
int		args;
{
	char		*strcpy();
    	int		*parm = &args;
	static char	buffer[256];
	char		*bufp = buffer;

	while (*fmt)
	{
	    if (*fmt == '%')
	    {
		fmt++;
		switch (*fmt)
		{
		    case 'd':
			addnum(&bufp, *(parm++), 10);
			break;

		    case 'o':
			addnum(&bufp, *(parm++), 8);
			break;

		    case 'c':
			*(bufp++) = *(parm++);
			break;

		    case 's':
			if (*parm)
			{
			    *(bufp++) = '"';
			    strcpy(bufp, *parm);
			    bufp += strlen(*parm);
			    *(bufp++) = '"';
			}
			else
			{
			    strcpy(bufp, "NULL");
			    bufp += 4;
			}
			parm++;
			break;
		}
	    }
	    else
		*(bufp++) = *fmt;

	    fmt++;
	}

	*(bufp++) = '\n';
	*bufp = '\0';
	write(tracefd, buffer, strlen(buffer));
}


static addnum(bufp, num, base)
char		**bufp;
register int	num, base;
{
	int	a;

    	if (num < 0)
	{
	    num = -num;
	    *((*bufp)++) = '-';
	}

	if ((a = num / base) != 0)
	    addnum(bufp, a, base);
	*((*bufp)++) = '0' + (num % base);
}
