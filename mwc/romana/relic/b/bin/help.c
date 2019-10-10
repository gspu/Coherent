/*
 * help.c
 * 9/2/93
 * Usage: help [ -dc ] [ -ffile ] [ -ifile] [ -r ] [ topic ] ...
 * Options:
 *	-dc	Use c as delimiter character (default: '@')
 *	-ffile	Use file as helpfile (default: /etc/helpfile)
 *	-iindex	Use index as helpindex (default: /etc/helpindex)
 *	-r	Rebuild helpindex and exit
 * Print command help information as given in HELPFILE.
 * Also looks for user-specific information in file $HELP.
 * Pipes output through $PAGER if defined.
 */

#include <stdio.h>
#include <sys/stat.h>

/* Externals. */
char	*getenv();

/* Manifest constants. */
#define	VERSION		"1.6"			/* Version number	*/
#define	NLINE		512			/* Longest helpfile line*/
#define	DELIM		'@'			/* Default delimiter	*/
#define	HELPFILE	"/etc/helpfile"		/* Default helpfile	*/
#define	HELPINDEX	"/etc/helpindex"	/* Default helpindex	*/
#define	MAXNAME		80			/* Max name length	*/
#define	USAGE		"Usage: help [ -dc ] [ -ffile ] [ -ifile] [ -r ] [ topic ] ...\n"

/* Structure to speed lookup time. */
typedef	struct	look	{
	long	l_seek;				/* seek into helpfile	*/
	char	*l_name;			/* topic		*/
} LOOK;

/* Globals. */
char	buf[BUFSIZ];			/* Input buffer			*/
char	delim = DELIM;			/* Help file delimiter		*/
char	*helpfile = HELPFILE;		/* Help file			*/
char	*helpindex = HELPINDEX;		/* Help index			*/
char	helpline[NLINE];		/* Help file text line		*/
FILE	*ofp = NULL;			/* Output file			*/
int	rflag;				/* Rebuild helpindex		*/
FILE	*shf;				/* System helpfile		*/
FILE	*uhf;				/* User helpfile		*/

/* Forward. */
int	help();
int	lookup();
LOOK	*lread();
int	rebuild();
void	usage();

main(argc, argv) int argc; char *argv[];
{
	register char **ap;
	register int status;
	char *cp;
	char *list[2];

	/* Process command line options. */
	while (argc > 1 && argv[1][0] == '-') {
		switch(argv[1][1]) {
		case 'd':	delim = argv[1][2];		break;
		case 'f':	helpfile = &argv[1][2];		break;
		case 'i':	helpindex = &argv[1][2];	break;
		case 'r':	rflag++;			break;
		case 'V':
			fprintf(stderr, "help: V%s\n", VERSION);
			break;
		default:	usage();			break;
		}
		--argc;
		++argv;
	}

	/* Open files and read environmental variables. */
	shf = fopen(helpfile, "r");		/* system helpfile */
	if (shf == NULL) {
		fprintf(stderr, "help: cannot open \"%s\"\n", helpfile);
		exit(1);
	}
	if (rflag)
		exit(rebuild(shf, helpindex));
	if ((cp = getenv("HELP")) != NULL)
		uhf = fopen(cp, "r");		/* user helpfile */
	if ((cp = getenv("PAGER")) != NULL && *cp != '\0' && isatty(fileno(stdout)))
		ofp = popen(cp, "w");
	if (ofp == NULL) {
		ofp = stdout;
		setbuf(stdout, buf);
	}
	ap = ++argv;

	/* If no args, print help info and LASTERROR info. */
	status = 0;
	if (argc < 2) {
		fprintf(stderr,
"The 'help' command prints a brief description of the usage of a command.\n"
"For example, to get information about the 'who' command, type:\n"
"\thelp who\n"
"The 'man' command provides more detailed descriptions of commands.\n"
"\n"
			);
		if ((list[0] = getenv("LASTERROR")) == NULL)
			exit(status);
		list[1] = NULL;
		ap = list;
	}

	/* Print help info on requested topics. */
	while (*ap != NULL) {
		if (help(*ap)) {
			fprintf(stderr, "help: no information on %s\n", *ap);
			status = 1;
		}
		if (*++ap != NULL)
			fputc('\n', ofp);
		fflush(ofp);
	}

	/* Done, clean up. */
	if (ofp != stdout)
		pclose(ofp);
	if (shf != NULL)
		fclose(shf);
	if (uhf != NULL)
		fclose(uhf);
	exit(status);
}

