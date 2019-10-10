#include <ctype.h>
#include <stdio.h>

main()
{
	FILE *fp;
	int ch;
	int filename[20];

	printf("Enter name of file to use: ");
	fflush(stdout);
	gets(filename);

	if ((fp = fopen(filename,"r")) != NULL)
	{
		while ((ch = fgetc(fp)) != EOF)
			putchar(isupper(ch) ? tolower(ch) : toupper(ch));
	} else
		 printf("Cannot open %s.\n", filename);
}
