#include <stdio.h>

char buf[BUFSIZ];
void fatal();

main(argc, argv)
int argc; 
char *argv[];
{
	register FILE *ifp, *ofp;
	register unsigned int n;

	if (argc != 3)
		fatal("Usage: fopen source destination");
	if ((ifp = fopen(argv[1], "r")) == NULL)
		fatal("cannot open input file");
	if ((ofp = fopen(argv[2], "w")) == NULL)
		fatal("cannot open output file");

	while ((n = fread(buf, 1, BUFSIZ, ifp)) != 0) {
		if (fwrite(buf, 1, n, ofp) != n)
			fatal("write error");
	}

	if (!feof(ifp))
		fatal("read error");
	if (fclose(ifp) == EOF || fclose(ofp) == EOF)
		fatal("cannot close");

	exit(0);
}

void fatal(message)
char *message;
{
	fprintf(stderr, "fatal: %s\n", message);
	exit(1);
}
