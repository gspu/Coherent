/*
 * Reverse the order of the characters
 * in every line of every file (or the
 * standard input if no files are given).
 */

#include <stdio.h>

#define	MAXLINE	1000

main(argc, argv)
char *argv[];
{
	FILE *fp;
	register int i;

	if (argc < 2)
		rev(stdin);
	else for (i=1; i<argc; i++)
		if ((fp = fopen(argv[i], "r")) == NULL)
			fprintf(stderr, "Cannot open `%s'\n", argv[i]);
		else {
			rev(fp);
			fclose(fp);
		}
	exit(0);
}

/*
 * Reverse the characters in every
 * line of the file described by
 * the given I/O stream.
 */
rev(stream)
FILE *stream;
{
	register char *ep;
	register c;
	static char line[MAXLINE];

	do {
		ep = line;
		while ((c=getc(stream))!='\n' && c!=EOF)
			*ep++=c;
		while (ep > line)
			putchar(*--ep);
		if (c=='\n')
			putchar('\n');
	} while (c != EOF);
}
