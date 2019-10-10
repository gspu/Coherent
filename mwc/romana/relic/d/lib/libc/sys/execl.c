/*
 * Execl(name, arg0, arg1, ..., argn, NULL)
 * Sys exec with a list of arguments and no environment
 * given.
 */

extern	char	**environ;

execl(name, arg0)
char *name;
char *arg0;
{
	execve(name, &arg0, environ);
}
