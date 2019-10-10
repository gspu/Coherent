#include <curses.h>
#include <stdio.h>
#include "contents.h"
#include "maillist.h"

void add_mail(win1, row, col, screen_num)
WINDOW *win1;
int row, col, screen_num;
{

WINDOW *master_win, *win3;

	wclear(win1);
	wrefresh(win1);

	if((win3=newwin(3,30,20,0))==NULL)
		{
		noraw();
		endwin();
		printf("ADD MAIL: Could not allocate MSG Window!\n");
		exit(1);
		}

	if((master_win=newwin(5,45,STARTWIN)) == NULL)
		{
		noraw();
		endwin();
		printf("ADD MAIL: Could not allocate Master MAILREC Window!\n");
		exit(1);
		}

	if(open_mode == 'r')
		display_mail(master_win,win3,row,col,screen_num);
	else
		{		
		add_del_screen(master_win);
		get_info(win3,master_win, row, col,screen_num);
		}

	delwin(win3);
	delwin(master_win);

}

void add_del_screen(master_win)
WINDOW *master_win;
{
int x;


	wmove(master_win,SITE);
	waddstr(master_win,"Sitename:");
	wmove(master_win,LOGIN);
	waddstr(master_win,"Login:");
	wmove(master_win,STATE);
	waddstr(master_win,"State/Country:");
	wmove(master_win,CITY);
	waddstr(master_win,"City/Province/Other:");
	
	/* highlite the available fields */

	wmove(master_win,SITEHI);
	for (x=0;x<sizeof(mail_rec.site);x++)
		{
		wstandout(master_win);
		wprintw(master_win," ");
		wstandend(master_win);
		}


	wmove(master_win,LOGHI);
	for (x=0;x<sizeof(mail_rec.login);x++)
		{
		wstandout(master_win);
		wprintw(master_win," ");
		wstandend(master_win);
		}

	wmove(master_win,STATEHI);
	for (x=0;x<sizeof(mail_rec.state);x++)
		{
		wstandout(master_win);
		wprintw(master_win," ");
		wstandend(master_win);
		}

	wmove(master_win,CITYHI);
	for (x=0;x<sizeof(mail_rec.city);x++)
		{
		wstandout(master_win);
		wprintw(master_win," ");
		wstandend(master_win);
		}

	wrefresh(master_win);
}


int get_info (win3, master_win, row, col, screen_num)
WINDOW * win3, *master_win;
int row, col, screen_num;

{
FILE *outfp;
int x;
int rec_mark=POS_FORMULA;

	noraw();
	echo();
	
	wmove(master_win,SITEHI);
	wrefresh(master_win);
	wgetstr(master_win,new_mail_rec.site);

	if(strlen(new_mail_rec.site)>0)
		{
		wmove(master_win,SITEHI);
		wstandout(master_win);
		waddstr(master_win,new_mail_rec.site);
		wstandend(master_win);
		wrefresh(master_win);
		}

	else 
		return(1);

/* pad the trailing spaces with NULLS to get rid of the extra curses
 * garbage that may be in the string.
*/

	for(x=strlen(new_mail_rec.site);x<sizeof(new_mail_rec.site);x++)
		new_mail_rec.site[x] = '\0';

	do	{
		wmove(master_win,LOGHI);
		wrefresh(master_win);
		wgetstr(master_win,new_mail_rec.login);
		wmove(master_win,LOGHI);
		wstandout(master_win);
		waddstr(master_win,new_mail_rec.login);
		wstandend(master_win);
		}
	while(strlen(new_mail_rec.login) <1);

	for(x=strlen(new_mail_rec.login);x<sizeof(new_mail_rec.login);x++)
		new_mail_rec.login[x] = '\0';

	wmove(master_win,STATEHI);
	wrefresh(master_win);
	wgetstr(master_win,new_mail_rec.state);
	wmove(master_win,STATEHI);
	wstandout(master_win);
	waddstr(master_win,new_mail_rec.state);
	wstandend(master_win);

	for(x=strlen(new_mail_rec.state);x<sizeof(new_mail_rec.state);x++)
		new_mail_rec.state[x] = '\0';

	wmove(master_win,CITYHI);
	wrefresh(master_win);
	wgetstr(master_win,new_mail_rec.city);
	wmove(master_win,CITYHI);
	wstandout(master_win);
	waddstr(master_win,new_mail_rec.city);
	wstandend(master_win);

	for(x=strlen(new_mail_rec.city);x<sizeof(new_mail_rec.city);x++)
		new_mail_rec.city[x] = '\0';

	raw();
	noecho();
	del_rec(win3, row, col, screen_num);
	uucp_upd();
	wclear(master_win);
	wrefresh(master_win);

}
