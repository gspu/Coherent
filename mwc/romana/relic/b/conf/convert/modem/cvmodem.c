/* cvmodem.c: convert /etc/modemcap to dial file for Taylor uucp.
 *
 * Read a modemcap entry, parse out modem names, cs, ce, is and hu strings
 * and build dial chat, dial-abort chat and dial-complete chat.
 */

#include <stdio.h>
#include <string.h>

#define MODEMCAP	"/etc/modemcap"
#define DIAL		"/usr/lib/uucp/dial"
#define MAXLEN		125

/* flags for get_cmd() */

#define IS	1
#define	HU	2
#define CS	3
#define CE	4
#define DS	5
#define CO	6

char	cs_cmd[MAXLEN];	/* command start string */
char	ce_cmd[MAXLEN];	/* command end string */
char	ds_cmd[MAXLEN];	/* command to start dialing */
char 	is_cmd[MAXLEN];	/* setup modem to dial */
char	hu_cmd[MAXLEN];	/* hang up and reset modem registers */
char	co_cmd[MAXLEN]; /* expected connect message when modem connects */

FILE *infp;		/* pointer to MODEMCAP file */

int get_cap();		/* get a modemcap entry */
int get_cmd();		/* parse command string from modemcap entry */
int bld_dial();	/* build dial file entry */

main()
{
	int  get_stat = 0;	/* status of get_cap() */


	if( (infp=fopen(MODEMCAP,"r")) == NULL){
		printf("Failed to open file %s!\n",MODEMCAP);
		exit(1);
	}

	do{
		get_stat = get_cap();
/*		printf("get_cap() returned [%d]\n",get_stat); */
	}
	while (get_stat == 0);

	fclose(infp);

}

/* get_cap(): 	read a modemcap entry, copying the modemcap entry identifies
 * 		(list of modem names) to a string to be parsed. Copy modemcap
 *		command lines to a string and parse out what we need to build
 *		our dial file entry.
 */

