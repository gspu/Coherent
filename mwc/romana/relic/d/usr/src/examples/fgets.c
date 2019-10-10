#include <stdio.h>
#define MAXLINE 128

void fatal();
char buf[MAXLINE];

main(argc, argv)
int argc; 
char *argv[];
{
	if (argc != 2 && argc != 3)
		fatal("Usage: fgets pattern [ file ]");

	if (argc==3 && freopen(argv[2], "r", stdin) == NULL)
		fatal("cannot open input file");

	while (fgets(buf, MAXLINE, stdin) != NULL) {
		if (pnmatch(buf, argv[1], 1))
			printf("%s", buf);
	}
	exit(0);
}

void fatal(s) char *s;
{
	fprintf(stderr, "fgets: %s\n", s);
	exit(1);
}

