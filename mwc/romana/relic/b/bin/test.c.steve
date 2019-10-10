/*
 * cmd/test.c
 * 5/4/93
 * Set status based on specified conditions, mostly related to files.
 * Used for control flow in shell scripts.
 * Cf. POSIX P1003.2/D11.2 section 4.62; extensions marked !POSIX below.
 * Usage: see usage() below.
 * Exit status:
 *	0	expression is true
 *	1	expression is false or absent
 *	2	syntax error or other error
 * This used to be a yacc grammar test.y, but the grammar is not really LR(1);
 * e.g. "-ne" can be used either as an ordinary argument or as a string,
 * which causes great complication in yylex()/yyparse() interaction.
 * This version uses ad hoc recursive parse instead.
 */

#include <stdio.h>
#include <stdlib.h>
#include <access.h>
#include <sys/stat.h>

#define	ERROR	2				/* error exit status */
#define	FALSE	""				/* false (empty) arg */
#define	TRUE	"T"				/* true (nonempty) arg */
#define	equal(s1, s2)	(strcmp((s1), (s2)) == 0)

/* Primary operators. */
typedef	struct	prim	{
	char	*p_name;			/* operator name */
	int	(*p_fn)();			/* function */
}	PRIM;

/* Forward function definitions. */
int	(*is_binary)();
int	(*is_unary)();
int	rparen();
int	test();
int	testsub();
void	usage();

/* Elementary test functions. */
int	x_b();
int	x_c();
int	x_d();
int	x_e();
int	x_ef();
int	x_eq();
int	x_f();
int	x_g();
int	x_ge();
int	x_gt();
int	x_k();
int	x_L();
int	x_le();
int	x_lt();
int	x_n();
int	x_ne();
int	x_nt();
int	x_ot();
int	x_p();
int	x_r();
int	x_s();
int	x_str_eq();
int	x_str_gt();
int	x_str_lt();
int	x_str_ne();
int	x_t();
int	x_u();
int	x_w();
int	x_x();
int	x_z();

/* Globals. */
struct	stat	sb, sb2;		/* stat() buffers */

/* Unary primaries, not including "!"; implicit leading '-'. */
PRIM	uprims[] = {
	"b",	x_b,
	"c",	x_c,
	"d",	x_d,
	"e",	x_e,
	"f",	x_f,			/* POSIX semantics, not BSD */
	"g",	x_g,
	"k",	x_k,			/* !POSIX */
	"L",	x_L,			/* !POSIX */
	"n",	x_n,
	"p",	x_p,
	"r",	x_r,
	"s",	x_s,
	"t",	x_t,			/* POSIX semantics, arg not optional */
	"u",	x_u,
	"w",	x_w,
	"x",	x_x,
	"z",	x_z
};
#define	NUPRIM	(sizeof(uprims)/sizeof(uprims[0]))

/* Binary primaries, not including string binaries (below), "-a", "-o"; implicit leading '-'. */
PRIM	bprims[] = {
	"ef",	x_ef,			/* !POSIX */
	"eq",	x_eq,
	"ge",	x_ge,
	"gt",	x_gt,
	"le",	x_le,
	"lt",	x_lt,
	"ne",	x_ne,
	"nt",	x_nt,			/* !POSIX */
	"ot",	x_ot			/* !POSIX */
};
#define	NBPRIM	(sizeof(bprims)/sizeof(bprims[0]))

/* String binary primaries, no leading '-'. */
PRIM	sprims[] = {
	"=",	x_str_eq,
	">",	x_str_gt,		/* !POSIX */
	"<",	x_str_lt,		/* !POSIX */
	"!=",	x_str_ne
};
#define	NSPRIM	(sizeof(sprims)/sizeof(sprims[0]))

main(argc, argv) register int argc; char *argv[];
{
	register int n;

	--argc;
	if (equal(argv[0], "[")) {
		if (!equal(argv[argc], "]")) {
			fprintf(stderr, "[: missing ]\n");
			exit(ERROR);
		}
		argc--;
	}
	++argv;
	if ((n = test(argc, argv)) == -1)
		usage();		/* syntax error */
	exit(!n);			/* flip returned status */
}

