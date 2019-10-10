#include <stdio.h>
#include <curses.h>
#include "contents.h"



main(argc, argv)
int argc;
char *argv[];
{

int x;


	if(argc < 2)
	{
		printf("Usage: mwcbbs [adr] filename\n");
		printf("[a] add\n[d] download\n[r] remove\n");
		exit(1);
	}


	if(argv[1][0] != 'a')
		if(argv[1][0] != 'd')
			if(argv[1][0] != 'r')
				{
				printf("Option %c not recognized!\n",argv[1][0]);
				exit(1);
				}

	initscr();
	raw();

	open_mode = argv[1][0];

	screen_num = 0;

	do	{
		getfilename();

		/* if we did NOT select the mail option, then read the indicated
		 * Content file.
		*/
		
		if (strcmp(workfile,"QUIT")==0)
				break;

		if (strcmp(workfile,FILE4)!=0)
			x = rfile();
		if ((open_mode == 'a') && (strcmp(workfile,MAILFILE) == 0))
			x = rfile();
		if ((open_mode == 'r') && (strcmp(workfile,MAILFILE) == 0))
			x = rfile();	

		show_files(x);

		}
	while( strcmp(workfile,FILE6) != 0);

	echo();
	noraw();
	endwin();

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
				


	/* if QUIT was selected from the main menu, exit this function */

	if (strcmp(workfile,FILE6)==0)
		return;


	noecho();

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


	/* if the mail option was selected, print the states to the window,
	 * else print the filenames from the read Contents file to the window.
	*/

	/* if we are in add/remove mode, then we want to print mail accounts
	 * by SITE to the workscreen, NOT the list of states.
	*/

	if ( (strcmp(workfile,FILE4) == 0) && (open_mode == 'd'))
		print_states(win1);
	else
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

	lite (win1, prevrow, prevcol, 1,0);

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

	/* when we 'quit', we need to make sure that the screen number
	 * is set to 0 so that when the next file is selected, we begin
	 * at the start of the file. If not, we run the risk of reading
	 * something and causing a dump, or worse, system corruption.
	*/

			case 'Q':
			case 'q':
				screen_num = 0;
				delwin(win1);
				delwin(win2);
				break;

			case 13:
			case 'S':
			case 's':
				wclear(win1);
				wrefresh(win1);
				wclear(win2);

				if((strcmp(workfile,mapfile[0])==0) || (strcmp(workfile,mapfile[1])==0) || (strcmp(workfile,mapfile[2])==0))
					{
					map_command(win2,prevrow,prevcol,screen_num);
					wclear(win1);
					rfile();
					write_win(win1);
					wrefresh(win1);
					break;
					}

				if( strcmp(workfile,MAILFILE) == 0)
					{
					if (open_mode == 'd')
						{
						print_mail_states(win2);
						wclear(win2);
						wrefresh(win2);
						refresh();
						wrefresh(win1);
						wclear(win1);
						print_states(win1);
						wrefresh(win1);
						break;
						}
					if ((open_mode == 'a') || (open_mode == 'r'))
						{
						add_mail(win2,prevrow,prevcol,screen_num);
						wclear(win2);
						wmove(win2,0,1);
						waddstr(win2,"Reading file. Please wait...");
						wrefresh(win2);	
						rfile();
						write_win(win1);
						wrefresh(win1);
						break;
						}						
					}


		/* if we're not working on a mailfile record, display the Contents
		 * form.
		*/
				if(strcmp(workfile,MAILFILE)!=0)
					{
					if (open_mode == 'r')
						{
						display_form(win2);
						display_record(win2,prevrow,prevcol,screen_num);
						wclear(win2);
						wmove(win2,0,0);
						waddstr(win2,"Reading file... please wait");
						wrefresh(win2);		
						rfile();
						}
					if (open_mode == 'a')
						{
						display_form(win2);
						add_rec(win2,prevrow,prevcol,screen_num);
						wclear(win2);
						wmove(win2,0,0);
						waddstr(win2,"Reading file... please wait");
						wrefresh(win2);
						rfile();
						}
					else
						{
						display_form(win2);
						display_record(win2,prevrow,prevcol,screen_num);
						}
					write_win(win1);
					wclear(win2);
					wrefresh(win2);
					menu();
					refresh();
					wrefresh(win1);
					break;	
					}
	
			default:
				newcol = prevcol;
				newrow = prevrow;
				break;

		}

	/* print previous file highlited in normal video */
	lite (win1, prevrow, prevcol, 0,0);	

	/* print new filename selection in inverse video */

	lite (win1, newrow, newcol, 1,0);

	/* set our previous coordinates so that we can go back
	 * and unlite a field when the next directin is given.
	*/

		prevrow = newrow;
		prevcol = newcol;

	
	}
	while (arrow != 'q');

	echo();

}
		

		

