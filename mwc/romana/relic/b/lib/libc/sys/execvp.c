/*
 * libc/sys/execvp.c
 * Sys exec with an array of arguments but no environment,
 * searching directories specified in PATH.
 */

extern char **environ;

/* execvp(name, argv) */
int
execvp(name, argv) char *name; char *argv[];
{
	return execvpe(name, argv, environ);
}

/* end of libc/sys/execvp.c */
