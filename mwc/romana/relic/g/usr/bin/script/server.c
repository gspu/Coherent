/*
 * File:	server.c
 *
 * Purpose:	main program for recording process
 *
 * $Log:	server.c,v $
 * Revision 1.2  92/06/15  10:04:57  bin
 * *** empty log message ***
 * 
 */

/*
 * Includes.
 */
#include <stdio.h>

/*
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */

typedef unsigned char uchar;
typedef unsigned int  uint;
typedef unsigned long ulong;

/*
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */
main();

/*
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */

/*
 * main()
 *
 * Usage is script [-l logfile] [command]
 *
 * All output from the specified command, including echoing of user
 * input, is recorded to the logfile.
 *
 * "logfile" defaults to L.pid, where "pid" is the number of the
 * recording process.
 *
 * "command" defaults to the whatever environment variable SHELL
 * specifies, and to "/bin/sh" if SHELL is not set.
 */
main(argc, argv, envp)
int argc;
char ** argv, ** envp;
{
	int master_fd, slave_fd, childpid;
	static char master_pty[32];
	static char slave_pty[32];
	static char log_file[60];
	int argn, n;

	sprintf(log_file, "Log.%d", getpid());
	argv++;
	for (argn = 1; argn < argc; argn++, argv++) {
		if (strncmp(*argv, "-l", 2) == 0) {
			if (strlen(*argv) > 2) {
				if (strlen(*argv) < 62)
					strcpy(log_file, *argv + 2);
			} else
				strcpy(log_file, *++argv);
			continue;
		}
		break;
	}

	if (!isatty(0) || !isatty(1)) {
		fprintf(stderr, "stdin and stdout must be tty devices\n");
		exit(1);
	}

	if ((master_fd = getpseudotty(master_pty, slave_pty)) < 0) {
		fprintf(stderr, "can't open master pty\n");
		exit(1);
	}

	if (tty_getmode(0) < 0) {
		fprintf(stderr, "can't getmode of stdin\n");
		exit(1);
	}

	if ((childpid = fork()) < 0) {
		fprintf(stderr, "forking error\n");
		exit(1);
	}

	if (childpid == 0) { /* child */
		setpgrp();

		if ((slave_fd = open(slave_pty, 2)) < 0) {
			fprintf(stderr, "can't open slave pty\n");
			exit(1);
		}
		close(master_fd);

		if (tty_setmode(slave_fd) < 0) {
			fprintf(stderr, "can't setmode of slave fd\n");
			exit(1);
		}
		exec_shell(slave_fd, argv, envp);
	}

	if (tty_raw(0) < 0) {
		fprintf(stderr, "tty_raw error on stdin\n");
		exit(1);
	}

	pass_all(master_fd, log_file);

	if (tty_reset(0) < 0) {
		fprintf(stderr, "tty_reset error on stdin\n");
		exit(1);
	}

	exit(0);
}
