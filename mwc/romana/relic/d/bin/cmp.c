/*
 * Compare two files.
 */

#include <stdio.h>
#include <sys/types.h>

#define	isdigit(c)	((c)>='0'&&(c)<='9')

int	lflag;		/* Print all differences */
int	sflag;		/* Return status only */
int	differ;		/* Non-zero if files differ */
long	byteno;
long	lineno = 1;
fsize_t	skip1, skip2;
long	atol();

char	eopen[] = "Cannot open %s\n";
char	usage[] = "Usage: cmp [-l] [-s] file1 file2 [skip1 skip2]\n";
#if	HEX
char	diffl[] = "%ld: 0x%02x 0x%02x\n";
#else
char	diffl[] = "%ld: %3o %3o\n";
#endif
char	diffn[] = "%s, %s differ at char %ld, line %ld\n";

main(argc, argv)
char *argv[];
{
#ifdef MSDOS
	msdoscvt("cmp", &argc, &argv);
#endif
	if (argc > 1) {
		if (*argv[1] == '-') {
			if (argv[1][1]=='s' && argv[1][2]=='\0')
				sflag = 1;
			else if (argv[1][1]=='l' && argv[1][2]=='\0')
				lflag = 1;
			else if (argv[1][1] == '\0') {
				argc++;
				argv--;
			} else
				err(usage);
			argc--;
			argv++;
		}
	}
	if (argc==5 && isdigit(*argv[3]) && isdigit(*argv[4])) {
		skip1 = atol(argv[3]);
		skip2 = atol(argv[4]);
		argc -= 2;
	}
	if (argc != 3)
		err(usage);
	cmp(argv[1], argv[2]);
	exit(differ ? 1 : 0);
}

/*
 * Do the work of comparing the two files
 */
cmp(a, b)
char *a, *b;
{
	FILE *fp1, *fp2;
	register c1, c2;

	if (a[0]=='-' && a[1]=='\0') {
		fp1 = stdin;
#ifdef	MSDOS
		_setbinary(stdin);
#endif
	}
	else if ((fp1 = fopen(a, "rb")) == NULL)
		err(eopen, a);
	if ((fp2 = fopen(b, "rb")) == NULL)
		err(eopen, b);
	while (skip1--)
		if ((c1 = getc(fp1)) == EOF) {
			remark("EOF in skipping on %s\n", a);
			exit(1);
		}
	while (skip2--)
		if ((c2 = getc(fp2)) == EOF) {
			remark("EOF in skipping on %s\n", b);
			exit(1);
		}
	for (;;) {
		c1 = getc(fp1);
		c2 = getc(fp2);
		if (c1==EOF || c2==EOF)
			break;
		byteno++;
		if (c1 != c2) {
			differ = 1;
			if (lflag)
				remark(diffl, byteno, c1, c2);
			else {
				remark(diffn, a, b, byteno, lineno);
				return;
			}
		}
		if (c1 == '\n')
			lineno++;
	}
	if (c1 != c2) {
		remark("EOF on %s\n", c1==EOF ? a : b);
		differ = 1;
	}
}

err(x)
{
	if (!sflag)
		fprintf(stderr, "%r", &x);
	exit(2);
}

remark(x)
{
	if (!sflag)
		printf("%r", &x);
}
