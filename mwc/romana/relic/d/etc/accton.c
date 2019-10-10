/*
 * Enable and disable system-wide
 * process accounting (for sa).
 */

#include <stdio.h>

main(argc, argv)
char *argv[];
{
	char *acfile;

	if (argc > 2)
		usage();
	acfile = argc==1 ? NULL : argv[1];
	if (acct(acfile) < 0)
		perror(acfile);
}

usage()
{
	fprintf(stderr, "Usage: /etc/accton [file]\n");
	exit(1);
}
