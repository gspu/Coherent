/*
 * Standard I/O Library.
 * Create a unique name for a temporary file ala SYS V.
 * Note: this differs from the real SYS V in that we don't limit the
 * length of the prefix to be five characters for portability reasons.
 */

#include <stdio.h>

#ifdef	GEMDOS
#define	FSPATHSEP	'\\'
#ifndef	P_tmpdir
#define	P_tmpdir	"\\tmp"
#endif
#endif
#ifdef	DOS
#define	FSPATHSEP	'\\'
#ifndef	P_tmpdir
#define	P_tmpdir	"\\tmp"
#endif
#endif
#ifndef	FSPATHSEP
#define	FSPATHSEP	'/'
#endif
#ifndef	P_tmpdir
#define	P_tmpdir	"/tmp"
#endif
#ifndef	L_tmpnam
#define	L_tmpnam	64
#endif
#define	TEMPLATE	"XXXXXX"
#define	PREFIX		"t"

char	*
tempnam(dir, pfx)
char	*dir, *pfx;
{
	register char *tbp, *stbp;
	extern	char *mktemp(), *strcpy();
	extern	char *malloc(), *getenv();

	if ((stbp = malloc(L_tmpnam)) != NULL) {
		if (dir == NULL && (dir = getenv("TMPDIR")) == NULL)
			dir = P_tmpdir;
		if (pfx == NULL)
			pfx = PREFIX;
		for (;;) {
			tbp = stbp;
			strcpy(tbp, dir);
			while (*tbp != 0)
				++tbp;
			if (tbp > stbp && tbp[-1] != FSPATHSEP)
				*tbp++ = FSPATHSEP;
			strcpy(tbp, pfx);
			while (*tbp != 0)
				++tbp;
			strcpy(tbp, TEMPLATE);
			mktemp(stbp);
			if (access(stbp, 0) == -1)
				break;
		}
	}
	return (stbp);
}
