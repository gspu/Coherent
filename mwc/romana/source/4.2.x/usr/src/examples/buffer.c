
#include <stdio.h>
main()
{
	char buffer[80];
	printf("Type your name:  ");
	fflush(stdout);
	gets(buffer);
	printf("Your name is %s\n", buffer);
}
