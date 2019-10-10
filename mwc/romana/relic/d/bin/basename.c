/*
 * Strip filename suffix and directory prefix.
 */

#include <stdio.h>

char	*basename();

main(argc, argv)
char *argv[];
{
	if (argc < 2 || argc > 3) {
		fprintf(stderr, "Usage: basename string [suffix]\n");
		exit(1);
	}
	printf("%s\n", basename(argv[1], argc==2 ? NULL : argv[2]));
	exit(0);
}

/*
 * Do the work of producing a stripping prefixes and
 * a suffix (if not NULL).
 */
char *
basename(s, suffix)
char *s;
char *suffix;
{
	register char *cp, *ep, *sp;
	char *p;

	for (ep = cp = s; *ep != '\0'; ep++)
		if (*ep == '/')
			cp = ep+1;
	if ((sp = suffix) != NULL) {
		ep = cp;
		while (*ep++ != '\0')
			;
		ep--;
		while (*sp++)
			ep--;
		if (ep < cp)
			return (cp);
		p = ep;
		for (sp = suffix; *sp!='\0' && *ep!='\0'; )
			if (*sp++ != *ep++)
				return (cp);
		if (*sp != '\0')
			return (cp);
		*p = '\0';
	}
	return (cp);
}
