#include <stdio.h>

main(argc, argv, envp)
int argc;		/* Number of args */
char *argv[];		/* Argument ptr array */
char *envp[];		/* Environment ptr array */
{
	int a;

	printf("The command name (argv[0]) is %s\n", argv[0]);
	printf("There are %d arguments:\n", argc-1);
	for (a=1; a < argc; a++)
		printf("\targument %2d:\t%s\n", a, argv[a]);

	printf("The environment is as follows:\n");
	a = 0;
	while (envp[a] != NULL)
		printf("\t%s\n", envp[a++]);
}

