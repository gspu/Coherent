/*
 * apropos.c
 *
 * implement a simple version of the Berkeley command "apropos".  This
 * command searches the file /usr/man/man.index for a word that the
 * user enters on the command line, and prints every index entry in which
 * that token appears.
 *
 * Usage:  apropos token [ token ... ]
 *
 * 3/17/1993 (aye and begorah!), fwb, first draft
 * 9/1/1993, fwb, changed behavior to continue when it reads a "bad" line
 * 10/26/1993, fwb, hacked to read all files with suffix .index in /usr/man
 * 10/26/1993, fwb, fixed a bug whereby some index files would be ignored
 */

#define BUFFERSIZE 200

#include <dirent.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void fatal();
void search_index();

main(argc, argv)
int argc; char **argv;
{
	char namebuffer[50];
	FILE *fp;
	DIR *dirp;
	struct dirent *dp;

	if (argc < 2) {
		fprintf(stderr, "Usage: apropos word\n");
		exit(EXIT_FAILURE);
	}

	/* read all files /usr/man/ *.index - fwb, 10/26/93 */
	dirp = opendir("/usr/man");

	while ((dp = readdir(dirp)) != NULL) {
		if ( pnmatch (dp->d_name, "*.index", 1) == 1) {
			sprintf(namebuffer, "%s%s", "/usr/man/", dp->d_name);
			if ((fp = fopen(namebuffer, "r")) == NULL)
				fatal("cannot open index file");
			search_index(fp, argc, argv);
			fclose(fp);
		}
	}
	closedir(dp);
	exit(EXIT_SUCCESS);
}

/*
 * search an index file for all lines that contain any of the tokens
 * given on the command line.
 */
void
search_index(fp, argc, argv)
FILE *fp;
int argc;
char **argv;
{		
	int i, j, k, lowerflag;
	char inb[BUFFERSIZE], testb[BUFFERSIZE], *inbp;

	/* if first character of user's word is upper-cased, then assume
	 * the user wants an exact match, and therefore do not
	 * shift the description into lower case.
	 */
	for (k = 1; k < argc; k++) {
		rewind(fp);

		if (isupper(argv[k][0]))
			lowerflag = 0;
		else
			lowerflag = 1;

		for (i = 1; fgets(inb, BUFFERSIZE-1, fp) != NULL; i++) {
			if ((inbp = strchr(inb, '\t')) == NULL)
				continue;

			inbp++;
			strcpy (testb, inbp);

			if (lowerflag)
				for (j = 0; j < strlen (testb); j++)
					if (isupper(testb[j]))
						testb[j] = tolower(testb[j]);

			if (strstr(testb, argv[k]) != NULL)
				printf("%s", inbp);
		}
	}
}

/* Cry and die. */
void
fatal(s) char *s;
{
	fprintf(stderr, "apropos: %r\n", &s);
	exit(1);
}

/* end of apropos.c */
