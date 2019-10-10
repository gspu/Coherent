/*
 * basename.c - find the rightmost component of a pathname.
 */
#include <stdio.h>

/*
 * Return a pointer to the basename of a full path (everthing to the
 * right of the rightmost '/'.
 */
char *
basename(path)
char *path;
{
	char *retval;
	extern char *strrchr();

	if (NULL == path)
		return(NULL);
	return ((NULL == (retval = strrchr(path, '/'))) ? path : (retval + 1));
} /* basename() */

#ifdef TEST

main(argc, argv)
int argc;
char *argv[];
{
	while (--argc > 0)
		printf("%s\n", basename(argv[argc]));
} /* main() */

#endif /* TEST */
