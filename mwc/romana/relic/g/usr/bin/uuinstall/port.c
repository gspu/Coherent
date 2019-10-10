/* config.c:	this is the code for opening the config files;
 *		printing a screen of existing ports, systems or
 *		dialers; viewing or deleting an existing entry.
 */

#include <stdio.h>
#include <curses.h>
#include <fcntl.h>
#include "uuinstall.h"

char names[MAXENTRIES][15];	/* array of port names read */

/* open_config_file(): does just that, opens a configuration file */

open_config_file()
{

	FILE * configfile;

	if(portflag){
		if ( (configfile = fopen(PORTFILE,"r")) == NULL){
			printf("Error opening %s.\n",PORTFILE);
			exit_program(-1);
		}
	}

	if(sysflag){
		if ( (configfile = fopen(SYSFILE,"r")) == NULL){
			printf("Error opening %s.\n",SYSFILE);
			exit_program(-1);
		}
	}

	if(dialflag){
		if ( (configfile = fopen(DIALFILE,"r")) == NULL){
			printf("Error opening %s.\n",DIALFILE);
			exit_program(-1);
		}
	}

	/* file exists, but is empty */
	if( -1 == read_entries(configfile)){
		fclose(configfile);
		return;
	}

	fclose(configfile);
	print_selections();

	lite(selwin,0,1,1);	/* highlight our first record */
	prompt();
	wrefresh(selwin);

}


/* read the information from the specified file. We want to keep track of the
 * port, system or dialer name we have read, the line we began reading it 
 * on and the total number of entries read;
 */


read_entries(configfile)
FILE * configfile;		/* pointer to our configuration file */
{

	int x = 0;			/* counter of lines read */
	int size = 0;			/* offset where we will find the
					 * port name, system name or dialer
					 * name on the first line of a port,
					 * dial or sys entry in a config file.
					 */
	char buffer[256];
	char lookfor[8];

	if(dialflag){
		strcpy(lookfor,"dialer");
		size = 7;
	}
	if(sysflag){
		strcpy(lookfor,"system");
		size = 7;
	}
	if(portflag){
		strcpy(lookfor,"port");
		size = 5;
	}

	total_entries_found = 0;		/* total number of port entries found */

	while(fgets(buffer,sizeof(buffer) -1, configfile) != NULL){
		x++;

		/* found first line of a configuration entry, copy name and
		 * the line number we found it on.
		 */

		if(strstr(buffer,lookfor)){
			/* there's a newline here, get rid of it */
			buffer[strlen(buffer) -1] = '\0';
	/* if someone has editted the file to comment out an entry, we still
	 * want to display it, but must accomodate the new offset within the
	 * string of the actual name we want caused by the addition of a
	 * '#' character.
	 */
			while(isspace( *(buffer + size) ) ){
				size++;
			}

			strcpy(names[total_entries_found],buffer + size);
			startpos[total_entries_found] = x;
			total_entries_found++;

			if (total_entries_found == MAXENTRIES){
				return;
			}
/*	printf("Found port %s on line %d\n",names[total_entries_found -1],x); */
		}
	}

	/* if the file exists but is empty, return -1 to the calling process */
	if(x == 0){
		return(-1);
	}		
}


/* print the entries we found to the selection window */

print_selections()
{
	int r,c,z;			/* counters */

	z = 0;

	for(r = 0; r < 20 ; r++){	/* 20 is the max # lines we have */
		if (z == total_entries_found)	/* if we have no more ports, then abort */
			break;

	/* increment our column by 15 positions. This will cause
	 * the port names to be printed to line up properly.
	 */

		for(c = 1; c < 75 ; c+=15){
			mvwaddstr(selwin,r,c, names[z]);
			z++;
			if (z == total_entries_found)	/* break if out of ports */
				break;
		}
	}
	wmove(selwin, 0,1);  /* put our cursor at the first entry on the win */

	return;

}


/* prompt(): print a message telling the  user what to do */

prompt()
{
	wclear(promptwin);
	wmove(promptwin,0,0);
	wprintw(promptwin,"Highlite  the entry  you wish  to work");
	wmove(promptwin,1,0);
	wprintw(promptwin,"with by  using arrow  keys or  the  vi");
	wmove(promptwin,2,0);
	wprintw(promptwin,"keys h,j,k &l. <RETURN> selects entry.");
	wmove(promptwin,3,0);
	wprintw(promptwin,"To exit, press 'q'.");	
	wrefresh(promptwin);
}
