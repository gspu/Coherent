/* libc/sys/execvp.c */

#include <stdio.h>
#include <errno.h>
#include <path.h>
#include <string.h>

extern	char	*getenv();

#define	SHELL	"/bin/sh"		/* shell name */

/*
 * execvp() is a version of execv()
 * which mimics the actions of the shell
 * in using PATH search rules and running a shell file.
 */
execvp(name, argp) char *name; char *argp[];
{
	register char *p1, *p2, *sp;
	char fname[MAXPATH];
	int isabs;

	if ((sp = getenv("PATH")) == NULL)		/* find PATH */
		sp = DEFPATH;				/* or take default */
	isabs = (strchr(name, PATHSEP) != NULL);	/* iff name is absolute */
	for (;;) {
		if (isabs)
			strcpy(fname, name);		/* use given name */
		else {
			for (p2 = fname; *sp != '\0' && *sp != LISTSEP; )
				*p2++ = *sp++;		/* copy pathname component */
			if (p2 != fname)
				*p2++ = PATHSEP;	/* append PATHSEP if nonempty */
			for (p1 = name; *p1 != '\0'; )
				*p2++ = *p1++;		/* append name */
			*p2 = '\0';			/* and NUL-terminate */
		}
		execv(fname, argp);			/* go for it */
		if (isabs && errno != ENOEXEC)
			break;				/* failed */
		if (errno == ENOEXEC) {			/* try again with sh */
			p1 = argp[-1];
			p2 = argp[0];
			argp[-1] = SHELL;
			argp[0] = fname;
			execv(argp[-1], argp-1);
			argp[-1] = p1;
			argp[0] = p2;
			break;				/* failed */
		}
		if (*sp == '\0')
			break;				/* end of PATH, failed */
		if (*sp == LISTSEP)
			sp++;
	}
	return -1;					/* failed */
}

/* end of execvp.c */
