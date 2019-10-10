/* add_dial.c: This will function just like add_port.c. It draws a
 *		template to be used by a user to add a dial entry and
 *		then gets the user's data.
 */

#include "uuinstall.h"

/* add_port calls the functions to draw a dial entry template, get the
 * data and finally to write the entry to the dial file.
 */

add_dial()
{
	char b;
	FILE * dialfd;

	dial_template();
	wrefresh(portwin);

	b = get_dial_data();

	/* time to save the data to the dial file */
	if(b == 'y'){
		if ( (dialfd = fopen(DIALFILE,"a")) == NULL){
			mvwaddstr(portwin,12,28,"Error opening dial file!");
			wrefresh(portwin);
			sleep(1);
			wclear(portwin);
			wrefresh(portwin);
			return;
		}

		wclear(portwin);
		mvwaddstr(portwin,12,32,"Adding entry...");
		wrefresh(portwin);
		sleep(1);

		fprintf(dialfd,"\n%s\n",dialname);
		fprintf(dialfd,"%s\n",dialchat);
		fprintf(dialfd,"%s\n",dialtout);

		if (strlen(dialfail1) != 0)
			fprintf(dialfd,"%s\n",dialfail1);

		if (strlen(dialfail2) != 0)
			fprintf(dialfd,"%s\n",dialfail2);

		if (strlen(dialfail3) != 0)
			fprintf(dialfd,"%s\n",dialfail3);

		if(strlen(dialplete) > 0)
			fprintf(dialfd,"%s\n",dialplete);
		if(strlen(dialabort) > 0)
			fprintf(dialfd,"%s\n",dialabort);

		fclose(dialfd);
	}

	wclear(portwin);
	wrefresh(portwin);
}

	
	
/* dial_template(): draw a template for a dial entry */

dial_template()
{

	/* label our fields */

	wclear(portwin);
	mvwaddstr(portwin,3,1,"dialer");
	mvwaddstr(portwin,4,1,"chat");
	mvwaddstr(portwin,5,1,"chat-timeout");
	mvwaddstr(portwin,6,1,"chat-fail");
	mvwaddstr(portwin,7,1,"chat-fail");
	mvwaddstr(portwin,8,1,"chat-fail");
	mvwaddstr(portwin,9,1,"complete-chat");
	mvwaddstr(portwin,10,1,"abort-chat");

	/* highlight our fields */
	wstandout(portwin);
	mvwaddstr(portwin,0,29,"Dial File Entry Screen");
	mvwaddstr(portwin,3,16,"               ");
	mvwaddstr(portwin,4,16,"                                       ");
	mvwaddstr(portwin,5,16,"   ");
	mvwaddstr(portwin,6,16,"                      ");
	mvwaddstr(portwin,7,16,"                      ");
	mvwaddstr(portwin,8,16,"                      ");
	mvwaddstr(portwin,9,16,"                                       ");
	mvwaddstr(portwin,10,16,"                                       ");
	wstandend(portwin);
}


/* get_dial_data():	this will position our cursor to get data for
 *			the fields we will fill.
 */

