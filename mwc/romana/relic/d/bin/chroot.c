static char _version[]="chroot version 1.0";
/*
 * chroot - Change root
 *
 * usage: chroot dir program [args]
 *
 * Changes root to "dir" and runs "program".  "program" should be a full
 * pathname relative to the new root.  "args" is an optional list of
 * arguments for program.
 */
#include <stdio.h>
#include <errno.h>

main(argc, argv)
int argc;
char *argv[];
{
	static char error_message[BUFSIZ];

	if (argc < 3) {
		fprintf(stderr, "usage:  %s dir program [args]\n", argv[0]);
	} else if (chroot(argv[1]) != 0) {
		sprintf(error_message, "chroot failed: %s", argv[1]);
		perror(error_message);
	} else if (chdir("/") != 0) {
		perror("chdir to / failed");
	} else if (execv(argv[2], &argv[2]) == -1) {
		sprintf(error_message, "exec failed: %s", argv[2]);
		perror(error_message);
	}
	exit(1);
}
