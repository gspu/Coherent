#include <stdio.h>

main()
{
	char string[80];
	char s1[10], s2[20];

	sprintf(string, "1234567890abcdefghijABCDEFGHIJ");
	sscanf(string, "%9c", s1);
	sscanf(string, "%19c", s2);

	printf("\n%s is the string\n", string);
	printf("%s: first 9 characters in string\n", s1);
	printf("%s: first 19 characters in string\n", s2);
}

