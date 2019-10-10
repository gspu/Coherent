/*
 * Copyright (c) 1981 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef COHERENT
#ifndef lint
static uchar sccsid[] = "@(#)getch.c	5.5 (Berkeley) 6/30/88";
#endif /* not lint */
#endif /* not COHERENT */

# include	<fcntl.h>
# include	"curses.ext"

/*
 *	This routine reads in a character from the window.
 *
 */
wgetch(win)
reg WINDOW	*win; {

	reg bool	weset = FALSE;
	reg int		inp;
	reg int		fflgs; /* fcntl flags to be restored */
	reg int		fmods; /* fcntl flags were modified */

	if (!win->_scroll && (win->_flags&_FULLWIN)
	    && win->_curx == win->_maxx - 1 && win->_cury == win->_maxy - 1)
		return ERR;
# ifdef DEBUG
	fprintf(outf, "WGETCH: _echoit = %c, _rawmode = %c\n", _echoit ? 'T' : 'F', _rawmode ? 'T' : 'F');
# endif
#ifndef COHERENT
	if (_echoit && !_rawmode) {
		cbreak();
		weset++;
	}

	/*
	 * Obtain the fcntl flags.
	 */
	fflgs = fcntl( fileno(stdin), F_GETFL, 0 );
	fmods = 0;

	/*
	 * Non-blocking input is required, fcntl flags may have to be changed.
	 */
	if ( win->_nodelay ) {
		if ( (fflgs & O_NDELAY) == 0 ) {
			fcntl( fileno(stdin), F_SETFL, fflgs | O_NDELAY );
			fmods++;
		}
	}

	/*
	 * Blocking input is required, fcntl flags may have to be changed.
	 */
	else {
		if ( (fflgs & O_NDELAY) != 0 ) {
			fcntl( fileno(stdin), F_SETFL, fflgs & ~O_NDELAY );
			fmods++;
		}
	}
#endif
	/*
	 * Get next character, or keyboard token.
	 * NOTE: Valid characters are in range 1 to 127.
	 *	 Valid keyboard tokens start at 256 [0400].
	 */
	if ( win->_use_keypad )
		inp = getkey();

	/*
	 * Get next character, without token translation.
	 */
	else
		inp = getchar();

	if ( (inp < 0200) && (inp > 0) ) {
# ifdef DEBUG
		fprintf(outf,"WGETCH got '%s'\n",unctrl(inp));
# endif
		if (_echoit) {
			mvwaddch(curscr, win->_cury + win->_begy,
				win->_curx + win->_begx, inp);
			waddch(win, inp);
		}
	}
# ifdef DEBUG
	else {
		fprintf( outf, "WGETCH got 0%o\n", inp );
	}
#endif

#ifndef COHERENT
	/*
	 * Fcntl flags were modified - restore previous value.
	 */
	if ( fmods )
		fcntl( fileno(stdin), F_SETFL, fflgs );

	if (weset)
		nocbreak();
#endif
	return inp;
}
