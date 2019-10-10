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
 *	comp_error.c -- Error message routines
 *
 *  $Log:	comp_error.c,v $
 * Revision 1.8  93/04/12  14:12:56  bin
 * Udo: third color update
 * 
 * Revision 1.2  92/04/13  14:35:55  bin
 * update by vlad
 * 
 * Revision 2.1  82/10/25  14:45:31  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/24  15:16:32  pavel
 * Beta-one Test Release
 * 
 * Revision 1.3  82/08/23  22:29:31  pavel
 * The REAL Alpha-one Release Version
 * 
 * Revision 1.2  82/08/19  19:09:44  pavel
 * Alpha Test Release One
 * 
 * Revision 1.1  82/08/12  18:36:02  pavel
 * Initial revision
 * 
 *
 */

#ifdef RCSHDR
static char RCSid[] =
	"$Header: /src386/usr/lib/ncurses/RCS/comp_error.c,v 1.8 93/04/12 14:12:56 bin Exp Locker: bin $";
#endif

#include "compiler.h"

extern char *string_table;
extern short term_names;


warning(fmt, a1, a2, a3, a4, a5, a6)
char	*fmt, *a1, *a2, *a3, *a4, *a5, *a6;
{
    fprintf (stderr, "tic: Warning: near line %d: ", curr_line);
    fprintf (stderr, "terminal '%s', ", string_table+term_names);
    fprintf (stderr, fmt, a1, a2, a3, a4, a5, a6);
    fprintf (stderr, "\n");
}


err_abort(fmt, a1, a2, a3, a4, a5, a6)
char	*fmt, *a1, *a2, *a3, *a4, *a5, *a6;
{
    fprintf (stderr, "tic: Line %d: ", curr_line);
    fprintf (stderr, "terminal '%s', ", string_table+term_names);
    fprintf (stderr, fmt, a1, a2, a3, a4, a5, a6);
    fprintf (stderr, "\n");
    exit(1);
}


syserr_abort(fmt, a1, a2, a3, a4, a5, a6)
char	*fmt, *a1, *a2, *a3, *a4, *a5, *a6;
{
    fprintf (stderr, "PROGRAM ERROR: Line %d: ", curr_line);
    fprintf (stderr, "terminal '%s', ", string_table+term_names);
    fprintf (stderr, fmt, a1, a2, a3, a4, a5, a6);
    fprintf (stderr, "\n");
    abort();
}
