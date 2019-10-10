/*
 * Print out the tty name of the
 * current user.
 */

#include <stdio.h>
char	*ttyname();

main(argc, argv)
char *argv[];
{
	char *tty;

	if ((tty = ttyname(fileno(stderr))) != NULL)
		printf("%s\n", tty); else
		printf("Not a tty\n");
	exit(0);
}
