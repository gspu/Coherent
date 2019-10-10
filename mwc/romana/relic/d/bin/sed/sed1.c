/*
 * sed/sed1.c
 * A stream editor.
 * Main.
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "sed.h"

main(argc, argv)
char *argv[];
{
	ECL **eclpp;
	int scrflag;
	register ECL *ep;
	register int i;
	register char *cp;

	scrflag = 0;
	initialise();
	eclpp = &eclp;
	for (i=1; i<argc; i++) {
		cp = &argv[i][0];
		if (*cp++ != '-') {
			*eclpp = ep = (ECL *)salloc(sizeof (ECL));
			eclpp = &ep->e_next;
			ep->e_argp = argv[i];
			continue;
		}
		while (*cp) {
			switch (*cp++) {
			case 'e':
				if (++i >= argc)
					usage();
				compstr(argv[i]);
				scrflag++;
				continue;
			case 'f':
				if (++i >= argc)
					usage();
				compfil(argv[i]);
				scrflag++;
				continue;
			case 'n':
				nflag++;
				continue;
			case 's':
				sflag++;
				continue;
			default:
				usage();
			}
		}
	}
	*eclpp = NULL;
	if (scrflag==0 && eclp!=NULL) {
		ep = eclp;
		eclp = eclp->e_next;
		compstr(ep->e_argp);
		free(ep);
	}
	compstr(":");
	load();
	if (nerr != 0)
		exit(1);
	ifp = eclp==NULL ? stdin : NULL;
	process();
}

/*
 * Print out a usage message.
 */
usage()
{
	fprintf(stderr, "Usage: sed [-n] [-e script] [-f sfile] [file]\n");
	exit(1);
}

/*
 * Initialisation.
 */
initialise()
{
	compp = &comp;
	filp = NULL;
	labp = NULL;
	qcbp = NULL;
	pattlen = 0;
	holdlen = 0;
	nflag = 0;
	sflag = 0;
	nerr = 0;
	lno = 0;
}

/*
 * Leave.
 */
leave()
{
	exit(0);
}

/*
 * Allocate `n' bytes of memory.
 */
char *
salloc(n)
{
	register char *cp;

	if ((cp = malloc(n)) == NULL) {
		fprintf(stderr, "Out of space\n");
		exit(1);
	}
	return cp;
}

/* end of sed/sed1.c */
