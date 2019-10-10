/*
 * cpp/cpp.c
 * Run cc0 (from LIBPATH under GEMDOS) for old cpp functionality.
 * Default to stdin and stdout.
 * See usage() for details.
 * v0   -- dag 870722
 * v1   -- steve 870730	COHERENT and MSDOS conditionalization
 * v2   -- steve 920716	-C and -P options
 * v4.0 -- steve 930128	-VCPLUS option
 */

#include <stdio.h>
#include <path.h>
#include "var.h"
#include "varmch.h"

#define	NEWCPP	1	/* must be 0 to compile using old cpp */
			/* because it does not understand "string1" "string2" */

#define	VERSION	"4.0"

#if	COHERENT
#define	CC0NAME	"cc0"
extern char **environ;
extern char *getenv();
#endif

#if	GEMDOS
#define	CC0NAME	"cc0.prg"
extern char **environ;
extern char *getenv();
#endif

#if	MSDOS
#define	CC0NAME	"cc0"
extern	char *malloc();
extern	char *strcat();
#endif

/* Forward. */
void	usage();
void	cpperror();

/* Globals. */
int 	cppargc;
char	*cppargs[128];
VARIANT	variant;
int 	verbose = 0;
char	vstr[2*VMAXIM/VGRANU + 1];

main(argc, argv) int argc; char *argv[];
{
	register char *ap;
	register int i, s;
#if	GEMDOS || COHERENT
	char *libptr;
#endif
#if	MSDOS
	register int len;
	register char *tail;
#endif

#if	COHERENT
	_addargs("CPP", &argc, &argv);
#endif

	cppargc = 4;
	setvariant(VCPP);
	argc--;
	while (argc-- > 0) {
		ap = *++argv;
#if	DEBUG
		printf("%d: %s\n", argc, ap);
#endif
		if (*ap == '-') {
			switch (ap[1]) {
			case 'I':
				cppargs[cppargc++] = ap;
				if (ap[2] == 0) {
					if ((cppargs[cppargc++] = *++argv)
									== NULL)
						usage();
					--argc;
				}
				break;

			case 'D':
			case 'U':
				cppargs[cppargc++] = ap;
				break;

			case 'o':
				if ((cppargs[3] = *++argv) == NULL)
					usage();
				--argc;
				break;

			case 'C':
				setvariant(VCPPC);
				break;

			case 'E':
			case 'P':
				setvariant(VCPPE);
				break;

			case 'Q':
				verbose = 0;
				setvariant(VQUIET);
				break;

			case '3':
				setvariant(V3GRAPH);
				break;

			case 'V':
				if (strcmp(&ap[2], "CPLUS") == 0) {
					setvariant(VCPLUS);
					break;
				}
				fprintf(stderr, "cpp: V%s\n", VERSION);
				verbose++;
				break;

			case '?':
			default:
				usage();
			}
		} else if (cppargs[2] != NULL) {
			usage();
		} else
			cppargs[2] = ap;
	}

	if (cppargs[2] == NULL)
		cppargs[2] = "-";
	if (cppargs[3] == NULL)
		cppargs[3] = "-";

#if	GEMDOS || COHERENT
	if ((libptr = getenv("LIBPATH")) == NULL)
		libptr = DEFLIBPATH;
	if ((ap = path(libptr, CC0NAME, AEXEC)) == NULL)
		cpperror("cannot find cc0");
	cppargs[0] = ap;
#endif
#if	MSDOS
	cppargs[0] = CC0NAME;
#endif
	makvariant(vstr);
	cppargs[1] = vstr;
	if (verbose) {
		for (i = 0; i<cppargc; i++)
			printf("%s ", cppargs[i]);
		printf("\n");
	}
#if	GEMDOS || COHERENT
	if (s = execve(cppargs[0], cppargs, environ))
		if (s < 0)
			perror(cppargs[0]);
#endif
#if	MSDOS
	for (len=0, i=1; i<cppargc; i++)
		len += strlen(cppargs[i]) + 1;
	if ((tail = malloc(len)) == NULL)
		cpperror("cannot allocate tail");
	for (tail[0]='\0', i=1; i<cppargc; i++) {
		strcat(tail, cppargs[i]);
		if (i < cppargc-1)
			strcat(tail, " ");
	}
	if ((s = execall(cppargs[0], tail)) == 0177)
		cpperror("cannot execute cc0");
#endif
	exit(s);
}

void
usage()
{
	fprintf(stderr, "Usage:\t/lib/cpp [ option... ] [ file ]\n");
#if	NEWCPP
	fprintf(stderr,
		"Options:\n"
#if	VCNEST
		/* UNDOCUMENTED: -3 */
#endif
		"\t-C\t\t\tdo not suppress comments\n"
		"\t-Dvariable[=val]\tdefine variable with val [default=1]\n"
		"\t-E\t\t\tstrip file line number information\n"
		"\t-Idirectory\t\tadd directory to #include file search list\n"
		"\t-o outfile\t\twrite output to outfile [default=stdout]\n"
		"\t-P\t\t\tstrip file line number information (same as -E)\n"
		"\t-Q\t\t\tsuppress all messages\n"
		"\t-Uvariable\t\tundefine variable\n"
		"\t-V\t\t\tprint verbose information\n"
		"\t-VCPLUS\t\t\tsuppress C++ - style online comments\n"
		);
#endif
	exit(1);
}

void
cpperror(s) char *s;
{
	fprintf(stderr, "cpp: %s\n", s);
	exit(1);
}

/* end of cpp/cpp.c */
