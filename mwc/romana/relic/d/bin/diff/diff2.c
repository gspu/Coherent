/*
 * This code is common between diff and /usr/lib/diffh.
 * It is a restructuring and re-write of the earlier
 * Tom Duff code so that reading options, opening files,
 * and outputing of the differences is common.
 */

#include "diff.h"
#ifdef COHERENT
#include <sys/stat.h>
#include <signal.h>
#else
#include <sys/types.h>
#endif
#include <path.h>

int	dflag;			/* Dynamic decision about -h or not */
int	bflag;			/* Strip trailing blanks, multiple blanks */
int	eflag;			/* Editor script */
int	sflag;			/* Sed script output (used by SCCS) */
int	rflag;			/* Reversed order of printed (usually -e) */
int	nflag;			/* Trailing '.' on edit script */
int	incr;			/* Line number increment for `rflag' diff */
char	*csymbol;		/* Symbol for C pre-processor (#ifdef) */
int	sepflag;		/* On if text from both files involved */
int	caflag;			/* On for a change or append */
unsigned ninsert;		/* Number of lines inserted */
unsigned ndelete;		/* Number of lines deleted */
unsigned nunchanged;		/* Number of line unchanged */
char	*tmpnam1, *tmpnam2;
FILE	*fp1;			/* First input stream */
FILE	*fp2;			/* Second input stream */
char	*fn1, *fn2;		/* Filenames */

char	buf1[BUFSIZ];
char	buf2[BUFSIZ];

FILE	*openfile();
int	rmexit();
int	catchsig();
int	streq();
int	bstreq();

int	(*equal)() = streq;

main(argc, argv)
int argc;
char *argv[];
{
	register char *ap;
	register char **sargv = argv;
	fsize_t size1, size2;

	while (argc>1 && argv[1][0]=='-' && argv[1][1]!='\0') {
		for (ap = &argv[1][1]; *ap != '\0'; ap++)
			switch (*ap) {
			case 'b':
				bflag = 1;
				equal = bstreq;
				break;

			case 'c':
				if (argc < 3)
					usage();
				csymbol = argv[2];
				argv++;
				argc--;
				break;

			case 'd':
				dflag = 1;
				break;

			case 'e':
				eflag = rflag = 1;
				break;

			case 'f':
				eflag = 1;
				rflag = 0;
				break;

			case 'n':
				eflag = nflag = 1;
				rflag = 0;
				break;

			case 's':
				eflag = sflag = 1;
				break;

			case 'h':
				diffh(sargv);
				break;

			default:
				usage();
			}
		argc--;
		argv++;
	}
	if (argc != 3)
		usage();
	if (eflag && csymbol!=NULL)
		cerr("-e and -c are incompatible options");
#ifdef COHERENT
	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		signal(SIGINT, catchsig);
	if (signal(SIGHUP, SIG_IGN) != SIG_IGN)
		signal(SIGHUP, catchsig);
	if (signal(SIGTERM, SIG_IGN) != SIG_IGN)
		signal(SIGTERM, catchsig);
#endif
	fp1 = openfile(argv[1], argv[2], &tmpnam1, &size1);
	fp2 = openfile(argv[2], argv[1], &tmpnam2, &size2);
	if (dflag && (size1>NBIGF || size2>NBIGF))
		diffh(argv);
	setbuf(fp1, buf1);
	setbuf(fp2, buf2);
	fn1 = argv[1];
	fn2 = argv[2];
	diff(fp1, fp2);
	if (sflag)
		printf(":I%d\n:D%d\n:U%d\n", ninsert, ndelete, nunchanged);
	rmexit(ninsert || ndelete);
}

/*
 * Compare input lines for equality.
 */
bstreq(s1, s2)
register char *s1, *s2;
{
	for (;;) {
		if (*s1==' ' || *s1=='\t') {
			do {
				s1++;
			} while (*s1==' ' || *s1=='\t');
			if (*s1 == '\n') {
				while (*s2==' ' || *s2=='\t')
					s2++;
				return (*s2 == '\n');
			}
			if (*s2!=' ' && *s2!='\t')
				return (0);
			do {
				s2++;
			} while (*s2==' ' || *s2=='\t');
		} else {
			if (*s1 == '\n')
				while (*s2==' ' || *s2=='\t')
					s2++;
			if (*s1++ != *s2)
				return (0);
			if (*s2++ == '\n')
				return (1);
		}
	}
}

/*
 * Open the files, allowing for special standard input
 * convention (`-') and directories.  Copy any special
 * files or pipes to a tempfile because they cannot be
 * rewound.
 * `ofn' is the other name, for directory concatenation.
 */
FILE *
openfile(fn, ofn, tfnp, sizep)
char *fn;
char *ofn;
char **tfnp;
fsize_t *sizep;
{
	register FILE *fp;
#ifdef COHERENT
	register char *cp;
	register int c;
	register FILE *tfp;
	char fname[300];
	struct stat sb;
	static int dirflag;

again:
#endif
	if (fn[0]=='-' && fn[1]=='\0')
		fp = stdin;
	else if ((fp = fopen(fn, "r")) == NULL)
		cerr("cannot open `%s'", fn);
#ifdef COHERENT
	fstat(fileno(fp), &sb);
	*sizep = sb.st_size;
	switch (sb.st_mode & S_IFMT) {
	case S_IFDIR:
		if (dirflag)
			break;
		dirflag++;
		for (cp = ofn; *cp != '\0'; cp++)
			;
		while (cp > ofn)
			if (*--cp == PATHSEP) {
				cp++;
				break;
			}
		sprintf(fname, "%s%c%s", fn, PATHSEP, cp);
		fn = fname;
		goto again;

	case S_IFREG:
		break;

	default:
		*tfnp = "/tmp/difXXXXXX";
		if ((tfp = fopen(mktemp(*tfnp), "w")) == NULL)
			cerr("cannot create tempfile");
		setbuf(fp, buf1);
		setbuf(tfp, buf2);
		while ((c = getc(fp)) != EOF)
			putc(c, tfp);
		fflush(tfp);
		if (ferror(tfp))
			cerr("write error on tempfile");
		fclose(tfp);
		if (fp != stdin)
			fclose(fp);
		if ((fp = fopen(*tfnp, "r")) == NULL)
			cerr("cannot re-open tempfile");
	}
#endif
	return (fp);
}

