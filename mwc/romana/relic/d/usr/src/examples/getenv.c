#include <stdio.h>

main()
{
	char *env;
	extern char *getenv();

	if ((env = getenv("PATH")) == NULL) {
		printf("Sorry, cannot find PATH\n");
		exit(1);
	}
	printf("PATH = %s\n", env);
	exit(0);
}
