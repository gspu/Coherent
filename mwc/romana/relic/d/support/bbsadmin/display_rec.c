/* this function will open the file and read the appropriate record,
 * then display it on the window in the approp. positions.
*/

#include <stdio.h>
#include <curses.h>
#include "contents.h"



void display_record (win2, row, col, screen_num)
WINDOW *win2;
int row, col, screen_num;

{
WINDOW *win3;
char choice;
FILE *infp;
int x;

	if ((infp = fopen(workfile,"r")) == NULL)
	{
		printf("\007ERROR opening file for input!\n");
		exit(1);
	}

	fseek(infp,REC_FORMULA, 0l);
	fread(&record, sizeof (struct entry),1,infp);
	fclose (infp);

	wstandout(win2);

	wmove(win2,NAMEHI);
	waddstr(win2,record.filename);

	wmove(win2,DESCHI);
	waddstr(win2,record.description);

	wmove(win2,DATEHI);
	waddch(win2,record.date[0]);
	waddch(win2,record.date[1]);
	waddch(win2,'/');
	waddch(win2,record.date[2]);
	waddch(win2,record.date[3]);
	waddch(win2,'/');
	waddch(win2,record.date[0]);
	waddch(win2,record.date[1]);

	
	wmove(win2,SIZEHI);
	waddstr(win2,record.filesize);

	if(open_mode != 'd')
		{
		wmove(win2,PARTHI);
		wprintw(win2,"%.2d",record.noparts);
		}

	wmove(win2,REQHI);
	waddstr(win2,record.requires);

	wmove(win2,NOTEHI);
	waddstr(win2,record.notes);

	if(open_mode != 'd')
		{
		wmove(win2,PATHHI);
		waddstr(win2,record.pathname);
		}

	wrefresh(win2);
	wstandend(win2);


/* allocate another window as a message area */

	if ( (win3=newwin(2,40,20,0)) == NULL)
		{
		printf("Memory allocation for win3 failed!\n");
		exit(1);
		}

/* the following tests for 'd' for delete from command line. If the user
 * used a 'd' on the command line, make sure that he/she really wants to
 * delete the record, then call the del_reocrd function.
*/
	if(open_mode == 'r')
		{
		wmove (win3,0,0);
		waddstr(win3,"Do you wish to delete this record?");
		wmove(win3,1,0);
		waddstr(win3, "[y] yes; any other key to abort.");
		wrefresh(win3);

		choice = '\0';
		while (choice == '\0')
			choice = wgetch(win3);

		if ((choice == 'y') || (choice == 'Y'))
			{
			wclear(win3);
			wmove(win3,0,0);
			waddstr(win3,"working... please wait");
			wrefresh(win3);
			del_rec(win3, row, col, screen_num);
			}
		
		wclear(win3);
		wmove(win3,0,0);
		waddstr(win3,"Press RETURN to continue...");
		wrefresh(win3);
		while(13 != wgetch(win3))
			;
		wclear(win3);
		wrefresh(win3);
		delwin(win3);
		}

	/* we are in download mode, so prompt for the next data screen */
	else
		{
		wclear(win3);
		wmove(win3,0,0);
		waddstr(win3,"Press <RETURN> for next screen.");
		wrefresh(win3);	
		while(13 != wgetch(win3))
			;
		wclear(win2);
		wmove(win2,1,0);
		waddstr(win2,"Size of file is: ");
		wmove(win2,1,40);
		waddstr(win2,"Number of parts to download: ");
		wstandout(win2);
		wmove(win2,1,18);
		waddstr(win2,record.filesize);
		wmove(win2,1,69);
		record.noparts = (record.noparts == 0) ? 1 : record.noparts;
		wprintw(win2,"%d",record.noparts);
		wstandend(win2);
		wmove(win2,3,5);
		wprintw(win2,"The following commands will be needed to download ");
		wstandout(win2);
		waddstr(win2,record.filename);
		wstandend(win2);

	/* if there is more than one part to download, call a function to generate
	 * the multiple uucp requests necessary to grab each piece from mwcbbs.
	*/

		if (record.noparts >1)
			{
			build_uucp(record);
			for (x=0;x<record.noparts;x++)
				{

	/* limit ourselves to 10 displayed commands */
				if (x==10)
					{
					wmove(win2,16,0);
					wprintw(win2,"There are %d more parts to download which do not appear on ths screen.", (record.noparts - x));
					break;
					}

				wmove(win2,5+x,0);
				waddstr(win2,getfiles[x]);
				}
			}
		else
			{
			strcpy(getfiles[0],HOST);
			strcat(getfiles[0],record.pathname);
			strcat(getfiles[0],RECEIVER);
			wmove(win2,5,0);
			waddstr(win2,getfiles);
			wrefresh(win2);
			}

		wclear(win3);
		wmove(win3,0,0);
		waddstr(win3,"Do you wish to download this file?");
		wmove(win3,1,0);
		waddstr(win3,"[y] yes or any other key to abort.");
		wrefresh(win2);
		wrefresh(win3);
		choice = '\0';
		while(choice == '\0')
			choice = wgetch(win3);

		wclear(win3);
		wrefresh(win3);

		if(choice == 'y' || choice == 'Y')
			{	
			wclear(win3);
			wmove(win3,0,0);
			waddstr(win3,"Processing requests...");
			wrefresh(win3);
			for(x = 0; x < record.noparts; x++)
				system(getfiles[x]);
			wclear(win3);
			wmove(win3,0,0);
			waddstr(win3,"Press <RETURN> to continue.");
			wrefresh(win3);
			while(13 != wgetch(win3))
				;
			wclear(win3);
			wrefresh(win3);
			}
		}
	delwin(win3);
}