get_dial_data()
{
	char * workstring;
	char b;

	/* initialize the fields we will fill */

	strcpy(dialname,"dialer ");
	strcpy(dialchat,"chat ");
	strcpy(dialtout,"chat-timeout ");
	strcpy(dialplete,"complete-chat ");
	strcpy(dialabort,"abort-chat ");

	/* get our dialer name */

	mvwaddstr(portwin,20,0,"Enter the name of the dialer that");
	mvwaddstr(portwin,21,0,"this entry describes.");
	mvwaddstr(portwin,22,0,"Leaving this field blank aborts entry.");
	wrefresh(portwin);
	workstring = get_data(portwin,3,16,14,0,1);
	if (strlen(workstring) == 0)
		return('n');
	strcat(dialname,workstring);

	/* get our chat information */
	wmove(portwin,20,0);
	wclrtobot(portwin);
	mvwaddstr(portwin,12,1,"Enter the chat script used to talk to the modem to set up its registers");
	mvwaddstr(portwin,13,1,"and dial out. The format of a chat script is: expect_msg <SPACE> send_msg.");
	mvwaddstr(portwin,14,1,"For example, a chat script of:");
	wstandout(portwin);
	mvwaddstr(portwin,14,38,"\"\" ATQ0E1V1L2M1DT\\D CONNECT\\s2400");
	wstandend(portwin);
	mvwaddstr(portwin,15,1,"tells uucico to expect NOTHING (\"\") and then sends the appropriate commands");
	mvwaddstr(portwin,16,1,"to the modem to turn on ECHO and VERBAL RESULT CODES, sets speaker volume and");
	mvwaddstr(portwin,17,1,"speaker duration and finally to dial the phone number from a system's");
	mvwaddstr(portwin,18,1,"entry in the sys file (\\D). The final message to expect is the message the");
	mvwaddstr(portwin,19,1,"modem will send when it has connected to another modem.");
	wstandout(portwin);
	mvwaddstr(portwin,21,1,"Important:");
	wstandend(portwin);
	mvwaddstr(portwin,21,11," Expect_msgs and send_msgs are separated by spaces. To represent a");
	mvwaddstr(portwin,22,1,"space in an expect_msg or send_msg, enter a \\s instead of a space.");
	wrefresh(portwin);

	workstring = get_data(portwin,4,16,60,1,0);
	strcat(dialchat, workstring);

	wmove(portwin,12,0);
	wclrtobot(portwin);

	/* get a chat timeout value */

	do{
		wmove(portwin,20,0);
		wclrtobot(portwin);
		mvwaddstr(portwin,20,0,"Enter the  seconds uucico  should wait for");
		mvwaddstr(portwin,21,0,"when expecting a chat message before giving");
		mvwaddstr(portwin,22,0,"up attempting to establish a connection.");
		wrefresh(portwin);

		workstring = get_data(portwin,5,16,3,1,0);
	}	
	while(0 == atoi(workstring) );
	strcat(dialtout,workstring);

	/* now get some dial-fail strings */

	wmove(portwin,20,0);
	wclrtobot(portwin);
	mvwaddstr(portwin,20,0,"Optional: Enter a message that the modem");
	mvwaddstr(portwin,21,0,"might return if the dial attempt failed.");
	mvwaddstr(portwin,22,0,"Examples:");
	wstandout(portwin);
	mvwaddstr(portwin,22,11,"NO\\sDIALTONE BUSY NO\\sCARRIER");
	wstandend(portwin);
	wrefresh(portwin);

	strcpy(workstring,"");
	workstring = get_data(portwin,6,16,22,0,2);
	if (strlen(workstring) == 0)
		strcpy(dialfail1,"");
	else{
		strcpy(dialfail1,"chat-fail ");
		strcat(dialfail1,workstring);
	}
	
	strcpy(workstring,"");
	workstring = get_data(portwin,7,16,22,0,2);
	if (strlen(workstring) == 0)
		strcpy(dialfail2,"");
	else{
		strcpy(dialfail2,"chat-fail ");
		strcat(dialfail2,workstring);
	}
	
	strcpy(workstring,"");
	workstring = get_data(portwin,8,16,22,0,2);
	if (strlen(workstring) == 0)
		strcpy(dialfail3,"");
	else{
		strcpy(dialfail3,"chat-fail ");
		strcat(dialfail3,workstring);
	}
	

	/* now for abort-chat and complete-chat */

	wmove(portwin,20,0);
	wclrtobot(portwin);
	mvwaddstr(portwin,12,0,"Optional: Enter a chat script to be used to set the modem after a call has");
	mvwaddstr(portwin,13,0,"successfully completed. The purpose of this might be to ensure that the modem");
	mvwaddstr(portwin,14,0,"hangs up the line, although this would be done automatically under an ideal");
	mvwaddstr(portwin,15,0,"modem configuration. Remember, expect_msgs and send_msgs are separated by");
	mvwaddstr(portwin,16,0,"spaces. To represent a space within an expect_msg or send_msg, use \\s.");
	mvwaddstr(portwin,17,0,"Example:");
	wstandout(portwin);
	mvwaddstr(portwin,17,12,"\"\" +++ OK AT\\sH0\\sE0");
	wstandend(portwin);
	mvwaddstr(portwin,18,0,"This tells uucico to expect NOTHING (\"\"), send a command to put the modem ");
	mvwaddstr(portwin,19,0,"into command mode (+++), expect OK from the modem, then send a hangup command.");
	wrefresh(portwin);

	workstring = get_data(portwin,9,16,80,0,0);
	if(strlen(workstring) == 0)
		strcpy(dialplete,"");
	else
		strcat(dialplete, workstring);

	wmove(portwin,13,0);
	wclrtoeol(portwin);
	mvwaddstr(portwin,13,0,"abnormally completed. The purpose of this might be to ensure that the modem");
	wrefresh(portwin);
	
	workstring = get_data(portwin,10,16,80,0,0);
	if(strlen(workstring) == 0)
		strcpy(dialabort,"");
	else
		strcat(dialabort,workstring);

	wmove(portwin,12,0);
	wclrtobot(portwin);
	mvwaddstr(portwin,17,23,"Do you wish to save this entry? (y/n)");
	wrefresh(portwin);

	do{
		b = wgetch(portwin);
	}
	while ((b != 'n') && (b != 'y'));

	return(b);
}
