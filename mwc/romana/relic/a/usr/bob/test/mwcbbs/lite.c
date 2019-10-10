/* this routine will take a pointer to a window and row/col
 * coordinates and print a filename found at the coordinates
 * in INVERSE video.
*/

#include <stdio.h>
#include <curses.h>
#include "contents.h"
#include "maillist.h"

int lite(win1, row, col, liteflag, main_menu)

WINDOW *win1;
int row, col, liteflag, main_menu;

{

int x,y;
char litestring[15];

	if(main_menu==1)
		col = 0;

	wmove(win1, row,col);

	/* get existing filename char by char, deleting each
	 * char as it is read into a string variable.
	*/
	
	for(x=0; x<15; x++)
	{
		litestring[x] = winch(win1);

	/* if we hit a blank, stop */	
		if(litestring[x] == ' ')
			break;

		wdelch(win1);
	}

	litestring[x] = '\0';

	/* we need to test what Litestring is, in case we're picking
	 * up a 2 name state (ie. West Virginia). If we are, then we
	 * continue to read the rest of the state name to get the
	 * full, proper name.
	*/

	if( (strcmp(litestring,"New")==0) || (strcmp(litestring,"North")==0)|| (strcmp(litestring,"South")==0) || (strcmp(litestring,"West")==0) || (strcmp(litestring,"Rhode")==0) )
		{
		wdelch(win1);
		litestring[x] = ' ';
		for(y=x+1;y<15;y++)
			{
			litestring[y] = winch(win1);
			if (litestring[y] == ' ')	
				break;
			wdelch(win1);
			}
		litestring[y] = '\0';
		x = y;
		}

	/* repad the spaces to keep the remaning filenames
	 * on this row in their proper columns.
	*/

/*	if(main_menu != 1)  */
		for(y=0;y<x;y++)
			winsch(win1,' ');

	if (liteflag == 1)
		wstandout(win1);

	waddstr(win1, litestring);
	wmove(win1, row, col);

	if (liteflag == 1)
		wstandend(win1);


	/* copy filename to a string to be displayed on stdscr.
	 * as a filename is highlited, it is also displayed on
	 * stdscr. 'selection' will be used in case the user
	 * hits return to locate the filename's entry in the
	 * Contents file.
	*/

	/* if we were called from the main menu functions, then we need to
	 * copy this to the 'workfile' string, not the selection string.
	*/

	if(main_menu==1)
		strcpy(workfile,litestring);
	else
		strcpy(selection, litestring);


	move (23,0);

	if(main_menu==1)
		printw("Selected filename is: %14s", workfile);
	else
		printw("Selected filename is: %14s",selection);


	refresh();
	wrefresh(win1);

	return(0);

}
