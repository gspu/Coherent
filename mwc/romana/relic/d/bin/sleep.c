/*
 * Sleep command
 */

#include <stdio.h>
#include <sys/mdata.h>

long	numeric();
long	atol();

main(argc, argv)
char *argv[];
{
	if (argc > 2)
		usage();
	if (argc > 1)
		lsleep(numeric(argv[1]));
	return (0);
}

/*
 * Check if argument is numeric and
 * return it.
 */
long
numeric(s)
register char *s;
{
	long n;

	n = atol(s);
	for (; *s!='\0'; s++)
		if (*s<'0' || *s>'9')
			usage();
	return (n);
}

/*
 * Sleep that takes a long argument.
 */
lsleep(n)
long n;
{
	register unsigned i;

	while (n > 0) {
		i = n>MAXUINT ? MAXUINT : n;
		sleep(i);
		n -= i;
	}
}

usage()
{
	fprintf(stderr, "Usage: sleep seconds\n");
	exit(1);
}
