static char _version[]="kill version 2.0";
/*
 * kill -- send a signal to a process
 */
#include	<signal.h>
#include	<ctype.h>
#include	<errno.h>
#include	<stdio.h>

#define	NOTREACHED return

int err = 0;

main( argc, argv)
register char	**argv;
{
	register	pid;
	char		**getsignal( );
	extern char	*sys_errlist[];
	extern		errno;
	static		int sig	= SIGTERM;

	/*
	 * Fetch the signal we are going to send, from the command line.
	 */
	argv = getsignal(argv, &sig);


	if (*argv == NULL) {
     		fprintf(stderr, "Usage: kill [ signal ] pid ...\n");
		exit(1);	
	}

	/*
	 * If we are sending this signal to our own process group
	 * leader, we don't want to receive this signal here.
	 */
	signal(sig, SIG_IGN);


	do {
		if (isdigit(**argv)==0 && **argv!='-') {
			error("\"%s\" is not a process id", *argv);
			continue;
		}
		pid = atoi(*argv);
		if (kill(pid, sig)) {
			if (errno == EINVAL)
				fatal("signal %d is invalid", sig);
			error("process %d: %s", pid, sys_errlist[errno]);
			continue;
		}
	} while (*++argv);
	exit(err);
}


/*
 * Attempt to extract a signal number from argv[].
 * Puts any signal number it finds in *sigp, and then returns
 * the next argument in argv[].
 */
char **
getsignal( av, sigp)
register char **av;
int *sigp;
{
	if ((++av)[0] == NULL)
		return (av);
	if (isdigit( av[0][0]))
		return (av);
	/*
	 * Do we have an explicit signal number?
	 */
	if (av[0][0] == '-') {
		++av[0];	/* Skip the '-'.  */
		if (isdigit( av[0][0])) {
			/*
			 * Convert the signal number to an integer and return.
			 */
			*sigp = atoi( &av[0][0]);
			return (++av);
		}
	}

	/*
	 * The signal must be symbolic.
	 */
	uppercase( av[0]);
	/*
	 * Strip off an optional leading "SIG".
	 */
	if (strncmp( av[0], "SIG", 3) == 0)
		*av += 3;
	/*
	 * Walk through the signal names looking for a match.
	 */
	for (*sigp = 1; notsame( *sigp, av[0]); ++*sigp) {
		/* Do nothing.  */
	}

	/*
	 * Bump argv to next argument.
	 */
	return (++av);
}


/*
 * Returns 0 only if 'sig' has symbolic name 'name'.
 */
notsame(sig, name)
char *name;
{
	static char	*names[] = {
#ifdef _I386
		0,
		"HUP", "INT", "QUIT", "ILL",
		"TRAP", "IOT", "EMT", "FPE",
		"KILL", "BUS", "SEGV", "SYS",
		"PIPE", "ALRM", "TERM", "USR1",
		"USR2", "CHLD", "PWR", "WINCH",
		"", "POLL",
		NULL
#else /* _I386 */
		0,
		"HUP",  "INT",  "QUIT", "ALRM",
		"TERM", "REST", "SYS",  "PIPE",
		"KILL", "TRAP", "SEGV", "ILL",
		"IOT",  "EMT",  "FPE",  "BUS",
		NULL
#endif /* _I386 */
	};

	if (sig>NSIG || names[sig]==NULL) {
		fatal( "no such signal SIG%s", name);
	}

#ifdef _I386
	/*
	 * SIGABRT and SIGCLD share numbers with other signals.
	 */
	if ( SIGABRT == sig ) {
		if ( 0 == strcmp( "ABRT", name ) ) {
			return (0);	/* Match! */
		}
	}
	if ( SIGCLD == sig ) {
		if ( 0 == strcmp( "CLD", name ) ) {
			return (0);	/* Match! */
		}
	}
#endif /* _I386 */

	return (strcmp( names[sig], name));
}


uppercase(s)
register char *s;
{
	for (; *s; ++s)
		if (islower( *s))
			*s &= ~040;
}


error(arg0)
char *arg0;
{
	fprintf( stderr, "kill: %r\n", &arg0);
	err = 1;
}


fatal(arg0)
char *arg0;
{
	fprintf( stderr, "kill: %r\n", &arg0);
	exit(1);
}
