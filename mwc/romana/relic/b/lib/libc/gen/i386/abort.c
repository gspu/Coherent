/*
 * libc/gen/i386/abort.c
 */

#include <signal.h>

void
abort(status) register int status;
{
	register int pid;

	pid = getpid();
	_finish();
	signal(SIGIOT, 0);
	kill(pid, SIGIOT);
	_exit(status);
	/* NOTREACHED */
}

/* end of libc/gen/i386/abort.c */
