/*
 * Standard I/O Library.
 * Create a unique name for a temporary file ala SYS V.
 */

#include <stdio.h>

#ifdef	GEMDOS
#define	FSPATHSEP	"\\"
#ifndef	P_tmpdir
#define	P_tmpdir	"\\tmp"
#endif
#endif
#ifdef	DOS
#define	FSPATHSEP	"\\"
#ifndef	P_tmpdir
#define	P_tmpdir	"\\tmp"
#endif
#endif
#ifndef	FSPATHSEP
#define	FSPATHSEP	"/"
#ifndef	P_tmpdir
#define	P_tmpdir	"/tmp"
#endif
#endif
#ifndef	L_tmpnam
#define	L_tmpnam	64
#endif
#define	TEMPLATE	"tXXXXXX"

char	*
tmpnam(s)
register char	*s;
{
	char *name;
	static	char namebuf[L_tmpnam];
	extern	char *mktemp(), *strcpy(), *strcat();

	for (;;) {
		if (s == NULL)
			s = namebuf;
		strcpy(s, P_tmpdir);
		strcat(s, FSPATHSEP);
		strcat(s, TEMPLATE);
		name = mktemp(s);
		if (access(name, 0) == -1)
			break;
	}
	return (s);
}
