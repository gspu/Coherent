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

	/* print our file choices. If printflag is set, then we only print
	 * the first 4 choices, the Contents files, since they are the only
	 * ones that can be printed.
	 */

	for (x=8;x<15;x++)
		{
		if((printflag==1) && (x == 12))
			x = 14;
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
	
	/* if printflag is set, then we test for a more narrow 
	 * range of files since only the Contents files (selections
	 * 0 thru 3) can be printed. Only these filenames should have 
	 * been printed by the function to print selections, anyways.
	*/

	if(printflag != 1)
		{
		x = 0;
		while( x<48 || x>54 )
			x = getch();
		}
	else
		{
		x = 0;
		while ( x<48 || x>51 )
			{
			x = getch();
			if (x == 54)
				break;
			}
		}

	/* if option 5 was selected, display a menu of net maps */

	if( (x-48)==5){
		get_net_map();
		return ;
	}

	/* if option 6 (QUIT) was selected, copy that to our workfile.
	 * otherwise, we are going to get a real file, so append
	 * the directory path specified in the .mwcbbs file, if one
	 * was specified as DATAPATH.
	 */

	if ( (x-48) == 6){
		strcpy(workfile,tryfile[x-48]);
	}else{
		strcpy(workfile,find_workfile);
		strcat(workfile,tryfile[x-48]);
	}

	echo();	
	if (strcmp(workfile,"QUIT")!= 0)
		{
		move(23,0);
		printw("Reading file %s ...",workfile);
		clrtoeol();
		refresh();
		}
	clear();
	refresh();
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
	while( (choice > 51) || (choice <48) )
		choice = getch();

	if ( (choice-48) == 3){
		strcpy(workfile,mapfile[choice-48]);
	}else{
		strcpy(workfile,find_workfile);
		strcat(workfile,mapfile[choice-48]);
	}


}
