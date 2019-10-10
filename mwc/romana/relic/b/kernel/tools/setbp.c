/*
 * setbp.c - set/clear kernel breakpoints
 *
 * Usage:  setbp bp# [addr type len]
 */
#include <stdio.h>
#include <sys/param.h>

char * cmd;

main(argc, argv)
int argc;
char ** argv;
{
	int a2, a3, a4, a5, res;

	cmd = argv[0];
	if (argc != 2 && argc != 5)
		usage();
	if (sscanf(argv[1], "%d", &a2) != 1)
		usage();

	if (argc == 2) {	/* argc == 2:  set breakpoint */
		res = cohcall(COH_CLRBP, a2);
		if (res == -1)
			perror("clrbp");
	} else {		/* argc == 5:  clear breakpoint */
		if (sscanf(argv[2], "%x", &a3) != 1)
			usage();
		if (sscanf(argv[3], "%d", &a4) != 1)
			usage();
		if (sscanf(argv[4], "%x", &a5) != 1)
			usage();
		res = cohcall(COH_SETBP, a2, a3, a4, a5);
		if (res == -1)
			perror("setbp");
	}
}

usage()
{
	fprintf(stderr, "Usage: %s bp# [hex-addr type len]\n", cmd);
	exit(1);
}
