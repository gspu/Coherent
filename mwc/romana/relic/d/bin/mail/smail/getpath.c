#ifndef lint
static char 	*sccsid="@(#)getpath.c	2.5 (smail) 9/15/87";
#endif

# include	<stdio.h>
# include	<sys/types.h>
# include	<ctype.h>
# include	"defs.h"

extern enum edebug debug;	/* how verbose we are 		*/ 
extern char *pathfile;		/* location of path database	*/

/*
**
** getpath(): look up key in ascii sorted path database.
**
*/

getpath( key, path , cost)
char *key;		/* what we are looking for */
char *path;		/* where the path results go */
int *cost;		/* where the cost results go */
{
	char *getline();

	int lineno;
	char	*next_line,	/* Next line read by getline().  */
		*line_key,	/* Key extracted by strtok() from next_line.  */
		*line_cost;	/* Cost extracted by strtok() from next_line.  */
	FILE *file;

DEBUG("getpath: looking for '%s'\n", key);

	lineno = 0;

	if((file = fopen(pathfile, "r")) == NULL) {
		(void) printf("can't access %s.\n", pathfile);
		return( EX_OSFILE );
	}

	/* Linear search for key "path" in file stream "file".  */

	while (NULL != (next_line = getline(file, &lineno))) {
		line_key = strtok(next_line, WS);

		/*
		 * If we have a match, copy the path; copy the
		 * cost if there is one.
		 */
		if (strcmpic(key, line_key) == 0) {
			strcpy(path, strtok(NULL, WS));

			if (NULL != (line_cost = strtok(NULL, WS)) ) {
				*cost = atoi(line_cost);
			} else {
				*cost = DEFCOST;
			}
			break;
		}
	}

	if (NULL == next_line) {
		fclose(file);
		return(EX_NOHOST);
	}

	fclose(file);
	return (EX_OK);
}
