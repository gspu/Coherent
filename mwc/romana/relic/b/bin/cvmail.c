#include <stdio.h>
#include <sys/stat.h>
#define TRACE		printf
#define MPATH		"/usr/spool/mail/"
#define TEMPFILE	"/tmp/mbox.new"
#define OLDSEP		"\001\001\n"
#define NEWSEP		"\001\001\001\001\n"
#define MAXLEN		512
#define TRUE		1
#define FALSE		0

main(argc, argv)
int argc;
char * argv[];

{
struct stat sbuf;
int status;
FILE *infile;
FILE *outfile;
char filename[66];
char fullname[66];
char msgline [MAXLEN];
int counter = 1;
typedef short int bool;
bool justsep;
char systemcmd [76];

	if(argc == 1){
		printf("mailbox to convert? ");
		scanf("%s", filename);
		strcpy(fullname, filename);
	}else{
		if (argc ==2){
			strcpy(fullname, argv[1]);
		}

		if (argc == 3){
			if(strlen(argv[1]) <= 2){
				if( (argv[1][0] == 'm') || (argv[1][1] == 'm')){
					strcpy(fullname,MPATH);
					strcat(fullname,argv[2]);
				}else{
					printf("invalid argument %s\n",
						argv[1]);
					exit(1);
				}
			}else{
				printf("Usage: cvmail [-m] filename\n");
				exit(1);
			}
		}
	}

	TRACE("Converting file %s\n", fullname);
	/* get owner & group information */
	if( status = stat(fullname,&sbuf)){
		printf("Cannot stat file %s\n",fullname);
		exit(1);
	}

	if( (infile = fopen(fullname,"r")) == NULL){
		printf("Cannot open %s for reading!\n", fullname);
		exit(1);
	}

	if( (outfile=fopen(TEMPFILE,"w")) == NULL){
		printf("Cannot open %s for writing!\n");
		exit(1);
	}

	/* print newsep at top of file */
	fprintf(outfile,NEWSEP);

	/* now loop thorugh the file. Read each line and copy it
	 * to the tempfile. If we encounter an oldsep, convert it
	 * to a newsep, acutally 2 newseps, since we end a message
	 * with a sep and begin a message with a sep. The 2nd sep
	 * becomes the start sep for the next msg.
	 */
	
	/* justsep is a boolean use to determine if we just wrote a
	 * new msgsep. If we did, then we want to write another to begin
	 * the next message. This will prevent 2 seps from appearing
	 * at the end of the new file.
	 */

	justsep = FALSE;
	while (fgets(msgline,MAXLEN,infile) != NULL){

		if(justsep == TRUE){
			fprintf(outfile,NEWSEP);
			justsep = FALSE;
			counter++;
		}

		if(strcmp(OLDSEP,msgline) == 0){
			strcpy(msgline,NEWSEP);
			justsep = TRUE;
		}

		fputs(msgline, outfile);
	}

	/* we're done reading and writing, close the files */
	fclose(infile);
	fclose(outfile);

	/* delete old mailbox */
	if ( 0 != unlink(fullname)){
		printf("Could not delete old mailbox. The converted mailbox is at: %s\n",TEMPFILE);
		exit(1);
	}

	/* copy new mailbox to old */

	if( 0 != link(TEMPFILE, fullname)){
		sprintf(systemcmd,"/bin/cp %s %s",TEMPFILE, fullname);
		if ( system(systemcmd) == -1){
				printf("Could not copy converted mailbox to destination. The ");
				printf("new mailbox is at %s\n",TEMPFILE);
				exit(1);
		}
	}

	/* delete temporary mailbox */
	if ( 0 != unlink(TEMPFILE)){
		printf("Could not temporary workfile. ");
		printf("Compare %s and %s before manually deleting\n",
			TEMPFILE, fullname);
		printf("before deleting the temporary file %s.\n",TEMPFILE);
		exit(1);
	}

	/* change ownership of new file to that of the old file */

	chown(fullname,sbuf.st_uid, sbuf.st_gid);
	chmod(fullname,00644);

	printf("Converted %d messages\n", counter);
}
