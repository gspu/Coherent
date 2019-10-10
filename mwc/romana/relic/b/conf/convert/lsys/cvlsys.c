/* cvlsys.c: convert the information in L.sys and Permissions to
 * Taylor uucp sys file format. This will also peek into L-devices
 * if we find an ACU entry in our L.sys entry.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define PERMISSIONS	"/usr/lib/uucp/Permissions"
#define LSYS		"/usr/lib/uucp/L.sys"
#define	TSYS		"/usr/lib/uucp/sys"


#define MAX		175
#define BEHEMOTH	1024

/* this is what we need to parse from our L.sys entry */

char system[9];			/* name of system to call/be called by */
char calltime[45];		/* valid time(s) to call */
char device[10];		/* device to call on */
char speed[7];			/* baud rate */
char telno[51];			/* phone number */
char chat_script[128];		/* chat script to log in with */
char acudev[15];		/* store ACU device from L-devices*/
char lsys[254];			/* line read from L.sys */

/* stuff from Permissions file: */

char	request[4];		/* REQUEST files from Permissions */
char	transfer[4];		/* SEND files from Permissions */
char	readpath[MAX];		/* READ from Permissions */
char	no_readpath[MAX];	/* NOREAD from Permissions */
char	writepath[MAX];		/* WRITE from Permissions */
char	no_writepath[MAX];	/* NOWRITE from Permissions */
char	commands[MAX];		/* COMMANDS from Permissions */
char	myname[10];		/* MYNAME from Permissions */

char	parseline[BEHEMOTH];	/* this will be our concatenated Permissions */

int getrest();			/* complete concatenating Permissions string */
int parseperm();		/* parse a field out of Permissions string */
int cvnowritwe();		/* convert NOREAD and NOWRITE to useable format */
int lsysentry();		/* read entry from L.sys file */
int permentry();		/* read entry from Permissions */
int parselsys();		/* parse L.sys entry */
int parseperm();		/* parse Permissions entry */
int getACU();			/* get ACU device from L-devices */
int readperm(); 		/* read Permissions file */
int printsys();			/* write sys file entry */


main()
{
	FILE *infp;

	if((infp = fopen(LSYS,"r")) == NULL){
		printf("Error opening %s for input.\n",LSYS);
		exit(1);
	}

	if(getACU()){
		printf("failed to find ACU device!\n");
		exit(1);
	}

	while(lsysentry(infp)){
		if(lsys[0] != '#'){	/* we don't want to process a comment */
			readperm(system);	/* get Permissions information */
			printsys();		/* print sys file entry */
			lsys[0] = '\0';		/* the terminator... he's back */
		}else{
			printf("Skipping comment line in L.sys\n");
		}
	}

	fclose(infp);
}


/* lsysentry(): read a line from L.sys and pass it on to be parsed
 * 	into sys entries for use by Taylor uucp.
 */

lsysentry(infp)
FILE *infp;
{

	char next_line[80];	/* dummy for possible line continuation */

	if(fgets(lsys,sizeof(lsys),infp) == NULL){
		return(0); /* probably hit EOF */
	}
#ifdef DEBUG
	printf("got %s",lsys);
#endif
	if(lsys[0] == '#'){	/* skip comments */
		return(1);
	}


	/* check for line continuation. we will read the next line and
	 * concatenate it to the just read lsys line.
	 */

	if(lsys[strlen(lsys) -2] == 92){	/* 92 = \ */
		if(fgets(next_line, sizeof(next_line),infp) == NULL){
			printf("Error reading continued line in %s.\n",LSYS);
			exit(1);
		}else{
	/* the next to last character should be the line continuation
	 * character followed by a newline. We'll just terminate our
	 * our string where we expect the continuation to be.
	 */
			lsys[strlen(lsys) -2] = '\0';
			strcat(lsys,next_line);
		}
#ifdef DEBUG
		printf("Line continuation detected. Just added:\n");
		printf("%s",next_line);
		printf("%s",lsys);
#endif
	}

	return(parselsys(lsys));
}


/* parselsys(): parses the lsys line just read into the fields needed
 * 	to build a Taylor sys entry.
 */

