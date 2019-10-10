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
**	lib_getch.c
**
**	The routine getch().
**
** $Log:	lib_getch.c,v $
 * Revision 1.9  93/04/12  14:13:39  bin
 * Udo: third color update
 * 
 * Revision 2.3  92/11/21  14:47:37  munk
 * Improved alarm signal handling
 *
 * Revision 1.3  92/06/10  14:17:06  bin
 * changed ref to sys/fcntl.h
 * 
 * Revision 1.2  92/04/13  14:37:30  bin
 * update by vlad
 * 
 * Revision 2.2  91/02/09  13:37:40  munk   
 * Conditional code added for systems that don't support ioctl with
 * the command FIONREAD (use fcntl O_NDELAY).
 * Bug fixed in kgetch(), do not put EOF in buffer.
 * Save the old alarm-timer and restart it, after use by kgetch().
 *
 * Revision 2.1  82/10/25  14:47:29  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/25  13:45:19  pavel
 * Beta-one Test Release
 * 
**
*/

#ifdef RCSHDR
static char RCSid[] =
	"$Header: /src386/usr/lib/ncurses/RCS/lib_getch.c,v 1.9 93/04/12 14:13:39 bin Exp Locker: bin $";
#endif

#include <signal.h>
#ifdef COHERENT
#include <fcntl.h>
#else
#include <fcntl.h>
#endif
#include "curses.h"
#include "curses.priv.h"

#define nextc()       (SP->_backcnt > 0  ?  SP->_backbuf[--SP->_backcnt] \
                                         :  getc(SP->_ifp))
				       
#define putback(ch)   SP->_backbuf[SP->_backcnt++] = ch


wgetch(win)
register WINDOW	*win;
{
	bool		setHere = FALSE; /* cbreak mode was set here         */
	register int	ch;		 /* 'int' because of keypad codes  */

#ifdef TRACE
	if (_tracing)
	    _tracef("wgetch(%o) called flags = 0%o", win, win->_flags);
#endif

	if (! win->_scroll  &&  (win->_flags & _FULLWIN)
						&&  win->_curx == win->_maxx
						&&  win->_cury == win->_maxy)
	    return(ERR);

#ifdef FIONREAD
	if (win->_nodelay)
	{
	    long count;

	    ioctl(fileno(SP->_ifp), FIONREAD, &count);

	    if (! count)
		return(-1);
	}
#else
	if (win->_nodelay)
	{
		int flags;

		flags = fcntl(fileno(SP->_ifp), F_GETFL, 0);
		fcntl(fileno(SP->_ifp), F_SETFL, flags | O_NDELAY);
		ch = fgetc(SP->_ifp);
		fcntl(fileno(SP->_ifp), F_SETFL, flags);
		if (ch == -1)
			return(-1);
		ungetc(ch, SP->_ifp);
	}
#endif

	if (SP->_echo  &&  ! (SP->_raw  ||  SP->_cbreak))
	{
		cbreak();
		setHere = TRUE;
	}

        if (win->_use_keypad)
            ch = kgetch();
        else {
	    while ((ch = nextc()) == EOF)	/* ignore interrupted */
	    	;				/* system call        */
	}

	if (SP->_echo  &&  ch < 0400 && ch >= 0)/* ch < 0400 => not a keypad key */
	{
	    mvwaddch(curscr, win->_begy + win->_cury,
                             win->_begx + win->_curx, ch | win->_attrs);
	    waddch(win, ch | win->_attrs);
	}

	if (setHere) {
	    nocbreak();
	 }

	return(ch);
}


/*
**      kgetch()
**
**      Get an input character, but take care of keypad sequences, returning
**      an appropriate code when one matches the input.  After each character
**      is received, set a one-second alarm call.  If no more of the sequence
**      is received by the time the alarm goes off, pass through the sequence
**      gotten so far.
**
*/

static  bool    alarmed;

static
int
kgetch()
{
        struct try	*ptr;
	register int	ch;
	char		buffer[10]; /* Assume no sequences longer than 10 */
	register char	*bufp = buffer;
	int		(*oldsig)();
	int		oldalarm;
	int		sigalrm();

	ptr = SP->_keytry;
	
	oldalarm = alarm(0);
	oldsig = signal(SIGALRM, sigalrm);
	signal(SIGALRM, oldsig);
	alarm(oldalarm);
	alarmed = FALSE;
	
	do
	{
    readagain:
	    ch = nextc();
	    if (ch != EOF)		/* do not put EOF in buffer */
	        *(bufp++) = ch;
	    else if (alarmed)		/* handle interrupted system */
	        break;			/* call properly             */
	    else
	    	goto readagain;
	    
	    while (ptr != NULL  &&  ptr->ch != ch)
	        ptr = ptr->sibling;
	    
	    if (ptr != NULL)
	    {
	        if (ptr->value != 0)
		{
		    signal(SIGALRM, oldsig);
		    alarm(oldalarm);
		    return(ptr->value);
		}
		else
		{
		    ptr = ptr->child;
		    signal(SIGALRM, sigalrm);
		    alarm(1);
	        }
	    }
	    
	} while (ptr != NULL);
	
	signal(SIGALRM, oldsig);
	alarm(oldalarm);
	
	while (--bufp > buffer)
	    putback(*bufp);
	    
	return(*bufp);
}


static
sigalrm()
{
        alarmed = TRUE;
	signal(SIGALRM, sigalrm);
}
