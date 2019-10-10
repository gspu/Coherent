/*
 * Cause the line printer daemon to
 * skip the current listing.
 * With `-r' option, make it restart the
 * current listing.
 * SIGTRAP is used for skipping; SIGREST for restarting.
 * (NOTE: this should be setuid to daemon).
 */

#include <stdio.h>
#include <signal.h>

#ifdef LASER
char	lockfile[] = "/usr/spool/hpd/dpid";
#else
char	lockfile[] = "/usr/spool/lpd/dpid";
#endif
char *argv0;

main(argc, argv)
char *argv[];
{
	int pid;
	register int fd;
	register int sig = SIGTRAP;

	argv0 = argv[0];
	if (argc>1 && *argv[1]=='-') {
		if (argv[1][1]=='r' && argv[1][2]=='\0')
			sig = SIGREST;
		else
			usage();
	}
	if ((fd = open(lockfile, 0)) < 0
	 || read(fd, &pid, sizeof pid) != sizeof (pid)
	 || kill(pid, sig) < 0) {
		fprintf(stderr, "Line printer daemon not active\n");
		exit(1);
	}
}

usage()
{
	fprintf(stderr, "Usage: %s [-r]\n", argv0);
	exit(1);
}
