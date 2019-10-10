/*
 * libc/sys/execl.c
 * Sys exec with a list of arguments but no environment.
 */

extern	char	**environ;

/* execl(name, arg0, arg1, ..., argn, NULL) */
/* VARARGS 1 */
int
execl(name, arg0) char *name; char *arg0;
{
	return execve(name, &arg0, environ);
}

/* end of libc/sys/execl.c */
