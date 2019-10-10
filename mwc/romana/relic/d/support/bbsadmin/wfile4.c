/* wfile.c
 * This routine will write a contents entry to a specified file
*/

#define MASTERFILE "Contents"
#define TRACE printf
#define PATHNAME "/usr/spool/uucppublic/"
#define LENPATH 22
#define NAMELOCATE 2,1
#define DESCLOCATE 4,1
#define DATELOCATE 6,1
#define SIZELOCATE 6,30
#define PARTLOCATE 6,49
#define REQLOCATE 9,1
#define NOTELOCATE 12,1
#define PATHLOCATE 15,1
#define PATHLOCATE2 16,1
#define PATHLOCATE3 17,1
#define NAMEHI 2,12
#define DESCHI 5,1
#define DATEHI 7,5
#define SIZEHI 7,30
#define PARTHI 7,60
#define REQHI 10,2
#define NOTEHI  13,2
#define PATHHI 18,2
#define PATHHI2 18,34

#include <stdio.h>
#include <curses.h>

char filename[14];

void entry_screen();
int get_data();
void getstring();

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


main()
{
WINDOW *win2;
int x;

	initscr();
	raw();

	if((win2=newwin(20, 79, 0,0)) == NULL)
	{
		printf("\007Window Memroy allocation for win2 failed!\n");
		exit(1);
	}


/* the following loop will draw the data entry screen and then go into
 * the function to get the data. This will continue until a '1' is
 * returned from get_data.
*/

	do
	{
		entry_screen(win2);
		x = get_data(win2);
	}
	while (x == 0);
	
	noraw();
	endwin();
}


/* this function will draw the file entry screen */

void entry_screen(win2)
WINDOW *win2;

{
int x;

	wclear(win2);

	/* print field labels */

	wmove (win2,0,31);
	waddstr (win2,"Add record to file");

	wmove (win2, NAMELOCATE );
	waddstr (win2,"Filename:");

	wmove (win2, DESCLOCATE);
	waddstr(win2,"Description:");

	wmove(win2, DATELOCATE );
	waddstr(win2,"Date added/modified:");

	wmove(win2, SIZELOCATE);
	waddstr(win2,"File size:");

	wmove(win2, PARTLOCATE);;
	waddstr(win2,"Number of parts to download:");

	wmove(win2, REQLOCATE );
	waddstr(win2,"Requires these other files:");

	wmove(win2, NOTELOCATE);
	waddstr(win2,"Other notes:");

	wmove(win2, PATHLOCATE);
	waddstr(win2, "Path to download file... Must give complete path including filename.");
	wmove (win2, PATHLOCATE2);
	waddstr(win2,"If a file is split into several parts, enter the first filename part");
	wmove (win2, PATHLOCATE3);
	waddstr(win2,"leaving the LAST character off of the filename:");


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

	wmove(win2,PARTHI);
	waddstr(win2,"  ");
	
	wmove (win2,REQHI);
	for (x=1;x<61;x++)
		waddstr(win2," ");

	wmove (win2,NOTEHI);
	for (x=1;x<69;x++)
		waddstr(win2," ");
	
	wmove (win2, PATHHI);
	waddstr(win2,PATHNAME);
		for (x=23;x<61;x++)
			waddstr(win2," ");

	wstandend(win2);

	refresh();
	wrefresh(win2);

}


/* this function will get the input and write it to a file */

int get_data(win2)
WINDOW *win2;

