/*
 * man.c
 * 10/31/90
 * Usage: man [ -w ] [ topic ... ]
 * Quick and dirty man hack.
 * Read manual index, print manual sections to screen via $PAGER.
 *
 * 9/25/93:  changed slightly to permit user to include parentheses
 * in article names (fwb).
 * 10/05/93: doubled size of BBBIGBUF, so man will work with more articles.
 * 10/26/93: hacked to read all files with suffix ".index" in /usr/man
 * 10/26/93: fixed a bug whereby some files would be printed multiple times
 * 12/21/93: Fixed bug where an old-style index with a tab in it would fail.
 * 02/18/94: Changed usage message to point people to "apropos"
 * 02/18/94: Rewrote so it reads only one index file at a time.  This is to
 * 	     keep it from exhausting its internal space with too many files.
 * 04/23/94: Fixed bug in which the -w option printed a bogus error message.
 * 04/24/94: Fixed bug in which an index file that contained a blank line
 *	     caused program to hang.
 * 08/12/94: Re-inserted code to display all available manual pages.
 * 09/21/94: Added a '$' to the pnmatch() statement, to limit what index
 *	     files are searched.
 */

#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define	BBBIGBUF	80000			/* index buffer size	*/
#define	DEFPAGER	"exec /bin/scat -1"	/* default $PAGER	*/
#define	NBUF		160			/* buf[] buffer size	*/
#define	NCMD		512			/* cmd[] buffer size	*/
#define USAGE		"Usage: man [-wd] [article [ ... ] ]\n\
-w displays each article's path; -d displays all available articles.\n\
To search for an article on a given topic, use the command \"apropos\"."
#define	VERSION		"1.91"			/* version id		*/

extern	char	*getenv();

/* Globals. */
char	buf[NBUF];
char	cmd[NCMD];
char	manindex[BBBIGBUF];
char	*indexp;
int	wflag;

/* Forward. */
char	*tempnam();
void	cmdcat();
void	fatal();
char	*lookup();

main(argc, argv)
int argc; char *argv[];
{
	int	i, fd, found, nfound;
	char	*cp, *endptr, namebuffer[100];
	DIR	*dirp;
	struct dirent *dp;

	/* Look for environmental $PAGER. */
	if ((cp = getenv("PAGER")) == NULL)
		cp = DEFPAGER;
	strcpy(cmd, cp);

	/* Parse option args. */
	if (argc > 1 && strcmp(argv[1], "-w") == 0) {
		++wflag;
		--argc;
		++argv;
	}

	if (argc > 1 && strcmp(argv[1], "-V") == 0) {
		fprintf(stderr, "man: V%s\n", VERSION);
		--argc;
		++argv;
	}

	/* dump the list of all available Lexicon articles */
	if (argc > 1 && strcmp(argv[1], "-d") == 0) {
		system("ls -C /usr/man/* | more");
		exit(EXIT_SUCCESS);
	}

	/* If no args, print manual help information. */
	if (argc == 1) {
		fprintf(stderr, "%s\n", USAGE);
		exit(EXIT_SUCCESS);
	}

	/* read all files /usr/man */
	dirp = opendir("/usr/man");

	manindex[0] = '\0';
	while ((dp = readdir(dirp)) != NULL) {
		/* find all file names that end in .index */
		if (pnmatch(dp->d_name, "*.index$", 1) == 1) {
			sprintf(namebuffer, "/usr/man/%s", dp->d_name);
			if ((fd = open(namebuffer, O_RDONLY)) == -1)
				fatal("cannot open index file");
			if ((i = read(fd, manindex, BBBIGBUF)) == -1)
				fatal("cannot read index file");
			if (i >= BBBIGBUF)
				fatal("index file too large");
			close(fd);

			manindex[i] = '\0';

			/* search this index for each requested item */
			for (i = 1; i < argc; i++) {
				found = 0;

				/* lop parentheses off of requested item */
				if ((endptr = strrchr(argv[i], '(')) != NULL)
					*endptr = '\0';

				indexp = manindex;
				while ((cp = lookup(argv[i])) != NULL) {
					found++;
					nfound++;

					if (wflag)
						printf("/usr/man/%s\n", cp);
					else {
						cmdcat(" ");
						cmdcat("/usr/man/");
						cmdcat(cp);
					}
				}
			}
		}
	}
	closedir(dp);

	if (!wflag && nfound)
		system(cmd);
	else
		if (!wflag)
			fatal("Could not find requested article(s)");
	

	exit(EXIT_SUCCESS);
}

/*
 * Concatenate given string to cmd buffer.
 * Complain if too long.
 */
void
cmdcat(s) register char *s;
{
	register int len;

	len = strlen(cmd);
	if (len + strlen(s) + 1 >= NCMD) {
		printf("cmd=%s\ns=%s\nlen=%d\n", cmd, s, len);
		fatal("command buffer overflow");
	}
	strcpy(&cmd[len], s);
}

/* Cry and die. */
void
fatal(s) char *s;
{
	fprintf(stderr, "man: %r\n", &s);
	exit(EXIT_FAILURE);
}

/*
 * Look up string s in helpfile index
 * Return name of file on match, else NULL.
 */
char *
lookup(s) char *s;
{
	register char *namep, *next, *endptr;

	while ((next = strchr(indexp, '\n')) != NULL) {
		if (next - indexp + 1 >= NBUF)
			fatal("index buffer overflow");
		strncpy(buf, indexp, next - indexp);

		/* crop off description - fwb, 4/13/93 */
		if ((endptr = strchr(buf, '\t')) != NULL) {
			/* description is after second tab */
			if ((endptr = strchr(endptr+1, '\t')) != NULL)
				*endptr = '\0';
			else
				buf[next-indexp] = '\0';
		} else
			buf[next-indexp] = '\0';

		/* crop terminal parentheses from index entry */
		if ((endptr = strrchr(buf, '(')) != NULL)
			*endptr = '\0';

		indexp = next + 1;		/* bump to next index line */

		/* split the article name from the file name. if an entry
		   contains neither a tab nor a space, it's bogus */
		if ((namep = strchr(buf, '\t')) == NULL)
			if ((namep = strrchr(buf, ' ')) == NULL)
				continue;

		*namep++ = '\0';	/* NUL-terminate index entry */

		/* crop terminal parentheses from user's request */
		if ((endptr = strrchr(namep, '(')) != NULL)
			*endptr = '\0';

		if (strcmp(namep, s) == 0)
			return buf;		/* gotcha */
	}
	return NULL;				/* no match */
}
/* end of man.c */
