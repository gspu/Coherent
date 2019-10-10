/* map_command.c
 * This will pull a map entry from a specified file and build the 
 * necessary command to download the map from mwcbbs.
*/

#include <stdio.h>
#include <curses.h> 
#include "contents.h"
#include "maillist.h"

void map_command (win2,row, col, screen_num)
WINDOW *win2;
int row, col, screen_num;
{
FILE *infp;
char mapcommand[78];
char choice = '\0';

	if ((infp=fopen(workfile,"r"))==NULL)
		{
		noraw();
		endwin();
		printf("Error opening file: %s for input!\n",workfile);
		exit(1);
		}

	fseek(infp,MAP_FORMULA,0l);
	fread(&map_rec,sizeof(struct map),1,infp);

	strcpy(mapcommand,HOST);
	strcat(mapcommand, map_rec.path);
	strcat(mapcommand, recdir);
	
	wclear(win2);
	wmove(win2,0,1);
	wprintw(win2,"The following command will be used to download %s",
		map_rec.name);
	wmove(win2,5,1);
	waddstr(win2,mapcommand);
	
	wmove(win2,17,1);
	waddstr(win2,"Press [y] to download this file");
	wmove(win2,18,1);
	waddstr(win2,"or any other key to cancel.");
	wrefresh(win2);

	while (choice == '\0')
		choice = wgetch(win2);

	if((choice == 'y') || (choice == 'Y'))
		system(mapcommand);
	wclear(win2);
	wrefresh(win2);

}