/* Return function pointer if arg is a binary primary, else return NULL. */
int
(*is_binary(s)) register char *s;
{
	register PRIM *pp, *endp;

	if (*s != '-') {	/* no leading '-', search string primaries */
		pp = sprims;
		endp = &sprims[NSPRIM];
	} else {		/* leading '-', search nonstring primaries */
		++s;		/* and skip leading '-' */
		pp = bprims;
		endp = &bprims[NBPRIM];
	}
	for ( ; pp < endp; pp++)
		if (equal(pp->p_name, s))
			return pp->p_fn;
	return NULL;
}

/* Return function pointer if arg is a unary primary, else return NULL. */
int
(*is_unary(s)) register char *s;
{
	register PRIM *pp;

	if (*s++ != '-')
		return NULL;
	for (pp = uprims; pp < &uprims[NUPRIM]; pp++)
		if (equal(pp->p_name, s))
			return pp->p_fn;
	return NULL;
}

/*
 * argv[0] is '(', so find the matching ')' and return its index.
 * Return -1 if not found.
 */
int
rparen(argc, argv) int argc; char *argv[];
{
	register int n, count;

	for (count = n = 1; n < argc; n++) {
		if (equal(argv[n], "("))
			++count;
		else if (equal(argv[n], ")") && --count == 0)
			return n;
	}
	return -1;
}

/*
 * Parse and evaluate the test expression.
 * The order in which subexpressions are tried here determines the parsing.
 * Handle parens, "-a", "-o", "!" directly here.
 * Tricky stuff, this may need some tinkering.
 * Return 1 if true, 0 if false, -1 if syntax error.
 */
int
test(argc, argv) register int argc; char *argv[];
{
	register int (*fnp)();
	register int n, i;

#if	DEBUG
	printf("test(argc=%d, argv={ ", argc);
	for (n = 0; n < argc; n++)
		printf("%s ", argv[n]);
	printf("})\n");
#endif
	if (argc == 0)
		return 0;
	if (argc == 1)
		return argv[0][0] != 0;
	/* We can assume argc >= 2 below here. */
	if (argc == 2 && (fnp = is_unary(argv[0])) != NULL)
		return (*fnp)(argv[1]);
	if (argc == 3 && (fnp = is_binary(argv[1])) != NULL)
		return (*fnp)(argv[0], argv[2]);
	if (argc <= 4 && equal(argv[0], "!")) {
		if (argc == 2)
			return argv[1][0] == 0;
		if (argc == 3 && (fnp = is_unary(argv[1])) != NULL)
			return !(*fnp)(argv[2]);
		if (argc == 4 && (fnp = is_binary(argv[2])) != NULL)
			return !(*fnp)(argv[1], argv[3]);
	}
	if (equal(argv[0], "(")) {
		/* Find the matching ")" and evaluate the subexpression. */
		if ((n = rparen(argc, argv)) == -1)
			return -1;
		else if ((i = test(n - 1, &argv[1])) == -1)
			return -1;
		/* Replace the subexpression by TRUE or FALSE, evaluate the rest. */
		argv[n] = (i) ? TRUE : FALSE;
		return test(argc - n, &argv[n]);
	}
	if ((n = testsub(argc, argv, "-o")) != -1)
		return n;
	if ((n = testsub(argc, argv, "-a")) != -1)
		return n;
	if (equal(argv[0], "!") && (n = test(argc-1, &argv[1])) != -1)
		return !n;
#if	DEBUG
	printf("\ttest(): failed!\n");
#endif
	return -1;
}

/*
 * Try to split test expression involving "-a" or "-o" into subexpressions.
 * The scan is r-to-l to make "-a" and "-o" left associative.
 * Watch out for parens.
 * Return 1 if true, 0 if false, -1 if syntax error.
 */
int
testsub(argc, argv, opt) int argc; char *argv[]; char *opt;
{
	register int n, parens, n1, n2;

	for (parens = 0, n = argc-1; n > 0; n--) {
		if (equal(argv[n], ")"))
			++parens;
		else if (equal(argv[n], "("))
			--parens;
		else if (equal(argv[n], opt) && parens == 0 && n < argc-1) {
			/* Try subdividing at arg n. */
			if ((n1 = test(n, argv)) != -1
			 && (n2 = test(argc-n-1, &argv[n+1])) != -1) {
#if	DEBUG
				printf("\ttestsub(): use %s at arg %d\n", opt, n);
#endif
				return (equal(opt, "-o")) ? (n1 || n2) : (n1 && n2);
			}
		}
	}
	return -1;
}

