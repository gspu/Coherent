#include <stdio.h>
#define BUFSIZE (20*512)

char buf[BUFSIZE];

void
fatal(s)
char *s;
{
	fprintf(stderr, "%s\n", s);
	exit(1);
}

main(argc, argv)
int argc; char *argv[];
{
	register int ifd, ofd;
	register unsigned int n;

	if (argc != 3)
		fatal("Usage: open source destination");
	if ((ifd = open(argv[1], 0)) == -1)
		fatal("cannot open input file");
	if ((ofd = creat(argv[2], 0)) == -1)
		fatal("cannot open output file");

	while ((n = read(ifd, buf, BUFSIZE)) != 0) {
		if (n == -1)
			fatal("read error");
		if (write(ofd, buf, n) != n)
			fatal("write error");
	}

	if (close(ifd) == -1 || close(ofd) == -1)
		fatal("cannot close");
	exit(0);
}


