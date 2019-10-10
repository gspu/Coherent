/*
 *	Under SCO UNIX release 3.2 version 2.0 the file
 *	/usr/include/sys/ptem.h is so buggy that it crashes
 *	the compiler :-(
 *	The struct winsize is extracted from this file.
 *
 *	09-DEC-1991 Udo Munk
 */

/*
 * Windowing structure to support JWINSIZE/TIOCSWINSZ/TIOCGWINSZ 
 */
struct winsize {
	unsigned short ws_row;       /* rows, in characters*/
	unsigned short ws_col;       /* columns, in character */
	unsigned short ws_xpixel;    /* horizontal size, pixels */
	unsigned short ws_ypixel;    /* vertical size, pixels */
};
