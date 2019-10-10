/*
 * Execle(name, arg0, arg1, ..., argn, NULL, envp);
 *
 * Sys exec with list of arguments and environment.
 */

#define	NULL	((char *)0)

/* VARARGS 1 */
execle(name, arg0)
char *name;
char *arg0;
{
	register char **envp;

	envp = &arg0;
	while (*envp++ != NULL)
		;
	execve(name, &arg0, *envp);
}
