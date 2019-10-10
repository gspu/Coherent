#include <ctype.h>
#include <stdio.h>
main()
{
	extern char *gets();
	extern int atoi();
	char string[64];
	int counter;
	int input;

	printf("Enter an integer: ");
	fflush(stdout);
	gets(string);

	for (counter=0; counter < strlen(string); counter++) {
		input = string[counter];
		if (!isascii(input)) {
			fprintf(stderr,
				"%s is not ASCII\n", string);
			exit(1);
		}
		if (!isdigit(input))
			if (input != '-' || counter != 0) {
				fprintf(stderr,
					"%s is not a number\n", string);
				exit(1);
			}
	}

	input = atoi(string);
	printf("abs(%d) is %d\n", input, abs(input));
	exit(0);
}

