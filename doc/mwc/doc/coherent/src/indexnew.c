/*
 * index.c
 * 3/22/91
 * Usage:  index [ infile [ outfile ] ]
 * Reads raw document index from infile or standard input,
 * writes [nt]roff output to outfile or standard output.
 * Each input line should have the format "%<key>[^<subkey>]=<ref>".
 * Comments on routines input(), output(), parse() give more detail
 * about input and output format.
 *
 * 6/3/93 - Changed output to conform to use new macros in "tmac.cohindex"
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>

/* External. */
extern	char	*mktemp();

/* Manifest constants. */
#define	VERSION	"2.2"			/* version number		*/
#define	MAXLINE	512			/* maximum input line length	*/
#define	SEP	'\001'			/* field separator		*/
#define	CARAT	'\002'			/* replacement for '^' for compare */
#define	IXTMP	"/tmp/ixXXXXXX"		/* tmpname prototype		*/
#define	USAGE	"Usage:\tindex [ -v ] [ infile [ outfile ] ]\n"

/* Forward. */
void	fatal();
void	filclose();
FILE	*filopen();
void	flush();
void	input();
void	output();
void	parse();
void	usage();

/* Output string at head of output file */
#define INXHEAD	\
".ce\n"\
"\\fBIndex\\f(RS\n"\
"\n"\
".ce\n"\
"\\fB# to _\\f(RS\n"\
"\n"

/* Globals. */
char	key[MAXLINE];			/* Index key, cooked.		*/
char	keyraw[MAXLINE];		/* Index key, raw.		*/
char	keyref[MAXLINE];		/* Index reference.		*/
char	keysec[MAXLINE];		/* Index secondary key.		*/
char	line[MAXLINE];			/* Input line buffer.		*/
int	ninput;				/* Number of index entries.	*/
int	nlines;				/* Number of input lines.	*/
int	noutput;			/* Number of output lines.	*/
char	tmp1[L_tmpnam];			/* Temporary filename.		*/
char	tmp2[L_tmpnam];			/* Temporary filename.		*/
int	vflag;				/* Verbose.			*/

main(argc, argv) int argc; char *argv[];
{
	register char *s;

	/* Process command line options. */
	while (argc > 1 && argv[1][0] == '-') {
		for (s = &argv[1][1]; *s; ++s) {
			switch(*s) {
			case 'v':
				++vflag;
				break;
			case 'V':
				fprintf(stderr, "index:  V%s\n", VERSION);
				break;
			default:
				usage();
			}
		}
		++argv;
		--argc;
	}
	if (argc > 3)
		usage();

	/* Construct temporary file names */
	if ((s = tmpnam(tmp1)) == NULL)
		fatal("cannot make first temporary file name");

	if ((s = tmpnam(tmp2)) == NULL)
		fatal("cannot make second temporary file name");

	/* Read input file, write sortable lines to tmp1. */
	input((argc > 1) ? argv[1] : NULL);

	/* Sort tmp1 to tmp2. */
	sprintf(line, "sort %s >%s", tmp1, tmp2);
	if (system(line) != 0)
		fatal("command \"%s\" failed", line);

	/* Read sorted tmp2, write output file. */
	output((argc > 2) ? argv[2] : NULL);

	/* Print verbose information if desired. */
	if (vflag) {
		fprintf(stderr, "%d total input lines\n", nlines);
		fprintf(stderr, "%d index input lines\n", ninput);
		fprintf(stderr, "%d output lines\n", noutput);
	}

	/* Done, remove tmps and exit. */
	unlink(tmp1);
	unlink(tmp2);
	exit(0);
}

/*
 * Print error message and exit.
 */
/* VARARGS */
void
fatal(s) char *s;
{
	fprintf(stderr, "index: %r\n", &s);
	if (tmp1[0] != '\0')
		unlink(tmp1);
	if (tmp2[0] != '\0')
		unlink(tmp2);
	exit(1);
}

/*
 * Close a file.
 */
