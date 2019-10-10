/*
 * libc/sys/execv.c
 * Sys exec with an array of arguments but no environment.
 */

extern char **environ;

/* execv(name, argv) */
int
execv(name, argv) char *name; char *argv[];
{
	return execve(name, argv, environ);
}

/* end of libc/sys/execv.c */
