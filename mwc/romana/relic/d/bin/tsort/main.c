#include <stdio.h>
#include "tsort.h"


main(argc, argv)
register int	argc;
register char	*argv[];
{
	register FILE *fp = stdin;

	if (argc > 2)
		usage();
	if (argc == 2) {
		fp = fopen(*++argv, "r");
		if (fp == NULL)
			die("Can't open %s", *argv);
	}
	getcon(fp);
	order();
	return (0);
}

usage()
{
	fprintf(stderr, "usage: tsort [file]\n");
	exit(1);
}