{
struct entry record;	
/* char pathname[38]; */
FILE *outfp;
WINDOW *win3;
int x;

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
	waddstr(win3,"Enter a filename, or press <RETURN> to end");
	wrefresh(win3);
	getstring(win2, win3, NAMEHI, 14, 0);
	strcpy(record.filename, filename);


	if(strlen(record.filename) == 0)
		exit(1);

	for(x=strlen(filename);x < sizeof (record.filename) ; x++)
		record.filename[x] = '\0';

	wclear(win3);
	wmove(win3,1,1);
	waddstr(win3,"Describe the uses of this package");
	wrefresh(win3);

	getstring(win2, win3, DESCHI, 78, 1);
	strcpy(record.description, filename);

	for(x=strlen(filename);x < sizeof (record.description) ; x++)
		record.description[x] = '\0';

	wclear(win3);
	wmove(win3,1,1);
	waddstr(win3,"Enter the date that the file was added or last modified");
	wmove(win3,2,1);
	waddstr(win3,"Enter no more than 6 characters");
	wrefresh(win3);

	getstring(win2, win3, DATEHI, 6, 1);
	strcpy(record.date, filename);

	for(x=strlen(filename);x < sizeof (record.date) ; x++)
		record.date[x] = '\0';

	wclear(win3);
	wmove(win3,1,1);
	waddstr(win3,"Enter the file size in number of bytes");
	wrefresh(win3);


	getstring(win2, win3, SIZEHI, 10, 1);
	strcpy(record.filesize, filename);

	for(x=strlen(filename);x < sizeof (record.filesize) ; x++)
		record.filesize[x] = '\0';

	wclear(win3);
	wmove(win3,3,1);
	waddstr(win3,"Enter the number of parts that this file is divided into");
	wrefresh(win3);

	getstring(win2, win3, PARTHI, 2, 1);
	record.noparts = atoi(filename);


	/* if the number of parts is 0 (not a split file), then we can
	 * cat the filename on to the end of the pathname. We can later
	 * test the length of the pathname to determine whether or not
	 * to ask for a pathname, as would be required for a split file.
	*/

	if(record.noparts ==0)
	{
		strcpy(record.pathname, PATHNAME);
		strcat(record.pathname, record.filename);
		wmove(win2,PATHHI);
		wstandout(win2);
		waddstr(win2, record.pathname);
		wstandend(win2);
		wrefresh(win2);
	}
	else
		strcpy(record.pathname, PATHNAME);


	wclear(win3);
	wmove(win3,3,1);
	waddstr(win3,"Enter the names of any support files required");
	wrefresh(win3);

	getstring(win2, win3, REQHI, 60, 0);
	if(strlen(filename) == 0 )
	{
		wmove(win2, REQHI);
		wstandout(win2);
		waddstr(win2,"none");
		wstandend(win2);
		wrefresh(win2);
		strcpy(record.requires, "none");
	}
	else
		strcpy(record.requires, filename);

	for(x=strlen(record.requires);x < sizeof (record.requires) ; x++)
		record.requires[x] = '\0';

	wclear(win3);
	wmove(win3,3,1);
	waddstr(win3,"Enter any relevant notes about this file");
	wrefresh(win3);

	getstring(win2, win3, NOTEHI, 78, 0);
	if(strlen(filename) == 0)
	{
		wmove(win2,NOTEHI);
		wstandout(win2);
		waddstr(win2,"none");
		wstandend(win2);
		wrefresh(win2);
		strcpy(record.notes, "none");
	}
	
	else
		strcpy(record.notes, filename);

	for(x=strlen(record.notes);x < sizeof (record.notes) ; x++)
		record.notes[x] = '\0';

	wclear(win3);
	wmove(win3,3,1);
	waddstr(win3,"Enter the pathlist to this file.");
	wrefresh(win3);

	/* if the length of the pathname is the default, as determined
	 * by "PATHNAME and LENPATH" #defs, then we need to get a
	 * completed pathlist.
	 * Remember, the need for this is also determined when asked
	 * for the number of parts (above).
	*/

	if(strlen(record.pathname) == LENPATH)
	{
		wmove(win2,PATHHI);
		wstandout(win2);
		waddstr(win2, PATHNAME);
		wstandend(win2);
		wrefresh(win2);
		getstring(win2, win3, PATHHI2, 37, 1);
		strcpy(record.pathname, PATHNAME);
		strcat(record.pathname, filename);
	}

	for(x=strlen(record.pathname);x < sizeof (record.pathname) ; x++)
		record.pathname[x] = '\0';

	/* time to write the record */

	if ((outfp = fopen(MASTERFILE,"a")) == NULL)
		{
		wclear(win3);
		wmove(win3,2,22);
		waddstr("\007ERROR opening file for writing!");
		wmove(win3,3,25);
		waddstr(win3,"press <RETURN> to continue");
		wrefresh(win3);
		noraw();
		while('\n' != wgetch(win3))
			;
		raw();
		exit(1);
		}

	else
		{
		fwrite(&record, sizeof(struct entry),1, outfp);
		fclose(outfp);
		wclear(win3);
		wmove(win3, 2, 25);
		waddstr(win3,"Record added... press <RETURN>");
		wrefresh(win3);
		noraw();
		while('\n' != wgetch(win3))
			;
		raw();
		wclear(win3);
		wrefresh(win3);
		}

	return(0);
	
}


/* this function will get a string char by char and print to the approp.
 * field on win2. This is to prevent people from entering too many characters.
*/

void getstring (win2, win3, row, col, maxchars, musthave)

WINDOW *win2, *win3;
int row, col, maxchars, musthave;

{


	noraw();
	wmove(win2,row,col);
	wrefresh(win2);
	wgetstr(win2,filename);
	wmove(win2,row,col);
	wstandout(win2);
	waddstr(win2,filename);
	wstandend(win2);
	wrefresh(win2);
	raw();

	

}
