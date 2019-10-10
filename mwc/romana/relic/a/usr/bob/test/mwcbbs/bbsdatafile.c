#define TRACE		printf
#define DATAFILE	".mwcbbs"

#include <stdio.h>
#include "contents.h"

void bbsdatafile()
{
int x,y;
char read_data[57];
char newpath[10];
char specpath[46];
FILE *datafile;

	if( (datafile=fopen(DATAFILE,"r")) != NULL){

		/* read a line from the file and read the
		 * first set of characters into a string. These
		 * characters should be upper case only and we
		 * will stop reading them when we hit an '=' sign.
		*/

		while (fgets(read_data,70,datafile) != NULL){
			x = 0;
			y = 0;
			while(read_data[x] != '='){
				if((read_data[x] == '\n') || (read_data[x] == '\0')){
					printf(".mwcbbs: unexpected NEWLINE or NULL in file.\n");
					exit(1);
				}
				if(isupper(read_data[x]) == 0){
					printf(".mwcbbs: Expected UPPER CASE character before '='.\n");
					exit(1);
				}
				newpath[x] = read_data[x];
				x++;
			}
			newpath[x] = '\0';
			x++;

			/* Since we got this far, we will take the remainder
			 * of the line read from .mwcbbs and place that in its
			 * own string.
			 */

			while(x < strlen(read_data + 1)){
				specpath[y] = read_data[x];
				x++;
				y++;

				/* if we are about to excede our pathname limit,
				 * warn the user and exit.
				 */

				if( y == 45 ){
					printf(".mwcbbs: WARNING: path for %s excedes 45 character limit!\n",
						newpath);
					exit(1);
				}
			}
			if(specpath[y-1] != '/'){
				specpath[y] = '/';
				y++;
			}
			specpath[y] = '\0';

		/* if the download path was specified, copy the pathname
		 * to the recdir (receive directory)  variable.
		 */

		if (strcmp(newpath,"DOWNPATH") == 0){
			strcpy(recdir," ");
			strcat(recdir,specpath);
		}
		
		/* if the data file path was specifed, copy the pathname
		 * to the find_workfile variable.
		 */

		if(strcmp(newpath,"DATAPATH") == 0){
			strcpy(find_workfile,specpath);
		}
		}
	fclose(datafile);
	}
	return ;
}