int rfile()

{

struct mail mailrec;
FILE *infp;
int EOF_FLAG;


	/* if the selected file is QUIT, exit this function */

	if( strcmp(workfile,"QUIT") == 0 )
		return;

	/* open file, abort on error */

	if ((infp = fopen(workfile,"r")) == NULL)
		{
		printf("\007ERROR opening file %s for input!\n", workfile);
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

	if((strcmp(workfile,mapfile[0])==0) || (strcmp(workfile,mapfile[1])==0) || (strcmp(workfile,mapfile[2])==0))
		fseek(infp,(sizeof (struct map) * (screen_num * 100)),0l);
	else
		{
		if(strcmp(workfile,MAILFILE)==0)
			fseek(infp, (sizeof (struct mail) * (screen_num * 100)), 0l);
		else
			fseek(infp, (sizeof (struct entry) * (screen_num * 100)), 0l);
		}

	limit = 0;


	if((strcmp(workfile,mapfile[0])==0) || (strcmp(workfile,mapfile[1])==0) || (strcmp(workfile,mapfile[2])==0))
		{
		while((fread(&map_rec,sizeof(struct map),1,infp)) != 0)
			{
			strcpy(filenames[limit], map_rec.name);
			place[limit] = limit;
			limit ++;
			if (limit == 100)
				break;
			}
   		 }
	else
		{
		if(strcmp(workfile,MAILFILE)==0)
			{
			while((fread(&mailrec,sizeof(struct mail),1,infp)) != 0)
				{
				strcpy(filenames[limit], mailrec.site);
				place[limit] = limit;
				limit ++;
				if (limit == 100)
					break;
				}
   			 }

		else
			{
			while((fread(&record, sizeof(struct entry),1, infp)) != 0)
				{
				strcpy(filenames[limit], record.filename);
				place[limit] = limit;
				limit++;
				if (limit == 100)
					break;
				}
			 }
		}


	/* if x made it to 100, then we did NOT hit EOF */

	if((strcmp(workfile,mapfile[0])==0) || (strcmp(workfile,mapfile[1])==0) || (strcmp(workfile,mapfile[2])==0))
			{
			if ( (limit == 100) && (fread (&map_rec,sizeof(struct map),1,infp)) !=  0)
				EOF_FLAG = 0;
			else
				EOF_FLAG = -1;
			}
	else
		{
		if(strcmp(workfile,MAILFILE)==0)
			{
			if ( (limit == 100) && (fread (&mailrec,sizeof(struct mail),1,infp)) !=  0)
				EOF_FLAG = 0;
			else
				EOF_FLAG = -1;
			}
		else	
			{
			if ( (limit == 100) && (fread (&record,sizeof(struct entry),1,infp)) !=  0)
				EOF_FLAG = 0;
			else
				EOF_FLAG = -1;
			}
		}
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
	wrefresh(win1);

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

	if(open_mode =='r')
		{
		move (21,0);
		printw("Select file to delete");
		refresh();
		}

	if(open_mode == 'a')
		{
		move (21,0);
		printw("select file that will FOLLOW");
		move (22,0);
		printw("the file to be added.");
		refresh();
		}

	if(open_mode == 'd')
		{
		move (21,0);
		if(strcmp(workfile,MAILFILE)==0)
			printw("Select state/other to view.");
		else
			printw("Select file to download.   ");
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


void del_rec(win3,row,col,screen_num)
WINDOW *win3;
int row,col,screen_num;

{
int x;
FILE *infp,*outfp;
int rec_mark = POS_FORMULA;

	if ( (infp=fopen(workfile,"r"))==NULL)
		{
		printf("Error opening file %s for input!\n",workfile);
		exit(1);
		}

	/* delete any previous temporary file that may be here */

	if (unlink(TEMPFILE)==-1);	

	if ( (outfp=fopen(TEMPFILE,"a"))==NULL)
		{
		printf("Error opening \"mwcbbs.tmp\" for output!\n");
		exit(1);
		}


	/* we will now read each record until we hit the record number
	 * calculated by rec_mark. to delete, we will simply exit the
	 * loop when we hit the correct record, skip the record with a 
	 * read, then follow that with another read/write loop to finish
  	 * off the file.
	*/


	for(x=0;x<rec_mark;x++)
		{
		if(strcmp(workfile,MAILFILE)==0)
			fread(&mail_rec, sizeof(struct mail),1,infp);
		else
			fread(&record, sizeof(struct entry),1,infp);
		wmove(win3,1,0);
		wprintw(win3,"Reading:");
		wrefresh(win3);
		if(strcmp(workfile,MAILFILE)==0)
			fwrite(&mail_rec, sizeof(struct mail),1,outfp);
		else
			fwrite(&record, sizeof(struct entry),1,outfp);
		wmove(win3,1,0);
		wprintw(win3,"Writing:");
		wrefresh(win3);
		}

	/* this will delete the record by using a dummy record to skip it */

	if(open_mode == 'r')
		if(strcmp(workfile,MAILFILE)==0)
			fread(&mail_rec, sizeof(struct mail),1,infp);
		else
			fread(&record, sizeof(struct entry),1,infp);


	/* this will write in a new record, if invoked in 'add' mode */
	if(open_mode == 'a')
		if(strcmp(workfile,MAILFILE)==0)
			fwrite(&new_mail_rec, sizeof(struct mail),1,outfp);
		else
			fwrite(&new_record, sizeof(struct entry),1,outfp);

	/* this will finish off the file. */

	if(strcmp(workfile,MAILFILE)==0)
		{
		while((fread(&mail_rec, sizeof(struct mail),1,infp)) != 0)
				{
				wmove(win3,1,0);
				wprintw(win3,"Reading");
				wrefresh(win3);
				fwrite(&mail_rec, sizeof(struct mail),1,outfp);
				wmove(win3,1,0);
				wprintw(win3,"Writing:");
				wrefresh(win3);
				}
		}
	else
		{
			while((fread(&record, sizeof(struct entry),1,infp)) != 0)
				{
				wmove(win3,1,0);
				wprintw(win3,"Reading");
				wrefresh(win3);
				fwrite(&record, sizeof(struct entry),1,outfp);
				wmove(win3,1,0);	
				wprintw(win3,"Writing:");
				wrefresh(win3);
				}
		}


/* close the files and move around as appropriate (delete the old then
 * move the new to old
*/

	fclose(infp);
	fclose(outfp);
	if(unlink(workfile) == -1)
		{
		endwin();
		printf("Could not remove old file!\n");
		printf("Updated file could be found as %s\n",TEMPFILE);
		exit(1);
		}

	if( link(TEMPFILE,workfile) == -1)
		{
		endwin();
		printf("Could not link new file!\n");
		exit(1);
		}

	if( unlink(TEMPFILE) == -1)
		{
		endwin();
		printf("Could not remove temporary file!\n");
		exit(1);
		}

}

/* this function will take the pathname and append the necessary
 * character(s) to generate the multiple requests necessary to
 * download multipart files.
*/

void build_uucp(record)


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











