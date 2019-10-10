/*
 * libc/sys/execlpe.c
 * Sys exec with a list of arguments and environment,
 * searching directories specified in PATH.
 */

#define	NULL	((char *)0)

/* execlpe(name, arg0, arg1, ..., argn, NULL, envp) */
/* VARARGS 1 */
int
execlpe(name, arg0) char *name; char *arg0;
{
	register char **envp;

	envp = &arg0;
	while (*envp++ != NULL)
		;
	return execvpe(name, &arg0, *envp);
}

/* end of libc/sys/execlpe.c */
