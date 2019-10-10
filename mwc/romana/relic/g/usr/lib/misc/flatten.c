/* 
 * Function flatten() concatinates an array of the strings to a string,
 * where the input strings are separated by spaces.
 */
#include <misc.h>

#define	BUF_SIZE	80		/* Size of the buffer to be alloc */

char	*flatten(argv)
char	*argv[];
{
	extern char	*realloc();
	char		*buf;			/* Buffer for output string */
	unsigned	count = BUF_SIZE;	/* Size of the buffer */
	unsigned	len;			/* Current length used */
	unsigned	i;			/* Length of new addition */

	buf = alloc(BUF_SIZE);
	*buf = '\0';
	len  = 1;	/* We always need a terminator */

	for (; *argv != NULL; argv++) {
		while ((len + (i = strlen(*argv))) > count) {
			count += BUF_SIZE;
			if ((buf = realloc(buf, count)) == NULL)
				fatal("out of space");
		}

		/* If it is not the first string, write string separator */
		if (len > 1) 
			strcpy(buf + len++ - 1, " ");

		strcpy(buf + len - 1, *argv);
		len += i;
	}
	return (realloc(buf, len));
}

#ifdef TEST
main(argc, argv)
int	argc;
char	**argv;
{
	puts(flatten(argv));
}
#endif
