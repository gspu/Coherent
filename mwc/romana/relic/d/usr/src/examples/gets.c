#include <stdio.h>

main()
{
	char buffer[80];

	printf("Type something: ");
	fflush(stdout);
	printf("%s\n%s\n", gets(buffer), buffer);
}

