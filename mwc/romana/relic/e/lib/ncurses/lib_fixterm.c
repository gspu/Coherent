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
 *	fixterm.c
 *
 *	Routines:
 *		fixterm()
 *		resetterm()
 *		saveterm()
 *		gettmode()
 *		setterm()
 *		baudrate()
 *		erasechar()
 *		killchar()
 *		flushinp()
 *		savetty()
 *		resetty()
 *
 *  $Log:	RCS/lib_fixterm.v $
 * Revision 2.2  92/29/11  15:27:58  munk
 * Conditional usage of termio
 *
 * Revision 2.1  82/10/25  14:47:22  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/24  15:17:36  pavel
 * Beta-one Test Release
 * 
 * Revision 1.3  82/08/23  22:30:27  pavel
 * The REAL Alpha-one Release Version
 * 
 * Revision 1.2  82/08/19  19:11:21  pavel
 * Alpha Test Release One
 * 
 * Revision 1.1  82/08/12  18:40:30  pavel
 * Initial revision
 * 
 *
 */

#ifdef RCSHDR
static char RCSid[] =
	"$Header:   RCS/lib_fixterm.v  Revision 2.1  82/10/25  14:47:22  pavel  Exp$";
#endif

#include "curses.h"
#include "curses.priv.h"
#include "term.h"
#ifdef COHERENT
#ifdef USE_TERMIO
#include <termio.h>
#else
#include <sgtty.h>
#endif
#else
#include <sys/ioctl.h>
#endif


/*
 *	fixterm()
 *	resetterm()
 *
 *	fixterm() establishes the tty modes contained in cur_term->Nttyb.
 *	resetterm() establishes those in cur_term->Ottyb.
 *
 */

fixterm()
{
#ifdef TRACE
	if (_tracing)
	    _tracef("fixterm() called");
#endif
#ifdef USE_TERMIO
	ioctl(cur_term->Filedes, TCSETA, &cur_term->Ntermio);
#else
        stty(cur_term->Filedes, &cur_term->Nttyb);
#endif
}


resetterm()
{
#ifdef TRACE
	if (_tracing)
	    _tracef("resetterm() called");
#endif
#ifdef USE_TERMIO
	ioctl(cur_term->Filedes, TCSETA, &cur_term->Otermio);
#else
        stty(cur_term->Filedes, &cur_term->Ottyb);
#endif
}


saveterm()
{
#ifdef TRACE
	if (_tracing)
	    _tracef("saveterm() called");
#endif
#ifdef USE_TERMIO
	ioctl(cur_term->Filedes, TCGETA, &cur_term->Ntermio);
#else
	gtty(cur_term->Filedes, &cur_term->Nttyb);
#endif
}


/*
 *	gettmode()
 *	setterm(type)
 *
 *	These are kept around for backward compatibilty.  gettmode() does
 *	nothing.  setterm() results in a proper call to setupterm()
 *
 */
gettmode()
{
#ifdef TRACE
	if (_tracing)
	    _tracef("gettmode() called");
#endif
}


setterm(type)
char	*type;
{
#ifdef TRACE
	if (_tracing)
	    _tracef("setterm() called");
#endif
    	setupterm(type, 1, 0);
}


/*
 *	erasechar()
 *
 *	Return erase character as given in cur_term->Ottyb.
 *
 */
char
erasechar()
{
#ifdef TRACE
	if (_tracing)
	    _tracef("erasechar() called");
#endif
#ifdef USE_TERMIO
	return(cur_term->Otermio.c_cc[VERASE]);
#else
	return(cur_term->Ottyb.sg_erase);
#endif
}


/*
 *	killchar()
 *
 *	Return kill character as given in cur_term->Ottyb.
 *
 */
char
killchar()
{
#ifdef TRACE
	if (_tracing)
	    _tracef("killchar() called");
#endif
#ifdef USE_TERMIO
	return(cur_term->Otermio.c_cc[VKILL]);
#else
	return(cur_term->Ottyb.sg_kill);
#endif
}


/*
 *	flushinp()
 *
 *	Flush any input on cur_term->Filedes
 *
 */
flushinp()
{
#ifdef TRACE
	if (_tracing)
	    _tracef("flushinp() called");
#endif
#ifdef USE_TERMIO
        ioctl(cur_term->Filedes, TCFLSH, 0);
#else
        ioctl(cur_term->Filedes, TIOCFLUSH, 0);
#endif
    
        if (SP)
	    SP->_backcnt = 0;
}


/*
 *	int
 *	baudrate()
 *
 *	Returns the current terminal's baud rate.
 *
 */
static int speeds[] =
{
	0, 50, 75, 110, 134, 150, 200, 300, 600,
	1200, 1800, 2400, 4800, 9600, 19200, 38400
};

int
baudrate()
{
	int speed;

#ifdef TRACE
	if (_tracing)
	    _tracef("baudrate() called");
#endif
#ifdef USE_TERMIO
	speed = cur_term->Ntermio.c_cflag & CBAUD;
#else
	speed = cur_term->Nttyb.sg_ospeed;
#endif
	return speeds[speed];
}


/*
**	savetty()  and  resetty()
**
**	Kept around for compatibility.
**	
*/

#ifdef USE_TERMIO
static struct termio	myTermio;
#else
static struct sgttyb	sgbuf;
#endif

savetty()
{
#ifdef TRACE
	if (_tracing)
	    _tracef("savetty() called");
#endif

#ifdef USE_TERMIO
	ioctl(cur_term->Filedes, TCGETA, &myTermio);
#else
	gtty(cur_term->Filedes, &sgbuf);
#endif
}


resetty()
{
#ifdef TRACE
	if (_tracing)
	    _tracef("resetty() called");
#endif

#ifdef USE_TERMIO
	ioctl(cur_term->Filedes, TCSETA, &myTermio);
#else
	stty(cur_term->Filedes, &sgbuf);
#endif
}
