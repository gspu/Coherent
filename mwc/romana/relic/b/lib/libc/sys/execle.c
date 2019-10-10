/*
 * libc/sys/execle.c
 * Sys exec with a list of arguments and environment.
 */

#define	NULL	((char *)0)

/* execle(name, arg0, arg1, ..., argn, NULL, envp) */
/* VARARGS 1 */
int
execle(name, arg0) char *name; char *arg0;
{
	register char **envp;

	envp = &arg0;
	while (*envp++ != NULL)
		;
	return execve(name, &arg0, *envp);
}

/* end of libc/sys/execle.c */