int get_cap()
{

	char *list_ptr;		/* pointer to modem name list */
	char *name_ptr;		/* pointer to individual modem name */
	char *cmd_ptr;		/* pointer to command within modemcap command
				 * line.
				 */
	char mdm_names[81];	/* modemcap line with modem names */
	char mdm_entry[25];	/* single modem name parsed from mdm_names */
	char work_line[81];	/* dummy: all lines read to here first to
				 * determine what we read.
				 */
	
	short hu_flag = 0;	/* flags to see what we've already scanned for */
	short is_flag = 0;
	short cs_flag = 0;
	short ce_flag = 0;
	short ds_flag = 0;
	short co_flag = 0;

	/* initialize our commands to build */
	strcpy(is_cmd,"");
	strcpy(hu_cmd,"");
	strcpy(cs_cmd,"");
	strcpy(ce_cmd,"");
	strcpy(ds_cmd,"");
	strcpy(co_cmd,"");

	do{
		if(fgets(work_line, MAXLEN, infp) == NULL){
			return(1);
		}
			/* skip comments and blank lines */
		if( (work_line[0]=='#') || (work_line[0] == '\n'))
			continue;

		/* if not a tab, it isn't a comment (we just tested for that)
		 * and therefore must be the first valid line of a modemcap
		 * entry which gives the modem names.
		 */

		if( (work_line[0] != '\t') && (work_line[0] != ' ')){
			strcpy(mdm_names, work_line);			
			strcpy(work_line, "");
			continue;
		}


		/* If we got this far, then we must be looking at a modemcap
		 * line listing commands.
		 */

		/* look for our cu, hu... commands */

		if(strlen(mdm_names) > 0){

			if((cmd_ptr = (strstr(work_line,":is"))) != NULL)
				if(get_cmd(IS, cmd_ptr))
					is_flag = 1;
				else{
					printf("An invalid initialization string (is:) was detected in the modemcap file.\n");
					printf("This program will now exit, please examine this line:\n");
					printf("%s",work_line);
					exit(1);
				}
			if((cmd_ptr = (strstr(work_line,":hu"))) != NULL)
				if(get_cmd(HU, cmd_ptr))
					hu_flag = 1;
				else{
					printf("An invalid hangup command string (hu:) was detected in the modemcap file.\n");
					printf("This program will now exit, please examine this line:\n");
					printf("%s",work_line);
					exit(1);
				}
			if((cmd_ptr = (strstr(work_line,":cs"))) != NULL)
				if(get_cmd(CS, cmd_ptr))
					cs_flag = 1;
				else{
					printf("An invalid command start string (cs:) was detected in the modemcap file.\n");
					printf("This program will now exit, please examine this line:\n");
					printf("%s",work_line);
					exit(1);
				}
			if((cmd_ptr = (strstr(work_line,":ce"))) != NULL)
				if(get_cmd(CE, cmd_ptr))
					ce_flag = 1;
				else{
					printf("An invalid command end string (ce:) was detected in the modemcap file.\n");
					printf("This program will now exit, please examine this line:\n");
					printf("%s",work_line);
					exit(1);
				}
			if((cmd_ptr = (strstr(work_line,":ds"))) != NULL)
				if(get_cmd(DS, cmd_ptr))
					ds_flag = 1;
				else{
					printf("An invalid start dial string (ds:) was detected in the modemcap file.\n");
					printf("This program will now exit, please examine this line:\n");
					printf("%s",work_line);
					exit(1);
				}

			/* a null connect message is valid, so we don't
			 * necessarily care about what get_cmd returned.
			 */
			if((cmd_ptr = (strstr(work_line,":co"))) != NULL){
				get_cmd(CO, cmd_ptr);
				co_flag = 1;
			}
		}else{
			continue;
		}
	}
	while( hu_flag + is_flag + ce_flag + cs_flag + ds_flag + co_flag != 6);

/*	printf("%s",mdm_names);
	printf("is: %s\n", is_cmd);
	printf("hu: %s\n", hu_cmd);
	printf("cs: %s\n", cs_cmd);
	printf("ce: %s\n", ce_cmd);
	printf("ds: %s\n", ds_cmd);
	printf("co: %s\n", co_cmd);
 */

	/* parse out our individual modem names from the list of names
	 * in the modem name list, then call the function to write the
	 * data into the dial file as a completed dial entry. Remember,
	 * we only want to do this if we have found all of the data we
	 * need from the entry in the modemcap file.
	 */

	list_ptr = mdm_names;	/* initialize our single entry string and */
	strcpy(mdm_entry,"");	/* set up our pointers. */


	if (hu_flag && is_flag && ce_flag && cs_flag && ds_flag && co_flag ){
		do{
			name_ptr = mdm_entry;
			while(*list_ptr != '|'){	/* copy our name, stoping at | */
				/* printf("[%c]",*list_ptr); */
				if( (*list_ptr == ' ') || (*list_ptr =='\\') )
					return(0);		/* abort on space */
				*name_ptr++ = *list_ptr++;
			}
			list_ptr++;			/* skip | */
			*name_ptr = '\0';		/* terminate name string */
			bld_dial(mdm_entry);
/*			printf("Single modem: %s\n",mdm_entry); */
			strcpy(mdm_entry,"");
		}
		while( (*list_ptr != ' ') && (*list_ptr != '\\'));
		strcpy(mdm_names,"");
		return(0);
	}else{
		return(1);
	}

}



/* get_cmd() will take a pointer to a string, a pointer to where to
 * begin parsing an entry out of a string and a flag to tell us what
 * what command we are parsing.
 */

int get_cmd(flag, pointer)
int flag;		/* flag for command string we're working with */
char *pointer;		/* where to start our search withing the work string */

