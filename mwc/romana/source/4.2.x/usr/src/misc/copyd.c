/*
 * Copy data from one file to another for a length.
 * Returns 1 on success, 0 on failure.
 */
#include <stdio.h>

copyd(ofp, ifp, len)
FILE *ofp, *ifp;
unsigned long len;
{
	register n, r;
	char buf[BUFSIZ];

	for (n = ftell(ifp) % BUFSIZ; len; (len -= n), (n = 0)) {
		if ((n = BUFSIZ - n) > len)
			n = len;
		if (!(r = fread(buf, 1, n, ifp))  ||
		    (r != fwrite(buf, 1, r, ofp)) ||
		    (r != n))
			return (0);
	}
	return (1);
}

#ifdef TEST
#include <misc.h>

main(argc, argv)
char *argv[];
{
	extern long atol();

	if (argc != 4)
		fatal("test to from length");	

	if (!copyd(xopen(argv[1], "w"), xopen(argv[2], "r"), atol(argv[3])))
		fatal("Error in copy");
}
#endif
