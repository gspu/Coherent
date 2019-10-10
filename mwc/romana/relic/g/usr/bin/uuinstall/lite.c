/* lite.c: this is used to read a character array from a curses
 * window, delimited by whitespace. The character array is then
 * reprinted to the window in standout mode, if the appropriate
 * flag is set.
 */

#include "uuinstall.h"

lite(win, row, col, flag)
WINDOW *win;
int row, col ,flag;
{
	int x,y;

	strcpy(litestring,"");

	/* now read char by char until we hit a whitespace, building
	 * our character array to be reprinted as we go along.
	 */

	wmove(win,row,col);
	for(x = 0 ; x < 15 ; x++){
		litestring[x] = winch(win);  /* build our char array */

		if(isspace(litestring[x])){	/* terminate string when */
			litestring[x] = '\0';	/* we hit a space and    */
			break;			/* terminate loop.	 */
		}

		wdelch(win);
	}


	/* now to repad this line with spaces, since wdel()ing will have
	 * skewed our columns.
	 */

	for(y = 0 ; y < x; y++)
		winsch(win, ' ');


	/* now to reprint character array to window, check the flag to
	 * see if it should be printed in standout mode.
	 */

	if(flag)
		wstandout(win);

	waddstr(win, litestring);
	wmove(win,row,col);

	if(flag)
		wstandend(win);

	return;
}
