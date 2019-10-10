/*
 * contents.c
 * 6/17/85
 * Usage:  contents [ infile [ outfile ] ]
 * Builds document table of contents from infile or standard input,
 * writes it to outfile or standard output.
 * Lines not beginning with '+' are ignored.
 * Contents lines are assumed to have the format "+<heading>=<page>".
 * The line "+" simply generates a blank line.
 * Revised 11/7/86 by fwb; removed code that counts digits in pages;
 * replaced with setting that right-justifies tab field.
 * Revised 11/7/90 by fwb; replaced output with a series of ".C?" macros,
 * to overcome problem with extra tables in the massaged output.
 * Revised 4/1/91 by fwb; added code to insert a \ before each '.
 */

#include <stdio.h>

#define	VERSION	"2.0"		/* version number */
#define	INLINE	120		/* maximum input line length */

extern	char	*index();
extern	char	*rindex();

/*
 * Globals.
 */
char	line[INLINE];		/* Input line. */
char	tmpline[INLINE];
main()
{
	char *secondfield, *thirdfield, *apos;

	printf(".NH \"Table of Contents\"\n");

	while (gets(line) != NULL) {
		if (line[0] != '+')
			continue;		/* Ignore non-contents lines */

		if (!strcmp(line, "+")) {
			printf("\n");		/* Generate blank line */
			continue;
		}

		/* see if the line contains an apostrophe.  If so, replace it */
		if ((apos = index(line, '\'')) != NULL) {
			strcpy(tmpline, apos+1);
			*apos = '\0';
			if (*(apos-1) == '\\')
				*(apos-1) = '\0';
			/* troff escape sequence for a literal apostrophe */
			strcat (line, "\\(aa");
			strcat (line, tmpline);
		}

		if ((thirdfield = rindex(line, '\t')) == NULL)
			fatal("no tab in line \"%s\"", line);

		*thirdfield++ = '\0';

		if ((secondfield = index(line, '\t')) == NULL)
			printf(".C%c \"%s\" %s\n",
				line[1], &line[2], thirdfield);
		else {
			*secondfield++ = '\0';
			printf(".C%c \"%s\" \"%s\" %s\n",
				line[1], &line[2], secondfield, thirdfield);
		}
	}
	printf("\.fi\n");
	exit(0);
}

/*
 * Print error message and exit.
 */
fatal(s)
char *s;
{
	fprintf (stderr, "contents:  %r\n", (char **)&s);
	exit(1);
}
/* end of contents.c */
