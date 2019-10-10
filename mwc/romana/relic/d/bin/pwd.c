/*
 * Rec'd from Lauren Weinstein, 7-16-84.
 * Print working directory
 */

#include <stdio.h>

main()
{
	register char *wd;
	char *getwd();

	if ((wd = getwd()) == NULL) {
		perror("pwd");
		exit(1);
	}
	printf("%s\n", wd);
	exit(0);
}
