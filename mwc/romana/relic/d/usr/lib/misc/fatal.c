/*
 * Put message and die.
 */
#include <stdio.h>
#include <errno.h>

fatal(s)
char *s;
{
	int save = errno;

	fprintf(stderr, "\nfatal: %r\n", &s);
	if (0 != (errno = save))
		perror("errno reports");
	exit(1);
}

#ifdef TEST
main()
{
	sqrt(-1.0);
	fatal("test %d", 1);
}
#endif
