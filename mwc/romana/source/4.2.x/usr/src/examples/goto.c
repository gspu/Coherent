#include <stdio.h>

main()
{
	char line[80];

	getline:
		printf("Enter line: ");
		fflush(stdout);
		gets(line);

/* a series of tests often is best done with goto's */
	if (*line == 'x') {
		printf("Bad line\n");
		goto getline;
	} else 
		if (*line == 'y') {
			printf("Try again\n");
			goto getline;
		} else 
			if (*line == 'q')
				goto goodbye;
			else
				goto getline;

	goodbye:
		printf("Goodbye.\n");
}
