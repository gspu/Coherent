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
**	lib_longname.c
**
**	The routine longname().
**
** $Log:	RCS/lib_longname.v $
 * Revision 2.2  91/04/20  19:16:14  munk
 * Usage of register variables
 *
 * Revision 2.1  82/10/25  14:47:49  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/25  13:46:21  pavel
 * Beta-one Test Release
 * 
**
*/

#ifdef RCSHDR
static char RCSid[] =
	"$Header:   RCS/lib_longname.v  Revision 2.2  91/04/20  19:16:14  munk   Exp$";
#endif

#include "curses.h"
#include "curses.priv.h"


char *
longname()
{
    	register char	*ptr;

#ifdef TRACE
	if (_tracing)
	    _tracef("longname() called");
#endif

	for (ptr = ttytype + strlen(ttytype); ptr > ttytype; ptr--)
	    if (*ptr == '|')
		return(ptr + 1);

        return(ttytype);
}
