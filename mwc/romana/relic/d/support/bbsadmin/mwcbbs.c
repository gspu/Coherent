#include "contents.h"
#include <stdio.h>
#include <curses.h>

char selection[15];
char filenames [MAXRECORDS][15];
char workfile[15];
char workstring[80];
char getfiles[26][115];
char open_mode;
int place[MAXRECORDS];
int limit, screen_num;

void show_files(); /* this should display the filenames on a curses screen */
int lite(); /* inverse/normal video display of a filename */
int rfile(); /* read records from a given file */
void write_win(); /*does the actual work of writing filenames to a window */
void display_form(); /* for for displaying selected filename */
void display_record(); /* display selected filename */
void menu(); /* menu printed at bottom of screen */
void build_uucp(); /* this will build multiple uucp requests */

struct entry{
		char filename [15];
		char filesize [10];
		char date[7];
		char description [78];
		char requires [60];
		char notes [78];
		char pathname [60];
		int noparts;
	    }


main(argc, argv)
int argc;
char *argv[];
{

int x;


	if(argc < 1)
	{
		printf("Usage: mwcbbs filename\n");
		exit(1);
	}


	if(strlen(argv[1]) == 0)
	{
		printf("filename not specified. Please enter a filename: ");
		gets (workfile);
	}
	else

		strcpy(workfile, argv[1]);

	open_mode = 'd';

	screen_num = 0;

	x = rfile();
	show_files  (x);

}




/*	show_files()
 *	This function will display the filenames read to a curses
 *	screen.
*/


void show_files(EOF_FLAG)
int EOF_FLAG;

{
char arrow;
int prevcol =1;
int prevrow =0;			/* prevcol = column before arrow	  */
int newrow =0;			/* prevrow = row before arrow	 	  */
int newcol =1;  		/* newrow = row after arrow		  */
int counter = 0;		/* newcol = column after arrow		  */
				
				
WINDOW *win1, *win2;		
				
	initscr();		
	noecho();
	raw();

	/* allocate memory for window. print message on failure. */

	if((win1=newwin(20, 79, 0,0)) == NULL)
	{
		printf("\007Window Memroy allocation for win1 failed!\n");
		exit(1);
	}

	if((win2=newwin(20, 79, 0,0)) == NULL)
	{
		printf("\007Window Memroy allocation for win2 failed!\n");
		exit(1);
	}

	write_win(win1);
	menu();


/*  highlite a filename. This is accomplished by going to a designated
 * row and column, as determined by the row and counter nested loops.
 * The innermost loop gets the character found, copies the retrieved 
 * character into a string and deletes the character from the screen.
 * When the filename has been deleted from the screen, it is reprinted
 * to the screen with highliting turned on. Padding for spaces must be
 * accounted for since deleting chars shifts everything on the line one
 * space to the left.
*/

	/* print the first file in inverse video */

	lite (win1, prevrow, prevcol, 1);

	do
	{

	/* now we need to get a key (preferably an arrow) */



	      arrow = getch();   /* This stupid code should allow to use arrows keys   */
	      if (arrow == 27)  /* that looks more frendly than hjkl. Vlad 8/15/91    */
		{
	         getch();
	         arrow = getch(); /* When an arrow key is pressed, an escape  */
	         if (arrow == 68) /* sequence is returned. The value '27'     */
	            arrow = 'h';  /* begins the sequence and the relevant     */
	         if (arrow == 67) /* values needed end the sequence. The      */
        	    arrow = 'l';  /* middle value is not needed, so it is     */
         	 if (arrow == 66) /* skipped over with a getch() statement    */
         	   arrow = 'j';
	         if (arrow == 65)
        	    arrow = 'k';            	
      	         }


/* each movement case in the following switch...case will test to see if the
 * new position returns a space. If a space is returned, then we will hit
 * an empty field, which we don't want to do. If we hit an empty space,
 * then don't move the cursor.
*/

		switch(arrow)
		{
			case 'h':	/* move left */
				newcol = prevcol - 15;
				if (newcol < 1)
					newcol = 61;
				if (' ' == mvwinch(win1,newrow,newcol))
					newcol = 1;
				break;


			case 'l':	/* move right */
				newcol = prevcol + 15;
				if (newcol > 61)
					newcol = 1;
				if (' ' == mvwinch(win1,newrow,newcol))
					newcol = 1;
				break;


			case 'j':	/* move down */
				newrow = prevrow + 1;
				if (newrow == 20)
					newrow = 0;
				if (' ' == mvwinch(win1,newrow,newcol))
					newrow = 0;
				break;


			case 'k':	/* move up */
				newrow = prevrow -1;
				if (newrow == -1)
					newrow = 19;
				if (' ' == mvwinch(win1,newrow,newcol))
					newrow = 0;
				break;


			case 'p':	
				screen_num --;
				newrow = 0;
				newcol = 1;
				if (screen_num < 0)
					screen_num = 0;
				else
				{
					EOF_FLAG = rfile();
					write_win(win1);
				}
				break;
	

			case 'n':	
				newrow = 0;
				newcol = 1;
				if (EOF_FLAG == -1)
					break;
				else
				{
					screen_num ++;
					EOF_FLAG = rfile();
					write_win(win1);
				}
				break;	


			case 'q':
				break;

			case 'Q':
				break;

			case 'S':
			case 's':
				wclear(win1);
				wrefresh(win1);
				wclear(win2);
				display_form(win2);
				display_record(win2,prevrow, prevcol, screen_num);
				clear();
				menu();
				write_win(win1);
				wclear(win2);
				wrefresh(win2);
				refresh();
				wrefresh(win1);
				break;

			default:
				newcol = prevcol;
				newrow = prevrow;
				break;

		}

	/* print previous file highlited in normal video */
	lite (win1, prevrow, prevcol, 0);	

	/* print new filename selection in inverse video */

	lite (win1, newrow, newcol, 1);

	/* set our previous coordinates so that we can go back
	 * and unlite a field when the next directin is given.
	*/

		prevrow = newrow;
		prevcol = newcol;

	
	}
	while (arrow != 'q');

	echo();
	noraw();
	endwin();
}
		

		