/* Print verbose usage message and die. */
void
usage()
{
	fprintf(stderr,
		"test: test expression syntax error\n"
		"Usage: test [ expression ]\n"
		);
#if	!DEBUG
	fprintf(stderr,
		"Unary primaries:\n"
		"\t-b file\t\tfile exists and is a block special file\n"
		"\t-c file\t\tfile exists and is a character special file\n"
		"\t-d file\t\tfile exists and is a directory\n"
		"\t-e file\t\tfile exists\n"
		"\t-f file\t\tfile exists and is a regular file\n"
		"\t-g file\t\tfile exists and is setgid\n"
		"\t-k file\t\tfile exists and has sticky bit set\t(not Posix)\n"
		"\t-L file\t\tfile is a link\t\t\t\t(not Posix)\n"
		"\t-n string\tstring length is nonzero\n"
		"\t-p file\t\tfile exists and is a named pipe (FIFO)\n"
		"\t-r file\t\tfile exists and is readable\n"
		"\t-s file\t\tfile exists and has nonzero size\n"
		"\t-t fd\t\tfd is the file descriptor of a terminal\n"
		"\t-u file\t\tfile exists and is setuid\n"
		"\t-w file\t\tfile exists and is writable\n"
		"\t-x file\t\tfile exists and is executable\n"
		"\t-z string\tstring length is zero\n"
		"\tstring\t\tstring is not the empty string\n"
		);
	fprintf(stderr,
		"Binary primaries:\n"
		"\ts1 = s2\t\tstrings s1 and s2 are identical\n"
		"\ts1 != s2\tstrings s1 and s2 are not identical\n"
		"\ts1 < s2\t\tstring s1 is less than s2\t\t(not Posix)\n"
		"\ts1 > s2\t\tstring s1 is greater than s2\t\t(not Posix)\n"
		"\tfile1 -ef file2\tfile1 and file2 are identical\t\t(not Posix)\n"
		"\tn1 -eq n2\tnumbers n1 and n2 are equal\n"
		"\tn1 -ge n2\tnumber n1 is greater than or equal to n2\n"
		"\tn1 -gt n2\tnumber n1 is greater than n2\n"
		"\tn1 -le n2\tnumber n1 is less than or equal to n2\n"
		"\tn1 -lt n2\tnumber n1 is less than n2\n"
		"\tn1 -ne n2\tnumbers n1 and n2 are not equal\n"
		"\tfile1 -nt file2\tfile1 is newer than file2\t\t(not Posix)\n"
		"\tfile1 -ot file2\tfile1 is older than file2\t\t(not Posix)\n"
		);
	fprintf(stderr,
		"Expression grouping:\n"
		"\t! exp\t\texp is false\n"
		"\texp1 -a exp2\texp1 and exp2 are true\t\t\t(not Posix)\n"
		"\texp1 -o exp2\texp1 or exp2 is true\t\t\t(not Posix)\n"
		"\t( exp )\t\tparentheses for grouping\t\t(not Posix)\n"
		);
#endif
	exit(ERROR);
}

/* Elementary test routines; each returns 1 if true, 0 if false. */
/* File exists and is a block special file. */
int
x_b(s) char *s;
{
	return stat(s, &sb)>=0 && (sb.st_mode&S_IFMT)==S_IFBLK;
}

/* File exists and is a character special file. */
int
x_c(s) char *s;
{
	return stat(s, &sb)>=0 && (sb.st_mode&S_IFMT)==S_IFCHR;
}

/* File exists and is a directory. */
int
x_d(s) char *s;
{
	return stat(s, &sb)>=0 && (sb.st_mode&S_IFMT)==S_IFDIR;
}

/* File exists. */
int
x_e(s) char *s;
{
	return stat(s, &sb)>=0;
}

