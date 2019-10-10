/* this function will get the input and write it to a file. This will be
 * used to get input from a formatted window and create a new file entry
 * in the specified workfile.
*/

#include <stdio.h>
#include <curses.h>
#include "contents.h"
#include "maillist.h"

void add_rec(win2, row, col, screen_num)
WINDOW *win2;

{

FILE *infp;
WINDOW *win3;
int x;
char choice;


	wrefresh(win2);

	if( (win3=newwin(4,79,20,0)) == NULL )
	{
		wclear(win2);
		wmove(win2,0,0);
		wstandout(win2);
		waddstr(win2,"\007Error allocating memory for win3. Press <RETURN>");
		wrefresh(win2);
		while('\n' != getch())
			;
		exit(1);
	}

	wclear(win3);
	wmove(win3,1,1);
	waddstr(win3,"Enter filename or <RETURN> to exit.");
	wrefresh(win3);
	getstring(win2, NAMEHI);
	strcpy(new_record.filename, workstring);
	if(strlen(new_record.filename) == 0)
		return;

	for(x=strlen(workstring);x < sizeof (new_record.filename) ; x++)
		new_record.filename[x] = '\0';


	wclear(win3);
	wmove(win3,1,1);
	waddstr(win3,"Describe the uses of this package");
	wrefresh(win3);

	getstring(win2, DESCHI);
	strcpy(new_record.description, workstring);

	for(x=strlen(workstring);x < sizeof (new_record.description) ; x++)
		new_record.description[x] = '\0';



	wclear(win3);
	wmove(win3,1,1);
	waddstr(win3,"Enter the date that the file was added or last modified");
	wmove(win3,2,1);
	waddstr(win3,"Enter no more than 6 characters");
	wrefresh(win3);

	getstring(win2,DATEHI);
	strcpy(new_record.date, workstring);
	for(x=strlen(workstring);x < sizeof (new_record.date) ; x++)
		new_record.date[x] = '\0';

	wclear(win3);
	wmove(win3,1,1);
	waddstr(win3,"Enter the file size in number of bytes");
	wrefresh(win3);
	getstring(win2, SIZEHI);
	strcpy(new_record.filesize, workstring);
	for(x=strlen(workstring);x < sizeof (new_record.filesize) ; x++)
		new_record.filesize[x] = '\0';


	wclear(win3);
	wmove(win3,3,1);
	waddstr(win3,"Enter the number of parts that this file is divided into");
	wrefresh(win3);
	getstring(win2, PARTHI);
	new_record.noparts = atoi(workstring);


	/* if the number of parts is 0 (not a split file), then we can
	 * cat the filename on to the end of the pathname. We can later
	 * test the length of the pathname to determine whether or not
	 * to ask for a pathname, as would be required for a split file.
	*/


		if(new_record.noparts ==0)
		{
			strcpy(new_record.pathname, PATHNAME);
			strcat(new_record.pathname, new_record.filename);
			wmove(win2,PATHHI);
			wstandout(win2);
			waddstr(win2, new_record.pathname);
			wstandend(win2);
			wrefresh(win2);
		}


	wclear(win3);
	wmove(win3,3,1);
	waddstr(win3,"Enter the names of any support files required");
	wrefresh(win3);
	getstring(win2, REQHI);

	if(strlen(workstring) == 0 )
		{
		wmove(win2, REQHI);
		wstandout(win2);
		waddstr(win2,"none");
		wstandend(win2);
		wrefresh(win2);
		strcpy(new_record.requires, "none");
	}
	else
		strcpy(new_record.requires, workstring);

	for(x=strlen(new_record.requires);x < sizeof (new_record.requires) ; x++)
		record.requires[x] = '\0';

	wclear(win3);
	wmove(win3,3,1);
	waddstr(win3,"Enter any relevant notes about this file");
	wrefresh(win3);
	getstring(win2, NOTEHI);
	if(strlen(workstring) == 0)
		{
		wmove(win2,NOTEHI);
		wstandout(win2);
		waddstr(win2,"none");
		wstandend(win2);
		wrefresh(win2);
		strcpy(new_record.notes, "none");
		}
	
	else
		strcpy(new_record.notes, workstring);
	for(x=strlen(new_record.notes);x < sizeof (new_record.notes) ; x++)
		new_record.notes[x] = '\0';

	/* if there is more than one part to download, or number of
	 * parts is not zero, we need to complete the pathlist.
	*/
	
	if(new_record.noparts != 0)
	{
		wclear(win3);
		wmove(win3,3,1);
		waddstr(win3,"Complete the pathlist to this file.");
		wrefresh(win3);
		wmove(win2,PATHHI);
		wstandout(win2);
		waddstr(win2, PATHNAME);
		wstandend(win2);
		wrefresh(win2);
		getstring(win2, PATHHI2);
		strcpy(new_record.pathname, PATHNAME);
		strcat(new_record.pathname, workstring);
	}
	for(x=strlen(new_record.pathname);x < sizeof (new_record.pathname) ; x++)
		new_record.pathname[x] = '\0';


	wclear(win3);
	wmove(win3,0,0);
	waddstr(win3,"Do you wish to write the record?");
	wmove(win3,1,0);
	waddstr(win3,"[y]es or any other to abort.");
	wrefresh(win3);
	choice ='\0';
	while(choice == '\0')
		choice = wgetch(win3);

	if(choice == 'y' || choice == 'Y')
		{
		wclear(win3);
		wmove(win3,0,0);
		waddstr(win3,"Updating...");
		wrefresh(win3);
		del_rec(win3,row,col,screen_num);
		}

	delwin(win3);

}


/* this function will get a string and print to the approp field 
 * on win2. This is called by add_rec.
*/

void getstring (win2, row, col)

WINDOW *win2;
int row, col;

{


	noraw();
	echo();
	wmove(win2,row,col);
	wrefresh(win2);
	wgetstr(win2,workstring);
	wstandout(win2);
	wmove(win2,row,col);
	wprintw(win2,"%s",workstring);
	wstandend(win2);
	wrefresh(win2);
	noecho();
	raw();
}
