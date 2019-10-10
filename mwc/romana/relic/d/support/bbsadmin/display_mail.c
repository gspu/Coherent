#include <stdio.h>
#include <curses.h>
#include "contents.h"
#include "maillist.h"

void display_mail (master_win,win3,row,col,screen_num)
WINDOW *master_win, *win3;
int row, col, screen_num;

{
FILE *infp;
char choice = '\0';

	if((infp=fopen(workfile,"r")) == NULL)
		{
		noraw();
		endwin();
		printf("(mail)ADD_DEL_SCREEN: could'nt open file for input!\n");
		exit(1);
		}

	/* draw entry template */
	add_del_screen(master_win);
	
	/* get the desired record */
	fseek(infp,MAIL_FORMULA,0l);
	fread(&mail_rec,sizeof(struct mail),1,infp);


	/* print the info from this record */
	wstandout(master_win);
	
	wmove(master_win,SITEHI);
	waddstr(master_win,mail_rec.site);

	wmove(master_win,LOGHI);
	waddstr(master_win,mail_rec.login);

	wmove(master_win,STATEHI);
	waddstr(master_win,mail_rec.state);
	
	wmove(master_win,CITYHI);
	waddstr(master_win,mail_rec.city);

	wstandend(master_win);
	wrefresh(master_win);
	fclose(infp);



	/* prompt the user to delete the record */

	wclear(win3);
	wmove(win3,0,0);
	waddstr(win3,"Press [y] to delete, or");
	wmove(win3,1,0);
	waddstr(win3,"any other to abort.");
	wrefresh(win3);

		
	while(choice == '\0')
		choice = wgetch(win3);


	/* if the user really chose to delete the record, then call del_rec */

	if ((choice == 'y') || (choice == 'Y'))
		{
		wclear(win3);
		wrefresh(win3);
		del_rec(win3,row,col,screen_num);
		}

	wclear(master_win);
	wrefresh(master_win);
	wclear(win3);
	wmove(win3,1,0);
	waddstr(win3,"Press <RETURN>");
	wrefresh(win3);
	while (13 != wgetch(win3));

}