void
filclose(fp, msg) FILE *fp; char *msg;
{
	if (fclose(fp) == EOF)
		fatal("cannot close %s file", msg);
}

/*
 * Open a file.
 */
FILE *
filopen(name, mode) char *name, *mode;
{
	register FILE *fp;

	if ((fp = fopen(name, mode)) == NULL)
		fatal("cannot open file \"%s\"", name);
	return fp;
}

/*
 * Flush out an index entry.
 */
void
flush(ofp, first, last) FILE *ofp; int first, last;
{
	if (first == last)
		fprintf(ofp, "%d", last);
	else
		fprintf(ofp, "%d-%d", first, last);
}

/*
 * Read input file.
 * Lines not beginning with '%' are ignored.
 * Lines beginning with '%' are parsed by parse().
 * The result is sorted by key first (case- and font-insensitive),
 * secondarily by keyraw (to restore case and font information),
 * and thirdly by keyref.
 */
void
input(file) char *file;
{
	register FILE *ifp, *ofp;

	ifp = (file == NULL) ? stdin : filopen(file, "r");
	ofp = filopen(tmp1, "w");

	while (fgets(line, MAXLINE-1, ifp) != NULL) {
		nlines++;			/* Bump line count. */
		if (line[0] != '%')
			continue;		/* Ignore non-index lines. */
		ninput++;			/* Bump entry count. */
		parse();			/* Parse the line. */
		if (*keyraw == '\0') {
			fprintf(stderr, "index: Warning: line %d ignored (empty key)\n", nlines);
			continue;
		}
		fprintf(ofp, "%s%c%s%c%s\n", key, SEP, keyraw, SEP, keyref);
	}

	if (file != NULL)
		filclose(ifp, "input");
	filclose(ofp, "output");
}

/*
 * Write output file.
 * The output consists of:
 *	1)  The index header INXHEAD, and
 *	2)  The sorted index
 */
#define	S_CLEAN	0			/* no references yet		*/
#define	S_NUM	1			/* numeric references buffered	*/
#define	S_ALPHA	2			/* in non-numeric references	*/
void
output(file) char *file;
{
	register FILE *ifp, *ofp;
	register char *s, *cooked, *raw, *ref, *sec;
	int first, last, this, state, letter, newkey;

	ifp = filopen(tmp2, "r");
	ofp = (file == NULL) ? stdout : filopen(file, "w");

	fprintf(ofp, "%s", INXHEAD);
	key[0] = keysec[0] = '\0';
	state = S_CLEAN;
	letter = 0;

	while (fgets(line, MAXLINE-1, ifp) != NULL) {
		/* Break input line into key, raw, ref, sec. */
		cooked = line;
		line[strlen(line)-1] = '\0';		/* zap trailing \n */

		if ((s = strchr(cooked, SEP)) == NULL)
			fatal("no first SEP, line=%s", line);
		else
			raw = s + 1;
		if ((s = strchr(raw, SEP)) == NULL)
			fatal("no second SEP, line=%s", line);
		else
			ref = s + 1;
		*(raw-1) = *(ref-1) = '\0';
		if ((sec = strchr(raw, CARAT)) != NULL)
			*sec++ = '\0';

		/* Output line information, buffering as needed. */
		if (strcmp(key, raw) != 0
				|| (sec != NULL && strcmp(keysec, sec) != 0)) {
			/* Flush previous. */
			if (state == S_NUM) {
				flush(ofp, first, last);
				fprintf(ofp, "\"\n");
			} else
				fputc('\n', ofp);
			state = S_CLEAN;

			if (*cooked != letter) {
				/* Separate sections by first letter. */
				letter = *cooked;

				if (letter >= 'a' && letter <= 'z')
					fprintf(ofp, "\n.ce\n\\fB%c\\f(RS\n\n",
						toupper(letter));
				else
					fputc('\n', ofp);
			}
			++noutput;
			if (strcmp(key, raw) != 0) {
				/* New key. */
				strcpy(key, raw);
				fflush(ofp);
				fprintf(ofp, ".I1 \"%s\" ", key);
				newkey = 1;
			} else
				newkey = 0;
			if (sec != NULL) {
				/* Secondary key. */
				strcpy(keysec, sec);
				if (newkey)
					fprintf(ofp, "\n");
				fprintf(ofp, ".I2 \"%s\" ", sec);
			}
/* HUH? */
/*
			  else
				sec[0] = '\0';
*/
		}

		/* Put out or buffer reference. */
		this = atoi(ref);
		if (state == S_CLEAN) {

			state = S_NUM;
			first = last = this;
			fprintf(ofp, "\"");
		} else if (state == S_NUM) {

			if (this == last)
				continue;	/* same as last, ignore */
			else if (this == last + 1) {
				last = this;	/* add to range */
				continue;
			}
			flush(ofp, first, last);
			fprintf(ofp, ", ");
			first = last = this;
		}
	}
	flush(ofp, first, last);
	fprintf(ofp, "\"\n");

	filclose(ifp, "temporary");
	if (file != NULL)
		filclose(ofp, "output");
}