/* this routine will take a pointer to a window and row/col
 * coordinates and print a filename found at the coordinates
 * in INVERSE video.
*/

int lite(win1, row, col, liteflag)

WINDOW *win1;
int row, col, liteflag;

{

int x,y;
char litestring[15];

	wmove(win1, row,col);

	/* get existing filename char by char, deleting each
	 * char as it is read into a string variable.
	*/
	
	for(x=0; x<15; x++)
	{
		litestring[x] = winch(win1);
	/* if we hit a space, we've hit the end of the filename */
		if(litestring[x] == ' ')
			break;
		wdelch(win1);
	}

	litestring[x] = '\0';


	/* repad the spaces to keep the remaning filenames
	 * on this row in their proper columns.
	*/

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

	strcpy(selection, litestring);
	move (23,0);
	printw("Selected filename is: %14s", selection);

	refresh();
	wrefresh(win1);

	return(0);

}
		

		


int rfile()

{
struct entry record;
FILE *infp;
int EOF_FLAG;

	/* open file, abort on error */

	if ((infp = fopen(workfile,"r")) == NULL)
		{
		printf("\007ERROR opening file for input!\n");
		exit(1);
		}

	/* open a file then go to a an offset calculated by a 
	 * value passed from the calling program. Once we hit
	 * the max number of records that can be held by a screen,
	 * terminate the read and set a flag to show that we did
	 * not hit EOF. If we did hit EOF, terminate the loop
	 * and set a flag to show that we did hit EOF.
	*/

	/* start reading from a specified point in the file */
	fseek(infp, (sizeof (struct entry) * (screen_num * 100)), 0l);

	limit = 0;
	while((fread(&record, sizeof(struct entry),1, infp)) != 0)
		{
		strcpy(filenames[limit], record.filename);
		place[limit] = limit;
		limit++;
		if (limit == 100)
			break;
		}

	/* if x made it to 100, then we did NOT hit EOF */

if ( (limit == 100) && (fread (&record,sizeof(struct entry),1,infp)) !=  0)
		EOF_FLAG = 0;
	else
		EOF_FLAG = -1;

	fclose(infp);
	return (EOF_FLAG);

}



/*
 * writewin();
 * this routine does the actual work of writing filenames to a window
*/

void write_win(win1)

WINDOW *win1;

{

int r,c;	/* these are our rows and columns */
int counter = 0;	/* this counts the number of files written */



	/* clear the window */
	wclear(win1);

	/* the following loop will write the filenames to the window.
	 * 15 characters per screen field are allowed, since a filename 
	 * can only be 14 chars in length. This will leave at least one 
	 * space between filenames.
	*/

		for (r = 0; r < 20; r++)
		{
	
			/* if we've run out of files, terminate loop */

			if (counter == limit)
				break;

			/* increment column by 15 positions. This
			 * will cause the filenames to line up
			 * on the window.
			*/
	
			for (c = 1; c < 75; c+= 15)
			{
			wmove(win1, r, c);
			waddstr(win1, filenames[counter]);

		/* increment counter. When it equals the number of
		 * records, passed as 'limit', the loop should
		 * terminate.
		*/

			counter ++;
			if (counter == limit)
				break;
			}
		}


}

/* this function will draw a template for the selected record */

void display_form(win2)
WINDOW *win2;


