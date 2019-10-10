/* getfilename() This will print the list of available files to 
 * stdscr and allow the user to choose one of them for further processing.
*/

#include <stdio.h>
#include <curses.h>
#include "contents.h"
#include "maillist.h"

void getfilename()
{
int x;
char tryfile[7][15] = {FILE0,FILE1,FILE2,FILE3,FILE4,FILE5,FILE6};

	clear();
	refresh();
	noecho();

	/* print our file choices */

	for (x=8;x<15;x++)
		{
		move(x,32);
		printw("%d. %s",(x-8),tryfile[x-8]);
		}

	move(23,0);
	printw("Press the number corresponding to the desired file/option.");
	refresh();

	/* while the returned value of getch is less than 0 or greater
	 * than 6, keep getting a keystroke. This is how we will select
	 * our file to use for further operations.
	*/

	x = 0;
	while( x<48 || x>54 )
		x = getch();

	if( (x-48)==5)
		get_net_map();
	else
		strcpy(workfile,tryfile[x-48]);

	echo();	
	if (strcmp(workfile,"QUIT")!= 0)
		{
		move(23,0);
		printw("Reading file %s ...",workfile);
		clrtoeol();
		refresh();
		}
}


void get_net_map()

{
int x;
char choice;

	clear();
	


	for (x=11;x<15;x++)
		{
		move(x,32);
		printw("%d. %s",(x-11),mapfile[x-11]);
		}
	move(23,0);
	printw("Press the number of the map file to read.");
	refresh();

	choice = 0;
	while( (choice < 48) || (x > 51) )
		choice = getch();

	strcpy(workfile,mapfile[choice - 48]);

}
