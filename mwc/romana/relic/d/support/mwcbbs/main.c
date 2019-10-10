#include <stdio.h>
#include <curses.h>
#include "contents.h"



main(argc, argv)
int argc;
char *argv[];

{

WINDOW *win1, *win2;		
int x;
char dummy[50];

	printflag = 0;
	strcpy(recdir,RECEIVER);
	if(argc > 3)
		{
		printf("Too many arguments!\n");
		exit(1);
		}

	/* parse the command line argument for a 'p' or a 'P' so
	 * that we know to PRINT a file. If we hit a '?', then
	 * print a usage: message.
	 */

	bbsdatafile();	/* read the .mwcbbs data file, if it exists */

	if((argc==2)||(argc==3))
		{
		for(x=0;x<strlen(argv[1]);x++)
			{

/* test for v */	if( argv[1][x] == 'v' ){
				printf("mwcbbs interface version %s\n",
					VERSION);
				exit(0);
			}
/* test for pP */	if((argv[1][x] == 112) || (argv[1][x] == 80))
				break;

/* test for dD */	if((argv[1][x] == 100) || (argv[1][x] == 68))
				{
				strcpy(dummy,argv[2]);
				if(argc < 3)
					{
					printf("No directory specified!\n");
					exit(1);
					}
				if (strlen(dummy) < 2)
					{
					printf("Invalid destination specified!\n");
					exit(1);
					}
				strcpy(recdir," ");
				strcat(recdir,dummy);
				break;
				}

			if(argv[1][x] == '?')
				{
				printf("Usage:\tmwcbbs [options] directory\n");
				printf("\tOptions:\n");
				printf("\t -[Pp] print Contents files\n");
				printf("\t -[Dd] specify destination directory\n");
				printf("\t -[v]  specify interface version\n");
				printf("\t -[?]  usage message\n");
				exit(1);
				}
			}
		}

	/* the following should only be executed if the above
	 * loop went all the way thru the argument passed, which means
	 * that we did not find a [pP].
	 */

	if ((argc==2) && (x==strlen(argv[1])))
		{
		printf("Unknown option %s\n",argv[1]);
		exit(1);
		}
	else

	/* we found that there is one argument and that it is [pP],
	 * so set a printflag. We should only set this flag if argc = 2.
	 * The if statement ensures that printflag will only be set
	 * if there was one argument passed.
	*/
		{
		if(argc==2)
			printflag = 1;
		}

	initscr();
	raw();
	noecho();

	/* allocate memory for windows. print message on failure. */

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

	screen_num = 0;


	/* now we get the user's choice of file to process */

	do	{
		getfilename();


		/* if we're printing, call the print function. After
		 * completion, exit the program. We want to exit the
		 * because we don't want the user to automatically 
		 * run the print function again and overwrite the 
		 * data file written by the previous run-through.
		*/


		/* added 11/21/91: If 'quit' was selected from the 
		 * menu, we don't want to run the print option. Beta
		 * version 3 would allow the print routine to be entered
		 * and would later exit with a 'cannot find QUIT' message.
		 */

		if((printflag == 1) && (strcmp(workfile,"QUIT") != 0))
			{
			print(win2);
			strcpy(workfile,"QUIT");
			}

		/* if we did NOT select the mail option, then read the indicated
		 * Content file.
		*/
		
		if (strcmp(workfile,"QUIT")==0)
				break;

		if (strstr(workfile,MAILFILE) == NULL)
			x = rfile();
		show_files(win1, win2, x);

		}
	while( strcmp(workfile,FILE6) != 0);

	noraw();
	endwin();

}




/*	show_files()
 *	This function will display the filenames read to a curses
 *	screen.
*/


void show_files(win1, win2, EOF_FLAG)
WINDOW *win1, *win2;
int EOF_FLAG;

