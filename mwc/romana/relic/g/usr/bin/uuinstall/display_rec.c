/* display_rec.c: Display a sys, port or dial entry. This will open
 * the appropriate file and read an entire port, sys, or dial entry,
 * diaplaying it on the screen. If there are too many lines to be
 * displayed, the user will be told that he may select the modify
 * option from the action menu to be pulled into an editor so that
 * he may view the entire entry in that manner.
 */

#include "uuinstall.h"

/* these are the possible operations we will perform. This is stored
 * in opflag.
 */

#define VIEW	0166
#define DELETE 	0144
#define EDIT	0155

display_rec(position, opflag)
int position;			/* the record # we are looking for */
int opflag;			/* this will tell us if we are viewing,
				 * deleting, or going to edit an entry.
				 */
{
	int counter = -1;
	int lines_before = 0;
	int entry_lines = 0;
	char fname[30] = {"/usr/lib/uucp/"};
	char workstring[80];
	char b;
	FILE * filefd;		/* point to file we will work with */
	char compare[7] = {""}; /* hold the string (sys, port or dial)
				 * that we will compare against when
				 * trying to determine the boundaries of
				 * an entry within one of these files.
				 */

	/* determine which file we're working with, build our filename
	 * string and compare string based upon this.
	 */

	if (dialflag){
		strcat(fname,"dial");
		strcpy(compare,"dialer");
	}
	if (sysflag){
		strcat(fname,"sys");
		strcpy(compare,"system");
	}
	if (portflag){
		strcat(fname,"port");
		strcpy(compare,"port");
	}

	/* open the file */
	if ((filefd = fopen(fname,"r")) == NULL){
		mvwaddstr(portwin,12,24,"Error opening file for reading!");
		wrefresh(portwin);
		sleep(1);
		wclear(portwin);
		wrefresh(portwin);
		return;
	}

	/* read the file, looking for "port", "system" or "dialer", depending
	 * upon which file we're reading. When a line like this is found,
	 * increment our counter, then test to see if the counter matches the
	 * position (or entry #) within the file we're searching. If it matches,
	 * terminate the loop. This is how we find the entry we want to display.
	 * We will also keep track of the number of lines we read before finding
	 * the desired entry and the number of lines in the entry. This
	 * information will be used to delete or edit an entry, if we are indeed 
	 * going to perform a delete or edit operation.
	 */

	while (fgets(workstring, sizeof(workstring) -1, filefd) != NULL){
		if (strstr(workstring,compare))
			counter ++;
		if(position == counter)
			break;
		if((opflag == DELETE) || (opflag == EDIT))
			lines_before ++;
	}

	/* clear other windows */
	wclear(selwin);
	wclear(promptwin);
	wrefresh(promptwin);
	wrefresh(selwin);

	/* display the entry for view and delete operations*/		

	if(opflag != EDIT){
		wclear(portwin);
		wmove(portwin,0,22);
		wstandout(portwin);
		wprintw(portwin,"Entry from file %s .",fname);
		wstandend(portwin);

		/* print the line that is already in our buffer */
		mvwaddstr(portwin,2,1,workstring);
	}
	entry_lines++;

	/* now print the rest of the entry. We read and print until
	 * we hit a line consisting only of a newline char.
	 */

	while (fgets(workstring, sizeof(workstring) -1, filefd) != NULL){
		if(workstring[0] != '\n'){	

			/* test for entry too long for screen. */
			if ( (2 + entry_lines) <= 22){
				if(opflag != EDIT)
					mvwaddstr(portwin,2+entry_lines,1,workstring);
			}else{
				if (opflag != EDIT){
					wstandout(portwin);
					mvwaddstr(portwin,23,3,"Entry too long! To view entire entry, select MODIFY mode from action menu.");
					wstandend(portwin);
					wrefresh(portwin);
				}
			}
			entry_lines++;
		}else{
			break;
		}
	}

	fclose(filefd);
	wrefresh(portwin);

	/* if we're deleting, get a confirmation before continuing. IF not,
	 * we're just viewing, so any old keypress will send us back to the
	 * main menu.
	 */

	if(opflag == DELETE){
		mvwaddstr(portwin,1,19,"Do you wish to delete this entry? (y/n)");
		wrefresh(portwin);
		b = ' ';
		do{
			b = wgetch(portwin);
		}
		while ( (b != 'n') && (b != 'y') );

		if (b == 'y')
			delete_entry(fname, lines_before, entry_lines, 0);
	}else{
		if(opflag == EDIT){
			delete_entry(fname, lines_before, entry_lines, 1);
		}else{
			mvwaddstr(portwin,1,21,"Press any key to return to main menu.");
			wrefresh(portwin);
			b = wgetch(portwin);
		}
	}
	wclear(portwin);
	wrefresh(portwin);
}