/*
 * Output a text line common to both files.
 * This is only used by the `-c' option.
 */
text(line)
char *line;
{
	++nunchanged;
	if (csymbol != NULL)
		fputs(line, stdout);
}

/*
 * Output a text line from the first file.
 */
text1(line)
register char *line;
{
	if (csymbol != NULL)
		fputs(line, stdout);
	else if (!eflag) {
		fputs("< ", stdout);
		fputs(line, stdout);
	}
	++ndelete;
}

/*
 * Output a line that is only in
 * the second file.
 */
text2(line)
register char *line;
{
	if (sflag) {
		putchar('\\');
		putchar('\n');
#if REALSED
		/*
		 * Sed is documented to strip
		 * leading blanks/tabs in appended
		 * text but doesn't.  This appears
		 * to be a feature rather than a bug.
		 */
		if (line[0]==' ' || line[0]=='\t')
			putchar('\\');
#endif
		while (*line != '\n') {
			if (*line == '\\')
				putchar(*line);
			putchar(*line++);
		}
	} else if (eflag) {
		if (!nflag && line[0]=='.' && line[1]=='\n' && line[2]=='\0')
			fputs("~\n.\ns/~/./\na\n", stdout);
		else
			fputs(line, stdout);
	} else if (csymbol != NULL)
		fputs(line, stdout);
	else {
		fputs("> ", stdout);
		fputs(line, stdout);
	}
	caflag = 1;
	++ninsert;
}

/*
 * Output the start of a set of
 * changed lines.
 * Change is from `f1',`f2' (other file is `t1,t2').
 */
change(f1, f2, t1, t2)
unsigned f1, f2, t1, t2;
{
	sepflag = 1;
	if (csymbol != NULL)
		printf("#ifndef %s\n", csymbol);
	else if (nflag) {
		printf("d%u %u\n", f1+incr, f2-f1+1);
		printf("a%u %u\n", f1+incr+f2-f1, t2-t1+1);
	} else {
		prange(f1, f2);
		if (sflag)
			putchar('c');
		else if (eflag)
			fputs("c\n", stdout);
		else {
			fputs(" c ", stdout);
			prange(t1, t2);
			putchar('\n');
		}
	}
	if (rflag)
		incr += t2-t1 + f1-f2;
}

/*
 * Output a set of appended lines.
 * Append after `f' the lines in
 * range `t1,t2'.
 */
append(f, t1, t2)
unsigned f, t1, t2;
{
	f--;
	if (csymbol != NULL)
		printf("#ifdef %s\n", csymbol);
	else if (nflag)
		printf("a%u %u\n", f+incr, t2-t1+1);
	else {
		prange(f, f);
		if (sflag)
			putchar('a');
		else if (eflag) {
			fputs("a\n", stdout);
		} else {
			fputs(" a ", stdout);
			prange(t1, t2);
			putchar('\n');
		}
	}
	if (rflag)
		incr += t2-t1+1;
}

/*
 * Output a set of deleted lines.
 * Delete range `f1,f2' (other file
 * number is `t'.
 */
delete(f1, f2, t)
unsigned f1, f2, t;
{
	if (csymbol != NULL)
		printf("#ifndef %s\n", csymbol);
	else if (nflag)
		printf("d%u %u\n", f1+incr, f2-f1+1);
	else {
		prange(f1, f2);
		if (eflag)
			fputs("d\n", stdout);
		else
			printf(" d %u\n", t-1);
	}
	if (rflag)
		incr -= f2-f1+1;
}

/*
 * Output the middle of a set of
 * changed lines.
 */
prsep()
{
	if (!sepflag)
		return;
	sepflag = 0;
	if (csymbol != NULL)
		fputs("#else\n", stdout);
	else if (!eflag)
		fputs("---\n", stdout);
}

/*
 * Output the end part of a set
 * of changed lines.
 */
prend()
{
	if (csymbol != NULL)
		fputs("#endif\n", stdout);
	else if (caflag) {
		if (sflag)
			putchar('\n');
		else if (eflag && !nflag)
			fputs(".\n", stdout);
	}
	caflag = 0;
}

/*
 * Print a line range either in the form
 * `a,b' or `a' (if a==b).
 */
prange(a, b)
register unsigned a, b;
{
	a += incr;
	b += incr;
	if (a == b)
		printf("%u", a);
	else
		printf("%u,%u", a, b);
}

/*
 * Exit, removing any possible temmpfiles.
 */
rmexit(s)
{
	if (tmpnam1 != NULL)
		unlink(tmpnam1);
	if (tmpnam2 != NULL)
		unlink(tmpnam2);
	exit(s);
}

/*
 * Catch signal and exit.
 */
catchsig()
{
	rmexit(2);
}
usage()
{
	fprintf(stderr, "Usage: diff [-bdefhns] [-c symbol] file1 file2\n");
	exit(2);
}

/* VARARGS */
cerr(x)
{
	fprintf(stderr, "diff: %r\n", &x);
	exit(2);
}
