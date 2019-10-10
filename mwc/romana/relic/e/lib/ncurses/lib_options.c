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
**	lib_options.c
**
**	The routines to handle option setting.
**
** $Log:	RCS/lib_options.v $
 * Revision 2.4  92/11/08  15:41:44  munk
 * Correct definition of malloc() and
 * usage of the cast operator
 *
 * Revision 2.3  91/12/03  21:51:11  munk
 * Added KEY_LL in function init_keytry()
 *
 * Revision 2.2  91/04/20  21:29:33  munk
 * Usage of register variables
 *
 * Revision 2.1  82/10/25  14:48:24  pavel
 * Added Copyright Notice
 * 
 * Revision 2.0  82/10/25  13:47:45  pavel
 * Beta-one Test Release
 * 
**
*/

#ifdef RCSHDR
static char RCSid[] =
	"$Header:   RCS/lib_options.v  Revision 2.2  91/04/20  21:29:33  munk   Exp$";
#endif

#include "term.h"
#include "curses.h"
#include "curses.priv.h"

char	*malloc();


static
outc(ch)
char	ch;
{
    	putc(ch, SP->_ofp);
}


idlok(win, flag)
WINDOW	*win;
int	flag;
{
#ifdef TRACE
	if (_tracing)
	    _tracef("idlok(%o,%d) called", win, flag);
#endif

    	if ((insert_line  &&  delete_line)
#ifdef UNIMPLEMENTED
	     ||  (change_scroll_region)
#endif
	   )
	    curscr->_idlok = flag;
}


clearok(win, flag)
WINDOW	*win;
int	flag;
{
#ifdef TRACE
	if (_tracing)
	    _tracef("clearok(%o,%d) called", win, flag);
#endif

    	if (win == curscr)
	    newscr->_clear = flag;
	else
	    win->_clear = flag;
}


leaveok(win, flag)
WINDOW	*win;
int	flag;
{
#ifdef TRACE
	if (_tracing)
	    _tracef("leaveok(%o,%d) called", win, flag);
#endif

    	win->_leave = flag;
}


scrollok(win, flag)
WINDOW	*win;
int	flag;
{
#ifdef TRACE
	if (_tracing)
	    _tracef("scrollok(%o,%d) called", win, flag);
#endif

    	win->_scroll = flag;
}


nodelay(win, flag)
WINDOW	*win;
int	flag;
{
#ifdef TRACE
	if (_tracing)
	    _tracef("nodelay(%o,%d) called", win, flag);
#endif

    	win->_nodelay = flag;
}


keypad(win, flag)
WINDOW	*win;
int	flag;
{
#ifdef TRACE
	if (_tracing)
	    _tracef("keypad(%o,%d) called", win, flag);
#endif

    	win->_use_keypad = flag;

	if (flag  &&  keypad_xmit)
	    tputs(keypad_xmit, 1, outc);
	else if (! flag  &&  keypad_local)
	    tputs(keypad_local, 1, outc);
	    
        if (SP->_keytry == UNINITIALISED)
	    init_keytry();
}


meta(win, flag)
WINDOW	*win;
int	flag;
{
#ifdef TRACE
	if (_tracing)
	    _tracef("meta(%o,%d) called", win, flag);
#endif

	win->_use_meta = flag;

	if (flag  &&  meta_on)
	    tputs(meta_on, 1, outc);
	else if (! flag  &&  meta_off)
	    tputs(meta_off, 1, outc);
}


/*
**      init_keytry()
**
**      Construct the try for the current terminal's keypad keys.
**
*/

static struct  try *newtry;

static
init_keytry()
{
        newtry = NULL;
	
        add_to_try(key_backspace, KEY_BACKSPACE);
        add_to_try(key_catab, KEY_CATAB);
        add_to_try(key_clear, KEY_CLEAR);
        add_to_try(key_ctab, KEY_CTAB);
        add_to_try(key_dc, KEY_DC);
        add_to_try(key_dl, KEY_DL);
        add_to_try(key_down, KEY_DOWN);
        add_to_try(key_eic, KEY_EIC);
        add_to_try(key_eol, KEY_EOL);
        add_to_try(key_eos, KEY_EOS);
        add_to_try(key_f0, KEY_F(0));
        add_to_try(key_f1, KEY_F(1));
        add_to_try(key_f2, KEY_F(2));
        add_to_try(key_f3, KEY_F(3));
        add_to_try(key_f4, KEY_F(4));
        add_to_try(key_f5, KEY_F(5));
        add_to_try(key_f6, KEY_F(6));
        add_to_try(key_f7, KEY_F(7));
        add_to_try(key_f8, KEY_F(8));
        add_to_try(key_f9, KEY_F(9));
        add_to_try(key_f10, KEY_F(10));
        add_to_try(key_home, KEY_HOME);
	add_to_try(key_ll, KEY_LL);
        add_to_try(key_ic, KEY_IC);
        add_to_try(key_il, KEY_IL);
        add_to_try(key_left, KEY_LEFT);
        add_to_try(key_npage, KEY_NPAGE);
        add_to_try(key_ppage, KEY_PPAGE);
        add_to_try(key_right, KEY_RIGHT);
        add_to_try(key_sf, KEY_SF);
        add_to_try(key_sr, KEY_SR);
        add_to_try(key_stab, KEY_STAB);
        add_to_try(key_up, KEY_UP);
	
	SP->_keytry = newtry;
}


add_to_try(str, code)
register char	*str;
short		code;
{
	void			free();
        static bool		out_of_memory = FALSE;
        register struct try	*ptr, *savedptr;

	if (! str  ||  out_of_memory)
	    return;
	
	if (newtry != NULL)    
	{
	    ptr = newtry;
	    
       	    for (;;)
	    {
	        while (ptr->ch != *str  &&  ptr->sibling != NULL)
	            ptr = ptr->sibling;
	    
	        if (ptr->ch == *str)
		{
		    if (*(++str))
		    {
	                if (ptr->child != NULL)
		            ptr = ptr->child;
                        else
		            break;
		    }
		    else
		    {
		        ptr->value = code;
			return;
		    }
		}
		else
	        {
		    if ((ptr->sibling = (struct try *) malloc(sizeof *ptr)) == NULL)
		    {
		        out_of_memory = TRUE;
			return;
		    }
		    
		    savedptr = ptr = ptr->sibling;
		    ptr->child = ptr->sibling = NULL;
		    ptr->ch = *str++;
		    ptr->value = 0;
		    
                    break;
	        }
	    } /* end for (;;) */  
	}
	else    /* newtry == NULL :: First sequence to be added */
	{
	    savedptr = ptr = newtry = (struct try *) malloc(sizeof *ptr);
	    
	    if (ptr == NULL)
	    {
	        out_of_memory = TRUE;
		return;
	    }
	    
	    ptr->child = ptr->sibling = NULL;
	    ptr->ch = *(str++);
	    ptr->value = 0;
	}
	
	    /* at this point, we are adding to the try.  ptr->child == NULL */
	    
	while (*str)
	{
	    ptr->child = (struct try *) malloc(sizeof *ptr);
	    
	    ptr = ptr->child;
	    
	    if (ptr == NULL)
	    {
	        out_of_memory = TRUE;
		
		ptr = savedptr;
		while (ptr != NULL) 
		{
		    savedptr = ptr->child;
		    free(ptr);
		    ptr = savedptr;
		}
		
		return;
	    }
	    
	    ptr->child = ptr->sibling = NULL;
	    ptr->ch = *(str++);
	    ptr->value = 0;
	}
	
	ptr->value = code;
	return;
}