{
char arrow;
int prevcol =1;
int prevrow =0;			/* prevcol = column before arrow	  */
int newrow =0;			/* prevrow = row before arrow	 	  */
int newcol =1;  		/* newrow = row after arrow		  */
int counter = 0;		/* newcol = column after arrow		  */
				
				


	/* if the mail option was selected, print the states to the window,
	 * else print the filenames from the read Contents file to the window.
	*/

	/* if we are in add/remove mode, then we want to print mail accounts
	 * by SITE to the workscreen, NOT the list of states.
	*/

	if ( (strstr(workfile,FILE4) != NULL) )
		{
		print_states(win1);
		EOF_FLAG = -1;
		}

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
	noecho();

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
				break;

			case 13:
			case 'S':
			case 's':
				wclear(win1);
				wrefresh(win1);
				wclear(win2);

				if((strstr(workfile,mapfile[0]) != NULL) || (strstr(workfile,mapfile[1])!= NULL) || (strstr(workfile,mapfile[2])!= NULL))
					{
					map_command(win2,prevrow,prevcol,screen_num);
					wclear(win1);
					rfile();
					write_win(win1);
					wrefresh(win1);
					break;
					}

				if( strstr(workfile,MAILFILE) != NULL)
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

		/* if we're not working on a mailfile record, display the Contents
		 * form.
		*/
				if(strstr(workfile,MAILFILE) == NULL)
					{
					display_form(win2);
					display_record(win2,prevrow,prevcol,screen_num);

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

}
		

		

int rfile()

{

FILE *infp;
int EOF_FLAG;


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

	if((strstr(workfile,mapfile[0])!= NULL) || (strstr(workfile,mapfile[1])!= NULL) || (strstr(workfile,mapfile[2])!= NULL))
		fseek(infp,(sizeof (struct map) * (screen_num * 100)),0l);
	else
		fseek(infp, (sizeof (struct entry) * (screen_num * 100)), 0l);

	limit = 0;


	if((strstr(workfile,mapfile[0])!= NULL) || (strstr(workfile,mapfile[1])!= NULL) || (strstr(workfile,mapfile[2])!= NULL))
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
		while((fread(&record, sizeof(struct entry),1, infp)) != 0)
			{
			strcpy(filenames[limit], record.filename);
			place[limit] = limit;
			limit++;
			if (limit == 100)
				break;
			}
		 }



	/* if x made it to 100, then we did NOT hit EOF */

	if((strstr(workfile,mapfile[0])!= NULL) || (strstr(workfile,mapfile[1])!= NULL) || (strstr(workfile,mapfile[2])!= NULL))
		{
		if ( (limit == 100) && (fread (&map_rec,sizeof(struct map),1,infp)) !=  0)
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

	wmove(win2, REQLOCATE );
	waddstr(win2,"Requires these other files:");

	wmove(win2, NOTELOCATE);
	waddstr(win2,"Other notes:");


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

	wmove (win2,REQHI);
	for (x=1;x<61;x++)
		waddstr(win2," ");

	wmove (win2,NOTEHI);
	for (x=1;x<69;x++)
		waddstr(win2," ");

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

	move (21,0);
	if(strstr(workfile,MAILFILE) != NULL)
		printw("Select state/other to view.");
	else
		printw("Select file to download.   ");
	refresh();


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



/* this function will take the pathname and append the necessary
 * character(s) to generate the multiple requests necessary to
 * download multipart files.
*/

void build_uucp(record)


{
int x,y,z;

	y = strlen(record.pathname);

	for(x=0;x< record.noparts;x++){
		strcpy(getfiles[x],HOST);
		record.pathname[y] = 97 + x;
		record.pathname[y+1] = '\0';
		strcat(getfiles[x],record.pathname);
		strcat(getfiles[x],recdir);
	
		for(z=strlen(getfiles[x]) ; z < sizeof(getfiles[x]) ; z++){
			getfiles[x][z] = '\0';
		}

	}


}
