/*
 * cpp.c
 * Run CC0 (from LIBPATH under GEMDOS) for old cpp functionality.
 * Default to stdin and stdout.
 * See usage() for details.
 * v0 -- dag 870722
 * v1 -- steve 870730	COHERENT and MSDOS conditionalization
 */
#include <stdio.h>
#include <path.h>
#include "var.h"
#include "varmch.h"

#define	VERBOSITY	0	/* must be 0 to compile using old cpp */

VARIANT variant;
char vstr[2*VMAXIM/VGRANU + 1];
char *cppargs[128];
int  cppargc;
int  verbose = 0;

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

main(argc, argv)
register int argc;
register char **argv;
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

	cppargc = 4;
	setvariant(VCPP);
	if (argc-- < 2)
		usage();
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
#if	VCNEST
				setvariant(VCNEST);
#endif
				break;

			case 'E':
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
				verbose++;
				break;
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
		cerr("cannot find cc0");
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
		cerr("cannot allocate tail");
	for (tail[0]='\0', i=1; i<cppargc; i++) {
		strcat(tail, cppargs[i]);
		if (i < cppargc-1)
			strcat(tail, " ");
	}
	if ((s = execall(cppargs[0], tail)) == 0177)
		cerr("cannot execute cc0");
#endif
	exit(s);
}

usage() {
	fprintf(stderr, "Usage:\tcpp [ option... ] [ file ]\n");
#if	VERBOSITY
	fprintf(stderr,
		"Options:\n"
		/* UNDOCUMENTED: -3 */
#if	VCNEST
		"   -C\t\t\tallow nested comments\n"
#endif
		"   -Dvariable[=val]\tdefine variable with val [default=1]\n"
		"   -E\t\t\tstrip all comments and line numbers\n"
		"   -Idirectory\t\tadd directory to #include file search list\n"
		"   -o outfile\t\twrite output to outfile [default=stdout]\n"
		"   -Q\t\t\tsupress all messages\n"
		"   -Uvariable\t\tundefine variable\n"
		"   -V\t\t\tprint verbose information\n"
		);
#endif
	exit(1);
}

cerr(s)
char *s;
{
	fprintf(stderr, "cpp: %s\n", s);
	exit(1);
}

/* end of cpp.c */
