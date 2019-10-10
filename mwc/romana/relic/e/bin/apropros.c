/*
 * apropos.c
 *
 * implement a simple version of the Berkeley command "apropos".  This
 * command searches the file /usr/man/man.index for a word that the
 * user enters on the command line, and prints every index entry in which
 * that token appears.
 *
 * Usage:  apropos token
 *
 * first draft, fwb, 3/17/1993 (aye and begorah!)
 */
#ifdef TEST
#	define INDEXFILE "/v/fwb/doc/coherent/man.index"
#else
#	define INDEXFILE "/usr/man/man.index"
#endif

#define BUFFERSIZE 200

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

main(argc, argv)
int argc; char **argv;
{
	char inb[BUFFERSIZE], testb[BUFFERSIZE], *inbp;
	int i, j, lowerflag;
	FILE *fp;

	if (argc < 2) {
		fprintf(stderr, "Usage: apropos word\n");
		exit(EXIT_FAILURE);
	}

	if ((fp = fopen(INDEXFILE, "r")) == NULL) {
		fprintf(stderr, "apropos: Cannot open index file %s\n",
			INDEXFILE);
		exit(EXIT_FAILURE);
	}

	/* if first character of user's word is upper-cased, then assume
	 * the user wants an exact match, and therefore do not
	 * shift the description into lower case.
	 */

	if (isupper(argv[1][0]))
		lowerflag = 0;
	else
		lowerflag = 1;

	for (i = 1; fgets(inb, BUFFERSIZE-1, fp) != NULL; i++) {
		if ((inbp = strchr(inb, '\t')) == NULL) {
			/* blank lines are OK */
			if (*inb != '\n' && *inb != '\0') {
				fprintf(stderr,
					"apropos: badly formed line %d in %s\n",
					i, INDEXFILE);
				exit(EXIT_FAILURE);
			} else
				continue;
		}
		inbp++;

		strcpy (testb, inbp);

		if (lowerflag)
			for (j = 0; j < strlen (testb); j++)
				if (isupper(testb[j]))
					testb[j] = tolower(testb[j]);

		if (strstr(testb, argv[1]) != NULL)
			printf("%s", inbp);
	}
	fclose(fp);
	exit(EXIT_SUCCESS);
}

