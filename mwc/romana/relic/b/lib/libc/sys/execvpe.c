/*
 * libc/sys/execvpe.c
 * Sys exec with an array of arguments and environment,
 * searching directories specified in PATH.
 */

#include <errno.h>
#include <path.h>
#include <string.h>
#include <stdlib.h>

#define	SHELL	"/bin/sh"	/* not DEFSHELL, which is just "sh" */


/* execvpe(name, argv, env) */
int
execvpe(name, argv, env) char *name; char *argv[]; char **env;
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
		execve(fname, argv, env);		/* go for it */
		if (isabs && errno != ENOEXEC)
			break;				/* failed */
		if (errno == ENOEXEC) {			/* try again with sh */
			p1 = argv[-1];
			p2 = argv[0];
			argv[-1] = SHELL;
			argv[0] = fname;
			execve(argv[-1], argv-1, env);
			argv[-1] = p1;
			argv[0] = p2;
			break;				/* failed */
		}
		if (*sp == '\0')
			break;				/* end of PATH, failed */
		if (*sp == LISTSEP)
			sp++;
	}
	return -1;					/* failed */
}

/* end of libc/sys/execvpe.c */
