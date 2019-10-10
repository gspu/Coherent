/*
 * copy.c
 * 11/4/92
 */

#include <unixstdio.h>

extern	void	fatal();

FILE *ifp, *ofp;

main(argc, argv) int argc; char *argv[];
{
	register int c;

	if (argc != 3)
		fatal("Usage: copy in out");
	if ((ifp = fopen(argv[1], "r")) == NULL)
		fatal("copy: cannot open input file");
	if ((ofp = fopen(argv[2], "w")) == NULL)
		fatal("copy: cannot open input file");
	while ((c = getc(ifp)) != EOF)
		putc(c, ofp);
	printf("feof(ifp)=%d\n", feof(ifp));
	printf("ferror(ifp)=%d\n", ferror(ifp));
	printf("ferror(ofp)=%d\n", ferror(ofp));
	fclose(ifp);
	fclose(ofp);
	exit(0);	
}

/* VARARGS */
void
fatal(msg) char *msg;
{
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

/* end of copy.c */