{

char *cmdptr;		/* pointer to command string */

	/* look at the flag passed and set our pointer accordingly */


	switch(flag){
		case IS:	/* modem initialization string to dial out */
			cmdptr = is_cmd;
			break;
		case HU:	/* modem hangup string and reset cmds */
			cmdptr = hu_cmd;
			break;
		case CS:	/* command start string */
			cmdptr = cs_cmd;
			break;
		case CE:	/* command terminator string */
			cmdptr = ce_cmd;
			break;
		case DS:	/* modem dial command */
			cmdptr = ds_cmd;
			break;
		case CO:	/* modem connect message */
			cmdptr = co_cmd;
			break;
		default:
			printf("Unknown command flag received [%d]\n",flag);
			return;
	}

	pointer+=4;			/* get past the '=' in command entry */


	/* we begin copying from the curreent position in the work_line,
	 * as this is SUPPOSED to be the start of the command. When we
	 * hit a SPACE, we convert it to a \s, as required by Taylor's
	 * modem chat configuration.
	 */

	while(*pointer != ':'){		
		if(*pointer == ' '){
			*cmdptr++ = '\\';
			*cmdptr++ = 's';
			pointer++;
			continue;
		}
		*cmdptr++ = *pointer++;	
	}
	*cmdptr = '\0';			/* terminate our string */

	if(flag == IS){			/* an is: ends with a \r (carriage */
		cmdptr--;		/* return). We change the 'r' to an */
		*cmdptr = 's';			/* 's' to make it look like a space.*/
	}

	/* if we went through all of this and came up with a command string
	 * of 0 length, then an invalid modemcap entry exists. We will
	 * return a 0 in this case, which will serve as a flag to 
	 * abort the process of converting the current and remaining
	 * modemcap entries.
	 */

	switch(flag){
		case IS:	/* modem initialization string to dial out */
/*			printf("is -x %s\n", is_cmd); */
			if(strlen(is_cmd) > 0)
				return 1;
			else
				return 0;
			/* not reached */
		case HU:	/* modem hangup string and reset cmds */
/*			printf("hu -x %s\n", hu_cmd); */
			if(strlen(hu_cmd) > 0)
				return 1;
			else
				return 0;
			/* not reached */
		case CS:	/* command start string */
/*			printf("cs -x %s\n", cs_cmd); */
			if(strlen(cs_cmd) > 0)
				return 1;
			else
				return 0;
			/* not reached */
		case CE:	/* command terminator string */
/*			printf("ce -x %s\n", ce_cmd); */
			if(strlen(ce_cmd) > 0)
				return 1;
			else
				return 0;
			/* not reached */
		case DS:	/* modem dial command */
/*			printf("ds -x %s\n", ds_cmd); */
			if(strlen(ds_cmd) > 0)
				return 1;
			else
				return 0;
			/* not reached */
		case CO:	/* modem connect message... NULL is valid */
			return 1;
			/* not reached */
	}

}

/* bld_dial():	given a name of a modem, take the cs, is, ds, cs and
 *		co information and write out a dial file entry.
 */

bld_dial(name)
char * name;		/* name of modem */
{
	FILE *outfile;

	/* open the file for append, abort on error */
	if((outfile=fopen(DIAL,"a")) == NULL){
		printf("Failed to open %s to append new entry for modem {%s}\n",
			DIAL, name);
		exit(1);
	}

	
	/* name of our dialer */
	fprintf(outfile,"dialer %s\n",name);

	/* dialer chat */
	fprintf(outfile,"chat \"\" %s%s\\D %s\n",
		is_cmd, ds_cmd,co_cmd);

	/* some default stuff... timeout before failing and chat failure
	 * detection.
	 */

	fprintf(outfile,"chat-timeout 60\n");
	fprintf(outfile,"chat-fail BUSY\n");
	fprintf(outfile,"chat-fail NO\\sCARRIER\n");
	fprintf(outfile,"chat-fail NO\\sANSWER\n");

	/* the following is a modem chat script to be done if a call
	 * is successful and uucico runs smoothly.
	 */

	fprintf(outfile,"complete-chat \"\" \\d+++\\d%s%s\n",
		cs_cmd, hu_cmd);

	/* the following is a modem chat script to be done is a call
	 * is NOT successful. This is the same chat for successful calls
	 * as a default for the purpose of converting modemcap entries.
	 */

	fprintf(outfile,"abort-chat \"\" \\d+++\\d%s%s\n\n",
		cs_cmd, hu_cmd);

	/* close our file */

	fclose(outfile);
}