/*
 * Parse input line from line[] into keyraw, key, keyref.
 * The input line must have the format "%<key>[^<subkey>]=<ref>".
 * keyraw is "<key>[^<subkey>]" portion, which may contain font changes.
 * key is the "cooked" version of keyraw used as primary sort key,
 * with case and font information mapped out (so "a", "Ab" and \fBabc\fR"
 * sort in that order) and non-ASCII characters stripped.
 * '^' in keyraw not followed by another '^' indicates a secondary index
 * entry and is replaced here by CARAT; '^^' in keyraw is replaced by '^'.
 * key is keyraw, "cooked" as follows:
 * keyref is the reference; numeric <ref>s are 0-padded to four digits
 * in keyref so that they sort correctly.
 */
void
parse()
{
	register char *cp;
	register char c;

	/* Zap trailing newline. */
	if ((cp = strchr(line, '\n')) != NULL)
		*cp = '\0';

	/* Sanity check. */
	if (strchr(line, SEP) != NULL)
		fatal("SEP in line %d: \"%s\"", nlines, line);
	else if (strchr(line, CARAT) != NULL)
		fatal("CARAT in line %d: \"%s\"", nlines, line);

	/* Strip unwanted characters. */
	for (cp = &line[1]; c = *cp; cp++) {
		if (!isascii(c) || iscntrl(c)) {
			fprintf(stderr, "index: Warning: line %d: nonASCII character 0x%02x removed\n", nlines, c & 0xFF);
			memcpy(cp, cp+1, strlen(cp));
			--cp;
		}
	}

	/* Replace "^^" with "^", "^" with CARAT. */
	for (cp = &line[1]; c = *cp; cp++)
		if (c == '^') {
			if (*(cp+1) == '^')	/* Replace '^^' with '^'. */
				memcpy(cp, cp+1, strlen(cp));
			else
				*cp = CARAT;	/* Replace '^' with CARAT. */
		}

	/* Set keyraw[], keyref[]. */
	if ((cp = strrchr(line, '=')) == NULL)
		fatal("missing '=' in line %d: \"%s\"", nlines, line);

	*cp++ = '\0';				/* Truncate raw key at '='. */
	strcpy(keyraw, &line[1]);		/* Raw key without '%'. */
	if (isdigit(*cp))
		sprintf(keyref, "%04d", atoi(cp)); /* Prepend leading 0s for sort */
	else
		strcpy(keyref, cp);		/* Nonnumeric key reference. */

	/* Cook keyraw[] to key[]. */
	strcpy(key, keyraw);
	for (cp = key; c = *cp; cp++) {
		if (isupper(c))
			*cp = tolower(c);	/* Convert UPPER to lower. */
		else if (c == '\\' && *(cp+1) == 'f') {
			memcpy(cp, cp+3, strlen(cp)-2);	/* Remove font escapes. */
			--cp;
		}
	}
}

/*
 * Print usage message and exit.
 */
void
usage()
{
	fprintf(stderr, USAGE);
	exit(1);
}

/* end of index.c */
