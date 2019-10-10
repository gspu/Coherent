/*
 * libc/sys/execlp.c
 * Sys exec with a list of arguments but no environment,
 * searching directories specified in PATH.
 */

extern char **environ;

/* execlp(name, arg0, arg1, ..., argn, NULL) */
/* VARARGS 1 */
int
execlp(name, arg0) char *name; char *arg0;
{
	return execvpe(name, &arg0, environ);
}

/* end of libc/sys/execlp.c */