/*
 * Look for information on a command.
 * Return 0 if found, 1 if not found.
 */
int
help(command) char *command;
{
	register int bad;
	if (bad = lookup(command, shf, helpindex))
		bad = lookup(command, uhf, NULL);
	return bad;
}

/*
 * Lookup a command in the given file by looking for DELIM-delimited lines.
 * The indexfile speeds up lookup when there is a very large system help file.
 * Return 0 if found, 1 if not found.
 */
int
lookup(cmd, fp, ind) register char *cmd; FILE *fp; char *ind;
{
	register LOOK *lp;
	register FILE *ifp;
	struct stat sb;
	long htime;
	int status;

	if (fp == NULL)
		return 1;

	fstat(fileno(fp), &sb);
	htime = sb.st_mtime;			/* helpfile mtime */
	if (stat(ind, &sb) < 0 || htime > sb.st_mtime)
		rebuild(fp, ind);
	status = 1;

	if ((ifp = fopen(ind, "r")) == NULL)
		return status;

	while ((lp = lread(ifp)) != NULL) {
		if (strcmp(cmd, lp->l_name) == 0) {
			fseek(fp, lp->l_seek, SEEK_SET);
			status = 0;

			fgets(helpline, NLINE, fp);
			while (fgets(helpline, NLINE, fp) != NULL) {
				if (helpline[0] == delim)
					break;
				fputs(helpline, ofp);
			}
			fputs("\n", ofp);
		}
	}
	fclose(ifp);
	return status;
}

/*
 * Read in a look structure.
 * Return NULL on end of file or error.
 */
LOOK *
lread(fp) register FILE *fp;
{
	register char *cp;
	static LOOK look;
	static char name[MAXNAME];

	if (fread(&look.l_seek, sizeof look.l_seek, 1, fp) != 1)
		return NULL;
	for (look.l_name = cp = name; (*cp = getc(fp)) != '\0'; ) {
		if (feof(fp))
			return NULL;
		else if (cp < &name[MAXNAME-1])
			++cp;
	}
	return &look;
}

/*
 * Rebuild an index file.
 * Return 0 on success, 1 on failure.
 */
int
rebuild(fp, ind) FILE *fp; char *ind;
{
	register FILE *ifp;
	register char *cp, *ep;
	long seek;

	if ((ifp = fopen(ind, "w")) == NULL)
		return 1;
	for (;;) {
		seek = ftell(fp);
		cp = helpline;
		if (fgets(cp, NLINE, fp) == NULL)
			break;
		if (*cp++ != delim)
			continue;		/* line must start with delim */
		while (*cp==' ' && *cp=='\t')
			cp++;			/* skip whitespace after delim */
		ep = cp;
		while (*ep!='\n' && *ep!='\0' && *ep!=' ' && *ep!='\t')
			ep++;			/* scan past keyword */
		*ep = '\0';

		/* Write index entry: seek and NUL-terminated keyword. */
		fwrite(&seek, 1, sizeof seek, ifp);
		fputs(cp, ifp);
		fputc('\0', ifp);
	}
	fclose(ifp);
	return 0;
}

/*
 * Print usage message and die.
 */
void
usage()
{
	fprintf(stderr, USAGE);
	exit(1);
}

/* end of help.c */
