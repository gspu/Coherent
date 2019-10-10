/*
 * Produce a continuous stream of `y's
 * or `n's.
 */

#include <stdio.h>

main(argc, argv)
char *argv[];
{
	char *yes = "y";

	if (argc > 2)
		usage();
	if (argc > 1)
		yes = argv[1];
	for (;;)
		puts(yes);
	/* NOTREACHED */
}

usage()
{
	fprintf(stderr, "Usage: yes [answer]\n");
	exit(1);
}