{
int x;

	clear();
	wclear(win2);

	/* print field labels */

	wmove (win2, NAMELOCATE );
	waddstr (win2,"Filename:");

	wmove (win2, DESCLOCATE);
	waddstr(win2,"Description:");

	wmove(win2, DATELOCATE );
	waddstr(win2,"Date added/modified:");

	wmove(win2, SIZELOCATE);
	waddstr(win2,"File size:");

	if(open_mode != 'd')
		{
		wmove(win2, PARTLOCATE);;
		waddstr(win2,"Number of parts to download:");
		}

	wmove(win2, REQLOCATE );
	waddstr(win2,"Requires these other files:");

	wmove(win2, NOTELOCATE);
	waddstr(win2,"Other notes:");

	if(open_mode != 'd')
		{
		wmove(win2, PATHLOCATE);
		waddstr(win2, "Path to download file... Must give complete path including filename.");
		wmove (win2, PATHLOCATE2);
		waddstr(win2,"If a file is split into several parts, enter the first filename part");
		wmove (win2, PATHLOCATE3);
		waddstr(win2,"leaving the LAST character off of the filename:");
		}


/* highlite available fields */

	wstandout(win2);

	wmove (win2, NAMEHI);
	waddstr(win2,"              ");

	wmove (win2,DESCHI);
	for (x=1;x<79;x++)
		waddstr(win2," ");

	wmove(win2, DATEHI);
	waddstr(win2,"      ");

	wmove(win2, SIZEHI);
	waddstr(win2,"          ");

	if (open_mode != 'd')
		{
		wmove(win2,PARTHI);
		waddstr(win2,"  ");
		}

	wmove (win2,REQHI);
	for (x=1;x<61;x++)
		waddstr(win2," ");

	wmove (win2,NOTEHI);
	for (x=1;x<69;x++)
		waddstr(win2," ");

	if(open_mode != 'd')
		{
		wmove (win2, PATHHI);
		waddstr(win2,PATHNAME);
			for (x=23;x<61;x++)
				waddstr(win2," ");
		}

	wstandend(win2);
	refresh();
	wrefresh(win2);



}

/* following function prints the menu at the bottom of stdscr */

void menu()

{
/* print a menu of options to stdscr. They will appear at the bottom of
 * the screen with the first letter highlited as an indication to
 * the user that pressing the highlited key will result in the indicated
 * action.
*/



	if(open_mode == 'd')
		{
		move (21,0);
		printw("Select file to download.");
		refresh();
		}

	move(21, 50);
	standout();
	printw("Options:");
	move(22,46);
	printw("n");
	move(23,46);
	printw("p");
	move(22,60);
	printw("q");
	move(23,60);
	printw("s");
	standend();
	move(22,47);
	addstr("ext page");
	move(23,47);
	addstr("rev. page");
	move(22,61);
	addstr("uit");
	move(23,61);
	addstr("elect file");


}


/* this function will open the file and read the appropriate record,
 * then display it on the window in the approp. positions.
*/

void display_record (win2, row, col, screen_num)
WINDOW *win2;
int row, col, screen_num;

{
WINDOW *win3;
struct entry record;
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
	wprintw(win2,"%.6s",record.date);

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


	wclear(win3);
	wmove(win3,0,0);
	waddstr(win3,"Press <RETURN> for next screen.");
	wrefresh(win3);	
	while(13 != wgetch(win3))
		;

/* the following will display an extended screen of download information
 * and ask the user if he/she wants to download the file */

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

		wclear(win2);
		wmove(win2,5,15);
		waddstr(win2,"The following file(s) will be recevied by your system:");
		wmove(win2,7,1);
		if (record.noparts <2)
			{
			waddstr(win2,record.pathname);
			wrefresh(win2);
			}
		else
			{
			record.pathname[strlen(record.pathname)] = 'a';
			waddstr(win2,record.pathname);
			record.pathname[(strlen(record.pathname) -1) ] = 97 + record.noparts-1;
			wmove(win2,8,15);
			waddstr(win2,"-thru-");
			wmove(win2,9,1);
			waddstr(win2,record.pathname);
			wmove(win2,11,1);
			waddstr(win2,"When you receive these files, concatenate them into one file");		
			wmove(win2,12,1);
			waddstr(win2,"named: ");
			wstandout(win2);
			wmove(win2,12,9);
			waddstr(win2,record.filename);
			wstandend(win2);
			wrefresh(win2);
			}
		wclear(win3);
		wmove(win3,0,0);
		waddstr(win3,"Press <RETURN> to continue.");
		wrefresh(win3);
		while(13 != wgetch(win3))
			;
		}

}


/* this function will take the pathname and append the necessary
 * character(s) to generate the multiple requests necessary to
 * download multipart files.
*/

void build_uucp(record)
struct entry *record;

{
int x,y;

	y = strlen(record.pathname);

	for(x=0;x< record.noparts;x++)
		{
		strcpy(getfiles[x],HOST);
		record.pathname[y] = 97 + x;
		strcat(getfiles[x],record.pathname);
		strcat(getfiles[x],RECEIVER);
		}
}
