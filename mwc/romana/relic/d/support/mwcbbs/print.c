/* print.c
 * With any luck, this program will read the Content file format
 * and print information to a text file which can later be spooled 
 * for printing by the user.
*/

#include <stdio.h>
#include <curses.h>
#include "contents.h"

#define PRINTFILE "mwcbbs.print"

void print(win2)
WINDOW *win2;

{
FILE *infp, *outfp;
char new_date[3];
int month, day, year;
int cmp_month, cmp_day, cmp_year;
char choice, dummy;


	/* we will first as if the user wants to specify a date. If
	 * the user responds 'yes', we will get a month and year
	 * only. Later in the read/fprintf loop, we will perform
	 * the comparison before printing a record to the file.
	*/

	wclear(win2);
	wmove(win2,0,10);
	wprintw(win2,"Do you wish to specify a start date for printing only");
	wmove(win2,1,10);
	wprintw(win2,"those file  entries NEWER than  the date you specify?");
	wmove(win2,3,10);
	wprintw(win2,"[y]es or any other key for no date specification.");
	wrefresh(win2);

	choice = 0;
	while( choice == 0)
		choice = wgetch(win2);

	if(choice == 'y')
	{

	noraw();

	/* print our screen to get the user's info */

	wclear(win2);

	wmove(win2,0,15);
	wprintw(win2,"Please specify the month and year  from which to");
	wmove(win2,1,15);
	wprintw(win2,"begin  searching new entries.  Only those  files");
	wmove(win2,2,15);
	wprintw(win2,"with a month and year greater than the specified");
	wmove(win2,3,15);
	wprintw(win2,"date will be printed to the file %s.",PRINTFILE);
	wmove(win2,5,35);
	wprintw(win2,"Month: ");
	wmove(win2,7,35);
	wprintw(win2,"Year:  ");
	wstandout(win2);
	wmove(win2,5,42);
	wprintw(win2,"  ");
	wmove(win2,7,42);
	wprintw(win2,"  ");
	wstandend(win2);
	wrefresh(win2);

	wmove(win2,5,42);
	wrefresh(win2);
	cmp_month = 99;
	while((cmp_month > 12) || (cmp_month < 1))
		{
		wscanw(win2,"%d",&cmp_month);
		wmove(win2,5,42);
		wrefresh(win2);
		wstandout(win2);
		wprintw(win2,"  ");
		wstandend(win2);
		wmove(win2,5,44);
		wprintw(win2,"               ");
		wmove(win2,5,42);
		wrefresh(win2);
		}
	wmove(win2,5,42);
	clrtoeol();
	wstandout(win2);
	wprintw(win2,"%2d",cmp_month);
	wstandend(win2);
	wmove(win2,5,44);
	wprintw(win2,"              ");
	wrefresh(win2);

	wmove(win2,7,42);
	wrefresh(win2);
	cmp_year = -1;
	while((cmp_year > 99) || (cmp_year<0))
		{
		wscanw(win2,"%d",&cmp_year);
		wmove(win2,7,42);
		wrefresh(win2);
		wstandout(win2);
		wprintw(win2,"  ");
		wstandend(win2);
		wmove(win2,7,44);
		wprintw(win2,"               ");
		wmove(win2,7,42);
		wrefresh(win2);
		}
	wmove(win2,7,42);
	clrtoeol();
	wstandout(win2);
	wprintw(win2,"%2d",cmp_year);
	wstandend(win2);
	wmove(win2,7,44);
	wprintw(win2,"              ");
	wrefresh(win2);

	raw();
	}

	if ((infp=(fopen(workfile,"r")))==NULL)
		{
		noraw();
		endwin();
		printf("Could not open file %s for input!\n",workfile);
		exit(1);
		}
	if ((outfp=(fopen(PRINTFILE,"w")))==NULL)
		{
		noraw();
		endwin();
		printf("Could not open file %s for writing!\n",PRINTFILE);
		exit(1);
		}

	
	if(choice != 'y')
		wclear(win2);
	wmove(win2,12,15);
	wprintw(win2,"Writing file %s... please wait.",PRINTFILE);
	wrefresh(win2);

	fprintf(outfp,"Contents file for: %s      ",workfile);

	/* if we're doing a date search, then print the date */
	if(choice == 'y')
		fprintf(outfp,"Search after: %d/%d\n",cmp_month, cmp_year);
	else
		fprintf(outfp,"\n");

	while ((fread(&record,sizeof(struct entry),1,infp))!=0)
		{
		strcpy(new_date,"");
		strncpy(new_date,record.date,2);
		month = atoi(new_date);
		strcpy(new_date,"");

		new_date[0] = record.date[2];
		new_date[1] = record.date[3];
		new_date[2] = '/';
		day = atoi(new_date);
		strcpy(new_date,"");

		new_date[0] = record.date[4];
		new_date[1] = record.date[5];
		new_date[2] = '\0';
		year = atoi(new_date);


if ((choice != 'y') || ((choice == 'y') && ((month > cmp_month) && (year >= cmp_year) || (year > cmp_year) )))
			{
			fprintf(outfp,"\nFILE: %15s\t\tDATE: %d/%d/%d\t\tSIZE: %s\n\n",
				record.filename, month, day, year ,record.filesize);
			fprintf(outfp,"Description:\n");
			fprintf(outfp," %s\n",record.description);
			fprintf(outfp,"Other notes:\n");
			fprintf(outfp," %s\n",record.notes);
			fprintf(outfp,"Required files:\n");
			fprintf(outfp," %s\n\n",record.requires);
			}
		}
	fclose(infp);
	fclose(outfp);
	wclear(win2);
	wmove(win2,0,10);
	wprintw(win2,"Because  we have  just  written  the  file %s,",PRINTFILE);
	wmove(win2,1,10);
	wprintw(win2,"the  program  will now  end so  that  you may  view  the");
	wmove(win2,2,10);
	wprintw(win2,"file and/or print it.  This is  to prevent  you from the");
	wmove(win2,3,10);
	wprintw(win2,"possibility of immediate accidental erasure of the file.");
	wmove(win2,6,10);
	wprintw(win2,"File written. Press <RETURN> to exit program.");
	clrtoeol();
	wrefresh(win2);
	while(13 != wgetch(win2)) ;
}
