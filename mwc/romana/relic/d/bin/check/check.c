/*
 * check -- check all of the normally-used filesystems
 * or those specified by calling `icheck' and `dcheck'.
 * If `-s' is specified, also try to correct any
 * problems encountered in any of these filesystems.
 */

/*

return '-2' (254) if an internal error occurred; '-1' (255) if
there are unfixed errors; '0' if no errors were found; and '1'
if errors were found, but fixed (system should then be rebooted).

*/

#include <stdio.h>
#include "check.h"

char	icheck[] = "/bin/icheck";
char	dcheck[] = "/bin/dcheck";
char	fixopt[] = "-s";

int	sflag;

main(argc, argv)
char *argv[];
{
	register estat = 0;
	register char **fsp;

	if (argc>1 && *argv[1]=='-') {
		if (argv[1][1]=='s' && argv[1][2]=='\0')
			sflag = 1;
		else
			usage();
		argv++;
		argc--;
	}
	if (argc > 1)
		fsp = &argv[1];
	else
		usage();
	while (*fsp != NULL)
		estat |= check(*fsp++);
	exit(estat);
}

/*
 * Do check for a single filesystem.
 */
check(fs)
char *fs;
{
	register int ierror, derror;
	register int bad = 0;

	if (ierror = run(icheck, fs, NULL))
		bad |= 1;
	if (derror = run(dcheck, fs, NULL))
		bad |= 1;
	if (sflag) {
		if ((ierror & ~IC_FIX) == 0) {
			if (derror & ~DC_FIX)
				return(-1);
			if (derror & DC_FIX) {
				if (derror = run(dcheck, fixopt, fs, NULL))
					return(-1);
				ierror = IC_MISS;	/*force fixup icheck*/
			}
			if (ierror & IC_FIX)
				if (ierror = run(icheck, fixopt, fs, NULL))
					return(-1);
		} else
			return(-1);
	}
	return (bad);
}

/*
 * Do a command -- either icheck or dcheck normally.
 */
/* VARARGS */
run(command, args)
char *command;
{
	register int pid;
	int status;

	if ((pid = fork()) < 0) {
		fprintf(stderr, "check: try again\n");
		exit(-2);
	}
	if (pid) {
		wait(&status);
		if ((status&0377) != 0)
			return(-2);
		return ((status>>8)&0377);
	} else {
		execv(command, &command);
		fprintf(stderr, "check: someone moved %s\n", command);
		exit(-2);
	}
	/* NOTREACHED */
}

usage()
{
	fprintf(stderr, "Usage: check [-s] filesystem [ ... ]\n");
	exit(-2);
}