/* Two files are the same. */
int
x_ef(s1, s2) char *s1, *s2;
{
	return stat(s1, &sb)>=0
	    && stat(s2, &sb2)>=0
	    && sb.st_dev==sb2.st_dev && sb.st_ino==sb2.st_ino;
}

/* Two numbers are equal. */
int
x_eq(s1, s2) char *s1, *s2;
{
	return atol(s1) == atol(s2);
}

/* File exists and is an ordinary file. */
/* This is POSIX semantics; BSD uses "file exists and is not a directory." */
int
x_f(s) char *s;
{
	return stat(s, &sb)>=0 && (sb.st_mode&S_IFMT)==S_IFREG;
}

/* File exists and is setgid. */
int
x_g(s) char *s;
{
	return stat(s, &sb)>=0 && (sb.st_mode&S_ISGID)!=0;
}

/* First number is greater than or equal to the second. */
int
x_ge(s1, s2) char *s1, *s2;
{
	return atol(s1) >= atol(s2);
}

/* First number is greater than the second. */
int
x_gt(s1, s2) char *s1, *s2;
{
	return atol(s1) > atol(s2);
}

/* File exists and has sticky bit set. */
int
x_k(s) char *s;
{
	return stat(s, &sb)>=0 && (sb.st_mode&S_ISVTX)!=0;
}

/* File is a link. */
int
x_L(s) char *s;
{
	return stat(s, &sb)>=0 && sb.st_nlink>1;
}

/* First number is less than or equal to the second. */
int
x_le(s1, s2) char *s1, *s2;
{
	return atol(s1) <= atol(s2);
}

/* First number is less than the second. */
int
x_lt(s1, s2) char *s1, *s2;
{
	return atol(s1) < atol(s2);
}

/* String length is non-zero. */
int
x_n(s) char *s;
{
	return s[0] != '\0';
}

/* Two numbers are not equal. */
int
x_ne(s1, s2) char *s1, *s2;
{
	return atol(s1) != atol(s2);
}

/* First file is newer than the second. */
int
x_nt(s1, s2) char *s1, *s2;
{
	return stat(s1, &sb)>=0
	    && stat(s2, &sb2)>=0
	    && sb.st_mtime>sb2.st_mtime;
}

/* First file is older than the second. */
int
x_ot(s1, s2) char *s1, *s2;
{
	return stat(s1, &sb)>=0
	    && stat(s2, &sb2)>=0
	    && sb.st_mtime<sb2.st_mtime;
}

/* File exists and is a named pipe. */
int
x_p(s) char *s;
{
	return stat(s, &sb)>=0 && (sb.st_mode&S_IFMT)==S_IFPIP;
}

/* File exists and is readable. */
int
x_r(s) char *s;
{
	return access(s, AREAD) >= 0;
}

/* File exists and has a non-zero size. */
int
x_s(s) char *s;
{
	return stat(s, &sb)>=0 && sb.st_size>0;
}

/* Two strings are lexicographically equal. */
int
x_str_eq(s1, s2) char *s1, *s2;
{
	return equal(s1, s2);
}

/* First string is lexicographically less than second. */
int
x_str_lt(s1, s2) char *s1, *s2;
{
	return strcmp(s1, s2) < 0;
}

/* First string is lexicographically greater than second. */
int
x_str_gt(s1, s2) char *s1, *s2;
{
	return strcmp(s1, s2) > 0;
}

/* Two strings are lexicographically unequal. */
int
x_str_ne(s1, s2) char *s1, *s2;
{
	return !equal(s1, s2);
}

/* File descriptor is associated with a terminal. */
/* Posix says the fd in "-t fd" arg is required, not optional. */
int
x_t(s) char *s;
{
	return isatty(atoi(s));
}

/* File exists and is setuid. */
int
x_u(s) char *s;
{
	return stat(s, &sb)>=0 && (sb.st_mode&S_ISUID)!=0;
}

/* File exists and is writeable. */
int
x_w(s) char *s;
{
	return access(s, AWRITE) >= 0;
}

/* File exists and is executable. */
int
x_x(s) char *s;
{
	return access(s, AEXEC) >= 0;
}

/* String length is zero. */
int
x_z(s) char *s;
{
	return s[0] == '\0';
}

/* end of cmd/test.c */
