#include <stdio.h>
#include <ctype.h>

#define	bool	char
#define	TRUE	1
#define	FALSE	0

extern char    buffer[];


/*
 * get_termcap:
 *	read next termcap entry into buffer from standard input.
 */
get_termcap ()
{
    int		c;
    char	*bptr;

    bptr = buffer;
    while ((c = getchar ()) != '\n') {
	if (c == '\\') {
	    if ((c = getchar ()) != '\n') {
		if (ungetc (c, stdin) == EOF) {
		    fprintf (stderr, "ungetc failed.\n");
		    exit (1);
		}
		*(bptr++) = '\\';
	    }
	}
	else {
	    *(bptr++) = c;
	}
    }
    *bptr = '\0';
}
