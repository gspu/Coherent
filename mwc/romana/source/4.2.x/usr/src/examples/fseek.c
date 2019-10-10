#include <stdio.h>

extern long atol();
void fatal();

main(argc, argv)
int argc; char *argv[];
{
	register FILE *ifp;
	register int c;
	long nchars, size;

	if (argc < 2 || argc > 3)
		fatal("Usage: fseek file [ nchars ]");
	nchars = (argc == 3) ? atol(argv[2]) : 100L;
	if ((ifp = fopen(argv[1], "r")) == NULL)
		fatal("cannot open input file");
	/* Seek to end */
	if (fseek(ifp, 0L, 2) == -1)
		fatal("seek error");
	/* Find current size */
	size = ftell(ifp);
	size = (size < nchars) ? 0L : size - nchars;

	/* Seek to point */
	if (fseek(ifp, size, 0) == -1)
		fatal("seek error");
	while ((c = getc(ifp)) != EOF)
		/* Copy rest to stdout */
		putchar(c);
	if (fclose(ifp) == EOF)
		fatal("cannot close");
	exit(0);
}

void fatal(message)
char *message;
{
	fprintf(stderr, "fseek: %s\n", message);
	exit(1);
}

