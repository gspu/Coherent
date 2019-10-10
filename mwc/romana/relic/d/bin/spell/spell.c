/*
 * Version of spell that uses compressed
 * dictionary but exhaustive dictionary read.
 * This version will work in any address space
 * but is perhaps marginally slower (on short
 * documents than the hashed version). It has the
 * virtue that it is guaranteed accurate (no misses).
 */

#include <stdio.h>
#include <ctype.h>

#define	NWORD	50

char	inword[NWORD];
char	lowword[NWORD];

char	*wget();
char	*getword();

main(argc, argv)
char *argv[];
{
	FILE *dfp;
	FILE *hfp = NULL;

	if (argc == 3) {
		argc--;
		hfp = fopen(argv[2], "a");
	}
	if (argc != 2)
		usage();
	if ((dfp = fopen(argv[1], "r")) == NULL)
		cerr("cannot open compressed dictionary `%s'", argv[1]);
	exit(spell(stdin, dfp, hfp));
}

/*
 * Called with the input words (sorted)
 * the the dictionary file pointer.
 * Returns non-zero if there are any spelling errors.
 */
spell(inf, dfp, hfp)
FILE *inf;
FILE *dfp;
FILE *hfp;
{
	register char *wp = "";
	register int s;
	register int stat = 0;

	for (;;) {
		if (wget(inword, lowword, inf) == NULL)
			return (stat);
		while ((s = strcmp(wp, lowword)) < 0)
			if ((wp = getword(dfp)) == NULL)
				return (stat);
		if (s != 0) {
			stat = 1;
			printf("%s\n", inword);
			if (hfp != NULL)
				fprintf(hfp, "%s\n", inword);
		}
	}
}

char *
wget(b1, b2, io)
char *b1, *b2;
FILE *io;
{
	register int c;
	register int lim;
	register char *s1;
	register char *s2;

again:
	s1 = b1;
	s2 = b2;
	lim = NWORD;
	while (--lim>0 && (c = getc(io))!=EOF) {
		if (c == '\n')
			break;
		*s1++ = c;
		*s2++ = isalpha(c) ? tolower(c) : c;
	}
	*s1 = '\0';
	*s2 = '\0';
	if (c == EOF)
		return (NULL);
	while (c != '\n')
		c = getc(io);
	if (s2 == b2)
		goto again;
	return (b1);
}

usage()
{
	fprintf(stderr, "Usage: spell dict [history]\n");
	exit(1);
}

/* VARARGS */
cerr(x)
{
	fprintf(stderr, "spell: %r\n", &x);
	exit(1);
}