parselsys(lsys)
char *lsys;
{
	char *ptrlsys;
	char *ptrlentry;

	strcpy(system,"");
	strcpy(calltime,"");
	strcpy(device,"");
	strcpy(speed,"");
	strcpy(telno,"");
	strcpy(chat_script,"");

	ptrlsys = lsys;

	/* parse out system name */
	ptrlentry = system;
	*ptrlentry = '\0';
	while(!isspace(*ptrlsys))
		*ptrlentry++ = *ptrlsys++;
	*ptrlentry = '\0';

	printf("Converting L.sys and Permissions entries for system %s.\n",
		system);

	while(isspace(*ptrlsys++))	/* skip whitespace */
		;
	
	/* get calltime */
	ptrlentry = calltime;
	*ptrlentry = '\0';

	ptrlsys--;

	while(!isspace(*ptrlsys)){
		*ptrlentry++ = *ptrlsys++;
	}
	*ptrlentry = '\0';

	while(isspace(*ptrlsys++))	/* skip whitespace */
		;

	ptrlsys--;

	/* get our device */
	ptrlentry = device;
	*ptrlentry = '\0';
	while(!isspace(*ptrlsys))
		*ptrlentry++ = *ptrlsys++;
	*ptrlentry = '\0';

	/* if this is a modem, which would be indicated by ACU in this field,
	 * then copy the name of the ACU device previously fetched into device
	 */

	if(device[0] == 'A'){
		if(0 == strcmp(acudev,NULL)){
			printf("FATAL Configuration error. No ACU device was.\n");
			printf("found in L-devices, yet you have defined an\n");
			printf("L.sys entry with an ACU device. Aborting...\n");
			exit(1);
		}
		strcpy(device, acudev);
	}

			
	while(isspace(*ptrlsys++))	/* skip whitespace */
		;


	ptrlsys--;

	/* get the speed (baud rate) */
	ptrlentry = speed;
	*ptrlentry = '\0';
	while(!isspace(*ptrlsys))
		*ptrlentry++ = *ptrlsys++;
	*ptrlentry = '\0';

	while(isspace(*ptrlsys++))	/* skip whitespace */
		;


	ptrlsys--;

	/* get telephone number */
	ptrlentry = telno;
	while(!isspace(*ptrlsys))
		*ptrlentry++ = *ptrlsys++;
	*ptrlentry = '\0';

	/* if no telephone number is found, then make sure the string is NULL */
	if(strcmp(telno,"\"\"") == 0)
		strcpy(telno,"");

	while(isspace(*ptrlsys++))	/* skip whitespace */
		;

	/* the rest is our chat script. We copy up to the newline. */
	ptrlentry = chat_script;
	*ptrlentry = '\0';
	ptrlsys--;
	while(*ptrlsys != '\n'){
		*ptrlentry++ = *ptrlsys++;
	}
	*ptrlentry = '\0';

#ifdef DEBUG
	printf("system %s\n",system);
	printf("time %s\n",calltime);
	printf("port %s\n",device);
	printf("baud %s\n",speed);
	printf("phone %s\n",telno);
	printf("chat {%s}\n", chat_script);
#endif

	return(1);
}



/* printsys():	print out system information to Taylor sys file */

printsys()
{

	FILE *outfp;

	if((outfp = fopen(TSYS,"a")) == NULL){
		printf("Could not open %s for writing!\n",TSYS);
		exit(1);
	}

	fprintf(outfp,"system %s\n",system);
	fprintf(outfp,"time %s\n",calltime);
	fprintf(outfp,"baud %s\n",speed);
	fprintf(outfp,"port %s\n",device);
	
	if(telno[0] != '\0')
		fprintf(outfp,"phone %s\n",telno);

	fprintf(outfp,"chat %s\n",chat_script);
	fprintf(outfp,"protocol g\nprotocol-parameter g window 3\n");
	fprintf(outfp,"protocol-parameter g packet-size 64\n");

	if(myname[0] != '\0')
		fprintf(outfp,"myname %s\n", myname);

	if(request[0] != '\0')
		fprintf(outfp,"request %s\n",request);

	if(transfer[0] != '\0')
		fprintf(outfp,"transfer %s\n",transfer);

	if(readpath[0] != '\0')
		fprintf(outfp,"remote-send %s %s\n",readpath, no_readpath);

	if(writepath[0] != '\0')
		fprintf(outfp,"remote-receive %s %s\n",writepath, no_writepath);

	if(commands[0] != '\0')
		fprintf(outfp,"commands %s\n",commands);

	fprintf(outfp,"\n");
	fclose(outfp);
}
