/*
 * captoinfo:
 *	Translate termcap terminal database to terminfo source
 *	format.
 *
 *	Captoinfo reads standard input, which is assumed to be
 *	a termcap file and writes the equivalent to standard
 *	output in terminfo source format.
 *
 * Robert Viduya - Georgia Institute of Technology.
 *
 *	gitpyr!robert
 */
#include <stdio.h>

#define	bool	char
#define	TRUE	1
#define	FALSE	0

char    buffer[2048];

main (argc, argv)
int	argc;
char	**argv;
{
    int		c;

    if (argc == 2) 
	if (freopen(argv[1], "r", stdin) == NULL) {
		fprintf(stderr, "captoinfo: cannot open file %s\n", argv[1]);
		exit(1);
	}

    while ((c = getchar ()) != EOF) {
	if (c == '#') {
	    (void) putchar (c);
	    do {
		c = getchar ();
		(void) putchar (c);
	    } while (c != '\n');
	}
	else {
	    if (ungetc (c, stdin) == EOF) {
		fprintf (stderr, "ungetc failed.\n");
		exit (1);
	    }
	    get_termcap ();
	    /* Quick hack to skip empty lines. Vlad */
	    if (!strlen(buffer)) {
		printf("\n");
		continue;
	    }
	    print_name ();
	    print_bools ();
	    print_nums ();
	    print_strs ();
	}
    }
    exit (0);
}
