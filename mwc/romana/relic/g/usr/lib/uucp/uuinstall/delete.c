/* delete.c: given a filename, the number of lines before the entry
 *	to delete, and the number of lines in the entry, delete the
 *	entry by copying the lines before the entry to a temp file,
 *	skipping the lines the entry we wish to delete occuppies,
 *	then copying the remaining lines to the temp file. Delete
 *	the old file and link the temp file to the newly modified file.
 *	A similar scheme is used to add a modified entry.
 */

#include "uuinstall.h"

delete_entry(fname,before,during,insert_flag)
char * fname;					/* file name to work with */
int before, during;
int insert_flag;				/* insert a modified entry? */
{
	FILE *filefd;
	FILE *tmpfd;
	FILE *newfd;
	struct termio thisterm;
	int x;
	char *env;
	char workstring[88];
	char syscmd[65];		/* used to build system command to edit */
	char b;

	extern char * getenv();

	wclear(portwin);

	/* open file for reading */
	if ((filefd = fopen(fname,"r")) == NULL){
		mvwaddstr(portwin,12,29,"Error opening file for reading!");
		wrefresh(portwin);
		sleep(1);
		wclear(portwin);
		wrefresh(portwin);
		return;
	}

	/* open edit file for writing */
	if(insert_flag){
		if ((newfd = fopen(UUEDIT,"w")) == NULL){
			mvwaddstr(portwin,12,29,"Error opening file for reading!");
			wrefresh(portwin);
			sleep(1);
			wclear(portwin);
			wrefresh(portwin);
			fclose(filefd);
			return;
		}
	}
	/* open tmp file for writing */
	if ((tmpfd = fopen(UUTMP,"w")) == NULL){
		mvwaddstr(portwin,12,29,"Error opening file for writing!");
		wrefresh(portwin);
		sleep(1);
		wclear(portwin);
		wrefresh(portwin);
		fclose(filefd);
		fclose(tmpfd);
		return;
	}

	wmove(portwin,12,0);
	wclrtoeol(portwin);	
	if(!insert_flag){
		mvwaddstr(portwin,12,29,"Updating...");
	}else{
		mvwaddstr(portwin,12,29,"Standby ...");
	}
	wrefresh(portwin);

	/* skip the lines we don't want to delete */

	for(x = 0 ; x < before ; x++){
		fgets(workstring, sizeof(workstring) -1, filefd);
		fputs(workstring, tmpfd);
		fflush(tmpfd);
	}

	/* now read the entry to delete, but don't copy it to the tmp file...
	 * UNLESS we're going to edit an entry, in which case, we write the
	 * indicated entry to another temp file and then invoke an editor on it.
	 */

	during++;

	for(x = 0; x < during ; x++){
		fgets(workstring, sizeof(workstring) -1, filefd);

		/* write to edit file */
		if(insert_flag){
			fputs(workstring,newfd);
		}
	}

	strcpy(workstring,"");

/* if edit, get the EDITOR environment variable. If there is none, use emacs */

	if (insert_flag){
		fputs("\n",newfd);
		fclose(newfd);

		/* build system command to edit temp editfile */
		if( (env = getenv("EDITOR")) != NULL){
			sprintf(syscmd,"%s %s",env, UUEDIT);
		}else{
			sprintf(syscmd,"%s %s","/usr/bin/me", UUEDIT);
		}

		/* save our current terminal characteristics. I can't
		 * ass/u/me that the editor will return us to our
		 * current state when it exits.
		 */
		ioctl(fileno(stdin), TCGETA, &thisterm);

		/* invoke editor */
		system(syscmd);

		/* now restore our saved information */
		ioctl(fileno(stdin), TCSETA, &thisterm);

	/* if we editted an entry, we need to insert the tmp editfile now. 
	 * Prompt to really save changes, re-open the edit file and
	 * copy its contents to the temp file.
	 */

		wclear(portwin);
		mvwaddstr(portwin,12,23,"Do you wish to save changes? (y/n)");
		wrefresh(portwin);
		do{
			b = ' ';
			b = wgetch(portwin);
		}
		while ((b != 'y') && (b != 'n'));

		if(b == 'y'){
			if((newfd = fopen(UUEDIT,"r")) == NULL){
				wclear(portwin);
				mvwaddstr(portwin,12,16,"Failed to open temp file to get modified entry!");
				mvwaddstr(portwin,14,20,"Leaving all files in original condition.");
				wrefresh(portwin);
				sleep(2);
				fclose(tmpfd);
				fclose(filefd);
				return;
			}

			while(fgets(workstring,sizeof(workstring) -1,newfd) != NULL){
				fputs(workstring, tmpfd);
				fflush(tmpfd);
			}
			fclose(newfd);
		}else{
			/* user doesn't want to save changes, so close the
			 * files we opened and delete the temp files.
			 */

			fclose(tmpfd);
			fclose(filefd);
			unlink(UUTMP);	/* don't care if this fails, it may */
			unlink(UUEDIT);	/* prove useful to keep these files */
			return;		/* around anyways. */
		}
	}

	/* now copy the rest of the file */
	while (fgets(workstring,sizeof(workstring)-1,filefd) != NULL){
		fputs(workstring, tmpfd);
		fflush(tmpfd);
	}

	/* close the files */

	fclose(filefd);
	fclose(tmpfd);

	/* now unlink the old and link in the new! */

	if (unlink(fname) == -1){
		wclear(portwin);
		mvwaddstr(portwin,12,28,"Error deleting old file!");
		wmove(portwin,13,15);
		wprintw(portwin,"Leaving old file intact. New data saved to %s.",UUTMP);
		wrefresh(portwin);
		sleep(2);
		return;
	}

	if(link(UUTMP,fname) == -1){
		wclear(portwin);
		wmove(portwin,12,15);
		wprintw(portwin,"Error writing data to %s. Data is saved in %s!",fname, UUTMP);
		wrefresh(portwin);
		sleep(2);
		return;
	}

	/* We don't care about the tmp file being unlinked, as there is no
	 * code here that appends to a tmp file.
	 */

	unlink(UUTMP);

	wclear(portwin);
	wrefresh(portwin);
}
